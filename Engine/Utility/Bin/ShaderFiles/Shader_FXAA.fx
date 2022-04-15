//#ifndef __FXAA_FILTER_FX__
//#define __FXAA_FILTER_FX__

#ifndef BUFFER_RCP_WIDTH
#define BUFFER_RCP_WIDTH 1920.f
#endif

#ifndef BUFFER_RCP_HEIGHT
#define BUFFER_RCP_HEIGHT 1080.f
#endif

#include "injFXaaSettings.h"

#define FXAA_PC 1
#define FXAA_HLSL_3 1

#include "FXAA3_11.h"

//#endif
float4x4 g_matView;
float4x4 g_matProj;

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

texture g_ScreenTexture;
sampler Sampler_Screen = sampler_state
{
	texture = g_ScreenTexture;
};

texture g_LumaTexture;
sampler lumaSampler = sampler_state
{
	texture = g_LumaTexture;
	AddressU = BORDER;
	AddressV = BORDER;
	SRGBTexture = FALSE;
};

#include "Post.h"

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = (VS_OUT)0.f;

	Output.vPosition = mul( Input.vPosition, mul( g_matView, g_matProj ) );
	Output.vTexCoord = Input.vTexCoord;

	return Output;
}

PS_OUT ps_fxaaFilter(PS_IN Input)
{
	PS_OUT Output = (PS_OUT)0.f;

	float4 c0 = FxaaPixelShader(
		// pos, Output color texture
		Input.vTexCoord,
		// tex, Input color texture
		Sampler_Screen,
		// fxaaQualityRcpFrame, gets coordinates for screen width and height, xy
		float2(BUFFER_RCP_WIDTH, BUFFER_RCP_HEIGHT),
		//fxaaConsoleRcpFrameOpt2, gets coordinates for screen width and height, xyzw
		float4(-2.0*BUFFER_RCP_WIDTH,-2.0*BUFFER_RCP_HEIGHT,2.0*BUFFER_RCP_WIDTH,2.0*BUFFER_RCP_HEIGHT),
		// Choose the amount of sub-pixel aliasing removal
		fxaaQualitySubpix,
		// The minimum amount of local contrast required to apply algorithm
		fxaaQualityEdgeThreshold,
		// Trims the algorithm from processing darks
		fxaaQualityEdgeThresholdMin
	);

	Output.vColor = c0;

	return Output;
}

technique Default
{
	pass
	{
		zwriteenable = false;
		alphatestenable = true;
		alpharef = 0x00;
		alphafunc = greater;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_fxaaFilter();
	}
};