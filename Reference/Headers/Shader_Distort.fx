

matrix		g_matView, g_matProj;

// Texture --------------------------------------------//
texture		g_SceneTexture;

sampler SceneSampler = sampler_state
{
	texture = g_SceneTexture;

	//minfilter = linear;
	//magfilter = linear;
	//mipfilter = linear;

	AddressU = Clamp;
	AddressV = Clamp;

};


texture		g_DistortTexture;

sampler DistortSampler = sampler_state
{
	texture = g_DistortTexture;

	//minfilter = linear;
	//magfilter = linear;
	//mipfilter = linear;
	
	//AddressU = Wrap;
	//AddressV = Wrap;
};

// Struct ---------------------------------------------//

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWVP;

	matWVP = mul(g_matView, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
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

	//vector		vDistort = tex2D(DistortSampler, In.vTexUV) - 0.5f;
	//vDistort = normalize(vDistort) * 0.01f;
	vector	vDistSampler = tex2D(DistortSampler, In.vTexUV);
	float2	vDistortUV = vDistSampler.rg;
	vDistortUV -= 0.5f;
	vDistortUV *= vDistSampler.b;

	vector	vColor = tex2D(SceneSampler, In.vTexUV + vDistortUV);
	Out.vColor = vColor;

	return Out;

}

technique DefaultDevice
{
	pass Default
	{
		ZwriteEnable = false;

		//AlphaTestEnable = true;
		//AlphaRef = 0x00;
		//AlphaFunc = greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}