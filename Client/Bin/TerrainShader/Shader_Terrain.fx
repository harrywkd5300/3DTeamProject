// float4x4
matrix				g_matWorld, g_matView, g_matProj;
matrix				g_matLightVP[2];	// 빛 시점에서의 View Projection 행렬..
matrix				g_matLightVPB[2];	// 쉐도우맵 텍스쳐에 접근하기 위한 행렬..

vector				g_vBrushPos = vector(20.f, 0.f, 10.f, 1.f);
float				g_fBrushSize = 10.f;

float				g_fUV = 1.f;

float				g_fCamFar = 1000.f;
float				g_fShadowRange[2];	// 쉐도우맵 텍스쳐의 영역( Cascade 영역 )..

// Base Texture
texture				g_BaseTexture;
sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_BaseNormalTexture;
sampler BaseNormalSampler = sampler_state
{
	texture = g_BaseNormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_BaseSpecTexture;
sampler BaseSpecSampler = sampler_state
{
	texture = g_BaseSpecTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// Dest Texture
texture				g_DestTexture;
sampler DestSampler = sampler_state
{
	texture = g_DestTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_DestNormalTexture;
sampler DestNormalSampler = sampler_state
{
	texture = g_DestNormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_DestSpecTexture;
sampler DestSpecSampler = sampler_state
{
	texture = g_DestSpecTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// Dest0 Texture
texture				g_Dest0Texture;
sampler Dest0Sampler = sampler_state
{
	texture = g_Dest0Texture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_Dest0NormalTexture;
sampler Dest0NormalSampler = sampler_state
{
	texture = g_Dest0NormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_Dest0SpecTexture;
sampler Dest0SpecSampler = sampler_state
{
	texture = g_Dest0SpecTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

// Dest1 Texture
texture				g_Dest1Texture;
sampler Dest1Sampler = sampler_state
{
	texture = g_Dest1Texture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_Dest1NormalTexture;
sampler Dest1NormalSampler = sampler_state
{
	texture = g_Dest1NormalTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture				g_Dest1SpecTexture;
sampler Dest1SpecSampler = sampler_state
{
	texture = g_Dest1SpecTexture;
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
	float3		vBinormal : TEXCOORD1; // By. KMH..
	float3		vTangent : TEXCOORD2; // By. KMH..
};

struct VS_OUT
{
	vector		vPosition : POSITION;	
	float2		vTexUV : TEXCOORD0;
	vector		vWorldNormal : TEXCOORD1;
	vector		vWorldPos : TEXCOORD2;
	vector		vViewPos : TEXCOORD3; // By.KMH - 투영 스페이스 좌표 대신 뷰 스페이스 좌표로..
	float3		vTangent : TEXCOORD4; // By. KMH..
	float3		vBinormal : TEXCOORD5; // By. KMH..
	//float4		vShadowMapUV : TEXCOORD6; // By. KMH..	<- 픽셀 쉐이더에서 직접 계산해야 함 ( Cascade 를 사용하면 )..
	//float4		vDepth : TEXCOORD7; // By. KMH..		<- 픽셀 쉐이더에서 직접 계산해야 함 ( Cascade 를 사용하면 )..
};

// VS_MAIN(vector		vPosition : POSITION, float2		vTexUV : TEXCOORD0)

float3 Compute_NormalMapping( float3 vTexColor, float3 T, float3 B, float3 N)
{
	float3 vNormal = vTexColor * 2.f - 1.f;
	float3x3 matTBN = float3x3( float3( T ), float3( B ), float3( N ) );
	matTBN = transpose( matTBN );
	vNormal = mul( matTBN, vNormal );

	//float3 vNormal_World = tex2D( Sampler_Normal, Input.vTexUV );
	//float3x3 matTangent = float3x3( normalize( Input.vTangent ), normalize( Input.vBinormal ), normalize( Input.vNormal ) );
	//matTangent = transpose( matTangent );
	//vNormal_World = mul( matTangent, vNormal_World );

	return vNormal;
}

// 버텍스 셰이더를 수행하기위한 메인함수.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;
	matrix			matLightWVP, matLightWVPB;

	matLightWVP = mul( g_matWorld, g_matLightVP[0] );
	matLightWVPB = mul( g_matWorld, g_matLightVPB[0] );

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	vector		vNormal_World = mul(vector(In.vNormal, 0.f), g_matWorld);

	vector		vPos_World = mul(vector(In.vPosition, 1.f), g_matWorld);

	Out.vTexUV = In.vTexUV;
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vWorldNormal = normalize(vNormal_World);
	Out.vWorldPos = vPos_World;
	Out.vViewPos = mul( vector(In.vPosition, 1.f), matWV );
	Out.vTangent = mul( In.vTangent, (float3x3)g_matWorld );
	Out.vBinormal = mul( In.vBinormal, (float3x3)g_matWorld );
	//Out.vShadowMapUV = mul(vector(In.vPosition, 1.f), matLightWVPB);
	//Out.vDepth = mul(vector(In.vPosition, 1.f), matLightWVP).xyzw; // 그림자와 비교를 위한 깊이값
	//Out.vTangent = In.vTangent;
	//Out.vBinormal = In.vBinormal;

	return Out;
}

struct PS_IN
{
	vector		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	vector		vWorldNormal : TEXCOORD1;
	vector		vWorldPos : TEXCOORD2;
	vector		vViewPos : TEXCOORD3;
	float3		vTangent : TEXCOORD4; // By. KMH..
	float3		vBinormal : TEXCOORD5; // By. KMH..
	//float4		vShadowMapUV : TEXCOORD6; // By. KMH..	<- 픽셀 쉐이더에서 직접 계산해야 함 ( Cascade 를 사용하면 )..
	//float4		vDepth : TEXCOORD7; // By. KMH..		<- 픽셀 쉐이더에서 직접 계산해야 함 ( Cascade 를 사용하면 )..
};

struct PS_OUT
{
	vector		vColor : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
	vector		vSpec : COLOR3;
};

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

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vResultColor;

	vector	vColor[5][3];

	vector  vFilter;

	vColor[0][0] = tex2D(BaseSampler, In.vTexUV * g_fUV);
	vColor[0][1] = tex2D(BaseNormalSampler, In.vTexUV * g_fUV);
	vColor[0][2] = tex2D(BaseSpecSampler, In.vTexUV * g_fUV);

	vColor[1][0] = tex2D(DestSampler, In.vTexUV * g_fUV);
	vColor[1][1] = tex2D(DestNormalSampler, In.vTexUV * g_fUV);
	vColor[1][2] = tex2D(DestSpecSampler, In.vTexUV * g_fUV);

	vColor[2][0] = tex2D(Dest0Sampler, In.vTexUV * g_fUV);
	vColor[2][1] = tex2D(Dest0NormalSampler, In.vTexUV * g_fUV);
	vColor[2][2] = tex2D(Dest0SpecSampler, In.vTexUV * g_fUV);

	vColor[3][0] = tex2D(Dest1Sampler, In.vTexUV * g_fUV);
	vColor[3][1] = tex2D(Dest1NormalSampler, In.vTexUV * g_fUV);
	vColor[3][2] = tex2D(Dest1SpecSampler, In.vTexUV * g_fUV);

	vFilter = tex2D(FilterSampler, In.vTexUV);

	vColor[4][0] = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushSize * 0.5f < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushSize * 0.5f
		 &&  g_vBrushPos.z - g_fBrushSize * 0.5f < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushSize * 0.5f)
	{
		float2		vNewUV;

		vNewUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushSize * 0.5f)) / g_fBrushSize;
		vNewUV.y = ((g_vBrushPos.z + g_fBrushSize * 0.5f) - In.vWorldPos.z) / g_fBrushSize;

		vColor[4][0] = tex2D(BrushSampler, vNewUV);
	}

	//vResultColor = (lerp(vColor[0][0], vColor[0][0], vFilter.r)) 
	//	         + (lerp(vColor[1][0], vColor[1][0], vFilter.g))
	//			 + (lerp(vColor[2][0], vColor[2][0], vFilter.b))
	//			 + (lerp(vColor[3][0], vColor[3][0], vFilter.a))
	//			 + vColor[4][0];

	vResultColor = vColor[0][0] * vFilter.a + vColor[1][0] * vFilter.r + vColor[2][0] * vFilter.g + vColor[3][0] * vFilter.b + vColor[4][0];

	// --------------------------- 그림자를 띄울지 말지 결정.. --------------------------- //
	float4  shadow = 0.f;

	float fDist = length( In.vViewPos.xyz );
	float4 vDepth = 0.f;
	bool bShadow = false;
	
	// 현재 카메라의 위치에 따라 그림자 맵을 다르게 가져온다..
	if ( fDist <= g_fShadowRange[0] )
	{
		vDepth = mul( In.vWorldPos, g_matLightVP[0] ).xyzw;
		shadow = tex2Dproj( ShadowNearSampler, mul( In.vWorldPos, g_matLightVPB[0] ).xyzw );
		bShadow = (shadow + vDepth.z < vDepth.w - 0.03f);
		//bShadow = (shadow < vDepth.z / vDepth.w - 0.01);
	}
	else if ( fDist <= g_fShadowRange[1] )
	{
		vDepth = mul( In.vWorldPos, g_matLightVP[1] ).xyzw;
		shadow = tex2Dproj( ShadowMidSampler, mul( In.vWorldPos, g_matLightVPB[1] ).xyzw );
		bShadow = (shadow + vDepth.z < vDepth.w - 0.03f);
		//bShadow = (shadow < vDepth.z / vDepth.w - 0.01);
	}

	vDepth = vDepth / vDepth.w;

	float3 Color = vResultColor.rgb;

	// if( 그림자에 가려진 부분이다 && 빛이 바라보는 영역 밖에 있지 않음 == 빛 프러스텀 안에 있다 )..
	// 빛 시점에서 투영 z 나누기를 거쳤을 때 x : -1 ~ 1, y : 1 ~ -1 사이에 있다는 것..
	if ( bShadow == true && !(vDepth.x < -1.f || vDepth.x > 1.f || vDepth.y < -1.f || vDepth.y > 1.f) )
	{
		//((shadow < vDepth.z / vDepth.w - 0.01) ? tex2Dproj( ShadowNearSampler, mul( In.vWorldPos, g_matLightVPB[0] ).xyzw ) : 1);

		Color *= ComputeShadowPower( shadow.rg, vDepth.z );
	}

	Out.vColor = float4( Color.rgb, 1.f );


	// 각원소가 -1~1로 표현되기때문에 0~1로 바꾸자.ㅔ
	//float3 vNormalArr[4];
	//vNormalArr[0] = ( Compute_NormalMapping( vColor[0][1].rgb, In.vTangent, In.vBinormal, In.vWorldNormal ) ) * vFilter.a;
	//vNormalArr[1] = ( Compute_NormalMapping( vColor[1][1].rgb, In.vTangent, In.vBinormal, In.vWorldNormal ) ) * vFilter.r;
	//vNormalArr[2] = ( Compute_NormalMapping( vColor[2][1].rgb, In.vTangent, In.vBinormal, In.vWorldNormal ) ) * vFilter.g;
	//vNormalArr[3] = ( Compute_NormalMapping( vColor[3][1].rgb, In.vTangent, In.vBinormal, In.vWorldNormal ) ) * vFilter.b;
	//float3 vNormal = vNormalArr[0] + vNormalArr[1] + vNormalArr[2] + vNormalArr[3];
	float3 vNormal = Compute_NormalMapping( vColor[0][1].rgb, In.vTangent, In.vBinormal, In.vWorldNormal );
	Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.0f, 0.f);
	//Out.vSpec = vColor[0][2] * vFilter.a + vColor[1][2] * vFilter.r + vColor[2][2] * vFilter.g + vColor[3][2] * vFilter.b;
	Out.vSpec = float4( vColor[0][2].rgb, 0.f );

	return Out;
}

PS_OUT PS_MAIN_SQUARE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vResultColor;

	vector	vColor[4][3];
	vColor[0][0] = tex2D(BaseSampler, In.vTexUV * 20.f);
	vColor[0][1] = tex2D(BaseNormalSampler, In.vTexUV * 20.f);
	vColor[0][2] = tex2D(BaseSpecSampler, In.vTexUV * 20.f);

	//vColor[1] = tex2D(DestSampler, In.vTexUV * 20.f);
	//vColor[2] = tex2D(FilterSampler, In.vTexUV);
	vColor[3][0] = (vector)0.f;

	if (g_vBrushPos.x - g_fBrushSize * 0.5f < In.vWorldPos.x && In.vWorldPos.x <= g_vBrushPos.x + g_fBrushSize * 0.5f
		 &&  g_vBrushPos.z - g_fBrushSize * 0.5f < In.vWorldPos.z && In.vWorldPos.z <= g_vBrushPos.z + g_fBrushSize * 0.5f)
	{
		float2		vNewUV;

		vNewUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushSize * 0.5f)) / g_fBrushSize;
		vNewUV.y = ((g_vBrushPos.z + g_fBrushSize * 0.5f) - In.vWorldPos.z) / g_fBrushSize;

		vColor[3][0] = tex2D(BrushSampler, vNewUV);
	}

	//Out.vColor = vColor[0] * vColor[2] + vColor[1] * (1.f - vColor[2]) + vColor[3];	

	Out.vColor = vColor[0][0] + vColor[3][0];
	//Out.vNormal = vector(vColor[0][1].xyz * 0.5f + 0.5f, 0.f);
	//Out.vSpec = vColor[0][2];

	Out.vColor.a = 1.f;

	// 각원소가 -1~1로 표현되기때문에 0~1로 바꾸자.ㅔ
	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vViewPos.z / g_fCamFar, 0.f, 0.0f, 0.f);

	return Out;
}

technique Default_Device
{
	pass DefaultRendering
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass WireFrame_Rendering
	{
		Lighting = false;
		FillMode = wireframe;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

}

