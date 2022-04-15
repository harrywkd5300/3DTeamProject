#include "Default.hpp"

struct VS_IN
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vFrustumPos : TEXCOORD1;
};

struct PS_IN
{
	float2 vTexCoord : TEXCOORD0;
	float4 vFrustumPos : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : COLOR0;
};

struct VS_OUT_FRUSTUMPOS
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vFrustumPos : TEXCOORD1;
};

struct PS_IN_FRUSTUMPOS
{
	float2 vTexCoord : TEXCOORD0;
	float4 vFrustumPos : TEXCOORD1;
};

texture g_Src0Texture;
sampler2D Sampler_Src0 = sampler_state
{
	Texture = <g_Src0Texture>;
	//AddressU = Clamp;
	//AddressV = Clamp;
	//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src1Texture;
sampler2D Sampler_Src1 = sampler_state
{
	Texture = <g_Src1Texture>;
	//AddressU = Clamp;
	//AddressV = Clamp;
	//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src2Texture;
sampler2D Sampler_Src2 = sampler_state
{
	Texture = <g_Src2Texture>;
	//AddressU = Clamp;
	//AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src3Texture;
sampler2D Sampler_Src3 = sampler_state
{
	Texture = <g_Src3Texture>;
	//AddressU = Clamp;
	//AddressV = Clamp;
	//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src4Texture;
sampler2D Sampler_Src4 = sampler_state
{
	Texture = <g_Src4Texture>;
//AddressU = Clamp;
//AddressV = Clamp;
//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_NormalTexture;
sampler2D Sampler_Normal = sampler_state
{
	Texture = <g_NormalTexture>;
	//AddressU = Clamp;
	//AddressV = Clamp;
	//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D Sampler_LUT = sampler_state
{
	Texture = <g_Src1Texture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

// -------------------------------- Use Edge Detection.. -------------------------------- //
float4 g_vCamPos = 0.f;
float4 g_vRimLightData = float4( 255.f / 255.f, 213.f / 255.f, 242.f / 255.f, 0.2f );

float RimLightIntensity( float3 vCamPosWS, float3 vPositionWS, float3 vNormalWS, float fRimWidth )
{
	// Check LookAtCamDir to My_NormalDir..
	float3 vLookCamDir = normalize( vCamPosWS.xyz - vPositionWS.xyz );
	return smoothstep
	(
		1.f - fRimWidth,
		1.f,
		1.f - max( 0.f, dot( normalize( vNormalWS.xyz ), vLookCamDir.xyz ) )
	);
}

// -------------------------------- Use SSAO.. -------------------------------- //
// x = default occlusion value
// y = kernel sphere radius
// z = multiple of kernel radius to use for range truncation
// w = delta scale multiplier
float4 g_vSSAOData = float4( 0.5f, 0.5f, 3.0f, 200.0f );

texture randomTexture;		// 랜덤한 좌표???? 를 가져오기 위한 텍스쳐(Noise 노말맵임)..
sampler2D RandNormal = sampler_state
{
	Texture = randomTexture;
	ADDRESSU = WRAP;
	ADDRESSV = WRAP;
	MAGFILTER = LINEAR;
	MINFILTER = LINEAR;
};

const float3 g_avKernel[8] = {
	normalize( float3( 1, 1, 1 ) ) * 0.125f,
	normalize( float3( -1,-1,-1 ) ) * 0.250f,
	normalize( float3( -1,-1, 1 ) ) * 0.375f,
	normalize( float3( -1, 1,-1 ) ) * 0.500f,
	normalize( float3( -1, 1 ,1 ) ) * 0.625f,
	normalize( float3( 1,-1,-1 ) ) * 0.750f,
	normalize( float3( 1,-1, 1 ) ) * 0.875f,
	normalize( float3( 1, 1,-1 ) ) * 1.000f
};


// -------------------------------- Use Fog.. -------------------------------- //
float	g_fPriorityFogFactor = 0.8f;		// Priority 그룹을 그릴 때 안개가 얼마만큼의 비율로 들어가게 할 것 인가( 0 ~ 1 )..
											// lerp( OriginColor, FogColor, g_fPriorityFogFactor )

float	g_fLimitFogFactor = 1.f;			// max fogFactor, g_fLimitFogFactor 보다 비율이 커지지 않는다..
											// fogFactor = min( g_fLimitFogFactor, fogFactor )..

float3	g_vFogColor = float3( 1.f, 1.f, 1.f );			// Fog 색..

// x : Density - 포그의 농도..
// y : FogEnd - 포그의 끝.. (== CamFar???? )..
// z : Distance 에 곱할 값.. ( 4 < z == 안개가 짙어짐, 4 > z == 안개가 흐려짐 )..
// w : 아직 안씀..
float4	g_vFogData = float4( 1.f, 100.f, 4.f, 0.f );	// 여러 개의 자료를 하나의 변수에 모아서 ( 값 받기 편해서???? )..

float3 Compute_Fog( float fViewDepth, float3 vFrustumPos, float3 vOriginRGB )
{
	float3 vOut = 0.f;

	float3 vPosVS = Compute_PosVS( vFrustumPos, fViewDepth ).xyz;

	float fDistance = min( 1.f, vPosVS.z / g_vFogData.y ) * g_vFogData.z;

	float fFogFactor = 0.f;

	//if ( true == bTest )
	//{
	//	float fDistanceDivFogEnd = fDistance / g_vFogData.z;
	//	if ( fDistanceDivFogEnd > g_fFogLevel1 )
	//		fFogFactor = min( g_fLimitFogFactor, exp( -(fDistance * g_vFogData.x) ) );			
	//	else
	//		fFogFactor = min( g_fLimitFogFactor, exp( -(fDistance * g_vFogData.x) * (fDistance * g_vFogData.x) ) );
	//}
	//else
	//	fFogFactor = min( g_fLimitFogFactor, exp( -(fDistance * g_vFogData.x) ) );

	fFogFactor = min( g_fLimitFogFactor, exp( -(fDistance * g_vFogData.x) * (fDistance * g_vFogData.x) ) );
	
	vOut = lerp( g_vFogColor, vOriginRGB, fFogFactor );

	return vOut;
}

// -------------------------------- Use Color Grading.. -------------------------------- //
float3 g_vGradingColor = float3( 1.f, 1.f, 1.f );