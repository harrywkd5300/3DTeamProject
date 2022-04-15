#include "Light.hpp"

VS_OUT vs_main( VS_IN Input )
{
	VS_OUT Output = (VS_OUT)0.f;

	float4x4 matWVP;
	matWVP = mul( g_matView, g_matProj );

	Output.vPosition = mul( Input.vPosition, matWVP );
	Output.vTexCoord = Input.vTexCoord;
	Output.vFrustumPos = Compute_Frustum( Input.vTexCoord, g_fCamFar, g_matInvProj );
	//Output.vFrustumPos = mul( Output.vFrustumPos, g_matInvView );

	return Output;
}

PS_OUT ps_DirectionalLight( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;
	float3 vNormal = tex2D( Sampler_Normal, Input.vTexCoord ).xyz * 2.f - 1.f;
	float3 vAO = tex2D( Sampler_SSAO, Input.vTexCoord ).rgb;
	float4 vSpecularIntensity = tex2D( Sampler_SpecularIntensity, Input.vTexCoord );
	
	// Get Pixel's Position ( World Space )..
	float4 vPosition = Compute_PosWS( Input.vFrustumPos, fDepth, g_matInvView );
	
	// Loop 에서 쓸 변수들 미리 생성 후 초기화..
	float fShade = 0.f;
	
	float4 vAmbient = 0.f;
	float4 vDiffuse = 0.f;
	
	float4 vSpecular = 0.f;
	float fSpecularIntensity = 0.f;
	
	
	// --------------- Compute Shade.. --------------- //
	fShade = Compute_Shade( g_vLightDir.xyz, vNormal.xyz );
	fShade = max( 0.f, dot( -normalize( g_vLightDir.xyz ), normalize( vNormal.xyz ) ) ).r;
	
	// --------------- Compute Albedo.. --------------- //
	vAmbient = g_vLightAmbient * g_vMtrlAmbient * g_vLightPower.y; vAmbient.rgb *= vAO.rgb;
	vDiffuse = g_vLightDiffuse * g_vMtrlDiffuse * g_vLightPower.x;
	
	// --------------- Compute Specular.. --------------- //
	vSpecular = g_vLightSpecular * g_vLightPower.z * g_vMtrlSpecular;
	fSpecularIntensity = Compute_SpecularIntensity( g_vLightDir.xyz, vNormal.xyz, vPosition.xyz, g_vCamPosition.xyz, vSpecularIntensity.g * g_vMtrlPower );
	vSpecular = float4(vSpecular.rgb * fSpecularIntensity * vSpecularIntensity.rrr, vSpecularIntensity.a);
	
	// 구한 값을 결과값에 추가..
	Output.vShade.rgb = fShade * vDiffuse.rgb + vAmbient.rgb;
	
	Output.vSpecular.rgb = vSpecular.rgb; Output.vSpecular.a = vSpecular.a;


	// 컬러 값과 곱한다..
	float3 vColor = tex2D( Sampler_Color, Input.vTexCoord ).rgb;

	Output.vShade.rgb *= vColor;
	Output.vSpecular.rgb *= vColor;

	return Output;
}

