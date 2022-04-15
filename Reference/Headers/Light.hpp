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
	float4 vShade : COLOR0;
	float4 vSpecular : COLOR1;
};

float4 g_vCamPosition = 0.f;
float4 g_vCamDir = 0.f;

float3 g_vLightPower = float3( 1.f, 1.f, 1.f );

float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

float4 g_vLightDir;
float4 g_vLightPos;

float g_fRange = 0.f;
float g_fTheta = 0.f;
float g_fPhi = 0.f;

float4 g_vMtrlDiffuse = float4( 1.f, 1.f, 1.f, 1.f );
float4 g_vMtrlAmbient = float4( 1.f, 1.f, 1.f, 1.f );

float4 g_vMtrlSpecular = float4( 1.f, 1.f, 1.f, 1.f );
float g_vMtrlPower = 255.f;

float g_fSmoothstep = 0.2f;

//float4x4 mWLP;
//float4x4 g_matLightView;
//float4x4 g_matLightProj;

texture g_NormalTexture;
sampler Sampler_Normal = sampler_state
{
	texture = g_NormalTexture;
	//AddressU = Clamp;
	//AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_ColorTexture;
sampler Sampler_Color = sampler_state
{
	texture = g_ColorTexture;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_SpecularIntensityTexture;
sampler Sampler_SpecularIntensity = sampler_state 
{
	texture = g_SpecularIntensityTexture;
	//AddressU = Clamp;
	//AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	//MipFilter = Linear;
	MipFilter = Linear;
};

texture g_SSAOTexture;
sampler Sampler_SSAO = sampler_state 
{
	texture = g_SSAOTexture;
	//AddressU = Clamp;
	//AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	//MipFilter = Linear;
	MipFilter = NONE;
};

float Compute_Shade( float3 vLightDirection, float3 vNormal )
{
	return max( 0.f, dot( -normalize( vLightDirection ), normalize( vNormal ) ) );
}

float Compute_DistanceAttenuation( float fRange, float fDist )
{
	return 1.f - smoothstep( fRange * g_fSmoothstep, fRange, fDist );
}

float Compute_SpecularIntensity( float3 vLight_Direction, float3 vNormal, float3 vWorldPos, float3 vCameraPos, float fMtrl_Power )
{
	float fOut = 0.f;

	vLight_Direction.xyz = normalize( vLight_Direction.xyz );
	vNormal.xyz = normalize( vNormal.xyz );

	float3 vReflect = normalize( reflect( vLight_Direction.xyz, vNormal.xyz ) );
	float3 vLook = normalize( vWorldPos.xyz - vCameraPos.xyz );
	float3 vCamDir = normalize( g_vCamDir.xyz );
	float fPasing = dot( vLook, vCamDir );
	//vLook = normalize( ( vLook + vCamDir ) * 0.5f );

	fOut = pow( max( 0.f, dot( vReflect, -vLook ) ), fMtrl_Power ) * fPasing;

	return fOut;
}
