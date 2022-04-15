/*------------------------------------------------------------------------------
						FINAL_LIMITER
------------------------------------------------------------------------------*/
// This has always to be the last pass, and can be used to limit the range of the final output 
float4 limiter (float4 colorInput,float2 tex)
{  
	float3 S1 = sqrt(colorInput.rgb);
	float3 S2 = sqrt(S1);
	float3 S3 = sqrt(S2);
	float3 sRGB = 0.662002687 * S1 + 0.684122060 * S2 - 0.323583601 * S3 - 0.225411470 * colorInput.rgb;
	
	float3 MinLimit = (sRGB * LimiterMinColor.rgb);
	float3 MaxLimit = (sRGB * (LimiterMaxColor.rgb - (LimiterMinColor.rgb * 4)));
	
	if (colorInput.r < MinLimit.r)
	{
		colorInput.r = MinLimit.r;
	}
	if (colorInput.r > MaxLimit.r)
	{
		colorInput.r = MaxLimit.r;
	}
	if (colorInput.g < MinLimit.g)
	{
		colorInput.g = MinLimit.g;
	}
	if (colorInput.g > MaxLimit.g)
	{
		colorInput.g = MaxLimit.g;
	}
	if (colorInput.b < MinLimit.b)
	{
		colorInput.b = MinLimit.b;
	}
	if (colorInput.b > MaxLimit.b)
	{
		colorInput.b = MaxLimit.b;
	}
	
    return colorInput;
}