////////////////////////////////////////////////////////////////////////////////
//	GraphicsD3D11/src/Shader.h
//	2013.oct.14, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	IShaderProgram implementation
#pragma once

#include "../../Core/src/IShaderProgram.h"	
#include "../../Core/src/common.h"
#include <unordered_map>

struct ID3D11InputLayout;
struct ID3D11VertexShader;
struct ID3D11HullShader;
struct ID3D11DomainShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
class cGraphicsApiD3D11;

class cShaderProgramD3D11 : public IShaderProgram {
	friend class cGraphicsApiD3D11;
public:

	struct tSamplerInfo {
		size_t gApiSamplerIdx;
		size_t slotIdx;

		tSamplerInfo(size_t gApiSamplerIdx, size_t slotIdx) :gApiSamplerIdx(gApiSamplerIdx), slotIdx(slotIdx){}
	};

	cShaderProgramD3D11(cGraphicsApiD3D11* parent,
		size_t vertexFormatSize, 
		ID3D11InputLayout* inputLayout = NULL,
		ID3D11VertexShader* vs = NULL, ID3D11HullShader* hs = NULL,
		ID3D11DomainShader* ds = NULL, ID3D11GeometryShader* gs = NULL, 
		ID3D11PixelShader* ps = NULL);
	~cShaderProgramD3D11();
	void Release() override;

	void SetTextureSlotsVS(const std::unordered_map<zsString, size_t>& texSlotsVS);
	void SetTextureSlotsPS(const std::unordered_map<zsString, size_t>& texSlotsPS);

	void SetSamplerStatesVS(const std::vector<tSamplerInfo>& samplerStatesVS);
	void SetSamplerStatesPS(const std::vector<tSamplerInfo>& samplerStatesPS);

	int GetTextureSlotVS(const zsString& varName) const;
	int GetTextureSlotPS(const zsString& varName) const;

	const std::vector<tSamplerInfo>& GetSamplerStatesVS() const;
	const std::vector<tSamplerInfo>& GetSamplerStatesPS() const;

	const ID3D11VertexShader* GetVertexShader() const;
	const ID3D11HullShader* GetHullShader() const;
	const ID3D11DomainShader* GetDomainShader() const;
	const ID3D11GeometryShader* GetGeometryShader() const;
	const ID3D11PixelShader* GetPixelShader() const;
	const ID3D11InputLayout* GetInputLayout() const;

	size_t GetVertexFormatSize() const override;
	
private:
	// parent
	cGraphicsApiD3D11* parent;

	// input layout
	ID3D11InputLayout* inputLayout;
	size_t vertexFormatSize;

	// In Sequence of the Dx pipeline :) 
	ID3D11VertexShader*		vs;
	ID3D11HullShader*		hs;
	ID3D11DomainShader*		ds;
	ID3D11GeometryShader*	gs;
	ID3D11PixelShader*		ps;

	// Texture slot look up
	// Texture name, slot idx
	std::unordered_map<zsString, size_t> textureSlotsVS;
	std::unordered_map<zsString, size_t> textureSlotsPS;

	// Sampler slot look up
	// Sampler hash, vector<slot idx>
	std::vector<tSamplerInfo> samplerStatesVS;
	std::vector<tSamplerInfo> samplerStatesPS;
};