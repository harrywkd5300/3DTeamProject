
// 전역변수는 상수화되어있다.
// 전역변수는 클라이언트로부터 값을 받아올 수 있다.
// float				g_fData = 10.f;

// float4x4
#include "shader_Base.hpp"

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

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

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
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor = vColor;

	//vector		vColor = vector(1.f, 0.f, 0.f, 1.f);

	//Out.vColor = vColor * In.vTexUV.x;	

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		/*Zenable = true;
		Zwriteenable = true;
		Cullmode = ccw;
		Fillmode = wireframe;*/
	/*	Alphablendenable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;*/

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Texture_Mapping
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};

//technique
//{
//	pass Default_Rendering
//	{
//		VertexShader = compile vs_1_0 VS_MAIN();
//		PixelShader = compile ps_1_0 PS_MAIN();
//	};
//
//	pass Texture_Mapping
//	{
//		VertexShader = compile vs_1_0 VS_MAIN();
//		PixelShader = compile ps_1_0 PS_MAIN();
//	};
//};