PS_OUT ps_PointLight( PS_IN Input )
{
	PS_OUT Output = (PS_OUT)0;

	//float LinearDepth = tex2D( Sampler_Depth, Input.vTexCoord ).x;
	////In.vViewRay = normalize(In.vViewRay);
	//
	////float3 vWorldPos = g_vCamera_Position.xyz + In.vViewRay.xyz * LinearDepth *1000.f;
	////float3 vWorldPos = g_vCamera_Position.xyz + normalize(In.vViewRay).xyz * 1000.f/LinearDepth;
	////float3 vWorldPos = g_vCamPosition.xyz + Input.vViewRay.xyz *  LinearDepth;
	//float3 vWorldPos = Input.vFrustumPos.xyz * LinearDepth;
	//
	//vector	vNormal = float4(tex2D( Sampler_Normal, Input.vTexCoord ).xyz, 0.f);
	//
	//vector			vLook;
	//vLook = float4(vWorldPos, 1.f) - g_vCamPosition;
	//vLook = vector(normalize( vLook.xyz ), 0.f);
	//
	//vector		vLightDir = float4(vWorldPos.xyz, 1.f) - g_vLightPos;
	//
	//float		fDistance = length( vLightDir );
	//
	//float		 fAttenuation = max( (g_fRange - fDistance), 0.f ) / g_fRange;
	//
	//Output.vShade += max( dot( normalize( vLightDir ) * -1.f, vNormal ), 0.f ) *  fAttenuation;
	//
	//Output.vShade += (Output.vShade + (g_vLightAmbient * g_vMtrlAmbient)) * (g_vLightDiffuse * g_vMtrlDiffuse);
	//
	//vector		vReflect;
	//vReflect = reflect( normalize( vLightDir ), vNormal );
	//
	//vector	vSpecular_Tex = tex2D( Sampler_SpecularIntensity, Input.vTexCoord );
	//Output.vSpecular = pow( max( dot( normalize( vLook ) * -1.f, normalize( vReflect ) ), 0.f ), g_vMtrlPower ) * (g_vLightSpecular * g_vMtrlSpecular ) * fAttenuation;
	//Output.vSpecular = Output.vSpecular * vSpecular_Tex;


	float fDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;
	float3 vNormal = tex2D( Sampler_Normal, Input.vTexCoord ).xyz * 2.f - 1.f;
	float3 vAO = tex2D( Sampler_SSAO, Input.vTexCoord ).rgb;
	float4 vSpecularIntensity = tex2D( Sampler_SpecularIntensity, Input.vTexCoord );
	
	// Get Pixel's Position ( World Space )..
	float4 vPosition = Compute_PosWS( Input.vFrustumPos, fDepth, g_matInvView );
	
	
	// --------------- Compute Dir And AttenuationFactor.. --------------- //
	float3 vLight_Direction = vPosition.xyz - g_vLightPos.xyz;
	float fDistance = length( vLight_Direction );
	
	float fAttenuationFactor = Compute_DistanceAttenuation( g_fRange, fDistance );
	
	// --------------- Compute Shade.. --------------- //
	float fShade = Compute_Shade( vLight_Direction.xyz, vNormal.xyz );
	
	// --------------- Compute Light Diffuse + Ambient.. --------------- //
	float4 vAmbient = g_vLightAmbient * g_vMtrlAmbient * g_vLightPower.y; vAmbient.rgb *= vAO.rgb;
	float4 vDiffuse = g_vLightDiffuse * g_vMtrlDiffuse * g_vLightPower.x;
	
	// --------------- Compute Specular.. --------------- //
	float4 vSpecular = g_vLightSpecular * g_vMtrlSpecular * g_vLightPower.z;
	float fSpecularIntensity = Compute_SpecularIntensity( vLight_Direction.xyz, vNormal.xyz, vPosition.xyz, g_vCamPosition.xyz, vSpecularIntensity.g * g_vMtrlPower );
	vSpecular = float4( vSpecular.rgb * fSpecularIntensity * vSpecularIntensity.rrr, vSpecularIntensity.a );
	
	// 구한 값을 결과값에 추가..
	Output.vShade.rgb = ( fShade * vDiffuse.rgb + vAmbient.rgb ) * fAttenuationFactor;
	
	Output.vSpecular.rgb = vSpecular.rgb * fAttenuationFactor; Output.vSpecular.a = vSpecular.a;


	// 컬러 값과 곱한다..
	float3 vColor = tex2D( Sampler_Color, Input.vTexCoord ).rgb;

	Output.vShade.rgb *= vColor;
	Output.vSpecular.rgb *= vColor;

	return Output;
}

