#include "GeometryBuilder.h"

// DAE loading
#include "ASSIMP/include/assimp/Importer.hpp"
#include "ASSIMP/include/assimp/Scene.h"
#include "ASSIMP/include/assimp/PostProcess.h"
#include "tipsify.h"

// Common and Math
#include "common.h"
#include "StrUtil.h"
#include "FileUtil.h"
#include "math/vec3.h"
#include "math/vec2.h"

// Exception
#include "Exception.h"

cGeometryBuilder::cGeometryBuilder() {
}

cGeometryBuilder::tGeometryDesc cGeometryBuilder::LoadGeometry(const zsString& filePath) {
	Assimp::Importer importer;

	// Check file existence
	if (!cFileUtil::isFileExits(filePath)) {
		ILog::GetInstance()->MsgBox(L"Can't load geometry file: " + filePath);
		throw FileNotFoundException();
	}

	// parse 3D geom file
	char ansiFilePath[256];
	cStrUtil::ToAnsi(filePath, ansiFilePath, 256);
	const aiScene* scene = importer.ReadFile(ansiFilePath, aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_ImproveCacheLocality | aiProcess_OptimizeGraph | aiProcess_OptimizeMeshes | aiProcess_FlipWindingOrder);

	// Scene meshes
	aiMesh** meshes = scene->mMeshes;
	size_t nMeshes = scene->mNumMeshes;


	// Each mesh have vertices, indices
	size_t nVertices = 0;
	size_t nIndex = 0;

	// Assimp library breakes material groups into meshes
	std::vector<tGeometryDesc::tMatGroup> matGroups;

	// Gather those from meshes
	for (size_t i = 0; i < nMeshes; i++) {
		aiMesh* mesh = meshes[i];

		size_t nMeshIndices = mesh->mNumFaces * 3;

		// Mat group
		tGeometryDesc::tMatGroup g;
		g.id = mesh->mMaterialIndex;
		g.indexOffset = nIndex;
		g.indexCount = nMeshIndices;
		matGroups.push_back(g);

		// vertex, index
		nVertices += mesh->mNumVertices;
		nIndex += nMeshIndices;
	}

	// Define Vertex Format ...
	struct baseVertex {
		Vec3 pos;
		Vec3 normal;
		Vec3 tangent;
		Vec2 tex;
		bool operator == (const baseVertex& v) { return pos == v.pos && normal == v.normal && tangent == v.tangent && tex == v.tex; }
	};

	std::vector<cVertexFormat::Attribute> attribs;
	cVertexFormat::Attribute a;
	// POSITION
		a.bitsPerComponent = cVertexFormat::_32_BIT;
		a.nComponents = 3;
		a.semantic = cVertexFormat::POSITION;
		a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// NORMAL
		a.bitsPerComponent = cVertexFormat::_32_BIT;
		a.nComponents = 3;
		a.semantic = cVertexFormat::NORMAL;
		a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// TANGENT
		a.bitsPerComponent = cVertexFormat::_32_BIT;
		a.nComponents = 3;
		a.semantic = cVertexFormat::COLOR;
		a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// TEX0
		a.bitsPerComponent = cVertexFormat::_32_BIT;
		a.nComponents = 2;
		a.semantic = cVertexFormat::TEXCOORD;
		a.type = cVertexFormat::FLOAT;
	attribs.push_back(a);

	// The vertex format !!!
	cVertexFormat vertexFormat(attribs);

	// Geometry read up
	baseVertex *vertices= new baseVertex[nVertices];
	unsigned *indices = new unsigned[nIndex];

	// Super TMP Vec3 for usage
	aiVector3D* supTmpVec;

	size_t indexI = 0;
	size_t vertexI = 0;
	size_t vertexOffset = 0;

	for(size_t i = 0; i < nMeshes;i++) {
		aiMesh* mesh = meshes[i];
		for (size_t j = 0; j < mesh->mNumFaces; indexI += 3, j++) {
			aiFace& face = mesh->mFaces[j];
			
			// For each face index
			for (size_t k = 0; k < 3; k++) {
				if (face.mNumIndices < 3)
					break;

				unsigned vertIdx = face.mIndices[k];
				// Index data
				indices[indexI + k] = vertIdx + vertexI;

				// Vertex Data
				if (mesh->HasPositions()) {
					supTmpVec = &mesh->mVertices[vertIdx];
					vertices[vertIdx + vertexI].pos = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				if (mesh->HasNormals()) {
					supTmpVec = &mesh->mNormals[vertIdx];
					vertices[vertIdx + vertexI].normal = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				if (mesh->HasTangentsAndBitangents()) {
					supTmpVec = &mesh->mTangents[vertIdx];
					vertices[vertIdx + vertexI].tangent = Vec3(supTmpVec->x, supTmpVec->y, supTmpVec->z);
				}

				// @TODO not general algorithm, wee need to handle more UV channels
				supTmpVec = &mesh->mTextureCoords[0][vertIdx];
				vertices[vertIdx + vertexI].tex = Vec2(supTmpVec->x, -supTmpVec->y); // Fucking hate Assimp
			}
		}

		vertexI += mesh->mNumVertices;
	}

	// TODO temporary DISABLING, because it is slow
	// Finally Index reordering for optimal post vertex cache
	//size_t* reorderedIndices = new size_t[nIndex];
	//reorderedIndices = tipsify(indices, nIndex / 3, nVertices, 16); // For vertexCache size 16 . I think it's ideal
	//SAFE_DELETE_ARRAY(indices);

	// Return geometric description about loaded DAE
	tGeometryDesc geomDesc;
		geomDesc.vertices = vertices;
		geomDesc.nVertices = nVertices;
		geomDesc.vertexFormat = vertexFormat;

		//geomDesc.indices = reorderedIndices;
		geomDesc.indices = indices;
		geomDesc.nIndices = nIndex;
		geomDesc.indexStride = sizeof(unsigned);
		geomDesc.matGroups = matGroups;
	return geomDesc;
}