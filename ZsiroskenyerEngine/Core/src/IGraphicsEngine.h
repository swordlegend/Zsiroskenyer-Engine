////////////////////////////////////////////////////////////////////////////////
//	GraphicsEngine/src/IGraphicsEngine.h
//	2013.oct.10, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Interface to the graphics engine.
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <cstdint>

class IGraphicsApi;
class IShaderManager;
class IGraphicsEntity;
class IGraphicsLight;
class cCamera;
class IWindow;
class IGeometryRef;
class IGeometryBuilder;

////////////////////////////////////////////////////////////////////////////////
//	Configuration

// global graphics engine config
union tGraphicsConfig {
	static enum eGraphicsApi : uint32_t {
		OPENGL_43 = 1,
		D3D11 = 2,
	};
	struct tRasterEngine {
		eGraphicsApi gxApi;
	} rasterEngine;
};

// scene rendering states
struct tRenderState {
	// hdr
	struct HdrRendering {
		bool enabled;
		int lensEffect;
		int starEffect;
	} hdr;

	// motion blur
	struct MotionBlur {
		int quality; // 0 disabled
	} motionBlur;

	// depth of field
	struct DepthOfField {
		int quaility; // 0 disabled
		float focusDistance; // negative: auto
	} depthOfField;

	// ambient occlusion
	struct AmbientOcclusion {
		static enum {
			DISABLE = 0,
			SSAO = 1,
			// SSDO = 2,
			// HBAO = 3,
			// SAO = 4,
		} method;
		int quality;
	};
};


////////////////////////////////////////////////////////////////////////////////
//	Errors
enum class eGraphicsResult : signed int {
	OK = 0,
	ERROR_UNKNOWN,
	ERROR_OUT_OF_MEMORY,
	ERROR_FILE_NOT_FOUND,
	ERROR_INVALID_CALL,
	ERROR_INVALID_ARG
};

////////////////////////////////////////////////////////////////////////////////
//	Graphics scene interface
class IGraphicsScene {
public:
	// entities & lights
	virtual IGraphicsEntity* CreateEntity(const wchar_t* geomPath, const wchar_t* mtlPath) = 0;
	virtual IGraphicsEntity* CreateEntity(IGeometryRef* customGeom, const wchar_t* mtlPath) = 0;
	virtual void DeleteEntity(const IGraphicsEntity* entity) = 0;
	virtual IGraphicsLight* CreateLight() = 0;
	virtual void DeleteLight(const IGraphicsLight* light) = 0;
	virtual void Clear() = 0;

	// scene state
	virtual cCamera& GetCamera() = 0;
	virtual tRenderState& GetState() = 0;

	// scene order
	virtual void MoveUp() = 0;
	virtual void MoveDown() = 0;
	virtual void MoveTop() = 0;
	virtual void MoveBottom() = 0;
};

////////////////////////////////////////////////////////////////////////////////
//	Graphics Engine interface
class IGraphicsEngine {
public:
	// virtual interface stuff
	virtual void				Release() = 0;

	virtual eGraphicsResult		Update(float elapsed = 0.0f) = 0;

	virtual IGeometryBuilder*	CreateCustomGeometry() = 0;
	virtual IGraphicsScene*		CreateScene(tRenderState state = tRenderState()) = 0;

	virtual void				DeleteScene(const IGraphicsScene* scene) = 0;

	virtual eGraphicsResult		ReloadShaders() = 0;
	virtual eGraphicsResult		Resize(unsigned width, unsigned height) = 0;

	// user functions
	virtual eGraphicsResult		SetConfig(tGraphicsConfig config) = 0;
	
	virtual const char*			GetLastErrorMessage() = 0;
	virtual IGraphicsApi*		GetGraphicsApi() = 0;
};
