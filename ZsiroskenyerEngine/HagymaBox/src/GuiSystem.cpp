#include "GuiSystem.h"

#include "Gui.h"
#include "GuiImage.h"

#include "../../Core/src/common.h"

cGuiSystem::cGuiSystem() 
{}

cGuiSystem::~cGuiSystem() {
	for (auto i = images.begin(); i != images.end(); i++)
		SAFE_DELETE(*i);

	for (auto i = guis.begin(); i != guis.end(); i++)
		SAFE_DELETE(*i);
}

cGui* cGuiSystem::CreateGui(IGraphicsScene* s, size_t width, size_t height) {
	cGui* g = new cGui(s, width, height);
	guis.push_back(g);
	return g;
}