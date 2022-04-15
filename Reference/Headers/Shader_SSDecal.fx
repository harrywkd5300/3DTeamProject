#include "Default.hpp"

struct VS_IN
{
	float4 vPosition : POSITION;
	float3 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float3 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct PS_IN
{
	float3 vTexCoord : TEXCOORD0;
	float4 vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : COLOR0;
};

float4x4 g_matWorld;
float4x4 g_matInvWorldView;

float	 g_SelectColor;

texture g_BaseTexture;
sampler Sampler_Base = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = ( VS_OUT )0.f;

	float4x4 matWV, matWVP;

	matWV = mul( g_matWorld, g_matView );
	matWVP = mul( matWV, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;
	Output.vProjPos = Output.vPosition;

	return Output;
}

PS_OUT ps_main( PS_IN Input )
{
	PS_OUT Output = ( PS_OUT )0.f;

	// z 나누기를 거친 좌표를 가지고 현재 위치를 UV 좌표로 옮기기..
	float2 vTexCoord_ScreenSpace = Input.vProjPos.xy / Input.vProjPos.w;
	vTexCoord_ScreenSpace.x = vTexCoord_ScreenSpace.x * 0.5f + 0.5f;
	vTexCoord_ScreenSpace.y = vTexCoord_ScreenSpace.y * -0.5f + 0.5f;


	// 구한 UV 좌표를 가지고 현재 위치(로컬 좌표) 를 구하자..
	float4 vDepth = tex2D( Sampler_Depth, vTexCoord_ScreenSpace );
	float4 vFrustumPos = Compute_Frustum( vTexCoord_ScreenSpace, g_fCamFar, g_matInvProj );
	float4 vPosition = Compute_PosVS( vFrustumPos, vDepth.r );
	vPosition = mul( vPosition, g_matInvWorldView );


	clip( 0.5f - abs( vPosition.xyz ) );	// 지형 등의 오브젝트와 닿는 면이 아니라면 픽셀 쉐이더에서 나가자..


	// 지형 등의 오브젝트와 닿는 면이라면 그리자..
	float2 vTexCoord = vPosition.xz + 0.5f;
	Output.vColor = tex2D( Sampler_Base, vTexCoord );

	if (Output.vColor.r >= 0.9f &&
		Output.vColor.g >= 0.9f &&
		Output.vColor.b >= 0.9f)
		Output.vColor.a = 0.f;

	Output.vColor.r += g_SelectColor;

	return Output;
}

technique
{
	pass Default
	{
		zenable = false;
		zwriteenable = false;

		alphablendenable = false;
		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_main();
	}


	pass Alpha_Testing
	{
		zenable = false;
		zwriteenable = false;

		alphatestenable = false;

		//alphatestenable = true;
		//alpharef = 0x30;
		//alphafunc = greater;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_main();
	}

}