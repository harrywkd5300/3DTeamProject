#include "Shader_Base.hpp"

// Common -----------------------------
float		g_fFadeValue = 0.f;
vector		g_vColor = (vector)1.f;
float		g_fBuff_Bright = 1.f;

// Sprite ------------------------------
float		g_fSliceX = 0.f;
float		g_fSliceY = 0.f;
float2		g_vUVOffset = (float2)0;

// Normal Buffer -----------------------
texture		g_BufferTexture;

sampler BufferSampler = sampler_state
{
	texture = g_BufferTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


// Mesh ------------------------------------
vector		g_vMeshPos;
float		g_fUVScale = 0.f;
int			g_iUVAniType = 0;

texture		g_MeshUVTexture;

sampler MeshUVSampler = sampler_state
{
	texture = g_MeshUVTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// Particle --------------------------------
vector		g_vCamera_Pos; // 파티클 사이즈
int			g_iViewportHeight;
float		g_fPTC_Bright;

texture		g_ParticleTexture;

sampler ParticleSampler = sampler_state
{
	texture = g_ParticleTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// Distortion ---------------------------
float		g_fTimeDelta = 0.f;

// Masking -----------------------------
float2		g_vUVSpeed[5]; // 마스킹 텍스쳐별 속도
//float2		g_fUVSpeed[5]; // 마스킹 텍스쳐별 속도
vector		g_vUseColor[5];

texture		g_BlendTexture_0;

sampler BlendSampler_0 = sampler_state
{
	texture = g_BlendTexture_0;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_BlendTexture_1;

sampler BlendSampler_1 = sampler_state
{
	texture = g_BlendTexture_1;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_BlendTexture_2;

sampler BlendSampler_2 = sampler_state
{
	texture = g_BlendTexture_2;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_BlendTexture_3;

sampler BlendSampler_3 = sampler_state
{
	texture = g_BlendTexture_3;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_BlendTexture_4;

sampler BlendSampler_4 = sampler_state
{
	texture = g_BlendTexture_4;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

////////////////////////////////////////////

// Default
texture				g_DepthTexture;

sampler DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};


// Distortion Normal
texture		g_DistortTexture;

sampler DistortSampler = sampler_state
{
	texture = g_DistortTexture;

	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


// Vertex Struct ////////////////////////////


struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
	vector		vWorldPos : TEXCOORD2;
};

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : TEXCOORD1;
	vector		vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

// Particle Struct
struct VS_IN_PTC
{
	float3		vPosition : POSITION;
	float		fSize : PSIZE;
	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT_PTC
{
	vector		vPosition : POSITION;
	float		fSize : PSIZE;
	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : COLOR1;
};

struct PS_IN_PTC
{
	//vector		vPosition : POSITION;
	//float		fSize : PSIZE;
	float4		vColor : COLOR0;
	float2		vTexUV : TEXCOORD0;
	vector		vProjPos : COLOR1;
};

struct PS_OUT_PTC
{
	vector		vColor : COLOR0;
};


// ------------------- For. Default ----------------------------------//

VS_OUT VS_MAIN_DEFAULT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	vector		vWorld_Pos = mul(vector(In.vPosition, 1.f), g_matWorld);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;
	Out.vWorldPos = vWorld_Pos;

	return Out;
}

PS_OUT PS_MAIN_DEFAULT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//vector	vColor = tex2D(BaseSampler, In.vTexUV);

	//float2	vNewUV;

	//vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	//vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	//vector	vDepth = tex2D(DepthSampler, vNewUV);

	//float	vViewZ = vDepth.r * 1000.0f;

	//float	fAlpha = min(vViewZ - In.vProjPos.w, 1.f);

	//Out.vColor = vColor;

	//Out.vColor.a = Out.vColor.a * fAlpha;

	return Out;
}


//--------------------------- For. Sprite -------------------------------------//

PS_OUT PS_MAIN_SPRITE_CUTBLACK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2		vNewUV;

	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepth = tex2D(DepthSampler, vNewUV);
	float		vViewZ = vDepth.r * 1000.0f;

	float		fAlpha = min(vViewZ - In.vProjPos.w, 1.f);

	float2		vSliceUV = (float2)0;

	vSliceUV.x = In.vTexUV.x / g_fSliceX;
	vSliceUV.y = In.vTexUV.y / g_fSliceY;

	vSliceUV += g_vUVOffset;

	vector	vColor = tex2D(BaseSampler, vSliceUV);

	Out.vColor = vColor;
	Out.vColor.a = Out.vColor.a * fAlpha; // 소프트 이펙트
	Out.vColor *= g_vColor;
	Out.vColor.rgb *= g_fBuff_Bright;

	return Out;

}

//--------------------------- For. Normal Buffer -------------------------------//

PS_OUT PS_MAIN_NORMAL_BUFFER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2		vNewUV;

	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepth = tex2D(DepthSampler, vNewUV);
	float		vViewZ = vDepth.r * 1000.0f;

	float		fAlpha = min(vViewZ - In.vProjPos.w, 1.f);

	vector	vColor = tex2D(BufferSampler, In.vTexUV);

	Out.vColor = vColor;
	Out.vColor.a = Out.vColor.a * fAlpha;
	Out.vColor *= g_vColor;
	Out.vColor.rgb *= g_fBuff_Bright;

	return Out;

}


//--------------------------- For. Mask -------------------------------------//
PS_OUT PS_MAIN_BUFFER_MASK(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor[5];

	float2	vUVDelta = In.vTexUV;

	// 1번 텍스쳐 -> 베이스 텍스쳐
	vUVDelta += g_vUVSpeed[0];
	vColor[0] = tex2D(BlendSampler_0, vUVDelta);
	vUVDelta = In.vTexUV;

	if (any( g_vUseColor[1]))
	{
		vUVDelta += g_vUVSpeed[1];
		vColor[1] = tex2D(BlendSampler_1, vUVDelta) * g_vUseColor[1];
		vColor[1] = vColor[1] + step(g_vUseColor[1], 0);
		vColor[0] *= vColor[1];
		vUVDelta = In.vTexUV;
	}
	if (any(g_vUseColor[2]))
	{
		vUVDelta += g_vUVSpeed[1];
		vColor[2] = tex2D(BlendSampler_2, vUVDelta) * g_vUseColor[2];
		vColor[2] = vColor[2] + step(g_vUseColor[2], 0);
		vColor[0] *= vColor[2];
		vUVDelta = In.vTexUV;
	}
	if (any(g_vUseColor[3]))
	{
		vUVDelta += g_vUVSpeed[3];
		vColor[3] = tex2D(BlendSampler_3, vUVDelta) * g_vUseColor[3];
		vColor[3] = vColor[3] + step(g_vUseColor[3], 0);
		vColor[0] *= vColor[3];
		vUVDelta = In.vTexUV;
	}
	if (any(g_vUseColor[4]))
	{
		vUVDelta += g_vUVSpeed[4];
		vColor[4] = tex2D(BlendSampler_4, vUVDelta) * g_vUseColor[4];
		vColor[4] = vColor[4] + step(g_vUseColor[4], 0);
		vColor[0] *= vColor[4];
		vUVDelta = In.vTexUV;
	}


	//// 2번 텍스쳐 비교 -----------------------------
	//if (g_vUseColor[1].r > 0)
	//{
	//	vUVDelta += g_vUVSpeed[1];
	//	vColor[1] = tex2D(BlendSampler_1, vUVDelta);
	//	vColor[0].r *= vColor[1].r;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[1].g > 0)
	//{
	//	vUVDelta += g_vUVSpeed[1];
	//	vColor[1] = tex2D(BlendSampler_1, vUVDelta);
	//	vColor[0].g *= vColor[1].g;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[1].b > 0)
	//{
	//	vUVDelta += g_vUVSpeed[1];
	//	vColor[1] = tex2D(BlendSampler_1, vUVDelta);
	//	vColor[0].b *= vColor[1].b;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[1].a > 0)
	//{
	//	vUVDelta += g_vUVSpeed[1];
	//	vColor[1] = tex2D(BlendSampler_1, vUVDelta);
	//	vColor[0].a *= vColor[1].a;
	//	vUVDelta = In.vTexUV;
	//}
	//// 3번 텍스쳐 비교 -----------------------------
	//if (g_vUseColor[2].r > 0)
	//{
	//	vUVDelta += g_vUVSpeed[2];
	//	vColor[2] = tex2D(BlendSampler_2, vUVDelta);
	//	vColor[0].r *= vColor[2].r;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[2].g > 0)
	//{
	//	vUVDelta += g_vUVSpeed[2];
	//	vColor[2] = tex2D(BlendSampler_2, vUVDelta);
	//	vColor[0].g *= vColor[2].g;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[2].b > 0)
	//{
	//	vUVDelta += g_vUVSpeed[2];
	//	vColor[2] = tex2D(BlendSampler_2, vUVDelta);
	//	vColor[0].b *= vColor[2].b;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[2].a > 0)
	//{
	//	vUVDelta += g_vUVSpeed[2];
	//	vColor[2] = tex2D(BlendSampler_2, vUVDelta);
	//	vColor[0].a *= vColor[2].a;
	//	vUVDelta = In.vTexUV;
	//}
	//// 4번 텍스쳐 비교 -----------------------------
	//if (g_vUseColor[3].r > 0)
	//{
	//	vUVDelta += g_vUVSpeed[3];
	//	vColor[3] = tex2D(BlendSampler_3, vUVDelta);
	//	vColor[0].r *= vColor[3].r;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[3].g > 0)
	//{
	//	vUVDelta += g_vUVSpeed[3];
	//	vColor[3] = tex2D(BlendSampler_3, vUVDelta);
	//	vColor[0].g *= vColor[3].g;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[3].b > 0)
	//{
	//	vUVDelta += g_vUVSpeed[3];
	//	vColor[3] = tex2D(BlendSampler_3, vUVDelta);
	//	vColor[0].b *= vColor[3].b;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[3].a > 0)
	//{
	//	vUVDelta += g_vUVSpeed[3];
	//	vColor[3] = tex2D(BlendSampler_3, vUVDelta);
	//	vColor[0].a *= vColor[3].a;
	//	vUVDelta = In.vTexUV;
	//}
	//// 5번 텍스쳐 비교 -----------------------------
	//if (g_vUseColor[4].r > 0)
	//{
	//	vUVDelta += g_vUVSpeed[4];
	//	vColor[4] = tex2D(BlendSampler_4, vUVDelta);
	//	vColor[0].r *= vColor[4].r;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[4].g > 0)
	//{
	//	vUVDelta += g_vUVSpeed[4];
	//	vColor[4] = tex2D(BlendSampler_4, vUVDelta);
	//	vColor[0].g *= vColor[4].g;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[4].b > 0)
	//{
	//	vUVDelta += g_vUVSpeed[4];
	//	vColor[4] = tex2D(BlendSampler_4, vUVDelta);
	//	vColor[0].b *= vColor[4].b;
	//	vUVDelta = In.vTexUV;
	//}
	//else if (g_vUseColor[4].a > 0)
	//{
	//	vUVDelta += g_vUVSpeed[4];
	//	vColor[4] = tex2D(BlendSampler_4, vUVDelta);
	//	vColor[0].a *= vColor[4].a;
	//	vUVDelta = In.vTexUV;
	//}

	Out.vColor = vColor[0];
	
	// ---- 소프트 이펙트 ---- //
	float2	vNewUV;

	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepth = tex2D(DepthSampler, vNewUV);
	float	vViewZ = vDepth.r * 1000.0f;
	float	fAlpha = min(vViewZ - In.vProjPos.w, 1.f);
	// ------------------------//

	Out.vColor.a = Out.vColor.a * fAlpha;
	Out.vColor *= g_vColor;
	Out.vColor.rgb *= g_fBuff_Bright;

	return Out;
}

// 기존코드
//PS_OUT PS_MAIN_BUFFER_MASK(PS_IN In)
//{
//	PS_OUT		Out = (PS_OUT)0;
//
//	//int		iTexCnt = g_iMixCnt; 
//
//	vector	vColor[5];
//
//	float2	vUVDelta = In.vTexUV;
//	vUVDelta.y += g_fUVSpeed[0];
//
//	//float2	vUVDelta = g_fUVSpeed[0] + In.vTexUV;
//
//	vColor[0] = tex2D(BlendSampler_0, vUVDelta);
//	vColor[1] = tex2D(BlendSampler_1, In.vTexUV);
//	vColor[2] = tex2D(BlendSampler_2, vUVDelta);
//	vColor[3] = tex2D(BlendSampler_3, In.vTexUV);
//	vColor[4] = tex2D(BlendSampler_4, In.vTexUV);
//
//	// ---- 소프트 이펙트 ---- //
//	float2	vNewUV;
//
//	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
//	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
//
//	vector	vDepth = tex2D(DepthSampler, vNewUV);
//
//	float	vViewZ = vDepth.r * 1000.0f;
//
//	float	fAlpha = min(vViewZ - In.vProjPos.w, 1.f);
//
//	// 0번 - 알파마스킹애니메이션
//	// 1번 - 형태
//	// 2번 - 컬러타일링
//	//Out.vColor = vColor[2];
//	Out.vColor = vColor[2];
//	Out.vColor.a = vColor[0].r * vColor[1].a;
//	//Out.vColor.a = vColor[0].g * vColor[1].a;
//
//	Out.vColor.a = Out.vColor.a * fAlpha;
//
//	return Out;
//}


//--------------------------- For. Mesh -------------------------------------//

PS_OUT PS_MAIN_MESH_UVANIMATION(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 1. UV 한 방향으로 이동 ------------------------------------------------//
	//float2	vUVDelta = In.vTexUV;
	//vUVDelta += g_fUVScale;

	//vector	vColor = tex2D(MeshUVSampler, vUVDelta);
	//Out.vColor = vColor;

	// 2. 방사형 이동 ---------------------------------------------------------//
	vector	vColor = tex2D(MeshUVSampler, In.vTexUV);

	if (g_vMeshPos.x - g_fUVScale * 0.5f < In.vWorldPos.x && In.vWorldPos.x <= g_vMeshPos.x + g_fUVScale * 0.5f
		&&  g_vMeshPos.z - g_fUVScale * 0.5f < In.vWorldPos.z && In.vWorldPos.z <= g_vMeshPos.z + g_fUVScale * 0.5f)
	{
		float2		vNewUV;

		vNewUV.x = (In.vWorldPos.x - (g_vMeshPos.x - g_fUVScale * 0.5f)) / g_fUVScale;
		vNewUV.y = ((g_vMeshPos.z + g_fUVScale * 0.5f) - In.vWorldPos.z) / g_fUVScale;

		Out.vColor = tex2D(MeshUVSampler, vNewUV);
	}

	Out.vColor.a *= saturate(g_fFadeValue);

	// 3. 소프트 이펙트 ---------------------------------------------------------//
	/*float2	vNewUV;

	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepth = tex2D(DepthSampler, vNewUV);

	float	vViewZ = vDepth.r * 1000.0f;

	float	fAlpha = min(vViewZ - In.vProjPos.w, 1.f);*/

	//Out.vColor.a = Out.vColor.a * fAlpha;

	return Out;
}



//--------------------------- For. Particle -------------------------------------//

VS_OUT_PTC VS_MAIN_PARTICLE(VS_IN_PTC In)
{
	VS_OUT_PTC		Out = (VS_OUT_PTC)0;

	matrix			matVP;

	matVP = mul(g_matView, g_matProj);

	float	fDist = distance(vector(In.vPosition, 1.f), g_vCamera_Pos); // 둘다 월드

	Out.vPosition = mul(vector(In.vPosition, 1.f), matVP);
	Out.fSize = g_iViewportHeight * In.fSize / fDist;
	Out.vColor = In.vColor;
	Out.vColor.rgb *= g_fPTC_Bright;

	Out.vProjPos = Out.vPosition;
	//Out.fSize = g_iViewportHeight * In.fSize / fDist * 0.1f;

	return Out;


	// 1. 파티클 트랜스폼 사용
	//matrix			matWV, matWVP;

	//matWV = mul(g_matWorld, g_matView);
	//matWVP = mul(matWV, g_matProj);

	//vector		vWorld_Pos = mul(vector(In.vPosition, 1.f), g_matWorld);
	//
	//float		fDist = distance(vWorld_Pos, g_vCamera_Pos);

	//Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	//Out.vColor = In.vColor;
	//Out.fSize = g_iViewportHeight * In.fSize / fDist;
	////Out.fSize = g_iViewportHeight * In.fSize / fDist * 0.1f;

	//return Out;
}

PS_OUT_PTC PS_MAIN_PARTICLE(PS_IN_PTC In)
{
	PS_OUT_PTC		Out = (PS_OUT_PTC)0;

	Out.vColor = tex2D(ParticleSampler, In.vTexUV);
	Out.vColor *= In.vColor;
	// 뷰 거리 비교해서 clip 하기
	float2	vNewUV;

	vNewUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vNewUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector	vDepth = tex2D(DepthSampler, vNewUV);

	float	vViewZ = vDepth.r * 1000.0f;

	float	fDraw = vViewZ - In.vProjPos.w;
	clip(fDraw < 0.f ? -1 : 1);

	return Out;
}


//--------------------------- For. Distortion -------------------------------------//

struct VS_IN_DISTORT
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT_DISTORT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	//vector		vProjPos : TEXCOORD1;
	//vector		vWorldPos : TEXCOORD2;
};

VS_OUT_DISTORT VS_MAIN_DISTORT(VS_IN_DISTORT In)
{
	VS_OUT_DISTORT		Out = (VS_OUT_DISTORT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN_DISTORT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	//vector		vProjPos : TEXCOORD1;
	//vector		vWorldPos : TEXCOORD2;
};

struct PS_OUT_DISTORT
{
	vector		vColor : COLOR0;
};

PS_OUT_DISTORT PS_MAIN_DISTORT(PS_IN_DISTORT In)
{
	PS_OUT_DISTORT		Out = (PS_OUT_DISTORT)0;

	float2	vNewUV = float2(In.vTexUV.x, In.vTexUV.y + g_fTimeDelta);
	//float2	vNewUV = In.vTexUV;

	vector	vColor = tex2D(DistortSampler, vNewUV);

	// ---- 노말맵을 빛 연산처럼 사용한 코드 ---- //
	//vColor = (vColor * 2.f) - 1.f;
	//vector	vLight = vector(0.f, 1.f, 0.f, 0.f);
	//vColor = dot(vLight, normalize(vColor));
	//Out.vColor = saturate(vColor);
	// ------------------------------------------ //

	Out.vColor = vColor;

	//Out.vColor = tex2D(DistortSampler, In.vTexUV);

	return Out;
}

// --------------------------- Pass Type -----------------------------------------//

PS_OUT PS_MAIN_HDR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	//vector vColor = tex2D(Sampler_Src0, In.vTexUV);

	//Out.vColor = HDRPass(vColor, In.vTexUV);

	return Out;
}

// --------------------------- Pass Type -----------------------------------------//

technique Default_Device
{
	pass Default_Rendering
	{
		Lighting = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_3_0 PS_MAIN_DEFAULT();
	}

	pass Sprite_CutBlack_Rendering
	{
		Lighting = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = One;

		ZEnable = true;
		ZWriteEnable = false;

		CullMode = false;

		VertexShader = compile vs_3_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_3_0 PS_MAIN_SPRITE_CUTBLACK();
	}

	pass Normal_Buffer_Rendering
	{
		Lighting = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		ZEnable = true;
		ZWriteEnable = false;

		CullMode = false;

		VertexShader = compile vs_3_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_3_0 PS_MAIN_NORMAL_BUFFER();
	}

	pass Buffer_Mask_Rendering
	{
		Lighting = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = One;

		ZEnable = true;
		ZWriteEnable = false;

		CullMode = false;

		VertexShader = compile vs_3_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_3_0 PS_MAIN_BUFFER_MASK();
	}

	pass Mesh_UVAnimation_Rendering
	{
		Lighting = false;

		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = One;

		ZEnable = false;
		//ZWriteEnable = false;

		CullMode = false;

		VertexShader = compile vs_3_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_3_0 PS_MAIN_MESH_UVANIMATION();
	}

	pass Particle_Rendering
	{
		Lighting = false;
		CullMode = false;

		PointSpriteEnable = true;

		//PointScaleEnable = true;
		//PointSize_Min = 0;
		//PointScale_A = 0;
		//PointScale_B = 0;
		//PointScale_C = 1;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = One;

		ZEnable = false;
		ZWriteEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN_PARTICLE();
		PixelShader = compile ps_3_0 PS_MAIN_PARTICLE();
	}


	pass Distortion_Rendering
	{
		Lighting = false;

		//AlphaBlendEnable = true;
		//ZWriteEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN_DISTORT();
		PixelShader = compile ps_3_0 PS_MAIN_DISTORT();
	}

	pass HDR_Rendering
	{
		Lighting = false;

		VertexShader = compile vs_3_0 VS_MAIN_DEFAULT();
		PixelShader = compile ps_3_0 PS_MAIN_HDR();
	}
};
