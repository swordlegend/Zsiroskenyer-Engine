////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/RsourceManager.cpp
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "ResourceManager.h" for more information.
////////////////////////////////////////////////////////////////////////////////
#include "ManagerResource.h"

// Geometry building
#include "GeometryBuilder.h"

// Graphics api
#include "../../GraphicsApi/src/IGraphicsApi.h"
#include "../../Common/src/IFile.h"

////////////////////////////////////////////////////////////////////////////////
//	ResourceManager

// load/unload geometries
cGeometryRef cManagerResource::LoadGeometry(const zsString& fileName) {
	cGeometry* geom;

	// lookup if already exists
	auto it = geometries.left.find(fileName);
	if (it==geometries.left.end()) {

		// Create geometry based on file extension
		zsString fileExtension = fileName.substr(fileName.length() - 3, 3);
		if(fileExtension == L"dae") {
			cGeometryBuilder b;
			cGeometryBuilder::tGeometryDesc& d = b.LoadGeometryDAE(fileName);

			IVertexBuffer *VB = gApi->CreateBufferVertex(d.nVertices, d.vertexStride, eBufferUsage::IMMUTABLE, d.vertices);
			IIndexBuffer *IB = gApi->CreateBufferIndex(d.nIndices * d.indexStride, eBufferUsage::IMMUTABLE, d.indices);
			geom = new cGeometry(VB, IB);
		}
		
		// insert into database
		geometries.insert(GeometryMapT::value_type(fileName, geom));
	}
	else {
		geom = it->second;
	}

	return cGeometryRef(this, geom);
}
void cManagerResource::UnloadGeometry(const cGeometry* geometry) {
	auto it = geometries.right.find(const_cast<cGeometry*>(geometry));
	delete it->first;
	geometries.right.erase(it);
}

// load/unload materials
cMaterialRef cManagerResource::LoadMaterial(const zsString& fileName) {
	cMaterial* mtl;

	// lookup if already exists
	auto it = materials.left.find(fileName);
	if (it == materials.left.end()) {
		// Open material file
		IFile* file = IFile::Create(fileName);

		// Number of subMaterials
		size_t nSubMaterials = file->GetNLines() / 8;

		// create material with nSubMaterials subMaterials
		mtl = new cMaterial(nSubMaterials);
		
		for(size_t i = 0; i < nSubMaterials; i++) {
			// subMaterial ID, not used yet
			const zsString& idLine = file->GetLine();

			// Diffuse
			auto floats = file->GetLine().GetFloats(7);
			(*mtl)[i].diffuse = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Specular
			floats = file->GetLine().GetFloats(8);
			(*mtl)[i].specular = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Emissive
			floats = file->GetLine().GetFloats(8);
			(*mtl)[i].emissive = Vec4(floats[0], floats[1], floats[2], floats[3]);

			// Glossiness
			floats = file->GetLine().GetFloats(10);
			(*mtl)[i].glossiness = floats[0];

			// Texture Diffuse
			(*mtl)[i].textureDiffuse = gApi->CreateTexture(file->GetLine().c_str() + 9);

			// Texture Normal
			(*mtl)[i].textureNormal = gApi->CreateTexture(file->GetLine().c_str() + 8);

			// Texture Specular
			(*mtl)[i].textureSpecular = gApi->CreateTexture(file->GetLine().c_str() + 10);

			// Texture Displace
			(*mtl)[i].textureDisplace = gApi->CreateTexture(file->GetLine().c_str() + 10);
		}

		// insert into database
		materials.insert(MaterialMapT::value_type(fileName, mtl));
	}
	else {
		mtl = it->second;
	}
	
	return cMaterialRef(this, mtl);
}

void cManagerResource::UnloadMaterial(const cMaterial* material) {
	auto it = materials.right.find(const_cast<cMaterial*>(material));
	delete it->first;
	materials.right.erase(it);
}


// constructors
cManagerResource::cManagerResource(IGraphicsApi* gApi) : gApi(gApi) {
}
cManagerResource::~cManagerResource() {
}


////////////////////////////////////////////////////////////////////////////////
//	References to resources

// geometry reference
cGeometryRef::cGeometryRef(cManagerResource* rm, cGeometry* ptr)
	:
	shared_ptr(ptr, [this](cGeometry* g){this->rm->UnloadGeometry(g);} ), 
	rm(rm)
{
}
cGeometryRef::cGeometryRef(const cGeometryRef& other) 
	:
	shared_ptr(other),
	rm(other.rm)
{
}
cGeometryRef::cGeometryRef()
	:
	shared_ptr(nullptr),
	rm(nullptr)
{
}

cGeometryRef& cGeometryRef::operator=(const cGeometryRef& other) {
	shared_ptr<cGeometry>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cGeometryRef::operator==(const cGeometryRef& other) {
	return std::operator==(*this, other);
}

cGeometry* cGeometryRef::get() const {
	return shared_ptr<cGeometry>::get();
}



// material reference
cMaterialRef::cMaterialRef(cManagerResource* rm, cMaterial* ptr)
	:
	shared_ptr(ptr, [this](cMaterial* m){this->rm->UnloadMaterial(m);} ), 
	rm(rm)
{
}
cMaterialRef::cMaterialRef(const cMaterialRef& other) 
	:
	shared_ptr(other),
	rm(other.rm)
{
}
cMaterialRef::cMaterialRef()
	:
	shared_ptr(nullptr),
	rm(nullptr)
{
}

cMaterialRef& cMaterialRef::operator=(const cMaterialRef& other) {
	shared_ptr<cMaterial>::operator=(other);
	rm = other.rm;
	return *this;
}

bool cMaterialRef::operator==(const cMaterialRef& other) {
	return std::operator==(*this, other);
}

cMaterial* cMaterialRef::get() const {
	return shared_ptr<cMaterial>::get();
}