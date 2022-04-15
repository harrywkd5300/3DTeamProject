// ================================================ Gaussian Blur.. ================================================ //
sampler s0 : register(s0);
sampler s1 : register(s1);
sampler s2 : register(s2);
sampler s3 : register(s3);

texture g_Src0Texture;
sampler Sampler_Src0 = sampler_state 
{ 
	texture = g_Src0Texture;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src1Texture;
sampler Sampler_Src1 = sampler_state 
{ 
	texture = <g_Src1Texture>;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src2Texture;
sampler Sampler_Src2 = sampler_state 
{ 
	texture = <g_Src2Texture>;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

texture g_Src3Texture;
sampler Sampler_Src3 = sampler_state 
{ 
	texture = <g_Src3Texture>;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler Sampler_DownSampling = sampler_state 
{
	texture = <g_Src0Texture>;
	//AddressU = Clamp;
	//AddressV = Clamp;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler Sampler_UpSampling = sampler_state 
{ 
	texture = <g_Src0Texture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler Sampler_Luminance = sampler_state
{
	texture = <g_Src0Texture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

float4 g_gaussFilterH[7] =
{
	-3.0, 0.0, 0.0,  1.0 / 64.0,
	-2.0, 0.0, 0.0,  6.0 / 64.0,
	-1.0, 0.0, 0.0, 15.0 / 64.0,
	0.0, 0.0, 0.0, 20.0 / 64.0,
	1.0, 0.0, 0.0, 15.0 / 64.0,
	2.0, 0.0, 0.0,  6.0 / 64.0,
	3.0, 0.0, 0.0,  1.0 / 64.0
};

float4 g_gaussFilterV[7] =
{
	0.0, -3.0, 0.0,  1.0 / 64.0,
	0.0, -2.0, 0.0,  6.0 / 64.0,
	0.0, -1.0, 0.0, 15.0 / 64.0,
	0.0, 0.0, 0.0, 20.0 / 64.0,
	0.0, 1.0, 0.0, 15.0 / 64.0,
	0.0, 2.0, 0.0,  6.0 / 64.0,
	0.0, 3.0, 0.0,  1.0 / 64.0
};

float g_fTexScaler = 1.0f / 128.0f;
float g_fTexOffset = 0.0f;

float g_fTexWidth = 1920.f;
float g_fTexHeight = 1080.f;
float weight[8];
float offsetX = 0.f;
float offsetY = 0.f;

float4 Compute_GaussianFilterH( sampler Sampler_Texture, float2 vTexCoord )
{
	float4 vColor = 0.0;

	int i = 0;
	for ( ; i < 7; ++i )
	{
		vColor += tex2D( Sampler_Texture, float2( vTexCoord.x + g_gaussFilterH[i].x * g_fTexScaler + g_fTexOffset, 
												  vTexCoord.y + g_gaussFilterH[i].y * g_fTexScaler + g_fTexOffset) ) * g_gaussFilterH[i].w;
	}

	vColor *= 4.f;

	return vColor;
}

float4 Compute_GaussianFilterV( sampler Sampler_Texture, float2 vTexCoord )
{
	float4 vColor = 0.0;

	int i = 0;
	for ( ; i < 7; ++i )
	{
		vColor += tex2D( Sampler_Texture, float2( vTexCoord.x + g_gaussFilterV[i].x * g_fTexScaler + g_fTexOffset, 
												  vTexCoord.y + g_gaussFilterV[i].y * g_fTexScaler + g_fTexOffset) ) * g_gaussFilterV[i].w;
	}

	vColor *= 4.f;

	return vColor;
}


// ================================================ DownSampling Blur.. ================================================ //
static const int    MAX_SAMPLES = 16;		// �ִ���ø���
float2 g_avSampleOffsets[MAX_SAMPLES];		// �ֺ� 16 �ؼ��� �������� ���� �迭..
float4 g_avSampleWeights[MAX_SAMPLES];		// ���ø�����ġ

static const float3 LUMINANCE_VECTOR  = float3(0.2125f, 0.7154f, 0.0721f);


// -------------------------- Use Bloom.. -------------------------- //
static const int g_cKernelSize = 13;

float2 PixelKernelH[g_cKernelSize] =
{
	{ -6.f, 0 },
	{ -5.f, 0 },
	{ -4.f, 0 },
	{ -3.f, 0 },
	{ -2.f, 0 },
	{ -1.f, 0 },
	{  0.f, 0 },
	{  1.f, 0 },
	{  2.f, 0 },
	{  3.f, 0 },
	{  4.f, 0 },
	{  5.f, 0 },
	{  6.f, 0 },
};

float2 PixelKernelV[g_cKernelSize] =
{
	{ 0, -6.f },
	{ 0, -5.f },
	{ 0, -4.f },
	{ 0, -3.f },
	{ 0, -2.f },
	{ 0, -1.f },
	{ 0,  0.f },
	{ 0,  1.f },
	{ 0,  2.f },
	{ 0,  3.f },
	{ 0,  4.f },
	{ 0,  5.f },
	{ 0,  6.f },
};

float2 TexelKernelH[g_cKernelSize]
<
	string ConvertPixelsToTexels = "PixelKernelH";
>;

float2 TexelKernelV[g_cKernelSize]
<
	string ConvertPixelsToTexels = "PixelKernelV";
>;

static const float BlurWeights[g_cKernelSize] = 
{
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

float g_fBloomScale = 1.5f;

// -------------------------- Use Tonemap And BrightPass.. -------------------------- //
// x : Middle Gray				== �߰� �׷��� ��( Middle Gray / ��� �ֵ� �� �� ���� ������ �� �����ش� )..
// y : BRIGHT_PASS_THRESHOLD	== �Ѱ��� ( �� �� ���ϴ� �ɷ��� )..
// z : BRIGHT_PASS_DIV			== 0 ~ 1 ������ ������ ����� ���� ���, ���� �������� ����Ʈ�� ���� �� ��� ������Ʈ�� �и��˴ϴ�..
// w : Avg Luminance..			== ��� �ֵ�..
float4	g_vBrightPassData = float4( 0.08f, 5.f, 10.f, 0.05f );

// x : Gamma		== [0 ~ 2] �߰� ���� �����Ͻʽÿ�. 1.000�� �߸��Դϴ�. �� ������ ����Ʈ ���� ������ ������ �����ϸ� ��� ���� �ʿ����� �ʽ��ϴ�.
// y : Exposure		== [-1 ~ 1] ���� ����
// z : Saturation	== [-1 ~ 1] ä��
// w : Bleach		== [0 ~ 1] �׸��ڸ� ��� �ϰ� ������ ����ϰ� ��..
float4	g_vToneMapData = float4( 1.f, 0.f, 0.f, 0.f );

// x : Defog		== [0 ~ 1] ������ ������ ��..
// y : Bloom Scale	== ��뿡 ������ ��..
// z : CrossFilter Scale	== ũ�ν� ���Ϳ� ������ ��..
// w : �Ⱦ�			== �Ⱦ�..
float4	g_vToneMapData2 = float4( 1.f, 0.f, 0.f, 0.f );


float	g_fElapsedTime = 0.1f;

float Compute_Luminance( float3 vRGB )
{
	return max( dot( vRGB, float3( 0.2125f, 0.7154f, 0.0721f ) ), 0.00001f );
}

///////////////////////////////////////////////////////////////////////////
// UnCharted2_ToonMaping????..
///////////////////////////////////////////////////////////////////////////
float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

float3 CalcExposedColor( float3 color, float avgLuminance, float offset, out float exposure, float fKeyValue, float fBrightPassDiv )
{
	//// Use geometric mean        
	//avgLuminance = max(avgLuminance, 0.001f);
	//
	//float keyValue = 1.03f - (2.0f / (2 + log10(avgLuminance + 1)));
	//
	//float linearExposure = (keyValue / avgLuminance);
	//exposure = log2(max(linearExposure, 0.0001f));
	//
	//exposure -= g_vToneMapData.z;

	// Use geometric mean
	avgLuminance = max( avgLuminance, 0.001f );
	float linearExposure = ( fKeyValue / avgLuminance );
	
	exposure = log2( max( linearExposure, 0.0001f ) );
	exposure += offset;
	exposure -= fBrightPassDiv;

	return exp2( exposure ) * color;
}

float3 UnCharted2_ToneMaping( float3 vColor )
{
	// Uncharted2ToonMaping..
	float ExposureBias = 2.0f;
	vColor = ExposureBias * vColor;
	float3 curr = ( ( vColor * ( A * vColor + C * B ) + D * E ) / ( vColor * ( A * vColor + B ) + D * F ) ) - E / F;

	float3 whiteScale = 1.0f / ( ( vColor * ( A * vColor + C * B ) + D * E ) / ( vColor * ( A * vColor + B ) + D * F ) ) - E / F;
	float3 color = curr * whiteScale;

	float3 retColor = max( 0.f, color );

	return retColor;
	//return pow( max( 0.f, retColor ), 1 / 2.2f );
}

float3 Flimic_ToneMaping( float3 vColor )
{
	float3 x = max( 0, vColor - 0.004f );
	float3 retColor = ( x * ( 6.2f * x + 0.5f ) ) / ( x * ( 6.2f * x + 1.7f ) + 0.06f );

	return retColor;
	//return pow( max( 0.f, retColor ), 2.2f );
}

float3 toneMap(float3 Color, float Luminance, float fKeyValue, float fBrightPassDiv)
{
	float exposure = 0.f;
	Color = CalcExposedColor( Color, Luminance, 0.f, exposure, fKeyValue, fBrightPassDiv );

	//Color = UnCharted2_ToneMaping( Color );
	Color = Flimic_ToneMaping( Color );

	return Color;
	//return pow( Color, 2.2f );
}