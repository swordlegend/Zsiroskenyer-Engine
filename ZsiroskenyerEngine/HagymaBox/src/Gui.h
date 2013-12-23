// Gui.h By Zsíroskenyér Team 2013.11.24 22:42
// Holding GuiControls
#pragma once

#include "../../Core/src/zsString.h"
#include <vector>

class IGraphicsScene;
class cGuiImage;
class cGraphicsEntity;

class cGui {
public:
	friend class cGuiSystem;

	// Create image on the gui with material etc
	cGuiImage* CreateImage(const zsString& mtlPath, size_t posX, size_t posY, size_t width, size_t height);
	IGraphicsScene* GetScene();

protected:
	// Internal function to create quad graphics entity with material etc
	cGraphicsEntity* CreateQuad(const zsString& mtlPath, size_t posX, size_t posY, size_t width, size_t height);

protected:
	cGui();
	cGui(IGraphicsScene* s, size_t width, size_t height);

protected:
	// Gui represented with cGraphicsScene
	IGraphicsScene* scene;

	// Width, height of gui
	size_t width, height;

	// Image gui controls
	std::vector<cGuiImage*> images;
};