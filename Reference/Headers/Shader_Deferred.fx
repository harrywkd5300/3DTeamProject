#include "Deferred.hpp"

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = (VS_OUT)0.f;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;

	return Output;
}

VS_OUT_FRUSTUMPOS vs_mainFrustumPos( VS_IN Input )
{
	VS_OUT_FRUSTUMPOS Output = (VS_OUT_FRUSTUMPOS)0.f;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;
	Output.vFrustumPos = Compute_Frustum( Input.vTexCoord, g_fCamFar, g_matInvProj );

	return Output;
}

// ---------------------------- LightRendering.. ---------------------------- //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_BlendRendering
// Desc: Compute Default Light and Color And RimLight..
//-----------------------------------------------------------------------------
PS_OUT ps_frameBufferLight( PS_IN Input )
{
	PS_OUT Output = ( PS_OUT )0.f;

	float4 vColor1 = tex2D( Sampler_Src0, Input.vTexCoord );		// Color..
	float4 vColor2 = tex2D( Sampler_Src1, Input.vTexCoord );		// Light Shade..
	float4 vColor3 = tex2D( Sampler_Src2, Input.vTexCoord );		// Light Specular..
	float4 vColor4 = tex2D( Sampler_Src3, Input.vTexCoord );		// RimLight..
	float3 vEmissive = tex2D( Sampler_Src4, Input.vTexCoord ).bbb;		// Specular Intensity..

	vEmissive *= vColor1.rgb;

	vColor4.rgb = g_vRimLightData.rgb * vColor4.aaa * vColor1.rgb * vColor3.a;

	float4 vResultColor = float4((vColor1.rgb * vColor2.rgb + vColor3.rgb + vColor4.rgb + vEmissive), vColor1.a);

	Output.vColor = vResultColor;

	return Output;
}


