#include "Default.hpp"
#include "Filter.hpp"

//#define GETAVGLUMINANCE_AND_USEAVGLUMINANCE // 평균 휘도를 추출하고 그것을 사용할 것인가??..

struct VS_IN
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUT_TEX8
{
	float4 vPosition : POSITION;
	float2 vTexCoord0 : TEXCOORD0;
	float2 vTexCoord1 : TEXCOORD1;
	float2 vTexCoord2 : TEXCOORD2;
	float2 vTexCoord3 : TEXCOORD3;
	float2 vTexCoord4 : TEXCOORD4;
	float2 vTexCoord5 : TEXCOORD5;
	float2 vTexCoord6 : TEXCOORD6;
	float2 vTexCoord7 : TEXCOORD7;
};

struct PS_IN
{
	float2 vTexCoord : TEXCOORD0;
};

struct PS_OUT
{
	float4 vColor : COLOR0;
};

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = (VS_OUT)0.f;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;

	return Output;
}

VS_OUT_TEX8 vs_mainGaussWidthTex8( VS_IN Input )
{
	VS_OUT_TEX8 Output = (VS_OUT_TEX8)0;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );

	Output.vTexCoord0 = Input.vTexCoord + float2(-1.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord1 = Input.vTexCoord + float2(-3.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord2 = Input.vTexCoord + float2(-5.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord3 = Input.vTexCoord + float2(-7.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord4 = Input.vTexCoord + float2(-9.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord5 = Input.vTexCoord + float2(-11.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord6 = Input.vTexCoord + float2(-13.0f / g_fTexWidth, 0.0f);
	Output.vTexCoord7 = Input.vTexCoord + float2(-15.0f / g_fTexWidth, 0.0f);

	return Output;
}

VS_OUT_TEX8 vs_mainGaussHeightTex8( VS_IN Input )
{
	VS_OUT_TEX8 Output = (VS_OUT_TEX8)0;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );

	Output.vTexCoord0 = Input.vTexCoord + float2(0.0f, -1.0f / g_fTexHeight);
	Output.vTexCoord1 = Input.vTexCoord + float2(0.0f, -3.0f / g_fTexHeight);
	Output.vTexCoord2 = Input.vTexCoord + float2(0.0f, -5.0f / g_fTexHeight);
	Output.vTexCoord3 = Input.vTexCoord + float2(0.0f, -7.0f / g_fTexHeight);
	Output.vTexCoord4 = Input.vTexCoord + float2(0.0f, -9.0f / g_fTexHeight);
	Output.vTexCoord5 = Input.vTexCoord + float2(0.0f, -11.0f / g_fTexHeight);
	Output.vTexCoord6 = Input.vTexCoord + float2(0.0f, -13.0f / g_fTexHeight);
	Output.vTexCoord7 = Input.vTexCoord + float2(0.0f, -15.0f / g_fTexHeight);

	return Output;
}

// ------------------------------ Gaussian Filter Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_gaussianFilterH
// Desc: Horizontal GaussFilter
//-----------------------------------------------------------------------------
PS_OUT ps_gaussianFilterH( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	Output.vColor = Compute_GaussianFilterH( Sampler_Src0, Input.vTexCoord );

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_gaussianFilterV
// Desc: Vertical GaussFilter
//-----------------------------------------------------------------------------
PS_OUT ps_gaussianFilterV( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	Output.vColor = Compute_GaussianFilterV( Sampler_Src0, Input.vTexCoord );

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_gaussianFilter2H
// Desc: Horizontal GaussFilter
//-----------------------------------------------------------------------------
PS_OUT ps_gaussianFilter2H(VS_OUT_TEX8 In)
{
	PS_OUT Output = (PS_OUT)0;

	float4 Color;

	Color = weight[0] * (tex2D( Sampler_UpSampling, In.vTexCoord0 )
						  + tex2D( Sampler_UpSampling, In.vTexCoord7 + offsetX ));
	Color += weight[1] * (tex2D( Sampler_UpSampling, In.vTexCoord1 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord6 + offsetX ));
	Color += weight[2] * (tex2D( Sampler_UpSampling, In.vTexCoord2 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord5 + offsetX ));
	Color += weight[3] * (tex2D( Sampler_UpSampling, In.vTexCoord3 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord4 + offsetX ));
	Color += weight[4] * (tex2D( Sampler_UpSampling, In.vTexCoord4 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord3 + offsetX ));
	Color += weight[5] * (tex2D( Sampler_UpSampling, In.vTexCoord5 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord2 + offsetX ));
	Color += weight[6] * (tex2D( Sampler_UpSampling, In.vTexCoord6 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord1 + offsetX ));
	Color += weight[7] * (tex2D( Sampler_UpSampling, In.vTexCoord7 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord0 + offsetX ));

	Output.vColor = Color;

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_gaussianFilter2V
// Desc: Vertical GaussFilter
//-----------------------------------------------------------------------------
PS_OUT ps_gaussianFilter2V( VS_OUT_TEX8 In )
{
	PS_OUT Output = (PS_OUT)0;

	float4 Color;

	Color = weight[0] * (tex2D( Sampler_UpSampling, In.vTexCoord0 )
						  + tex2D( Sampler_UpSampling, In.vTexCoord7 + offsetY ));
	Color += weight[1] * (tex2D( Sampler_UpSampling, In.vTexCoord1 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord6 + offsetY ));
	Color += weight[2] * (tex2D( Sampler_UpSampling, In.vTexCoord2 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord5 + offsetY ));
	Color += weight[3] * (tex2D( Sampler_UpSampling, In.vTexCoord3 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord4 + offsetY ));
	Color += weight[4] * (tex2D( Sampler_UpSampling, In.vTexCoord4 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord3 + offsetY ));
	Color += weight[5] * (tex2D( Sampler_UpSampling, In.vTexCoord5 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord2 + offsetY ));
	Color += weight[6] * (tex2D( Sampler_UpSampling, In.vTexCoord6 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord1 + offsetY ));
	Color += weight[7] * (tex2D( Sampler_UpSampling, In.vTexCoord7 )
						   + tex2D( Sampler_UpSampling, In.vTexCoord0 + offsetY ));

	Output.vColor = Color;

	return Output;
}


// ------------------------------ Scale Filter Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_downFilterX4
// Desc: Perform a high-pass filter and on the source texture and scale down.
//-----------------------------------------------------------------------------
PS_OUT ps_downFilterX4( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vSample = 0.0f;

	for ( int i = 0; i < 16; i++ ) {
		vSample += tex2D( Sampler_DownSampling, Input.vTexCoord + g_avSampleOffsets[i].xy );
	}

	Output.vColor = vSample / 16;

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_upFilter
// Desc: 크기가 작은 텍스쳐에서 큰 텍스쳐로 옮길 때 사용..
//-----------------------------------------------------------------------------
PS_OUT ps_upFilter( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	Output.vColor = tex2D( Sampler_UpSampling, Input.vTexCoord );

	return Output;
}


// ------------------------------ BrightPass Filter Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_BrightPassFilter
// Desc: Perform a high-pass filter on the source texture
//-----------------------------------------------------------------------------
PS_OUT ps_BrightPassFilter( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vSample = tex2D( Sampler_Src0, Input.vTexCoord );
#ifdef GETAVGLUMINANCE_AND_USEAVGLUMINANCE
	float	fAdaptedLum = tex2D( Sampler_Src1, float2(0.5f, 0.5f) ).r;
#else
	float	fAdaptedLum = g_vBrightPassData.w;
#endif

	// Determine what the pixel's value will be after tone-mapping occurs
	vSample.rgb *= g_vBrightPassData.x / (fAdaptedLum + 0.001f);

	// 어두운 픽셀을 빼는 작업 중..
	vSample.rgb -= g_vBrightPassData.y;

	// 0 보다 작다면 0 으로..
	vSample = max( vSample, 0.0f );

	vSample.rgb /= (g_vBrightPassData.z + vSample.x);

	Output.vColor = vSample;

	return Output;
}


// ------------------------------ Bloom Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_bloomH
// Desc: Blurs the image horizontally
//-----------------------------------------------------------------------------
PS_OUT ps_bloomH( float2 Tex : TEXCOORD0 )
{
	PS_OUT Output = (PS_OUT)0;

	float4 Color = 0;

	for ( int i = 0; i < g_cKernelSize; i++ )
		Color += tex2D( Sampler_Src0, Tex + TexelKernelH[i].xy ) * BlurWeights[i];

	Output.vColor = Color * g_fBloomScale;

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_bloomV
// Desc: Blurs the image vertically
//-----------------------------------------------------------------------------
PS_OUT ps_bloomV( float2 Tex : TEXCOORD0 )
{
	PS_OUT Output = (PS_OUT)0;

	float4 Color = 0;

	for ( int i = 0; i < g_cKernelSize; i++ )
		Color += tex2D( Sampler_Src0, Tex + TexelKernelV[i].xy ) * BlurWeights[i];

	Output.vColor = Color * g_fBloomScale;

	return Output;
}


// ------------------------------ ToneMap Filter Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ToneMapFilter
// Desc: Perform a tone map filter on the source texture
//-----------------------------------------------------------------------------
PS_OUT ps_toneMapFilter( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	//////////////////////////////////////////////////////////////////////////////////
	// ToneMap : https://delightlylinux.wordpress.com/2014/03/10/sweetfx-tonemap/
	//////////////////////////////////////////////////////////////////////////////////
	float4 colorInput = tex2D( Sampler_Src0, Input.vTexCoord );

	float3 color = colorInput.rgb;

	color = saturate( color - g_vToneMapData2.x );	// 제거할 색조를 제거해버리기..

	color *= pow( 2.0f, g_vToneMapData.y ); // Exposure

	color = pow( color, g_vToneMapData.x );	// 감마 보정 작업 중..

	//#define BlueShift 0.00	//Blueshift
	//float4 d = color * float4(1.05f, 0.97f, 1.27f, color.a);
	//color = lerp(color, d, BlueShift);

	// 현재 픽셀의 휘도를 구한다..
	float3 lumCoeff = float3(0.2126, 0.7152, 0.0722);
	float lum = dot( lumCoeff, color.rgb );

	float3 blend = lum.rrr; //dont use float3

	float L = saturate( 10.0 * (lum - 0.45) );

	float3 result1 = 2.0f * color.rgb * blend;
	float3 result2 = 1.0f - 2.0f * (1.0f - blend) * (1.0f - color.rgb);

	float3 newColor = lerp( result1, result2, L );
	//float A2 = g_vToneMapData.w * color.rgb;
	float3 A2 = g_vToneMapData.w * color.rgb;
	float3 mixRGB = A2 * newColor;

	color.rgb += ((1.0f - A2) * mixRGB);

	//float3 middlegray = float(color.r + color.g + color.b) / 3;
	float3 middlegray = dot( color, (1.0 / 3.0) ); //1fps slower than the original on nvidia, 2 fps faster on AMD

	float3 diffcolor = color - middlegray; //float 3 here
	colorInput.rgb = (color + diffcolor * g_vToneMapData.z) / (1 + (diffcolor*g_vToneMapData.z)); //saturation

	float3 vBloom = tex2D( Sampler_Src2, Input.vTexCoord ).rgb;
	float3 vCrossFilter = tex2D( Sampler_Src3, Input.vTexCoord ).rgb;

	// 최종 색에 블룸, 크로스 필터 값 더해주기..
	colorInput.rgb += vBloom * g_vToneMapData2.y + vCrossFilter * g_vToneMapData2.z;

	Output.vColor = colorInput;

	return Output;
}



// ------------------------------ AvgLuminance Filter Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_luminanceInit
// Desc: 처음 Scene 텍스쳐에서 Luminance 를 추출한다..
//-----------------------------------------------------------------------------
PS_OUT ps_luminanceInit( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float3 vSample = 0.0f;
	float  fLogLumSum = 0.0f;

	for ( int iSample = 0; iSample < 9; iSample++ )
	{
		// Compute the sum of log(luminance) throughout the sample points
		vSample = tex2D( Sampler_Luminance, Input.vTexCoord.xy + g_avSampleOffsets[iSample].xy ).rgb;
		fLogLumSum += log( dot( vSample.rgb, LUMINANCE_VECTOR ) + 0.0001f );
	}

	// Divide the sum to complete the average
	fLogLumSum /= 9;

	Output.vColor = float4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0f);

	return Output;
}

//-----------------------------------------------------------------------------
// Name: ps_luminanceIterative                                     
// Desc: 추출한 Luminance 를 계속 다운 샘플링 한다..
//-----------------------------------------------------------------------------
PS_OUT ps_luminanceIterative( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fResampleSum = 0.0f;

	for ( int iSample = 0; iSample < 16; iSample++ )
	{
		// Compute the sum of luminance throughout the sample points
		fResampleSum += tex2D( Sampler_Luminance, Input.vTexCoord + g_avSampleOffsets[iSample] ).r;
	}

	// Divide the sum to complete the average
	fResampleSum /= 16;

	Output.vColor = float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);

	return Output;
}

//-----------------------------------------------------------------------------
// Name: ps_luminanceFinal                                    
// Desc: 4x4 Luminance 텍스쳐를 1x1 Luminance 텍스쳐로 옮기는 작업..
//-----------------------------------------------------------------------------
PS_OUT ps_luminanceFinal( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fResampleSum = 0.0f;

	for ( int iSample = 0; iSample < 16; iSample++ )
	{
		// Compute the sum of luminance throughout the sample points
		fResampleSum += tex2D( Sampler_Luminance, Input.vTexCoord + g_avSampleOffsets[iSample] ).r;
	}

	// Divide the sum to complete the average, and perform an exp() to complete
	// the average luminance calculation
	fResampleSum = exp( fResampleSum / 16 );

	Output.vColor = float4(fResampleSum, fResampleSum, fResampleSum, 1.0f);

	return Output;
}

//-----------------------------------------------------------------------------
// Name: ps_calcAdaptedLuminance
// Desc: 
//-----------------------------------------------------------------------------
PS_OUT ps_calcAdaptedLuminance( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fAdaptedLum = tex2D( Sampler_Src0, float2(0.5f, 0.5f) ).r;
	float fCurrentLum = tex2D( Sampler_Src1, float2(0.5f, 0.5f) ).r;

	// The user's adapted luminance level is simulated by closing the gap between
	// adapted luminance and current luminance by 2% every frame, based on a
	// 30 fps rate. This is not an accurate model of human adaptation, which can
	// take longer than half an hour.
	float fNewAdaptation = fAdaptedLum + (fCurrentLum - fAdaptedLum) * (1 - pow( 0.98f, 30 * g_fElapsedTime ));
	Output.vColor = float4(fNewAdaptation, fNewAdaptation, fNewAdaptation, 1.0f);

	return Output;
}


//-----------------------------------------------------------------------------
// Name: ps_star
// Desc: 가져온 텍스쳐를 늘려준다..
//-----------------------------------------------------------------------------
PS_OUT ps_star( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vSample = 0.0f;
	float4 vColor = 0.0f;

	float2 vSamplePosition;

	// Sample from eight points along the star line
	for ( int iSample = 0; iSample < 8; iSample++ )
	{
		vSamplePosition = Input.vTexCoord + g_avSampleOffsets[iSample];
		vSample = tex2D( s0, vSamplePosition );
		vColor += g_avSampleWeights[iSample] * vSample;
	}

	Output.vColor = vColor;

	return Output;
}

//-----------------------------------------------------------------------------
// Name: ps_mergeTextures_4PS
// Desc: 늘린 4개의 텍스쳐를 합쳐 하나의 Closs 모양을 만든다..
//-----------------------------------------------------------------------------
PS_OUT ps_mergeTextures_4PS( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor = 0.0f;

	vColor += g_avSampleWeights[0] * tex2D( s0, Input.vTexCoord );
	vColor += g_avSampleWeights[1] * tex2D( s1, Input.vTexCoord );
	vColor += g_avSampleWeights[2] * tex2D( s2, Input.vTexCoord );
	vColor += g_avSampleWeights[3] * tex2D( s3, Input.vTexCoord );

	Output.vColor = vColor;

	return Output;
}


technique GaussianFilter
{
	pass GaussianFilterH
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_gaussianFilterH();
	}

	pass GaussianFilterV
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_gaussianFilterV();
	}

	pass GaussianFilter2H
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_mainGaussWidthTex8();
		PixelShader = compile ps_3_0 ps_gaussianFilter2H();
	}

	pass GaussianFilter2V
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_mainGaussHeightTex8();
		PixelShader = compile ps_3_0 ps_gaussianFilter2V();
	}
}

technique Bloom
{
	pass BloomH
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_bloomH();
	}

	pass BloomV
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_bloomV();
	}
}

technique DownFilter
{
	pass DownFilterX4
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_downFilterX4();
	}

	pass UpFilterX4
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_upFilter();
	}
}

technique BrightPass
{
	pass BrightPass
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_BrightPassFilter();
	}
}

technique Tonemap
{
	pass Default
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_toneMapFilter();
	}
}

technique AvgLuminance
{
	pass Init
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		minfilter[0] = linear;
		magfilter[0] = linear;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_luminanceInit();
	}

	pass Iterative
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		minfilter[0] = point;
		magfilter[0] = point;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_luminanceIterative();
	}

	pass Final
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		minfilter[0] = point;
		magfilter[0] = point;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_luminanceFinal();
	}

	pass Final
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		minfilter[0] = point;
		magfilter[0] = point;
		minfilter[1] = point;
		magfilter[1] = point;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_calcAdaptedLuminance();
	}
}

technique ClossFilter
{
	pass Star
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		minfilter[0] = linear;
		magfilter[0] = linear;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_star();
	}

	pass Merge
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		minfilter[0] = linear;
		magfilter[0] = linear;
		minfilter[1] = linear;
		magfilter[1] = linear;
		minfilter[2] = linear;
		magfilter[2] = linear;
		minfilter[3] = linear;
		magfilter[3] = linear;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_mergeTextures_4PS();
	}
};