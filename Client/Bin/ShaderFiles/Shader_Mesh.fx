

// float4x4
matrix				g_matWorld, g_matView, g_matProj;
matrix				g_matLightVP[2];	// 빛 시점에서의 View Projection 행렬..
matrix				g_matLightVPB[2];	// 쉐도우맵 텍스쳐에 접근하기 위한 행렬..

float				g_fCamFar = 1000.f;
float				g_fShadowRange[2];	// 쉐도우맵 텍스쳐의 영역( Cascade 영역 )..

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



/*정점inLocalSpace*/
struct VS_IN
{
	// 위치
	float3		vPosition : POSITION;

	float3		vNormal : NORMAL;
	// 텍스쳐 uv좌표
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

// 체비쇼프 함수를 사용하여 그림자의 표현 가능 색의 영역을 늘린다..
float ComputeShadowPower( float2 moments, float cur_depth )
{
	//VSM
	//cur_depth 는 자기 뎁스 값입니다.
	float fragDepth = cur_depth;
	float fLit = 1.0f;

	float E_x2 = moments.y;
	float Ex_2 = moments.x * moments.x;
	float variance = (E_x2 - Ex_2);

	//최소값으로 보정해 두어야만
	//그림자 영역 안에,
	//깊이값의 미묘한 편차에 의한, 지저분한 면이 생기지 않습니다.
	//이 최소값은 투영행렬의 영향(거리값 등....)을 받습니다

	variance = max( variance, 0.00005f );

	float mD = (fragDepth - moments.x);
	float mD_2 = mD * mD;
	float p = (variance / (variance + mD_2));

	//쉐도우뎁스값이 자기값보다 앞에 있다면(작다면)
	//가리워진것으로 판정하여 그림자 처리를 해주어야 하는데,
	//원래는 이것
	//max( p, fragDepth <= moments.x );

	//값뒤집기
	//의외로 좋은 결과
	fLit = max( p, fragDepth > moments.x );
	fLit = (1.f - fLit) + 0.5f;
	//fLit = min( 1.f, fLit );
	if ( fLit > 1.f )
		fLit = 1.f;

	//컬러에 적용
	//컬러값 * fLit 으로 사용하였습니다.
	//fLit 값이 작을 수록, 픽셀이 어두워 집니다. 

	return fLit;
}

float3 CalcShadow( float3 vViewPos, float4 vWorldPos, float3 vColor )
{
	// --------------------------- 그림자를 띄울지 말지 결정.. --------------------------- //
	float4  shadow = 0.f;

	float fDist = length( vViewPos.xyz );
	float4 vDepth = 0.f;
	bool bShadow = false;

	// 현재 카메라의 위치에 따라 그림자 맵을 다르게 가져온다..
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

	// if( 그림자에 가려진 부분이다 && 빛이 바라보는 영역 밖에 있지 않음 == 빛 프러스텀 안에 있다 )..
	// 빛 시점에서 투영 z 나누기를 거쳤을 때 x : -1 ~ 1, y : 1 ~ -1 사이에 있다는 것..
	if ( bShadow == true && !(vDepth.x < -1.f || vDepth.x > 1.f || vDepth.y < -1.f || vDepth.y > 1.f) )
	{
		Color *= ComputeShadowPower( shadow.rg, vDepth.z );
	}

	return Color;
}

// 버텍스 셰이더를 수행하기위한 메인함수.
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

	// 각원소가 -1~1로 표현되기때문에 0~1로 바꾸자.ㅔ
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

	// 각원소가 -1~1로 표현되기때문에 0~1로 바꾸자.ㅔ
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

// 단순 텍스쳐 매핑 하고 렌더 하는 기능을 수행..
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

// 텍스쳐 매핑과 그림자 렌더를 수행함..
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
