
#include <cassert>
#include "CreateGraphicsEngine.h"
#include "../../Core/src/DynamicLibrary.h"


static DllHandle hDllRaster = 0;
IGraphicsEngine*(*CreateGraphicsEngineRaster)(IWindow*, unsigned, unsigned, tGraphicsConfig, const char** errorMessage);

IGraphicsEngine* CreateGraphicsEngine(IWindow* targetWindow, unsigned screenWidth, unsigned screenHeight, tGraphicsConfig config, const char** errorMessage) {
	if (hDllRaster) {
		if (CreateGraphicsEngineRaster)
			return CreateGraphicsEngineRaster(targetWindow, screenWidth, screenHeight, config, errorMessage);
		else
			return 0;
	}
	else {
		hDllRaster = LoadDynamicLibrary("GraphicsEngineRaster");
		if (!hDllRaster) {
			return nullptr;
		}
		CreateGraphicsEngineRaster = (IGraphicsEngine*(*)(IWindow*, unsigned, unsigned, tGraphicsConfig, const char**))GetFunctionAddress(hDllRaster, "CreateGraphicsEngineRaster");
		if (!CreateGraphicsEngineRaster) {
			assert(false);
			return nullptr;
		}
		return CreateGraphicsEngineRaster(targetWindow, screenWidth, screenHeight, config, errorMessage);
	}
}