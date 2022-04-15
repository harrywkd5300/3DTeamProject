// float4x4
matrix				g_matWorld, g_matView, g_matProj;

vector				g_vBrushPos = vector(20.f, 0.f, 10.f, 1.f);
float				g_fBrushSize = 10.f;
float				g_fCamFar = 1000.f;

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_DestTexture;

sampler DestSampler = sampler_state
{
	texture = g_DestTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_FilterTexture;

sampler FilterSampler = sampler_state
{
	texture = g_FilterTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_BrushTexture;

sampler BrushSampler = sampler_state
{
	texture = g_BrushTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


/*정점inLocalSpace*/
struct VS_IN
{
	// 위치
	float3		vPosition : POSITION;

	float3		vNormal : NORMAL;
	// 텍스쳐 uv좌표
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;	
	float2		vTexUV : TEXCOORD0;
	vector		vWorldNormal : TEXCOORD1;
	vector		vWorldPos : TEXCOORD2;
	vector		vViewPos : TEXCOORD3;

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

	vector		vNormal_World = mul(vector(In.vNormal, 0.f), g_matWorld);

	vector		vPos_World = mul(vector(In.vPosition, 1.f), g_matWorld);

	Out.vWorldNormal = normalize(vNormal_World);
	Out.vWorldPos = vPos_World;

	Out.vViewPos = mul( vector( In.vPosition, 1.f ), matWV );

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vWorldNormal : TEXCOORD1;
	vector		vWorldPos : TEXCOORD2;
	vector		vViewPos : TEXCOORD3;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vResultColor;
	vector	vColor[4];
	vColor[0] = tex2D(BaseSampler, In.vTexUV * 20.f);
	vColor[1] = tex2D(DestSampler, In.vTexUV * 20.f);
	vColor[2] = tex2D(FilterSampler, In.vTexUV);
	vColor[3] = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushSize * 0.5f < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushSize * 0.5f
		&&  g_vBrushPos.z - g_fBrushSize * 0.5f < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushSize * 0.5f)
	{
		float2		vNewUV;

		vNewUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushSize * 0.5f)) / g_fBrushSize;
		vNewUV.y = ((g_vBrushPos.z + g_fBrushSize * 0.5f) - In.vWorldPos.z) / g_fBrushSize;

		vColor[3] = tex2D(BrushSampler, vNewUV);
		//vColor[3] = vector(1.f, 0.f, 0.f, 1.f);
	}

	Out.vColor = vColor[0] * vColor[2] + vColor[1] * (1.f - vColor[2]) + vColor[3];	
	Out.vColor.a = 1.f;

	// 각원소가 -1~1로 표현되기때문에 0~1로 바꾸자.ㅔ
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = In.vViewPos.z / g_fCamFar;
	//Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	
	return Out;
}

technique Default_Device
{
	pass DefaultRendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

}

