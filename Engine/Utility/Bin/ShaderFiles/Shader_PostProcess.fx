#include "PostProcess.hpp"

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = (VS_OUT)0.f;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;

	return Output;
}

VS_OUT_BLUR vs_mainMotionBlur( VS_IN Input )
{
	VS_OUT_BLUR Output = (VS_OUT_BLUR)0.f;

	float4 vFrustumPos = Compute_Frustum( Input.vTexCoord, g_fCamFar, g_matInvProj );

	Output.vPosition = mul( Input.vPosition, mul( g_matView, g_matProj ) );
	Output.vFrustumPos = vFrustumPos;
	Output.vTexCoord = Input.vTexCoord;

	return Output;
}


// ------------------------------ MotionBlur Technique ------------------------------ //
#ifdef USEVELOCITYTARGET
//-----------------------------------------------------------------------------
// Pixel Shader: ps_createVelocityBuffer
// Desc: Create Velocity Buffer..
//-----------------------------------------------------------------------------
PS_OUT ps_createVelocityBuffer( PS_IN_BLUR Input )
{
	PS_OUT Output = ( PS_OUT )0.f;

	float fDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;

	// Prev View 와 Cur View 행렬을 사용하여 좌표 구함..
	float4 vPosWS = Compute_PosWS( Input.vFrustumPos, fDepth, g_matInvView );
	float4 vNewPos = mul( vPosWS, g_matCamView );
	float4 vOldPos = mul( vPosWS, g_matPrevView );

	// 속도 구함..
	float4 s0 = mul( vOldPos, g_matProj ); s0 /= s0.w;
	float4 s1 = mul( vNewPos, g_matProj ); s1 /= s1.w;

	float4 vVelocity = (s1 - s0) * 0.5f;
	vVelocity = vVelocity * 0.5f + 0.5f;

	Output.vColor = vVelocity;

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_motionBlur
// Desc: 속도 버퍼를 사용한 Camera MotionBlur
//-----------------------------------------------------------------------------
PS_OUT ps_motionBlur( PS_IN_BLUR Input )
{
	PS_OUT Output = (PS_OUT)0.f;

	float4 vColor = tex2D( Sampler_FrameBuffer, Input.vTexCoord );
	float2 vVelocity = tex2D( Sampler_Src1, Input.vTexCoord ).xy;
	vVelocity = vVelocity * 2.f - 1.f;

	if ( vColor.a <= 0.00001f )
	{
		Output.vColor = vColor;
		return Output;
	}

	// Initialize Color Data..
	int iNumSamples = 4;
	float2 vTexCoord = Input.vTexCoord;
	vTexCoord += vVelocity;

	// Max Velocity 값을 벗어나지 못하도록 값 조정..
	float fMaxVelocity = g_fMaxVelocity / (float)iNumSamples;
	float2 vAbsVelocity = abs( vVelocity );
	vVelocity.x = min( fMaxVelocity, vAbsVelocity.x ) * (vVelocity.x) / vAbsVelocity.x;
	vVelocity.y = min( fMaxVelocity, vAbsVelocity.y ) * (vVelocity.y) / vAbsVelocity.y;

	for ( int i = 1; i < iNumSamples; ++i, vTexCoord += vVelocity.xy )
	{
		// Velocity 만큼 더한 위치의 픽셀값을 얻어온다..
		float3 vCurrentColor = tex2D( Sampler_FrameBuffer, vTexCoord );
		// Add Color..
		vColor.rgb += vCurrentColor.rgb;
	}

	// Div Loop Count ( Get Average Blur Color )..
	float4 finalColor = float4(vColor.rgb / ((float)iNumSamples), vColor.a);

	Output.vColor = finalColor;

	return Output;
}
#else
//-----------------------------------------------------------------------------
// Pixel Shader: ps_motionBlur
// Desc: 속도 버퍼를 사용하지 않은 Camera MotionBlur
//-----------------------------------------------------------------------------
PS_OUT ps_motionBlur( PS_IN_BLUR Input )
{
	PS_OUT Output = (PS_OUT)0.f;

	float4 vColor = tex2D( Sampler_FrameBuffer, Input.vTexCoord );
	float fDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;

	// Prev View 와 Cur View 행렬을 사용하여 좌표 구함..
	float4 vPosWS = Compute_PosWS( Input.vFrustumPos, fDepth, g_matInvView );
	float4 vNewPos = mul( vPosWS, g_matCamView );
	float4 vOldPos = mul( vPosWS, g_matPrevView );

	// 속도 구함..
	float4 s0 = mul( vOldPos, g_matProj ); s0 /= s0.w;
	float4 s1 = mul( vNewPos, g_matProj ); s1 /= s1.w;

	float4 vVelocity = (s1 - s0) * 0.5f;

	if ( vColor.a <= 0.00001f )
	{
		Output.vColor = vColor;
		return Output;
	}

	// Initialize Color Data..
	int iNumSamples = 4;
	float2 vTexCoord = Input.vTexCoord;
	vTexCoord += vVelocity.xy;

	// Max Velocity 값을 벗어나지 못하도록 값 조정..
	float fMaxVelocity = g_fMaxVelocity / (float)iNumSamples;
	float2 vAbsVelocity = abs( vVelocity );
	vVelocity.x = min( fMaxVelocity, vAbsVelocity.x ) * (vVelocity.x) / vAbsVelocity.x;
	vVelocity.y = min( fMaxVelocity, vAbsVelocity.y ) * (vVelocity.y) / vAbsVelocity.y;

	for ( int i = 1; i < iNumSamples; ++i, vTexCoord += vVelocity.xy )
	{
		// Velocity 만큼 더한 위치의 픽셀값을 얻어온다..
		float3 vCurrentColor = tex2D( Sampler_FrameBuffer, vTexCoord );
		// Add Color..
		vColor.rgb += vCurrentColor.rgb;
	}

	// Div Loop Count ( Get Average Blur Color )..
	float4 finalColor = float4(vColor.rgb / ((float)iNumSamples), vColor.a);

	Output.vColor = finalColor;

	return Output;
}
#endif


// ------------------------------ DOF Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_dof
// Desc: Depth 를 기반으로 흐림 텍스쳐와 원본 텍스쳐를 조합하는 작업을 한다..
//-----------------------------------------------------------------------------
PS_OUT ps_dof( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor = tex2D( Sampler_Src0, Input.vTexCoord );
	float4 vCrush = tex2D( Sampler_Crush, Input.vTexCoord );
	float4 vDepth = tex2D( Sampler_Depth, Input.vTexCoord );

	//float2 vPos = float2(
	//	(max( g_vFocusData.z, abs( g_vFocusData.x - Input.vTexCoord.x ) ) - g_vFocusData.z),
	//	(max( g_vFocusData.w, abs( g_vFocusData.y - Input.vTexCoord.y ) ) - g_vFocusData.w));

	// 0.005 * (0.03 - (0 ~ 1))
	//float fT = saturate( (g_fRange / g_fCamFar) * abs( (g_fFocusZ / g_fCamFar) - vDepth.r ) );
	//float fT = (vPos.x + vPos.y) / 2.f * g_fDOFScale;
	//Output.vColor = (vColor * (1.f - fT)) + (vCrush * fT);

	// Z 거리에 의한 DOF..
	float fZRangeLerpT = saturate( (abs( g_vFocusData.z - (vDepth.r * g_fCamFar) ) / g_vFocusData.w) - 1.f );

	// X, Y(Screen Space) 거리에 의한 DOF..
	float2 vPos = abs( g_vFocusData.xy - Input.vTexCoord ) * g_fDOFScale;
	float fXYRangeLerpT = vPos.x * vPos.y;

	float4 vFinalColor = lerp( vColor, vCrush, max( 0.f, min( 1.f, fZRangeLerpT * fXYRangeLerpT ) ) );
	
	Output.vColor = vFinalColor;

	return Output;
}


// ------------------------------ LightShaft Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_createOcclusion
// Desc: 오브젝트가 렌러 되있는 곳은 0, 아닌 곳은 1로 채워넣음..
//-----------------------------------------------------------------------------
PS_OUT ps_createOcclusion( VS_OUT Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fAlphaColor = tex2D( Sampler_FrameBuffer, Input.vTexCoord ).a;

	//clip( max( 0.f, fAlphaColor - 0.0001f ) );

	//Output.vColor = max( 0.f, 1.f - fAlphaColor );
	Output.vColor = max( 0.f, 1.f - fAlphaColor );

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_lightShaft
// Desc: 빛의 산란을 렌더링함???..
//-----------------------------------------------------------------------------
PS_OUT ps_lightShaft( VS_OUT Input )
{
	PS_OUT Output = (PS_OUT)0;

	// Calculate vector from pixel to light source in screen space.
	half2 deltaTexCoord = (Input.vTexCoord - g_vSunData.xy);
	// Divide by number of samples and scale by control factor.
	deltaTexCoord *= 1.0f / (float)g_iNumSamples * g_vLightShaftValue.x;
	// Store initial sample.
	float4 vColor = tex2D( Sampler_FrameBuffer, Input.vTexCoord );
	half3 color = 0.f;
	// Set up illumination decay factor.
	half illuminationDecay = g_vLightShaftValue2.z;

	float2 texCoord = Input.vTexCoord;

	// Evaluate summation from Equation 3 NUM_SAMPLES iterations.
	for ( int i = 0; i < g_iNumSamples; i++ )
	{
		// Step sample location along ray.
		texCoord -= deltaTexCoord;
		// Retrieve sample at new location.
		half4 sample = tex2D( Sampler_FrameBuffer, texCoord );

		//fTexWidth, fTexHeight 는 스크린 크기 (1280, 720 같은....) 
		//sun 방향성 값으로,
		//광선이 뻗어나가는 방향을 정할수 있습니다
		texCoord.x += 1.f / g_vLightShaftValue2.x * g_vSunData.z;
		texCoord.y += 1.f / g_vLightShaftValue2.y * g_vSunData.w;

		// Apply sample attenuation scale/decay factors.
		sample *= illuminationDecay * g_vLightShaftValue.z;
		// Accumulate combined color.
		//color.rgb += sample.rgb * ((1.f - sample.a) * -1.f);
		//color.rgb += sample.rgb * ((1.f - vColor.a) * -1.f);
		color.rgb += sample;
		// Update exponential decay factor.
		illuminationDecay *= g_vLightShaftValue.y;
	}

	// Output final color with a further scale control factor.
	//Output.vColor = float4(color * _Exposure, 1);
	Output.vColor = float4(color.rgb * g_vLightShaftValue.w + vColor.rgb, 1);

//#define NUM_SAMPLES 64
//
//	// 화면 공간에서 픽셀로부터 광원을 향하는 벡터 계산..
//	float2 vDeltaTexCoord = (Input.vTexCoord - g_vSunData.xy);
//
//	// 샘플링할 수로 나누고 제어 팩터로 스케일링한다..
//	vDeltaTexCoord *= 1.f / NUM_SAMPLES * g_vLightShaftValue.x;
//
//	// 샘플링..
//	float3 vColor = tex2D( Sampler_FrameBuffer, Input.vTexCoord );
//
//	// 일루미네이션 감소 팩터 설정..
//	float fIlluminationDecay = 1.f;
//
//	// 방정식 3의 적용..
//	float2 vTexCoord = Input.vTexCoord;
//	for ( int i = 0; i < NUM_SAMPLES; ++i )
//	{
//		//광선을 따라 샘플링..
//		vTexCoord -= vDeltaTexCoord;
//
//		// 샘플을 새 좌표로 위치 시킴..
//		float3 vSample = tex2D( Sampler_FrameBuffer, vTexCoord );
//
//		// 스케일 / 감쇄 팩터를 적용해 감소를 수행..
//		vSample *= fIlluminationDecay * g_vLightShaftValue.z;
//
//		// 합쳐진 색 저장..
//		vColor += vSample;
//
//		// 감쇄 팩터 지수 업데이트..
//		fIlluminationDecay *= g_vLightShaftValue.y;
//	}
//
//	Output.vColor = saturate( float4(vColor * g_vLightShaftValue.w, 1.f) );

	return Output;
}

//-----------------------------------------------------------------------------
// Pixel Shader: ps_lightShaft
// Desc: 빛의 산란을 렌더링함???..
//-----------------------------------------------------------------------------
PS_OUT ps_lightShaftEnd( VS_OUT Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor1 = tex2D( Sampler_FrameBuffer, Input.vTexCoord );
	float4 vColor2 = tex2D( Sampler_Src0, Input.vTexCoord );

	Output.vColor= vColor1 + vColor1 * vColor2;

	return Output;
}

// ------------------------------ RainAmbience Technique ------------------------------ //
//-----------------------------------------------------------------------------
// Pixel Shader: ps_rainAmbience
// Desc: 비 온 느낌을 주기 위한 용도..
//-----------------------------------------------------------------------------
PS_OUT ps_rainAmbience( VS_OUT Input )
{
	PS_OUT Output = (PS_OUT)0;

	float4 vColor = tex2D( Sampler_FrameBuffer, Input.vTexCoord );

	Output.vColor = 1.f;

	return Output;
}

//-----------------------------------------------------------------------------
// Technique..
//-----------------------------------------------------------------------------
technique MotionBlur
{
#ifdef USEVELOCITYTARGET
	pass CreateVelocityBuffer
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_mainMotionBlur();
		PixelShader = compile ps_3_0 ps_createVelocityBuffer();
	}

	pass CameraMotionBlur
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_motionBlur();
	}
#else
	pass CameraMotionBlur
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_mainMotionBlur();
		PixelShader = compile ps_3_0 ps_motionBlur();
	}
#endif
}

technique DOF
{
	pass DepthBased
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_dof();
	}
}

technique LightShaft
{
	pass CreateOcclusion
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_createOcclusion();
	}

	pass LightShaft
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_lightShaft();
	}

	pass LightShaftEnd
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_lightShaftEnd();
	}
}

technique RainAmbience
{
	pass Default
	{
		ZEnable = false;
		ZWriteEnable = false;

		alphablendenable = false;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_rainAmbience();
	}
}