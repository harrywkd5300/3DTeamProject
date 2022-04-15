/*------------------------------------------------------------------------------
						NOISE
------------------------------------------------------------------------------*/
float4 NoisePass(float4 colorInput, float2 tex)
{
	float d1 = tex.x * tex.y * 123456;
	float d2 = fmod(d1,13) * fmod(d1,123);	
	float myX = fmod(d2,0.01);
	float myY = fmod(d2,0.012);
	float3 Noise1 = tex2D(s0, tex + float2(myX,myY));
	#if (NoiseMode == 1)
		colorInput.rgb = lerp(colorInput.rgb, Noise1, NoiseStrength);
	#endif
	float d3 = tex.x * tex.y * 1000;
	float d4 = fmod(d3, 13) * fmod(d3, 123);	
	float d5 = fmod(d4, 0.01);
	float3 Noise2 = colorInput.rgb + colorInput.rgb * saturate(0.1f + d5.xxx * 100);
	float2 d6; 
	sincos(tex.y * LineAmount, d6.x, d6.y);
	Noise2 += colorInput.rgb * float3(d6.x, d6.y, d6.x) * LineIntensity;
	#if (NoiseMode == 2)
		colorInput.rgb = lerp(colorInput.rgb, Noise2, saturate(NoiseIntensity));
	#endif
	#if (NoiseMode == 3)
		float3 Noise1b = lerp(colorInput.rgb, Noise1, NoiseStrength);
		float3 Noise2b = lerp(colorInput.rgb, Noise2, saturate(NoiseIntensity));
		float3 blend = (Noise1b + Noise2b) / 2;
		colorInput.rgb = lerp(colorInput.rgb, blend, 1.0);
	#endif	
	return colorInput;
}