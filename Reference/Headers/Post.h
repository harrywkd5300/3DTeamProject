/*======================================================================================
                             POST FXAA RENDERING PASSES
======================================================================================*/

//These values are normally defined by the injector dlls, but not when analyzed by GPU Shaderanalyzer
//I need to ensure they always have a value to be able to compile them whenever I'm not using the injector.
#ifdef SMAA_PIXEL_SIZE
#ifndef BUFFER_RCP_WIDTH
#define BUFFER_RCP_WIDTH SMAA_PIXEL_SIZE.x
#define BUFFER_RCP_HEIGHT SMAA_PIXEL_SIZE.y
#define BUFFER_WIDTH (1.0 / SMAA_PIXEL_SIZE.x)
#define BUFFER_HEIGHT (1.0 / SMAA_PIXEL_SIZE.y)
#endif
#endif

#ifndef BUFFER_RCP_WIDTH
#define BUFFER_RCP_WIDTH (1.0 / 1920.0)
#define BUFFER_RCP_HEIGHT (1.0 / 1080.0)
#define BUFFER_WIDTH 1920
#define BUFFER_HEIGHT 1080
#endif

#define screen_size float2(BUFFER_WIDTH,BUFFER_HEIGHT)

#define pixel float2(px,py)

// -- Define DirectX9 FXAA specific aliases --
#if FXAA_HLSL_3 == 1
#define myTex2D(s,p) tex2D(s,p)

//#define s0 colorTexG
//#define s1 colorTexG //TODO make a nearest sampler if needed
#endif

// -- Define DirectX10/11 FXAA specific aliases --
#if FXAA_HLSL_4 == 1
//#define myTex2D(s,p) s.SampleLevel(screenSampler, p, 0)
#define myTex2D(s,p) s.Sample(screenSampler, p)

#define s0 gLumaTexture
#define s1 gLumaTexture //TODO make a nearest sampler if needed
#endif


// -- Define DirectX9 specific aliases --
#if SMAA_HLSL_3 == 1
#define myTex2D(s,p) tex2D(s,p)

#define s0 colorTexG
#define s1 colorTexG //TODO make a nearest sampler if needed
#endif

// -- Define DirectX10/11 specific aliases --
#if SMAA_HLSL_4 == 1 || SMAA_HLSL_4_1 == 1
//#define myTex2D(s,p) s.SampleLevel(LinearSampler, p, 0)
#define myTex2D(s,p) s.Sample(LinearSampler, p)

#define s0 colorTexGamma
#define s1 colorTexGamma //TODO make a nearest sampler if needed
#endif



#if (FXAA_HLSL_4 == 1 || FXAA_HLSL_5 == 1)
#define tex2D(s,p) gScreenTexture.SampleLevel(s,p,0)
#define s0 screenSampler
#define width BUFFER_WIDTH
#define height BUFFER_HEIGHT
#define px BUFFER_RCP_WIDTH
#define py BUFFER_RCP_HEIGHT
#define dx ((AverageBlur * 0.05)*px)
#define dy ((AverageBlur * 0.05)*py)
#else
#define s0 lumaSampler
#define width BUFFER_WIDTH
#define height BUFFER_HEIGHT
#define px BUFFER_RCP_WIDTH
#define py BUFFER_RCP_HEIGHT
#define dx ((AverageBlur * 0.05)*px)
#define dy ((AverageBlur * 0.05)*py)
#endif
/*------------------------------------------------------------------------------
						FILTER TO USE CHECK
------------------------------------------------------------------------------*/
#if (USE_CUSTOM == 1)
#include "CustomShader.h"
#endif
#if (USE_PRE_SHARPEN == 1)
#include "PreSharpen.h"
#endif
#if (USE_HDR == 1)
#include "HDR.h"
#endif
#if (USE_BLOOM == 1)
#include "Bloom.h"
#endif
#if (USE_TONEMAP == 1)
#include "Tonemap.h"
#endif
#if (USE_TECHNICOLOR == 1)
#include "Technicolor.h"
#endif
#if (USE_SEPIA == 1)
#include "Sepia.h"
#endif
#if (USE_NOISE == 1)
#include "Noise.h"
#endif
#if (USE_RADIAL == 1)
#include "Radial.h"
#endif
#if (USE_SCANLINES == 1)
#include "ScanLines.h"
#endif
#if (USE_ANAGLYPH == 1)
#include "Anaglyph.h"
#endif
#if (USE_VIGNETTE == 1)
#include "Vignette.h"
#endif
#if (USE_POST_SHARPEN == 1)
#include "PostSharpen.h"
#endif
#if (USE_FINAL_LIMITER == 1)
#include "FinalLimiter.h"
#endif
#if (USE_SPLITSCREEN == 1)
#include "SplitScreen.h"
#endif
/*------------------------------------------------------------------------------
						RENDERING PASSES
------------------------------------------------------------------------------*/
bool g_UseSepia = false;

