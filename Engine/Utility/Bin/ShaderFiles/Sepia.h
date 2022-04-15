/*------------------------------------------------------------------------------
						SEPIA
------------------------------------------------------------------------------*/

float4 SepiaPass( float4 colorInput, float2 tex )
{
	float3 sepia = colorInput.rgb;
	// Calculating amounts of input, grey and sepia colors to blend and combine
	float grey = dot(sepia, float3(0.3, 0.59, 0.11));
	sepia *= ColorTone;
	float3 blend = (grey * GreyPower) + (colorInput.rgb / (GreyPower + 1));
	colorInput.rgb = lerp(blend, sepia, SepiaPower);
	// returning the final color
	return colorInput;
}