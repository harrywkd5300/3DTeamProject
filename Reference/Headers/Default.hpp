float4x4 g_matInvView = 0.f;
float4x4 g_matInvProj = 0.f;
float4x4 g_matView, g_matProj;
float g_fCamFar = 1000.f;

texture g_DepthTexture;
sampler Sampler_Depth = sampler_state
{
	texture = g_DepthTexture;
	AddressU = Clamp;
	AddressV = Clamp;
	MinFilter = Point;
	//MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

float4 Compute_Frustum( float2 vTexCoord, float fCamFar, float4x4 matInvProj )
{
	float4 vFrustumPos = 0.f;

	vFrustumPos.x = vTexCoord.x * 2.f - 1.f;
	vFrustumPos.y = vTexCoord.y * -2.f + 1.f;
	vFrustumPos.z = 1.f;
	vFrustumPos.w = 1.f;

	vFrustumPos *= fCamFar;

	vFrustumPos = mul( vFrustumPos, matInvProj );
	//vFrustumPos.w = 1.f;

	return vFrustumPos;
}

float4 Compute_PosWS( float4 vFrustumPos, float fDepth, float4x4 matInvView )
{
	float3 vRay = vFrustumPos.xyz;
	float4 vPosition = mul( float4( vRay * fDepth, 1.f ), matInvView );

	return vPosition;
}

float4 Compute_PosVS( float3 vFrustumPos, float fDepth )
{
	float3 vRay = vFrustumPos.xyz;
	float4 vPosition = float4( vRay * fDepth, 1.f );

	return vPosition;
}