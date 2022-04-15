

// float4x4
matrix				g_matWorld, g_matView, g_matProj;
matrix				g_matLightVP[2];	// �� ���������� View Projection ���..
matrix				g_matLightVPB[2];	// ������� �ؽ��Ŀ� �����ϱ� ���� ���..

float				g_fCamFar = 1000.f;
float				g_fShadowRange[2];	// ������� �ؽ����� ����( Cascade ���� )..

texture				g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;

	//AddressU = CLAMP;
	//AddressV = CLAMP;
};

texture				g_NormalTexture;
sampler NormalSampler = sampler_state
{
	texture = <g_NormalTexture>;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_SpecularTexture;
sampler SpecularSampler = sampler_state
{
	texture = <g_SpecularTexture>;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_ShadowNearTexture;
sampler ShadowNearSampler = sampler_state
{
	texture = g_ShadowNearTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_ShadowMidTexture;
sampler ShadowMidSampler = sampler_state
{
	texture = g_ShadowMidTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_ShadowFarTexture;
sampler ShadowFarSampler = sampler_state
{
	texture = g_ShadowFarTexture;
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

struct VS_IN_TBN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float3		vTangent : TANGENT;
	float3		vBinormal : BINORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;	
	vector		vWorldNormal : TEXCOORD1;
	vector		vViewPos : TEXCOORD2;
	vector		vWorldPos : TEXCOORD3;
};

struct VS_OUT_TBN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vViewPos : TEXCOORD1;
	float3		T : TEXCOORD2;
	float3		B : TEXCOORD3;
	float3		N : TEXCOORD4;
	vector		vWorldPos : TEXCOORD5;
};

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;	
	vector		vWorldNormal : TEXCOORD1;
	vector		vViewPos : TEXCOORD2;
	vector		vWorldPos : TEXCOORD3;
};

struct PS_IN_TBN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vViewPos : TEXCOORD1;
	float3		T : TEXCOORD2;
	float3		B : TEXCOORD3;
	float3		N : TEXCOORD4;
	vector		vWorldPos : TEXCOORD5;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
};

struct PS_OUT_SPECULAR
{
	vector		vColor : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
	vector		vSpecular : COLOR3;
};

float3 Compute_NormalMapping( sampler Sampler, float2 vTexCoord, float3 T, float3 B, float3 N)
{
	float3 vNormal = tex2D( Sampler, vTexCoord ).xyz * 2.f - 1.f;
	float3x3 matTBN = float3x3( float3( T ), float3( B ), float3( N ) );
	matTBN = transpose( matTBN );
	vNormal = mul( matTBN, vNormal );

	//float3 vNormal_World = tex2D( Sampler_Normal, Input.vTexUV );
	//float3x3 matTangent = float3x3( normalize( Input.vTangent ), normalize( Input.vBinormal ), normalize( Input.vNormal ) );
	//matTangent = transpose( matTangent );
	//vNormal_World = mul( matTangent, vNormal_World );

	return vNormal;
}

// ü����� �Լ��� ����Ͽ� �׸����� ǥ�� ���� ���� ������ �ø���..
float ComputeShadowPower( float2 moments, float cur_depth )
{
	//VSM
	//cur_depth �� �ڱ� ���� ���Դϴ�.
	float fragDepth = cur_depth;
	float fLit = 1.0f;

	float E_x2 = moments.y;
	float Ex_2 = moments.x * moments.x;
	float variance = (E_x2 - Ex_2);

	//�ּҰ����� ������ �ξ�߸�
	//�׸��� ���� �ȿ�,
	//���̰��� �̹��� ������ ����, �������� ���� ������ �ʽ��ϴ�.
	//�� �ּҰ��� ��������� ����(�Ÿ��� ��....)�� �޽��ϴ�

	variance = max( variance, 0.00005f );

	float mD = (fragDepth - moments.x);
	float mD_2 = mD * mD;
	float p = (variance / (variance + mD_2));

	//�����쵪������ �ڱⰪ���� �տ� �ִٸ�(�۴ٸ�)
	//�������������� �����Ͽ� �׸��� ó���� ���־�� �ϴµ�,
	//������ �̰�
	//max( p, fragDepth <= moments.x );

	//��������
	//�ǿܷ� ���� ���
	fLit = max( p, fragDepth > moments.x );
	fLit = (1.f - fLit) + 0.5f;
	//fLit = min( 1.f, fLit );
	if ( fLit > 1.f )
		fLit = 1.f;

	//�÷��� ����
	//�÷��� * fLit ���� ����Ͽ����ϴ�.
	//fLit ���� ���� ����, �ȼ��� ��ο� ���ϴ�. 

	return fLit;
}

float3 CalcShadow( float3 vViewPos, float4 vWorldPos, float3 vColor )
{
	// --------------------------- �׸��ڸ� ����� ���� ����.. --------------------------- //
	float4  shadow = 0.f;

	float fDist = length( vViewPos.xyz );
	float4 vDepth = 0.f;
	bool bShadow = false;

	// ���� ī�޶��� ��ġ�� ���� �׸��� ���� �ٸ��� �����´�..
	if ( fDist <= g_fShadowRange[0] )
	{
		vDepth = mul( vWorldPos, g_matLightVP[0] );
		shadow = tex2Dproj( ShadowNearSampler, mul( vWorldPos, g_matLightVPB[0] ) );
		bShadow = (shadow.x + vDepth.z < vDepth.w - 0.03f);
	}
	else if ( fDist <= g_fShadowRange[1] )
	{
		vDepth = mul( vWorldPos, g_matLightVP[1] );
		shadow = tex2Dproj( ShadowMidSampler, mul( vWorldPos, g_matLightVPB[1] ).xyzw );
		bShadow = (shadow.x + vDepth.z < vDepth.w - 0.03f);
	}

	vDepth = vDepth / vDepth.w;

	float3 Color = vColor.rgb;

	// if( �׸��ڿ� ������ �κ��̴� && ���� �ٶ󺸴� ���� �ۿ� ���� ���� == �� �������� �ȿ� �ִ� )..
	// �� �������� ���� z �����⸦ ������ �� x : -1 ~ 1, y : 1 ~ -1 ���̿� �ִٴ� ��..
	if ( bShadow == true && !(vDepth.x < -1.f || vDepth.x > 1.f || vDepth.y < -1.f || vDepth.y > 1.f) )
	{
		Color *= ComputeShadowPower( shadow.rg, vDepth.z );
	}

	return Color;
}

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

	Out.vViewPos = mul( vector( In.vPosition, 1.f ), matWV );

	Out.vWorldPos = mul( vector( In.vPosition, 1.f ), g_matWorld );

	return Out;
}

