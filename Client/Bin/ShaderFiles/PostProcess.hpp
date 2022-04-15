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

//float g_fMaxVelocity = 0.0005f;	// Max �ӵ�..
float g_fMaxVelocity = 5.f;	// Max �ӵ�..

// -------------------------- Use DOF.. -------------------------- //
float g_fFocusZ = 20.f;		// ī�޶� ����..
float g_fRange = 30.f;		// ī�޶� �������κ��� �󸶱��� ������� �ʰ� �� ���ΰ�???..
// x, y		:	ī�޶� ����(Screen Space)..
// z		:	ī�޶� ����(����Z)..
// w		:	ī�޶� ����(����Z) �������� �󸶸�ŭ �帮�� �ʰ� �� ���ΰ�(Range)..
float4 g_vFocusData = float4( 0.5f, 0.5f, 10000.f, 0.f );
float g_fDOFScale = 8.f;	// DOF �� �󸶳� ���� ���ΰ�??

// -------------------------- Use LightShaft.. -------------------------- //
int g_iNumSamples = 100;		// ���� ���� Ƚ��..

// x,y == Sun Screen Pos	->	�¾��� ��ǥ..
// z,w == Sun Screen Dir	->	�¾��� ����..
float4 g_vSunData = float4( 0.f, 0.f, 0.f, 0.f );

// x == Density		->	�е�???( �� ���� �������� ���÷� ���� ���� ������ �������� )..
// y == Decay		->	0 ~ 1 �� �������� �� ������ ���⵵�� ������ �������κ��� �����ϸ鼭 �л�..
// z == Weight		->	�� ���ÿ� ���� ����..
// w == Exposure	->	��ü���� ����..
float4 g_vLightShaftValue = float4( 1.f, 1.f, 1.f, 1.f );

// x,y == Tex Size			->	�ؽ��� ���μ��� ������..
// z == IlluminationDecay	->	���� ���� ��ġ???( ������ ���鼭 g_vVolumeLightValue.y �� ���������� �ϰ� �ȴ� )..
// w == �Ⱦ�					->	�Ⱦ�..
float4 g_vLightShaftValue2 = float4( 1920.f / 4.f, 1080.f / 4.f, 1.f, 1.f );