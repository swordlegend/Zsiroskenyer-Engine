// GraphicsD3D11.h By Zs�roskeny�r team 2013.10.10 1:32 last modified N�meth Rich�rd
// IGraphicsApi interface implementation based on the 3D graphics SDK DirectX11
#pragma once
#ifdef _MSC_VER
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>

#undef ERROR_FILE_NOT_FOUND // fucking dx collides with eGapiResult::ERROR_FILE_NOT_FOUND
#undef max // fasz kivan m�r ezzel a buzi windowssal
#include "../../Core/src/GAPI.h"
#include "../../Core/src/IWindow.h"
#include "../../Core/src/common.h"

#include <map>

class IVertexBuffer;
class IIndexBuffer;
class IConstantBuffer;
class IShaderProgram;
class IGraphicsApi;
class cTexture2DD3D11;
class cShaderProgramD3D11;

// DLL accessor
extern "C"
__declspec(dllexport)
IGraphicsApi* CreateGraphicsApiD3D11(IWindow* targetWindow, unsigned backBufferWidth, unsigned backBufferHeight);



class cGraphicsApiD3D11 : public IGraphicsApi {
public:
	// Compiling profiles for cg
	enum class eProfileCG {
		SM_5_0_BEGIN = 0,
		VS_5_0 = 0,
		HS_5_0,
		DS_5_0,
		GS_5_0,
		PS_5_0,
		SM_4_0_BEGIN = 5,
		VS_4_0 = 5,
		HS_4_0,
		DS_4_0,
		GS_4_0,
		PS_4_0,
		SM_3_0_BEGIN = 10,
		VS_3_0 = 10,
		PS_3_0,
		SM_2_0_BEGIN = 12,
		VS_2_0 = 12,
		PS_2_0,
	};

	cGraphicsApiD3D11();
	~cGraphicsApiD3D11();
	void Release() override;

	// --- resources --- //
	eGapiResult	CreateVertexBuffer(IVertexBuffer** resource, size_t size, eUsage usage, void* data = NULL) override;
	eGapiResult	CreateIndexBuffer(IIndexBuffer** resource, size_t size, eUsage usage, void* data = NULL) override;
	eGapiResult CreateTexture(ITexture2D** resource, const zsString& filePath) override;
	eGapiResult CreateTexture(ITexture2D** resource, ITexture2D::tDesc desc, void* data = NULL) override;
	eGapiResult CreateShaderProgram(IShaderProgram** resource, const zsString& shaderPath) override;

	eGapiResult WriteResource(IIndexBuffer* buffer, void* source, size_t size = ZS_MAX(size_t), size_t offset = 0) override;
	eGapiResult WriteResource(IVertexBuffer* buffer, void* source, size_t size = ZS_MAX(size_t), size_t offset = 0) override;

	eGapiResult ReadResource(IIndexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	eGapiResult ReadResource(IVertexBuffer* buffer, void* dest, size_t size, size_t offset = 0) override;
	eGapiResult ReadResource(ITexture2D* texture, void* dest, size_t size, size_t offset = 0) override;

	eGapiResult CopyResource(ITexture2D* src, ITexture2D* dst) override;

	// --- render --- //
	void Clear(bool target = true, bool depth = false, bool stencil = false, const Vec4& clearColor = Vec4(0.3f, 0.3f, 0.3f, 1.0f)) override;
	void ClearTexture(ITexture2D* t, unsigned clearFlag = 0, const Vec4& clearColor = Vec4(), float depthVal = 1.0f, size_t stencilVal = 0);

	void Draw(size_t nVertices, size_t idxStartVertex = 0) override;
	void DrawIndexed(size_t nIndices, size_t idxStartIndex = 0) override;
	void DrawInstanced(size_t nVerticesPerInstance, size_t nInstances, size_t idxStartVertex = 0, size_t idxStartInstance = 0) override;
	void DrawInstancedIndexed(size_t nIndicesPerInstance, size_t nInstances, size_t idxStartIndex = 0, size_t idxStartInstance = 0) override;

	void Present() override;

	// --- render state --- //
	eGapiResult SetRenderTargetDefault() override;
	eGapiResult SetRenderTargets(unsigned nTargets, const ITexture2D* const* renderTargets, ITexture2D* depthStencilTarget = NULL) override;
	eGapiResult SetBackBufferSize(unsigned width, unsigned height) override;

	void SetVertexBuffer(const IVertexBuffer* vertexBuffer, size_t vertexStride) override;
	void SetIndexBuffer(const IIndexBuffer* indexBuffer) override;
	void SetInstanceData(/*whatever*/) override;
	eGapiResult SetTexture(const ITexture2D* t, size_t slotIdx) override;
	eGapiResult SetTexture(const zsString& varName, const ITexture2D* t) override;
	eGapiResult SetVSConstantBuffer(const void* data, size_t size, size_t slotIdx) override;
	eGapiResult SetPSConstantBuffer(const void* data, size_t size, size_t slotIdx) override;
	void SetShaderProgram(IShaderProgram* shProg) override;
	void SetPrimitiveTopology(ePrimitiveTopology t) override;
	eGapiResult SetWindow(IWindow *renderWindow) override;

	eGapiResult SetBlendState(tBlendDesc desc) override;
	eGapiResult SetDepthStencilState(tDepthStencilDesc desc, uint8_t stencilRef) override;

	// --- misc --- //
	ITexture2D* GetDefaultRenderTarget() const override;
	std::string GetLastErrorMessage() const override;

private:
	eGapiResult CreateDevice();
	eGapiResult CreateMostAcceptableSwapChain(size_t width, size_t height, HWND windowHandle);
	eGapiResult CreateViewsForBB();
	eGapiResult CreateDefaultStates();
	HRESULT CompileShaderFromFile(const zsString& fileName, const zsString& entry, const zsString& profile, zsString* compilerMessage, ID3DBlob** ppBlobOut);
	eGapiResult CompileCgToHLSL(const zsString& cgFileName, const zsString& hlslFileName, eProfileCG compileProfile);
	void ApplyConstantBuffers();
protected:
	// Error handling
	zsString lastErrorMessage;
	
	// backBuffer will be the main render target
	cTexture2DD3D11* defaultRenderTarget;
	D3D11_VIEWPORT defaultVP;

	// Main Dx interfaces
	ID3D11DeviceContext *d3dcon;
	ID3D11Device *d3ddev;
	IDXGISwapChain *d3dsc;

// inner TMP

	// Shader program stuff
	cShaderProgramD3D11* activeShaderProg;

	// Viewports and RTs
	D3D11_VIEWPORT activeViewports[16];
	ID3D11RenderTargetView* activeRTVs[16];

	// Constant buffers handling
	void* vsConstBufferData; // These updated every ex. SetPSConstantBuffer(.....) call
	void* psConstBufferData;
	size_t vsConstBufferSize;
	size_t psConstBufferSize;
	ID3D11Buffer* vsConstBuffer; // Then these gets updates when Draw called
	ID3D11Buffer* psConstBuffer;
	bool vsConstBufferStateChanged; // Don't want to apply constant buffers when it's not changed
	bool psConstBufferStateChanged;

	// States <D3D11_____DESC, ID3D11____*>
	std::map<size_t, ID3D11DepthStencilState*> depthStencilStates;
	std::map<size_t, ID3D11BlendState*> blendStates;
};