// ---------------------------- No Light.. ---------------------------- //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_mainDefault
// Desc: Render RenderTarget..
//-----------------------------------------------------------------------------
PS_OUT ps_mainDefault( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	Output.vColor = tex2D( Sampler_Src0, Input.vTexCoord );

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_frameBuffer
// Desc: Render Color And RimLight..
//-----------------------------------------------------------------------------
PS_OUT ps_frameBuffer( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor1 = tex2D( Sampler_Src0, Input.vTexCoord );	// Color..
	float4 vColor2 = tex2D( Sampler_Src1, Input.vTexCoord );	// RimLight..
	vColor2.rgb = g_vRimLightData.rgb * vColor2.r;

	float4 vResultColor = float4(vColor1.rgb + (vColor1.rgb * vColor2.rgb), vColor1.a);

	Output.vColor = vResultColor;
	Output.vColor = vColor1;

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_mainPriorityRender
// Desc: Render Priority..
//-----------------------------------------------------------------------------
PS_OUT ps_mainPriorityRender( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor1 = tex2D( Sampler_Src0, Input.vTexCoord );	// Color..

	Output.vColor = float4(vColor1.rgb, 0.f);

	return Output;
}

// ---------------------------- FogRendering.. ---------------------------- //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_mainFog
// Desc: Render Sceen And Fog..
//-----------------------------------------------------------------------------
PS_OUT ps_mainFog( PS_IN_FRUSTUMPOS Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor1 = tex2D( Sampler_Src0, Input.vTexCoord );			// Pixel Color..
	float fFixelDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;		// Pixel Depth..

	vColor1.rgb = Compute_Fog( fFixelDepth, Input.vFrustumPos.xyz, vColor1.rgb );

	Output.vColor = vColor1;

	return Output;
}


// ---------------------------- Debug.. ---------------------------- //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_debugTarget
// Desc: Render DebugTarget..
//-----------------------------------------------------------------------------
int g_iDebugOption = 0;
PS_OUT ps_debugTarget( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor = tex2D( Sampler_Src0, Input.vTexCoord );

	if ( g_iDebugOption == 0 )
		Output.vColor = vColor;
	else if ( g_iDebugOption == 1 )
	{
		vColor.a /= 3.f;
		//if ( vColor.a <= 85.f )
		//	Output.vColor = float4(vColor.a * 3.f, 0.f, 0.f, 1.f);
		//else if ( vColor.a <= 170.f )
		//	Output.vColor = float4(0.f, (vColor.a - 85.f) * 3.f, 0.f, 1.f);
		//else
		//	Output.vColor = float4(0.f, 0.f, (vColor.a - 170.f) * 3.f, 1.f);
		Output.vColor = vColor.a;
	}
	else if ( g_iDebugOption == 2 )
		Output.vColor = float4(vColor.r, 0.f, 0.f, vColor.a);
	else if ( g_iDebugOption == 3 )
		Output.vColor = float4(0.f, vColor.g, 0.f, vColor.a);
	else if ( g_iDebugOption == 4 )
		Output.vColor = float4(0.f, 0.f, vColor.b, vColor.a);
	else if ( g_iDebugOption == 5 )
		Output.vColor = float4(Input.vTexCoord, 0.f, vColor.a);
	//Output.vColor = vColor;

	return Output;
}

// ------------------------------ SSAO Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_ssao
// Desc: Compute Ambient Power??..
//-----------------------------------------------------------------------------
PS_OUT ps_ssao( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fRadius = g_vSSAOData.y; 
	float fPixelDepth = tex2D( Sampler_Depth, Input.vTexCoord.xy ).r;
	float fDepth = fPixelDepth * g_fCamFar;
	float3 vKernelScale = float3(fRadius / fDepth, fRadius / fDepth, fRadius / g_fCamFar);

	float fOcclusion = 0.0f;

	// 랜덤한 방향 벡터를 얻는다..
	float3 random = tex2D( RandNormal, Input.vTexCoord.xy * (7.0f + 1.f) ).xyz;
	random = random * 2.0f - 1.0f;

	for ( int i = 0; i < 8; ++i )
	{
		float3 vRotatedKernel = reflect( g_avKernel[i], random ) * vKernelScale;
		float fSampleDepth = tex2D( Sampler_Depth, vRotatedKernel.xy + Input.vTexCoord.xy ).r;
		float fDelta = max( fSampleDepth - fPixelDepth + vRotatedKernel.z, 0 );
		float fRange = abs( fDelta ) / (vKernelScale.z * g_vSSAOData.z);
		fOcclusion += lerp( fDelta * g_vSSAOData.w, g_vSSAOData.x, saturate( fRange ) );
	}

	/* 2 중 for 문 사용 할 때.. */
	//for ( int j = 1; j < 2; ++j )
	//{
	//	// 랜덤한 방향 벡터를 얻는다..
	//	float3 random = tex2D( RandNormal, Input.vTexCoord.xy * ( 7.0f + (float)j ) ).xyz;
	//	random = random * 2.0f - 1.0f;
	//
	//	for ( int i = 0; i < 8; ++i )
	//	{
	//		float3 vRotatedKernel = reflect( g_avKernel[i], random ) * vKernelScale;
	//		float fSampleDepth = tex2D( Sampler_Depth, vRotatedKernel.xy + Input.vTexCoord.xy ).r;
	//		float fDelta = max( fSampleDepth - fPixelDepth + vRotatedKernel.z, 0 );
	//		float fRange = abs( fDelta ) / ( vKernelScale.z * g_vSSAOData.z );
	//		fOcclusion += lerp( fDelta * g_vSSAOData.w, g_vSSAOData.x, saturate( fRange ) );
	//	}
	//}

	Output.vColor = fOcclusion / ( 1.f * 8.0f ); 

	// 범위 재매핑
	Output.vColor = lerp( 0.1f, 0.6, saturate( Output.vColor.x ) );

	return Output;
}

// ------------------------------ EdgeDetection Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_rimLight
// Desc: Compute Edge ( Check Normal and camera view dir )..
//-----------------------------------------------------------------------------
PS_OUT ps_rimLight( PS_IN_FRUSTUMPOS Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;
	float3 vNormal = tex2D( Sampler_Normal, Input.vTexCoord ).xyz * 2.f - 1.f;

	float4 vPosition = Compute_PosWS( Input.vFrustumPos, fDepth, g_matInvView );

	// Edge 를 검출하는 부분..
	float fIntensity = RimLightIntensity( g_vCamPos.xyz, vPosition.xyz, vNormal.xyz, g_vRimLightData.w );

	Output.vColor = fIntensity;

	return Output;
}

float width = 1920.f;

float4 sampleAs3DTexture(sampler2D tex, float3 uv, float width) {
	float sliceSize = 1.0 / width;              // space of 1 slice
	float slicePixelSize = sliceSize / width;           // space of 1 pixel
	float sliceInnerSize = slicePixelSize * (width - 1.0);  // space of width pixels
	float zSlice0 = min(floor(uv.z * width), width - 1.0);
	float zSlice1 = min(zSlice0 + 1.0, width - 1.0);
	float xOffset = slicePixelSize * 0.5 + uv.x * sliceInnerSize;
	float s0 = xOffset + (zSlice0 * sliceSize);
	float s1 = xOffset + (zSlice1 * sliceSize);
	float4 slice0Color = tex2D(tex, float2(s0, uv.y));
	float4 slice1Color = tex2D(tex, float2(s1, uv.y));
	float zOffset = fmod(uv.z * width, 1.0);
	float4 result = lerp(slice0Color, slice1Color, zOffset);
	return result;
}

// ------------------------------ EdgeDetection Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_rimLight
// Desc: Compute Edge ( Check Normal and camera view dir )..
//-----------------------------------------------------------------------------
PS_OUT ps_mainColorGrading( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	// LUT Scale =  256x16, left side is .b == 0, 
	float4 color = 0.f;
	//color.rgb = tex2D( Sampler_Src0, Input.vTexCoord.xy ).rgb;
	//color.b *= 15;
	//float shift = floor( color.b );
	//color.r = (color.r + shift) / 16;
	//color.rgb = lerp( tex2D( Sampler_LUT, color.rg ).rgb, tex2D( Sampler_LUT, color.rg + float2(0.0625, 0) ).rgb, color.b - shift );

	// LUT Scale =  256x16, left side is .b == 0, 
	color.rgb = tex2D( Sampler_Src0, Input.vTexCoord.xy ).rgb;
	color.b *= 15;
	float shift = floor( color.b );
	color.r = (color.r + shift) / 16;
	color.rgb = lerp( tex2D( Sampler_LUT, color.rg ).rgb, tex2D( Sampler_LUT, color.rg + float2(0.0625, 0) ).rgb, color.b - shift );

	Output.vColor = color;

	//float4 pixel = tex2D( Sampler_Src0, Input.vTexCoord );
	//
	////float4 gradedPixel = sampleAs3DTexture(colorGrade, pixel.rgb, 16);
	//float4 gradedPixel = sampleAs3DTexture(Sampler_LUT, pixel.rgb, 16);
	//gradedPixel.a = pixel.a;
	//pixel = gradedPixel;
	//
	//Output.vColor = pixel;

	//float4 vColor = tex2D( Sampler_Src0, Input.vTexCoord );
	//Output.vColor = tex1D( Sampler_LUT, vColor.r );

	//float4 vColor = tex2D( Sampler_Src0, Input.vTexCoord );
	//
	//float2 fOffset = float2(0.5f / 256.f, 0.5f / 16.f);
	//float fScale = 16.f / 16.f;
	//
	//float fIntB = floor( vColor.b * 14.9999f ) / 16.f;
	//float fFracB = vColor.b * 16.f - fIntB * 16.f;
	//
	//float U = fIntB + vColor.r * fScale / 16.f;
	//float V = vColor.g * fScale;
	//
	//float3 RG0 = tex2D( Sampler_LUT, fOffset + float2(U, V) ).rgb;
	//float3 RG1 = tex2D( Sampler_LUT, fOffset + float2(U + 1.f / 16.f, V) ).rgb;
	//
	//Output.vColor = float4( lerp( RG0, RG1, fFracB ).rgb , vColor.a);

	return Output;
}

technique LightRendering
{
	pass Default
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_frameBufferLight();
	}
};

technique NoLightRendering
{
	pass RenderSingleTexture
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_mainDefault();
	}

	pass RenderSingleTextureAlphaTest
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		//alphatestenable = false;
		alphatestenable = true;
		alpharef = 0x00;
		alphafunc = greater;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_mainDefault();
	}

	pass RenderFrameBuffer
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_frameBuffer();
	}

	pass RenderPriority
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_mainPriorityRender();
	}
};

