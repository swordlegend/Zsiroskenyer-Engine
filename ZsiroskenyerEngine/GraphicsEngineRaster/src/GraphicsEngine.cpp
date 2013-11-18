////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/GraphicsEngine.cpp
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsEngine.h"
#include "../../Core/src/Factory.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "ShaderManager.h"

#include "Geometry.h"
#include "..\..\Core\src\GraphicsEntity.h"
#include "..\..\Core\src\Camera.h"

#include "..\..\Core\src\IGraphicsApi.h"
#include "..\..\Core\src\IShaderProgram.h"
#include "..\..\Core\src\IConstantBuffer.h"
#include "..\..\Core\src\IVertexBuffer.h"
#include "..\..\Core\src\IIndexBuffer.h"

#include "..\..\Core\src\Exception.h"
#include "..\..\Core\src\math/Matrix44.h"
#include "..\..\Core\src\common.h"

// Construction of the graphics engine
cGraphicsEngine::cGraphicsEngine() {
	gApi = Factory.CreateGraphicsD3D11();
	if (!gApi)
		throw UnknownErrorException("failed to create graphics api");
	shaderManager = new cShaderManager(gApi);
	resourceManager = new cResourceManager(gApi);
	sceneManager = new cSceneManager();

	// Basic 3D geom rendering
	shaderManager->LoadShader(L"shaders/",L"test.cg");

	// Now, For debugging
	shaderManager->LoadShader(L"shaders/",L"LINE_RENDERER.cg");
}
cGraphicsEngine::~cGraphicsEngine() {
	SAFE_DELETE(sceneManager);
	SAFE_DELETE(resourceManager)
	SAFE_DELETE(shaderManager);
	SAFE_RELEASE(gApi);
}

void cGraphicsEngine::Release() {
	delete this;
}

// TODO: Reload all resources not only shaders
void cGraphicsEngine::ReloadResources() {
	shaderManager->ReloadShader(L"shaders/", L"test.cg");
	shaderManager->ReloadShader(L"shaders/", L"LINE_RENDERER.cg");
}

void cGraphicsEngine::RenderSceneForward() {

	ASSERT(sceneManager->GetActiveCamera() != NULL);

	// Set BackBuffer
	gApi->SetRenderTargetDefault();

	// Set ShaderProgram
	IShaderProgram* shaderP = shaderManager->GetShaderByName(L"test.cg");
	gApi->SetShaderProgram(shaderP);
	
	// Get camera params
	cCamera* cam = sceneManager->GetActiveCamera();
	Matrix44 viewMat = cam->GetViewMatrix();
	Matrix44 projMat = cam->GetProjMatrix();
	
	// Render each instanceGroup
	for (auto& group : sceneManager->GetInstanceGroups()) {
		// Set Geometry
		const IIndexBuffer* ib = (*group->geom).GetIndexBuffer();
		gApi->SetIndexBuffer(ib);
		gApi->SetVertexBuffer((*(group->geom)).GetVertexBuffer(), shaderP->GetVertexFormatSize());

		// Set SubMaterials
		for(size_t i = 0; i < (*group->mtl).GetNSubMaterials(); i++) {
			gApi->SetTexture((*(group->mtl))[i].textureDiffuse.get(), 0);
			gApi->SetTexture((*(group->mtl))[i].textureNormal.get(), 1);
		}
		
		// Draw each entity
		for (auto& entity : group->entities) {
			// Entity world matrix
			Matrix44 world = entity->GetWorldMatrix();

			// WorldViewProj matrix
			Matrix44 wvp = world * viewMat * projMat;

			struct myBuffer
			{
				Matrix44 wvp;
				Matrix44 world;
				Vec3 camPos;

			} buff;
			buff.wvp = wvp;
			buff.world = world;
			buff.camPos = cam->GetPos();

			IConstantBuffer* buffer;
			gApi->CreateConstantBuffer(&buffer, sizeof(buff), eUsage::DEFAULT, &buff);
			gApi->SetVSConstantBuffer(buffer, 0);
			// Create, load constant buffers, World and WorldViewProj
			/*IConstantBuffer* wvpBuffer = gApi->CreateConstantBuffer(sizeof(Matrix44), eUsage::DEFAULT, &wvp);
			IConstantBuffer* worldBuffer= gApi->CreateConstantBuffer(sizeof(Matrix44), eUsage::DEFAULT, &world);
				gApi->SetVSConstantBuffer(wvpBuffer, 0);
				gApi->SetVSConstantBuffer(worldBuffer, 4);
				*/
			
			// Draw entity..
			gApi->DrawIndexed(ib->GetSize() / sizeof(unsigned));

			// Free up constantBuffer
			buffer->Release();
			//wvpBuffer->Release();
			//worldBuffer->Release();
		}
	}
}

void cGraphicsEngine::SetActiveCamera(cCamera* cam) {
	sceneManager->SetActiveCamera(cam);
}

cGraphicsEntity* cGraphicsEngine::CreateEntity(const zsString& geomPath, const zsString& mtlPath) {
	cGeometryRef geom = resourceManager->GetGeometry(geomPath);
	cMaterialRef mtl = resourceManager->GetMaterial(mtlPath);
	if (!geom || !mtl) {
		return NULL;
	}
	return sceneManager->AddEntity(std::move(geom), std::move(mtl));
}

cSceneManager* cGraphicsEngine::GetSceneManager()  {
	return sceneManager;
}

cResourceManager* cGraphicsEngine::GetResourceManager() {
	return resourceManager;
}

IGraphicsApi* cGraphicsEngine::GetGraphicsApi() {
	return gApi;
}

IShaderManager* cGraphicsEngine::GetShaderManager() {
	return shaderManager;
}

cCamera* cGraphicsEngine::GetActiveCamera() {
	return sceneManager->GetActiveCamera();
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////DEFERRED RENDERER///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

cGraphicsEngine::cDeferredRenderer::cDeferredRenderer(cGraphicsEngine& parent)
:gEngine(parent) {
	IGraphicsApi* gapi = parent.GetGraphicsApi();

	ITexture2D* bb = gapi->GetDefaultRenderTarget();
	//eGapiResult gr = gapi->CreateTexture(diffuse, )
}

cGraphicsEngine::cDeferredRenderer::~cDeferredRenderer() {
	SAFE_RELEASE(diffuse);
	SAFE_RELEASE(normal);
	SAFE_RELEASE(specular);
}