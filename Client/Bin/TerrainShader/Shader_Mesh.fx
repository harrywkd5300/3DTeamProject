

// float4x4
matrix				g_matWorld, g_matView, g_matProj;

vector				g_Diffuse;

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};



/*����inLocalSpace*/
struct VS_IN
{
	// ��ġ
	float3		vPosition : POSITION;

	float3		vNormal : NORMAL;
	// �ؽ��� uv��ǥ
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;	
	vector		vWorldNormal : TEXCOORD1;
	vector		vProjPos : TEXCOORD2;
};



// ���ؽ� ���̴��� �����ϱ����� �����Լ�.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	vector			vNormal_World = mul(vector(In.vNormal, 0.f), g_matWorld);
		
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	Out.vWorldNormal = normalize(vNormal_World);

	Out.vTexUV = In.vTexUV;	

	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;	
	vector		vWorldNormal : TEXCOORD1;
	vector		vProjPos : TEXCOORD2;
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

	vector	vColor;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);		
	//Out.vColor.a = 1.f;
	Out.vColor += g_Diffuse;
	// �����Ұ� -1~1�� ǥ���Ǳ⶧���� 0~1�� �ٲ���.��
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.0f);

	return Out;
}

technique Default_Device
{


	pass AlphaTestRendering
	{
		AlphaBlendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0x3f;
		AlphaFunc = Greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass DefaultRendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
};