PS_OUT ps_SpotLight( PS_IN Input )
{
	//////////////////////////////////////////////////////////////////////////
	//
	// 스포트 라이트..
	// http://telnet.or.kr/directx/graphics/programmingguide/fixedfunction/lightsandmaterials/spotlightmodel.htm : 스포트라이트 조명 모델 설명..
	// https://www.gamedev.net/forums/topic/545762-pixel-shader-spotlight-with-hlsl/ : 스포트라이트 구현..
	//
	//////////////////////////////////////////////////////////////////////////

	PS_OUT Output = ( PS_OUT )0.f;

	float fDepth = tex2D( Sampler_Depth, Input.vTexCoord ).r;
	float4 vNormal = normalize( tex2D( Sampler_Normal, Input.vTexCoord ) * 2.f - 1.f );
	float3 vAO = tex2D( Sampler_SSAO, Input.vTexCoord ).rgb;
	float4 vSpecularIntensity = tex2D( Sampler_SpecularIntensity, Input.vTexCoord );

	// Get Pixel's Position ( World Space )..
	float4 vPosition = Compute_PosWS( Input.vFrustumPos, fDepth, g_matInvView );


	// --------------- Compute DistAttenuationFactor.. --------------- //
	float3 vPixelLookToLight = g_vLightPos.xyz - vPosition.xyz;
	float fDistance = length( vPixelLookToLight );
	vPixelLookToLight = normalize( vPixelLookToLight );
	float fDistAttenuationFactor = max( 0.f, (g_fRange - fDistance) / g_fRange );
	//fDistAttenuationFactor = ceil( fDistAttenuationFactor );

	float fShade = Compute_Shade( -vPixelLookToLight.xyz, vNormal.xyz );

	// --------------- Compute AngleAttenuationFactor.. --------------- //
	float fCosAngle = max( 0.f, dot( -normalize( g_vLightDir.xyz ), vPixelLookToLight ) );
	float fAngleAttenuationFactor = 1.f - smoothstep( g_fPhi, g_fTheta, fCosAngle );

	// --------------- Compute ResultAttenuationFactor.. --------------- //
	// 거리와 각도에 따른 Attenuation 값을 곱한다..
	float fAttenuationFactor = fAngleAttenuationFactor * fDistAttenuationFactor;

	// --------------- Compute Albedo.. --------------- //
	float4 vAmbient = g_vLightAmbient * g_vMtrlAmbient * g_vLightPower.y; vAmbient.rgb *= vAO.rgb;
	float4 vDiffuse = g_vLightDiffuse * g_vMtrlDiffuse * g_vLightPower.x;

	// --------------- Compute Specular.. --------------- //
	float4 vSpecular = g_vLightSpecular * g_vMtrlSpecular * g_vLightPower.z;
	float fSpecularIntensity = Compute_SpecularIntensity( -vPixelLookToLight.xyz, vNormal.xyz, vPosition.xyz, g_vCamPosition.xyz, vSpecularIntensity.g * g_vMtrlPower );
	vSpecular = float4(vSpecular.rgb * fSpecularIntensity * vSpecularIntensity.rrr, vSpecularIntensity.a);

	Output.vShade.rgb = (fShade * vDiffuse.rgb + vAmbient.rgb) * fAttenuationFactor;

	Output.vSpecular = float4(vSpecular.xyz * fAttenuationFactor, vSpecular.a);


	// 컬러 값과 곱한다..
	float3 vColor = tex2D( Sampler_Color, Input.vTexCoord ).rgb;

	Output.vShade.rgb *= vColor;
	Output.vSpecular.rgb *= vColor;

	return Output;
}

technique
{
	pass Default_DirectionalLight
	{
		ZEnable = false;
		ZWriteEnable = false;

		LIGHTING = FALSE;

		AlphaBlendEnable = TRUE;
		DestBlend = one;
		SrcBlend = one;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_DirectionalLight();
	}

	pass Default_PointLight
	{
		ZEnable = false;
		ZWriteEnable = false;
		
		LIGHTING = FALSE;

		AlphaBlendEnable = TRUE;
		DestBlend = one;
		SrcBlend = one;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_PointLight();
	}

	pass Default_SpotLight
	{
		ZEnable = false;
		ZWriteEnable = false;

		LIGHTING = FALSE;

		AlphaBlendEnable = TRUE;
		DestBlend = one;
		SrcBlend = one;

		alphatestenable = false;

		VertexShader = compile vs_3_0 vs_main();
		PixelShader = compile ps_3_0 ps_SpotLight();
	}
}