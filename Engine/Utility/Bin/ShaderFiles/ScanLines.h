/*------------------------------------------------------------------------------
						SCAN LINES
------------------------------------------------------------------------------*/
float4 ScanlinePass(float4 colorInput, float2 tex)
{ 
 	float4 LineColor;
	LineColor = tex2D( s0, tex.xy);
	LineColor.r = LineColor.r*sin(tex.y*(100 * ScanLineWidth));
	LineColor.g = LineColor.g*sin(tex.y*(200 * ScanLineWidth));
	LineColor.b = LineColor.b*sin(tex.y*(400 * ScanLineWidth));
	#if (ScanLineColor == 0)
		LineColor.rgb = (LineColor.r+LineColor.g+LineColor.b)/1.0f; //greyscalemode
	#endif
	return lerp(colorInput, LineColor, ScanLinePower);
} 