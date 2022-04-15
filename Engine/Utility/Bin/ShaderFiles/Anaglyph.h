/*------------------------------------------------------------------------------
						ANAGLYPH
------------------------------------------------------------------------------*/
float4 AnaglyphPass( float4 colorInput, float2 Tex )
{
	float4 Anaglyph = colorInput;
	// Setting RGB channel colors
	float red = dot(Anaglyph.rgb, float4(2.55, 0, 0, 0));
	float green = dot(Anaglyph.rgb, float4(0,2.55,0, 0));
	float blue = dot(Anaglyph.rgb, float4(0,0,2.55,0));
	// Setting the RGB channel powers
	float4 red2 = red * 0.111;
	float4 green2 = green * 0.111;
	float4 blue2 = blue * 0.111;
	// Left Eye (Red)
	float4 LeftEye = tex2D(s0, float2(Tex + float2(-AnaglyphSeperation,0))).rgba;
	red2 = max(red2, LeftEye) - AnaglyphRed;
	// Right Eye (Cyan)
	float4 RightEye = tex2D(s0, float2(Tex + float2(AnaglyphSeperation,0))).rgba;
	green2 = max(green2,RightEye) - AnaglyphGreen;
	blue2 = max(blue2,RightEye) - AnaglyphBlue;
	float4 cyan = (green2 + blue2) / 2;
	// Combine
	Anaglyph.r = cyan.r;
	Anaglyph.g = red2.g;
	Anaglyph.b = red2.b;
	Anaglyph.a = max(red2.a,cyan.a);
	return Anaglyph;
}