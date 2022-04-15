/*------------------------------------------------------------------------------
						VIGNETTE
------------------------------------------------------------------------------*/

float4 VignettePass( float4 colorInput, float2 tex )
{
	float4 vignette = colorInput;
	float2 VignetteCenter = float2(0.5,0.5);
	float2 tc = tex - VignetteCenter;
	tc.x *=(BUFFER_RCP_HEIGHT / BUFFER_RCP_WIDTH);
	tc.y *=(BUFFER_RCP_HEIGHT / BUFFER_RCP_WIDTH);
	float v = (length(tc) / VignetteRadius * 1.15);
	vignette.rgb += pow(v, 4) * VignetteAmount;
	return lerp(colorInput, vignette, VignetteStrength);
}
