
texture g_ColorTexture;

sampler ColorSampler = sampler_state
{
	texture = g_ColorTexture;
};


texture g_ShadeTexture;

sampler ShadeSampler = sampler_state
{
	texture = g_ShadeTexture;
};


texture g_SpecularTexture;

sampler SpecularSampler = sampler_state
{
	texture = g_SpecularTexture;
};

struct PS_IN
{
	float2		vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0.f;

	vector	vColor = tex2D(ColorSampler, In.vTexUV);
	vector	vShade = tex2D(ShadeSampler, In.vTexUV);	
	vector	vSpecular = tex2D(SpecularSampler, In.vTexUV);

	Out.vColor = vColor * vShade + vSpecular;

	return Out;
}

technique DefaultDevice
{
	pass Default
	{
		ZwriteEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0x00;
		AlphaFunc = greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}