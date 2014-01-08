////////////////////////////////////////////////////////////////////////////////
// Helper class for processing HDR


#include "GraphicsEngine.h"
#include "ShaderManager.h"
#include <cassert>
#include <stdexcept>
#include <algorithm>


////////////////////////////////////////////////////////////////////////////////
//	Constructor & destructor
cGraphicsEngine::cHDRProcessor::cHDRProcessor(cGraphicsEngine& parent) : parent(parent), gApi(parent.gApi) {
	// set vars
	avgLuminance = 0;
	adaptedLuminance = 0;
	sourceWidth = sourceHeight = 0;
	blurBuffer = NULL;
	downSampled = NULL;
	luminanceStaging = NULL;
	source = NULL;
	dest = NULL;
	for (auto& t : luminanceBuffer)
		t = NULL;

	// create shaders
	shaderLumSample = parent.shaderManager->LoadShader("shaders/hdr_luminance_sample.cg");
	shaderLumAvg = parent.shaderManager->LoadShader("shaders/hdr_luminance_avg.cg");
	shaderCompose = parent.shaderManager->LoadShader("shaders/hdr_compose.cg");
	shaderBlurHoriz = parent.shaderManager->LoadShader("shaders/hdr_blur_horiz.cg");
	shaderBlurVert = parent.shaderManager->LoadShader("shaders/hdr_blur_vert.cg");
	shaderOverbright = parent.shaderManager->LoadShader("shaders/hdr_overbright_downsample.cg");
	if (!shaderLumSample || !shaderLumAvg || !shaderBlurHoriz || !shaderBlurVert || !shaderOverbright || !shaderCompose) {
		Cleanup();
		throw std::runtime_error("failed to create shaders");
	}

	// create constant buffers
	/*eGapiResult rcb = gApi->CreateConstantBuffer(&cbCompose, sizeof(float)+12, eUsage::DEFAULT);
	if (rcb != eGapiResult::OK) {
		Cleanup();
		throw std::runtime_error("failed to create shader constant buffers");
	}*/

	// create luminance textures
	unsigned resolution=1;
	ITexture2D::tDesc desc;
	
	desc.format = eFormat::R32_FLOAT;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.usage = eUsage::DEFAULT;
	for (int i = 0; i < 10; i++) {
		desc.width = desc.height = resolution;
		eGapiResult r = parent.gApi->CreateTexture(&luminanceBuffer[9-i], desc);
		if (r != eGapiResult::OK) {
			Cleanup();
			throw std::runtime_error("failed to create luminance buffers");
		}
		resolution *= 2;
	}

	desc.bind = 0;
	desc.usage = eUsage::STAGING;
	desc.width = desc.height = 1;
	eGapiResult r = parent.gApi->CreateTexture(&luminanceStaging, desc);
	if (r != eGapiResult::OK) {
		Cleanup();
		throw std::runtime_error("failed to create luminance cpu copy buffer");
	}
}

cGraphicsEngine::cHDRProcessor::~cHDRProcessor() {
	// release all textures
	Cleanup();
}

void cGraphicsEngine::cHDRProcessor::Cleanup() {
	// release all textures
	for (auto& t : luminanceBuffer)
		SAFE_RELEASE(t);
	SAFE_RELEASE(blurBuffer);
	SAFE_RELEASE(downSampled);
	SAFE_RELEASE(luminanceStaging);
}



////////////////////////////////////////////////////////////////////////////////
//	Methods

//	Set HDR range float buffer
eGraphicsResult cGraphicsEngine::cHDRProcessor::SetSource(ITexture2D* srcTexture, unsigned sourceWidth, unsigned sourceHeight) {
	// release old downsampled buffers
	SAFE_RELEASE(blurBuffer);
	SAFE_RELEASE(downSampled);

	// set internal vars
	this->sourceWidth = sourceWidth;
	this->sourceHeight = sourceHeight;
	this->source = srcTexture;
	assert(srcTexture != NULL);

	// create downsampled buffers
	ITexture2D::tDesc desc;
	desc.bind = (int)eBind::RENDER_TARGET | (int)eBind::SHADER_RESOURCE;
	desc.format = eFormat::R16G16B16A16_FLOAT;
	desc.width = sourceWidth / 2;
	desc.height = sourceHeight / 2;
	auto r = parent.gApi->CreateTexture(&downSampled, desc);
	if (r != eGapiResult::OK) {
		return eGraphicsResult::ERROR_UNKNOWN;
	}
	r = parent.gApi->CreateTexture(&blurBuffer, desc);
	if (r != eGapiResult::OK) {
		SAFE_RELEASE(downSampled)
		return eGraphicsResult::ERROR_UNKNOWN;
	}

	return eGraphicsResult::OK;
}