VS_OUT_TBN VS_MAIN_TBN(VS_IN_TBN In)
{
	VS_OUT_TBN Out = (VS_OUT_TBN)0;

	matrix matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	float3 T = mul( In.vTangent, (float3x3)g_matWorld );
	float3 B = mul( In.vBinormal, (float3x3)g_matWorld );
	float3 N = mul( In.vNormal, (float3x3)g_matWorld );

	Out.vPosition = mul( vector(In.vPosition, 1.f), matWVP );
	Out.vTexUV = In.vTexUV;
	Out.vViewPos = mul( vector(In.vPosition, 1.f), matWV );
	Out.vWorldPos = mul( vector( In.vPosition, 1.f ), g_matWorld );
	Out.T = normalize( T );
	Out.B = normalize( B );
	Out.N = normalize( N );

	return Out;
}

////////////////////////////////////////////// DefaultRendering.. //////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_D
// Desc: Default Diffuse Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_D(PS_IN In)
{
	PS_OUT_SPECULAR		Out = (PS_OUT_SPECULAR)0;

	vector	vColor;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);		
	//Out.vColor.a = 1.f;

	// �����Ұ� -1~1�� ǥ���Ǳ⶧���� 0~1�� �ٲ���.��
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_DN
// Desc: Default Diffuse Normal Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_DN( PS_IN_TBN In )
{
	PS_OUT_SPECULAR Out = (PS_OUT_SPECULAR)0;
	
	float3 vNormal = Compute_NormalMapping( NormalSampler, In.vTexUV, In.T, In.B, In.N ) * 0.5f + 0.5f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = float4(vNormal, 0.f);
	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);
	//Out.vNormal = float4(In.N * 0.5f + 0.5f, 0.f);

	return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_DS
