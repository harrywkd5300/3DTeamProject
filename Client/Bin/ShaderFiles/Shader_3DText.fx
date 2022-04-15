
#include "shader_Base.hpp"

// VertexShader
// 리턴 함수이름(인자값) { }

/*정점inLocalSpace*/
struct VS_IN
{
	// 위치
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vWorldNormal : TEXCOORD1;
};

// VS_MAIN(vector		vPosition : POSITION, float2		vTexUV : TEXCOORD0)

// 버텍스 셰이더를 수행하기위한 메인함수.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	vector			vNormal_World = mul(vector(In.vNormal, 0.f), g_matWorld);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldNormal = normalize(vNormal_World);

	//vector			vTmp, vTmp1;
	//vTmp.x = 10;
	//vTmp.xy = 10;
	//vTmp.xz = 10;
	//vTmp.xyz = 10;
	//vTmp.rg = 10;

	//vTmp * vTmp1;

	//vTmp = 1.f;

	//vTmp + 1;

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vWorldNormal : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
	vector		vNormal : COLOR1;

};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor = vColor;
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	//vector		vColor = vector(1.f, 0.f, 0.f, 1.f);

	//Out.vColor = vColor * In.vTexUV.x;	

	return Out;
}

technique Default_Device
{
	pass 3DText_Rendering
	{
		/*Zenable = true;
		Zwriteenable = true;
		Cullmode = ccw;
		Fillmode = wireframe;*/
		/*	Alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;*/

		Cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

};