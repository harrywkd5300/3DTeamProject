#pragma once
#include "Default.hpp"

#ifndef USEVELOCITYTARGET
//#define USEVELOCITYTARGET
#endif

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

struct VS_OUT_BLUR
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
	float4 vFrustumPos : TEXCOORD1;
};

struct PS_IN_BLUR
{
	float2 vTexCoord : TEXCOORD0;
	float4 vFrustumPos : TEXCOORD1;
};

struct VS_OUT_TEX4
{
	float4 vPosition : POSITION;
	float2 vTexCoord0 : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
};

struct PS_IN_TEX4
{
	float2 vTexCoord0 : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
};

texture g_Src0Texture;
sampler2D Sampler_Src0 = sampler_state
{
	Texture = <g_Src0Texture>;
	AddressU = Clamp;
	AddressV = Clamp;
	//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src1Texture;
sampler2D Sampler_Src1 = sampler_state
{
	Texture = <g_Src1Texture>;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_FrameBufferTexture;
sampler Sampler_FrameBuffer = sampler_state 
{
	texture = g_FrameBufferTexture;
	AddressU = Clamp;
	AddressV = Clamp;
	//MinFilter = Point;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_NormalTexture;
sampler Sampler_Normal = sampler_state 
{
	texture = g_NormalTexture; 
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_CrushTexture;
sampler Sampler_Crush = sampler_state
{
	texture = g_CrushTexture;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D Sampler_Point0 = sampler_state
{
	Texture = <g_FrameBufferTexture>;
	MinFilter = point;
	MagFilter = point;
	MipFilter = point;
	MaxAnisotropy = 1;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// -------------------------- Use Motion Blur.. -------------------------- //
float4x4 g_matPrevView;		// View Matrix before 1 Frame..
float4x4 g_matCamView;		// 

//float g_fMaxVelocity = 0.0005f;	// Max 속도..
float g_fMaxVelocity = 5.f;	// Max 속도..

// -------------------------- Use DOF.. -------------------------- //
float g_fFocusZ = 20.f;		// 카메라 초점..
float g_fRange = 30.f;		// 카메라 초점으로부터 얼마까지 흐려지지 않게 할 것인가???..
// x, y		:	카메라 초점(Screen Space)..
// z		:	카메라 초점(깊이Z)..
// w		:	카메라 초점(깊이Z) 에서부터 얼마만큼 흐리지 않게 할 것인가(Range)..
float4 g_vFocusData = float4( 0.5f, 0.5f, 10000.f, 0.f );
float g_fDOFScale = 8.f;	// DOF 를 얼마나 먹일 것인가??

// -------------------------- Use LightShaft.. -------------------------- //
int g_iNumSamples = 100;		// 루프 도는 횟수..

// x,y == Sun Screen Pos	->	태양의 좌표..
// z,w == Sun Screen Dir	->	태양의 방향..
float4 g_vSunData = float4( 0.f, 0.f, 0.f, 0.f );

// x == Density		->	밀도???( 이 값이 작을수록 샘플로 갖고 오는 영역이 좁아진다 )..
// y == Decay		->	0 ~ 1 의 범위에서 각 샘플의 영향도를 광선이 광원으로부터 진행하면서 분산..
// z == Weight		->	각 샘플에 대한 강도..
// w == Exposure	->	전체적인 강도..
float4 g_vLightShaftValue = float4( 1.f, 1.f, 1.f, 1.f );

// x,y == Tex Size			->	텍스쳐 가로세로 사이즈..
// z == IlluminationDecay	->	빛의 증감 수치???( 루프를 돌면서 g_vVolumeLightValue.y 와 곱셈연산을 하게 된다 )..
// w == 안씀					->	안씀..
float4 g_vLightShaftValue2 = float4( 1920.f / 4.f, 1080.f / 4.f, 1.f, 1.f );