// Desc: Default Diffuse Specular Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_DS( PS_IN In )
{
	PS_OUT_SPECULAR Out = (PS_OUT_SPECULAR)0;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = vector(tex2D( SpecularSampler, In.vTexUV ).rgb, 1.f);

	return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_DNS
// Desc: Default Diffuse Normal Specular Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_DNS( PS_IN_TBN In )
{
	PS_OUT_SPECULAR Out = (PS_OUT_SPECULAR)0;

	float3 vNormal = Compute_NormalMapping( NormalSampler, In.vTexUV, In.T, In.B, In.N ) * 0.5f + 0.5f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vNormal = float4(vNormal, 0.f);
	//Out.vNormal = float4( In.N * 0.5f + 0.5f, 0.f );
	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = vector(tex2D( SpecularSampler, In.vTexUV ).rgb, 1.f);

	return Out;
}


////////////////////////////////////////////// ShadowRendering.. //////////////////////////////////////////////
//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_Shadow_D
// Desc: Default Diffuse Mapping And Shadow Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_Shadow_D(PS_IN In)
{
	PS_OUT_SPECULAR		Out = (PS_OUT_SPECULAR)0;

	vector	vColor = tex2D( BaseSampler, In.vTexUV );

	vColor.rgb = CalcShadow( In.vViewPos.xyz, In.vWorldPos.xyzw, vColor.rgb );
	Out.vColor = float4( vColor.rgb, vColor.a );

	// �����Ұ� -1~1�� ǥ���Ǳ⶧���� 0~1�� �ٲ���.��
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_Shadow_DN
// Desc: Default Diffuse Normal Mapping And Shadow Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_Shadow_DN( PS_IN_TBN In )
{
	PS_OUT_SPECULAR Out = (PS_OUT_SPECULAR)0;

	float3 vNormal = Compute_NormalMapping( NormalSampler, In.vTexUV, In.T, In.B, In.N ) * 0.5f + 0.5f;

	vector	vColor = tex2D( BaseSampler, In.vTexUV );
	vColor.rgb = CalcShadow( In.vViewPos.xyz, In.vWorldPos.xyzw, vColor.rgb );

	Out.vColor = float4( vColor.rgb, vColor.a );
	Out.vNormal = float4(vNormal, 0.f);
	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);
	//Out.vNormal = float4(In.N * 0.5f + 0.5f, 0.f);

	return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_Shadow_DS
// Desc: Default Diffuse Specular Mapping And Shadow Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_Shadow_DS( PS_IN In )
{
	PS_OUT_SPECULAR Out = (PS_OUT_SPECULAR)0;

	vector	vColor = tex2D( BaseSampler, In.vTexUV );
	vColor.rgb = CalcShadow( In.vViewPos.xyz, In.vWorldPos.xyzw, vColor.rgb );

	Out.vColor = float4( vColor.rgb, vColor.a );
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = tex2D( SpecularSampler, In.vTexUV );

	return Out;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_main_Shadow_DNS
// Desc: Default Diffuse Normal Specular Mapping And Shadow Mapping..
//-----------------------------------------------------------------------------
PS_OUT_SPECULAR ps_main_Shadow_DNS( PS_IN_TBN In )
{
	PS_OUT_SPECULAR Out = (PS_OUT_SPECULAR)0;

	float3 vNormal = Compute_NormalMapping( NormalSampler, In.vTexUV, In.T, In.B, In.N ) * 0.5f + 0.5f;

	vector	vColor = tex2D( BaseSampler, In.vTexUV );
	vColor.rgb = CalcShadow( In.vViewPos.xyz, In.vWorldPos.xyzw, vColor.rgb );

	Out.vColor = float4( vColor.rgb, vColor.a );
	Out.vNormal = float4(vNormal, 0.f);
	//Out.vNormal = float4( In.N * 0.5f + 0.5f, 0.f );
	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.f, 0.0f);
	Out.vSpecular = tex2D( SpecularSampler, In.vTexUV );

	return Out;
}

// �ܼ� �ؽ��� ���� �ϰ� ���� �ϴ� ����� ����..
technique DefaultRendering
{
	//////////////////////////// Default Option Mesh Mapping.. ////////////////////////////
	pass Mapping_Diffuse
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_D();
	}

	pass Mapping_DiffuseNormal
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_DN();
	}

	pass Mapping_DiffuseSpecular
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_DS();
	}

	pass Mapping_DiffuseNormalSpecular
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_DNS();
	}

	
	//////////////////////////// AlphaTest Option Mesh Mapping.. ////////////////////////////
	pass AlphaTest_Mapping_Diffuse
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_D();
	}

	pass AlphaTest_Mapping_DiffuseNormal
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_DN();
	}

	pass AlphaTest_Mapping_DiffuseSpecular
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_DS();
	}

	pass AlphaTest_Mapping_DiffuseNormalSpecular
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_DNS();
	}
};

// �ؽ��� ���ΰ� �׸��� ������ ������..
technique ShadowRendering
{
	//////////////////////////// Default Option Mesh Mapping.. ////////////////////////////
	pass Mapping_Diffuse
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_Shadow_D();
	}

	pass Mapping_DiffuseNormal
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_Shadow_DN();
	}

	pass Mapping_DiffuseSpecular
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_Shadow_DS();
	}

	pass Mapping_DiffuseNormalSpecular
	{
		alphablendenable = false;

		alphatestenable = false;

		CullMode = ccw;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_Shadow_DNS();
	}


	//////////////////////////// AlphaTest Option Mesh Mapping.. ////////////////////////////
	pass AlphaTest_Mapping_Diffuse
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_Shadow_D();
	}

	pass AlphaTest_Mapping_DiffuseNormal
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_Shadow_DN();
	}

	pass AlphaTest_Mapping_DiffuseSpecular
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 ps_main_Shadow_DS();
	}

	pass AlphaTest_Mapping_DiffuseNormalSpecular
	{
		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x30;
		alphafunc = greater;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN_TBN();
		PixelShader = compile ps_3_0 ps_main_Shadow_DNS();
	}
};
