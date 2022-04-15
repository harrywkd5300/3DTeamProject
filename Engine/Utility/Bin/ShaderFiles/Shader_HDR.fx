#include "HDR.hpp"

struct VS_IN
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct PS_IN
{
	float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : COLOR0;
};

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = (VS_OUT)0.f;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_hdr
// Desc: HDR ผ๖วเ..
//-----------------------------------------------------------------------------
PS_OUT ps_hdr( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	return Output;
}

technique HDR
{
	pass HDRProgress
	{
		ZWriteEnable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_hdr();
	}
};