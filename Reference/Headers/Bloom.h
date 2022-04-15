/*------------------------------------------------------------------------------
						BLOOM
------------------------------------------------------------------------------*/
float4 BloomPass( float4 colorInput,float2 tex  )
{
	float3 BlurColor =  0;
	float3 Blurtemp = 0;
	float3 BloomCombined = 0;
	float MaxDistance = sqrt(8*BloomWidth);
	float CurDistance = 0;
	for(float Samplex = 2.5; Samplex > -2.5; Samplex--)
	{
		for(float Sampley = -2.5; Sampley < 2.5; Sampley++)
		{
			CurDistance = sqrt (((Samplex * Samplex) + (Sampley*Sampley))*BloomWidth);
			Blurtemp.rgb = tex2D(s0, float2(tex.x +(tex.x*Samplex*px*BloomWidth),tex.y +(tex.y*Sampley*py*BloomWidth)));
			BlurColor.rgb += lerp(Blurtemp.rgb,colorInput.rgb, 1 - ((MaxDistance - CurDistance)/MaxDistance));
		}
	}
	BlurColor.rgb = (BlurColor.rgb / (25 - (BloomThreshold * 5)));
	float BloomColor1 = (dot(colorInput.rgb,float3(0.299f, 0.587f, 0.114f)));
	float BloomColor2 = (dot(colorInput.rgb,float3(-0.172f, -0.339f, 0.511f)));
	float BloomColor3 = (dot(colorInput.rgb,float3(-0.511f, -0.428f, 0.083f)));
	float BloomRed = (BloomColor1 + 1.540f * BloomColor3) * (BloomPower * 0.075f);
	float BloomGreen = (BloomColor1 - 0.459f * BloomColor3 - 0.183f * BloomColor2) * (BloomPower * 0.075f);
	float BloomBlue = (BloomColor1 + 1.816f * BloomColor3) * (BloomPower * 0.075f);
	BloomCombined.rgb = (BloomRed,BloomGreen,BloomBlue);
	colorInput.rgb =lerp(colorInput,BlurColor,BloomCombined).rgb;
	return  colorInput;
}