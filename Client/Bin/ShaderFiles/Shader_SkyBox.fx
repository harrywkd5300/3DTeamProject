
// 스카이박스
matrix				g_matWorld, g_matView, g_matProj;

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
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
	float3		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
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

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float3		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = texCUBE(BaseSampler, In.vTexUV);

	Out.vColor = vColor;
	Out.vColor.a = 1.f; // By.KJY

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		Lighting = false;
		Cullmode = cw;
		Zenable = false;
		Zwriteenable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Texture_Mapping
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};
