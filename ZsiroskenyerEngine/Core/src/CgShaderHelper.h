// CgShaderHelper.h by Zsíroskenyér Team 2014.01.21 11:07
// This helper class is for compiling, parsing nvidia cg shaders
// and parsing cgc.exe generated HLSL, GLSL files

#include "Cg/cg.h"

#include "PipelineState.h"
#include "zsString.h"
#include <unordered_map>
#include <list>

#include "VertexFormat.h"

// @TODO cgShaderHelper only supports 1 pass 1 tech per effect per cg file
class cCgShaderHelper {
public:
	// Compiling profiles for cg
	enum class eProfileCG {
		VS_5_0,
		HS_5_0,
		DS_5_0,
		GS_5_0,
		PS_5_0,
		VS_4_0,
		HS_4_0,
		DS_4_0,
		GS_4_0,
		PS_4_0,
		VS_3_0,
		PS_3_0,
		VS_2_0,
		PS_2_0,
	};

	struct tCgInfo {
		bool vsExists;
		bool hsExists;
		bool dsExists;
		bool gsExists;
		bool psExists;

		zsString vsEntryName;
		zsString hsEntryName;
		zsString dsEntryName;
		zsString gsEntryName;
		zsString psEntryName;

		tCgInfo() :vsExists(false), hsExists(false), dsExists(false), gsExists(false), psExists(false){}
	};

	cCgShaderHelper(const zsString& shaderPath);

	bool CompileCg(const zsString& cgFilePath, const zsString& shaderOut, cCgShaderHelper::eProfileCG compileProfile);

	std::unordered_map<zsString, uint16_t> GetHLSLTextureSlots(const zsString& hlslFilePath);

	std::unordered_map<zsString, tSamplerDesc> GetSamplerStates();
	cVertexFormat GetVSInputFormat();
	const cCgShaderHelper::tCgInfo& GetDomainInfo();

	const wchar_t* cCgShaderHelper::GetLastErrorMsg();
protected:
	// Error handling
	zsString lastErrorMsg;

	// Cg context for effect creation, effect parts
	CGcontext con;
	CGeffect effect; // .cg
	CGtechnique tech; // technique in effect
	CGpass pass; // pass in technique
	CGprogram vs, hs, ds, gs, ps; // shader programs in pass

	std::list<zsString> cgFileLines;
	cCgShaderHelper::tCgInfo info;
};