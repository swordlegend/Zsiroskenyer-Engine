////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Material.cpp
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "Material.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "Material.h"
#include <stdexcept>

#include "../../Core/src/common.h"

// constructor
cMaterial::cMaterial() 
:subMaterials(nullptr), nSubMaterials(0) {
}

cMaterial::cMaterial(size_t nSubMaterials)
:nSubMaterials(nSubMaterials) {
	subMaterials = new tSubMaterial[nSubMaterials];
}

cMaterial::~cMaterial() {
	SAFE_DELETE_ARRAY(subMaterials);
}

cMaterial::tSubMaterial::tSubMaterial()
:	diffuse(0,0,0,1),
	emissive(0,0,0,1),
	specular(0,0,0,1)
{
}

// bullshit
void cMaterial::SetSize(size_t nSubMaterials) {
	this->~cMaterial(); // ugly but i don't want to copy-paste
	new (this) cMaterial(nSubMaterials); // ugly too...
}

cMaterial::tSubMaterial& cMaterial::operator[](size_t idx) {
	if (idx >= nSubMaterials)
		throw std::out_of_range("no such material id");
	return subMaterials[idx];
}

const cMaterial::tSubMaterial& cMaterial::operator[](size_t idx) const {
	if (idx >= nSubMaterials)
		throw std::out_of_range("no such material id");
	return subMaterials[idx];
}

size_t cMaterial::GetNSubMaterials() const {
	return nSubMaterials;
}