// Set LDR output buffer
void cGraphicsEngine::cHDRProcessor::SetDestination(ITexture2D* dest) {
	this->dest = dest;
}

////////////////////////////////////////////////////////////////////////////////
//	Update
#include <iostream>
float elapsedTotal = 0.0f; // debug print state changes

void cGraphicsEngine::cHDRProcessor::Update(float elapsedSec) {
	float elapsed = 1e-8;
	if (elapsedSec >= 1e-8f) {
		elapsed = elapsedSec;
	}
	elapsedTotal += elapsed;

	// calculate average luminance
	gApi->SetShaderProgram(shaderLumSample);
	gApi->SetRenderTargets(1, &luminanceBuffer[0], NULL);
	gApi->SetTexture(L"textureInput", source);
	gApi->Draw(3);

	parent.gApi->SetShaderProgram(shaderLumAvg);
	for (int i = 1; i < 10; i++) {
		gApi->SetRenderTargets(1, &luminanceBuffer[i], NULL);
		gApi->SetTexture(L"textureInput", luminanceBuffer[i - 1]);
		gApi->Draw(3);
	}

	// extract average luminance from buffer
	auto r = gApi->CopyResource(luminanceBuffer[9], luminanceStaging);
	gApi->ReadResource(luminanceStaging, &avgLuminance, sizeof(float));

	// calculate adaptation
	avgLuminance = std::max(1e-15f, avgLuminance);
	float logAvgLum = log10(avgLuminance);
	float rodSensitivity = 0.04 / (0.04 + logAvgLum);
	float speed = rodSensitivity*0.4f + (1 - rodSensitivity)*0.4;
	float a = adaptedLuminance;
	adaptedLuminance = adaptedLuminance + (logAvgLum - adaptedLuminance)*(1 - exp(-elapsed / speed));

	if (isnan(adaptedLuminance)) {
		__debugbreak();
	}

	// shader constants
	struct {
		float logAvgLum;
		float blueShift;
		float pad1;
		float pad2;
	} shaderConstants;

	
	// downsample that bullshit for blurring
	shaderConstants.logAvgLum = adaptedLuminance;

	gApi->SetRenderTargets(1, &downSampled);
	gApi->SetShaderProgram(shaderOverbright);
	gApi->SetPSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);
	//gApi->SetPSConstantBuffer(cbCompose, 0);
	gApi->SetTexture(L"textureInput", source);
	gApi->Draw(3);
	
	// now blur that bullshit
	gApi->SetRenderTargets(1, &blurBuffer);
	gApi->SetShaderProgram(shaderBlurHoriz);
	gApi->SetTexture(L"textureInput", downSampled);
	gApi->Draw(3);
	gApi->SetRenderTargets(1, &downSampled);
	gApi->SetShaderProgram(shaderBlurVert);
	gApi->SetTexture(L"textureInput", blurBuffer);
	gApi->Draw(3);
	
	// compose to destination buffer
	shaderConstants.logAvgLum = adaptedLuminance;
	// blueshift: mesopic range from -2.3 to 0.7 mcd/m2
	shaderConstants.blueShift = 1.0f - std::min(std::max((adaptedLuminance + 2.3f) / (0.7f + 2.3f), 0.0f), 1.0f);

	gApi->SetRenderTargets(1, &dest);
	gApi->SetShaderProgram(shaderCompose);
	//gApi->SetConstantBufferData(cbCompose, &shaderConstants);
	//gApi->SetPSConstantBuffer(cbCompose, 0);
	gApi->SetPSConstantBuffer(&shaderConstants, sizeof(shaderConstants), 0);
	gApi->SetTexture(L"textureInput", source);
	gApi->SetTexture(L"blurTexture", downSampled);
	gApi->Draw(3);


/*	// display HDR information
	if (elapsedTotal >= 1.0f) {
		std::cout << "Avg. luminance = " << avgLuminance << ", log10(lum) =  " << log10(avgLuminance) << std::endl;
		std::cout << "   Adaptation = " << adaptedLuminance << std::endl;
		std::cout << "   [" << avgLuminance*1.998e-3 << ", " << avgLuminance*1.998 << "]\n";
		std::cout << "   Blueshift = " << shaderConstants.blueShift << std::endl;
		elapsedTotal = 0.0f;
	}*/
}