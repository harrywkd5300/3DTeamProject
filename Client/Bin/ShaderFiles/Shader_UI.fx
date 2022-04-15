
// 전역변수는 상수화되어있다.
// 전역변수는 클라이언트로부터 값을 받아올 수 있다.
// float				g_fData = 10.f;

// float4x4
#include "shader_Base.hpp"

texture				g_UITexture;
texture				g_UIFixTexture;
texture				g_GradTexture;

float				g_fUTimeDelta = 0.f;
float				g_fVTimeDelta = 0.f;
float				g_fAlpha = 1.f;
float				g_fUAlpha = 1.f;

vector				g_vColor;

sampler UISampler = sampler_state
{
	texture = g_UITexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;	
};

sampler UIFixSampler = sampler_state
{
	texture = g_UIFixTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	AddressU = BORDER;
	AddressV = BORDER;
};

sampler GradSampler = sampler_state
{
	texture = g_GradTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;	
};


// VertexShader
// 리턴 함수이름(인자값) { }

/*정점inLocalSpace*/
struct VS_IN
{
	// 위치
	float3		vPosition : POSITION;

	// 텍스쳐 uv좌표
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

// VS_MAIN(vector		vPosition : POSITION, float2		vTexUV : TEXCOORD0)

// 버텍스 셰이더를 수행하기위한 메인함수.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matW,matWV, matWVP;
	
	matW = g_matWorld;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	
	if(Out.vPosition.z <= 0 || Out.vPosition.z >= Out.vPosition.w)
	Out.vPosition.z = Out.vPosition.w * 0.5f;

	Out.vTexUV = In.vTexUV;		
	
	return Out;
}
struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = tex2D(UISampler, In.vTexUV);

	vColor.a *= g_fAlpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_ANIMAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vUVDelta = In.vTexUV;

	vUVDelta.x += g_fUTimeDelta;
	vUVDelta.y += g_fVTimeDelta;

	vector vColor = tex2D(UISampler, vUVDelta);
		
	vColor.a *= g_fAlpha;		
	
	if (In.vTexUV.x > g_fUAlpha)
		vColor.a = 0.f;

	Out.vColor = vColor;
	
	return Out;
}

PS_OUT PS_GRADMAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vUVDelta = In.vTexUV;
	vUVDelta.x += g_fUTimeDelta;
	vUVDelta.y += g_fVTimeDelta;

	vector	vColor[3];
	vColor[0] = tex2D(UIFixSampler, vUVDelta);
	vColor[1] = tex2D(GradSampler, In.vTexUV);
		
	vColor[2] = vColor[1] * g_vColor ;
	vColor[0].a *= g_fAlpha;
	vColor[0] *= vColor[2];
	
	if (In.vTexUV.x > g_fUAlpha)
		vColor[0].a = 0.f;

	Out.vColor = vColor[0];

	return Out;
}

PS_OUT PS_FIXMAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vUVDelta = In.vTexUV;

	vUVDelta.x += g_fUTimeDelta;
	vUVDelta.y += g_fVTimeDelta;

	vector vColor = tex2D(UIFixSampler, vUVDelta);

	vColor.a *= g_fAlpha;

	Out.vColor = vColor;

	return Out;
}

PS_OUT PS_COLORMAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vColor = tex2D(UISampler, In.vTexUV);
	vColor *= g_vColor;

	vColor.a *= g_fAlpha;

	Out.vColor = vColor;

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Texture_AlphaBlend
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass Texture_Animation
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_ANIMAIN();
	}
	pass Texture_Grad
	{		
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_GRADMAIN();
	}
	pass Texture_Fix
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_FIXMAIN();
	}
	pass Texture_Color
	{	

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_COLORMAIN();
	}
	pass Texture_Depth
	{
		ZwriteEnable = true;
		ZEnable = true;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Texture_DepthAni
	{
		ZwriteEnable = true;
		ZEnable = true;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_ANIMAIN();
	}

};


