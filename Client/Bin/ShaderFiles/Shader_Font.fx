#include "Default.hpp"

vector		g_vColor;
float		g_fMaxWidthNum;
float		g_fMaxHeightNum;
float		g_fAlpha;
float2		g_vUVOffset;


matrix				g_matWorld;

texture				g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;

};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	/*float4		vFrustumPos : TEXCOORD1;
	float4		vViewPosition : TEXCOORD2;*/
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	matrix			matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;	
	return Out;
}

VS_OUT VS_3DMAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	matrix			matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	//Out.vPosition = mul(vector(In.vPosition, 1.f), matWV);
	//Out.vViewPosition = Out.vPosition;

	matWVP = mul(matWV, g_matProj);

	//Out.vFrustumPos = Compute_Frustum(In.vTexUV, g_fCamFar, g_matInvProj);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	
	// 뷰좌표를 구해놓고 깊이버퍼에서 갖고올때 뷰좌표로 갖고와서 비교 하면 좀더 빠르다.
	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	//float4		vFrustumPos : TEXCOORD1;
	//float4		vViewPosition : TEXCOORD2;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2		vSliceUV = (float2)0;

	vSliceUV.x = In.vTexUV.x / g_fMaxWidthNum;
	vSliceUV.y = In.vTexUV.y / g_fMaxHeightNum;

	vSliceUV += g_vUVOffset;

	vector	vColor = tex2D(BaseSampler, vSliceUV);
	vector	vEraseColor = tex2D(BaseSampler, vSliceUV);

	vColor *= vEraseColor.r;


	Out.vColor = vColor * g_vColor;
	
	return Out;
}
PS_OUT PS_3DMAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2		vSliceUV = (float2)0;

	vSliceUV.x = In.vTexUV.x / g_fMaxWidthNum;
	vSliceUV.y = In.vTexUV.y / g_fMaxHeightNum;

	vSliceUV += g_vUVOffset;
	
	//vector	vDepth = tex2D(Sampler_Depth, vSliceUV); 화면상의 UV 
	//Compute_PosVS(In.vFrustumPos,) //프로스텀할때도 위의 UV를 넘겨야한다.
	// Z를 비교하였을때 내 Z가 더크다면 더 뒤에있는것이다. 
	
	vector	vColor = tex2D(BaseSampler, vSliceUV);
	vector	vEraseColor = tex2D(BaseSampler, vSliceUV);

	vColor *= vEraseColor.r;

	Out.vColor = vColor * g_vColor;

	return Out;
}

technique Default_Device
{
	pass D2DAlpha_Rendering
	{	
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		Lighting = false;
		CullMode = none;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass D3DAlpha_Rendering
	{
		ZwriteEnable = true;
		ZEnable = true;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		Lighting = false;
		CullMode = none;
		VertexShader = compile vs_3_0 VS_3DMAIN();
		PixelShader = compile ps_3_0 PS_3DMAIN();
	}

	pass D3DNotDepthAlpha_Rendering
	{
		ZwriteEnable = false;
		ZEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		Lighting = false;
		CullMode = none;
		VertexShader = compile vs_3_0 VS_3DMAIN();
		PixelShader = compile ps_3_0 PS_3DMAIN();
	}

	
};

