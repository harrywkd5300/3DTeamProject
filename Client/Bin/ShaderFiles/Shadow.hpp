#include "Default.hpp"

struct VS_IN
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUTSHADOW
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vDepth : TEXCOORD1;
};

struct PS_INSHADOW
{
	float2 vTexCoord : TEXCOORD0;
	float4 vDepth : TEXCOORD1;
};

struct PS_OUT
{
	float4 vColor : COLOR0;
};

struct VS_OUTPUT_QUAD
{
	float4 vPosition : POSITION;
	float2 vTexCoord0 : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
	float2 vTexCoord4 : TEXCOORD4;
	float2 vTexCoord5 : TEXCOORD5;
	float2 vTexCoord6 : TEXCOORD6;
	float2 vTexCoord7 : TEXCOORD7;
};

float4x4 g_matWorld;
float4x4 g_matLightVP;

float3	g_vCamPos = float3( 0.f, 0.f, 0.f );

float MAP_SIZE = 2048.f;

texture g_SrcTexture;
sampler Sampler_Src = sampler_state
{
	texture = g_SrcTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

float2 ComputeMoments( float _fDepth, float2 _uv )
{
	float2 Moments;

	Moments.x = _fDepth;
	//Moments.y = _fDepth * _fDepth;



	//깊이의 미분계수를 계산한다.
	float dx = ddx(_fDepth);
	float dy = ddy(_fDepth); 
	Moments.y = _fDepth * _fDepth + 0.25f * (dx*dx + dy*dy);


	return Moments;
}