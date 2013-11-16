// ps_4_0 output by Cg compiler
// cgc version 3.1.0013, build date Apr 18 2012
// command line args: -profile ps_5_0
// source file: shaders/test.cg
//vendor NVIDIA Corporation
//version 3.1.0.13
//profile ps_5_0
//program PS_MAIN
//semantic wvp : C0
//semantic world : C4
//semantic camPos : C8
//semantic diffuseTex : TEXUNIT0
//semantic normalTex : TEXUNIT1
//var sampler2D diffuseTex : TEXUNIT0 : _diffuseTex 0 : -1 : 1
//var sampler2D normalTex : TEXUNIT1 :  1 : -1 : 1
//var float4 In.posH : $vin.SV_POSITION :  : 0 : 0
//var float3 In.normalW : $vin.TEXCOORD0 :  : 0 : 0
//var float3 In.tangentW : $vin.TEXCOORD1 :  : 0 : 0
//var float2 In.tex0 : $vin.TEXCOORD2 :  : 0 : 1
//var float3 In.posW : $vin.TEXCOORD3 :  : 0 : 1
//var float3x3 In.TBN : $vin.TEXCOORD4 : , 3 : 0 : 1
//var float4 PS_MAIN.color : $vout.COLOR : COLOR : -1 : 1

#pragma pack_matrix(row_major)

struct VS_IN {
    float3 _posL : SV_Position;
    float3 _normalL : NORMAL;
    float3 _tangentL : TANGENT;
    float2 _tex0 : TEXCOORD0;
};

struct VS_OUT {
    float4 _posH : SV_POSITION;
    float3 _normalW1 : TEXCOORD0;
    float3 _tangentW : TEXCOORD1;
    float2 _tex01 : TEXCOORD2;
    float3 _posW : TEXCOORD3;
    float3x3 _TBN : TEXCOORD4;
};

struct PS_OUT {
    float4 _color : SV_Target;
};

struct X1X {
    float4 _SV_POSITION : SV_POSITION;
    float3 _TEXCOORD0 : TEXCOORD0;
    float3 _TEXCOORD1 : TEXCOORD1;
    float2 _TEXCOORD2 : TEXCOORD2;
    float3 _TEXCOORD3 : TEXCOORD3;
    float3 _TEXCOORD4 : TEXCOORD4;
    float3 _TEXCOORD5 : TEXCOORD5;
    float3 _TEXCOORD6 : TEXCOORD6;
};

Texture2D<float4> _TMP23 : register(t0);
SamplerState _diffuseTex : TEXUNIT0;

 // main procedure, the original name was PS_MAIN
PS_OUT main( in X1X cin)
{

    VS_OUT _In;
    PS_OUT _pout;

    _In._tex01 = cin._TEXCOORD2;
    _pout._color = _TMP23.Sample(_diffuseTex, _In._tex01);
    return _pout;
} // main end