technique FogRendering
{
	pass RenderSingleTexture
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphatestenable = false;

		alphablendenable = false;

		VertexShader = compile vs_3_0 vs_mainFrustumPos();
		PixelShader = compile ps_3_0 ps_mainFog();
	}

	pass RenderSingleTextureAlphaTest
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x00;
		alphafunc = greater;

		VertexShader = compile vs_3_0 vs_mainFrustumPos();
		PixelShader = compile ps_3_0 ps_mainFog();
	}
};

technique PriorityRendering
{
	pass NoFog
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphatestenable = false;

		alphablendenable = false;

		VertexShader = compile vs_3_0 vs_mainFrustumPos();
		PixelShader = compile ps_3_0 ps_mainDefault();
	}

	pass Fog
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphatestenable = false;

		alphablendenable = false;

		VertexShader = compile vs_3_0 vs_mainFrustumPos();
		PixelShader = compile ps_3_0 ps_mainFog();
	}
};

technique Debug
{
	pass Debuging_RenderTarget_Texture
	{
		ZEnable = false;
		ZWriteEnable = false;

		//alphablendenable = true;
		//srcblend = srcalpha;
		//destblend = invsrcalpha;
		alphablendenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_debugTarget();
	}
};

technique SSAO
{
	pass Default
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_ssao();
	}
};

technique EdgeDection
{
	pass RimLight
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_mainFrustumPos();
		PixelShader = compile ps_3_0 ps_rimLight();
	}
};

technique ColorGrading
{
	pass DefaultRendering
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_mainColorGrading();
	}

	pass AlphaTestRendering
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = true;
		alpharef = 0x00;
		alphafunc = greater;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_mainColorGrading();
	}
};