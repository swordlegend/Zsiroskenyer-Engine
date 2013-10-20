#include "testFiles.h"

#include "../../Common/src/IWindow.h"
#include "../../GraphicsEngine/src/IGraphicsEngine.h"
#include "../../GraphicsEngine/src/Camera.h"
#include "../../Common/src/Factory.h"
#include "../../Common/src/math/Quat.h"
#include "../../GraphicsEngineRaster/src/ManagerResource.h"
#include <vector>

int ricsiMain() {
	
	// Create interfaces
	IGraphicsEngine* mgrGEngine = Factory.CreateEngineGraphics();
	IManagerScene* mgrScene = mgrGEngine->GetManagerScene();
	cManagerResource* mgrRes = mgrGEngine->GetManagerResource();
	IGraphicsApi* gApi = mgrGEngine->GetGraphicsApi();

	// Create basic window
	IWindow::tDesc desc;
		desc.brush = IWindow::eBrush::RENDER_;
		desc.captionName = L"Zsíroskenyér Engine [Tech Demo]";
		desc.clientHeight = 600;
		desc.clientWidth = 800;
		desc.style = IWindow::eStyle::OVERLAPPED;
	IWindow* myWindow = Factory.CreateWindow(desc);

	// Set Render Window
	gApi->SetWindow(myWindow);

	// Create camera
	cCamera *cam = new cCamera(ZS_PIDIV2, (float)desc.clientWidth / desc.clientHeight, 0.01f, 5000.0f);
	mgrScene->SetActiveCamera(cam);

	// Create 3D object
	cEntity& e = mgrScene->AddEntity(L"objects/box.dae", L"material");
	e.position = Vec3(0,50,0);

	// Load texture, assign to object..
	//ITexture2D* testDiffuse = mgrRes->LoadTexture(L"textures/ground_DIFFUSE.jpg");
	//ITexture2D* testNormal = mgrRes->LoadTexture(L"textures/ground_NORMAL.jpg");
	//ITexture2D* testSpecular = mgrRes->LoadTexture(L"textures/ground_SPECULAR.jpg");
	//ITexture2D* testDisplacement = mgrRes->LoadTexture(L"textures/ground_DISPLACEMENT.jpg");

	/*
	for(size_t i = 0; i < 1 ; i++)
		for(size_t j = 0; j < 1 ; j++) {
			cEntity& e =  mgrScene->AddEntity(L"box.dae", L"material");
			entities.push_back(&e);
			e.position = Vec3(i * 10, 90, j * 10);
			e.isVisible = true;
		}
	*/
	
	// Main loop
	while(myWindow->IsOpened()) {
		myWindow->PeekAllMessages();

		mgrGEngine->RenderSceneForward();

		// Hardcoded rotation 	( euler Z for rotationg entities )
		static float zVal = 0.0f;
		zVal += 0.001f;
		//for(cEntity* e : entities) {
			e.rotation = Quat::EulerAnglesToQuat(Vec3(zVal, zVal + 0.4, zVal));
		//}

		// Present SwapChain
		gApi->Present();
	}
	
	// Free up
	mgrGEngine->Release();
	SAFE_DELETE(cam);
	return 0;
}