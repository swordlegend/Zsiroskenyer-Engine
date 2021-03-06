////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Material.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	Stores the description of a multi-material.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/ITexture2D.h"
#include "../../Core/src/math/Vec4.h"
#include "ResourceReference.h"

class cMaterial {
public:
	cMaterial();
	cMaterial(size_t nSubMaterials);
	~cMaterial();
	
	void SetSize(size_t nSubMaterials);

	struct tSubMaterial {
		cTextureRef textureDiffuse;
		cTextureRef textureNormal;
		cTextureRef textureSpecular;
		cTextureRef textureDisplace;
		Vec4 diffuse;
		Vec4 specular;
		Vec4 emissive;
		float glossiness;
		tSubMaterial();
	};

	tSubMaterial& operator[](size_t idx);
	const tSubMaterial& operator[](size_t idx) const;
	size_t GetNSubMaterials() const;

private:
	tSubMaterial* subMaterials;
	size_t nSubMaterials;
};