float4 main( float2 tex )
{
	// PreSharpenPass (has to be the first pass because it samples multiple texels)
	#if (USE_PRE_SHARPEN == 1)
		float4 pass1 = SharpenPass(tex);
	#else
		float4 pass1 = tex2D(s0,tex);
	#endif
	// Shader1
	#if (Check1 == 1)
		float4 pass2 = Shader1(pass1,tex);
	#else
		float4 pass2 = pass1;
	#endif
	// Shader2
	#if (Check2 == 1)
		float4 pass3 = Shader2(pass2,tex);
	#else
		float4 pass3 = pass2;
	#endif
	// Shader3
	#if (Check3 == 1)
		float4 pass4 = Shader3(pass3, tex);
	#else
		float4 pass4 = pass3;
	#endif
	// Shader4
	#if (Check4 == 1)
		float4 pass5 = Shader4(pass4, tex);
	#else
		float4 pass5 = pass4;
	#endif
	// Shader5
	//#if (Check5 == 1)
		float4 pass6;
		if (g_UseSepia)
		{
			pass6 = Shader5(pass5, tex);
		}
		else
		{
			pass6 = pass5;
		}
	//#else
		
	//#endif
	// Shader6
	#if (Check6 == 1)
		float4 pass7 = Shader6(pass6,tex);
	#else
		float4 pass7 = pass6;
	#endif	
	// Shader7
	#if (Check7 == 1)
		float4 pass8 = Shader7(pass7,tex);
	#else
		float4 pass8 = pass7;
	#endif	
	// Shader8
	#if (Check8 == 1)
		float4 pass9 = Shader8(pass8,tex);
	#else
		float4 pass9 = pass8;
	#endif	
	// Shader9
	#if (Check9 == 1)
		float4 pass10 = Shader9(pass9,tex);
	#else
		float4 pass10 = pass9;
	#endif
	//VignettePass
	#if (USE_VIGNETTE == 1)
		float4 pass11 = VignettePass(pass10, tex);
	#else
		float4 pass11 = pass10;
	#endif
	// AnaglyphPass
	#if (USE_ANAGLYPH == 1)
		float4 pass12 = AnaglyphPass(pass11,tex);
	#else
		float4 pass12 = pass11;
	#endif
	// PostSharpenPass
	#if (USE_POST_SHARPEN == 1)
		float4 pass13 = PostSharpenPass (pass12,tex);
	#else
		float4 pass13 = pass12;
	#endif
	// FinalLimiterPass (always last shader)
	#if (USE_FINAL_LIMITER == 1)
		float4 pass14 = limiter (pass13,tex);
	#else
		float4 pass14 = pass13;
	#endif
	// SplitScreenPass (after all shaders)
	#if (USE_SPLITSCREEN == 1)
		float4 pass15 = SplitScreenPass(pass14,tex);
	#else
		float4 pass15 = pass14;
	#endif
	return pass15;
}