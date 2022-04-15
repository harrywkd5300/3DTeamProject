/*------------------------------------------------------------------------------
						HDR
------------------------------------------------------------------------------*/
float4 HDRPass( float4 colorInput, float2 tex )
{
	float3 c_center = tex2D(s0, tex.xy).rgb;
	float radius1 = 0.793;
	float3 	bloom_sum1   = tex2D(s0, tex + float2(-1.5, -1.5) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(-2.5, 0) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(-1.5, 1.5) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(0, 2.5) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(1.5, 1.5) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(2.5, 0) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(1.5, -1.5) * radius1);
				bloom_sum1 += tex2D(s0, tex + float2(0, -2.5) * radius1);
				bloom_sum1 *= 0.005;
	float3	bloom_sum2   = tex2D(s0, tex + float2(-1.5, -1.5) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(-2.5, 0) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(-1.5, 1.5) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(0, 2.5) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(1.5, 1.5) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(2.5, 0) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(1.5, -1.5) * radius2);
				bloom_sum2 += tex2D(s0, tex + float2(0, -2.5) * radius2);
				bloom_sum2 *= 0.010;
	float dist = radius2 - radius1;
	float3 HDR = (c_center + (bloom_sum2 - bloom_sum1)) * dist;
	float3 blend = HDR + colorInput.rgb;
	colorInput.rgb = HDR + pow(abs(blend), HDRPower) ;
	return colorInput;
}