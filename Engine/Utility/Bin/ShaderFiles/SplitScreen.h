/*------------------------------------------------------------------------------
						SPLITSCREEN
------------------------------------------------------------------------------*/

float4 SplitScreenPass( float4 colorInput, float2 tex )
{
	//Vertical 50/50 split
	#if SplitScreenMode == 1
		return (tex.x < 0.5) ? tex2D(s0, tex) : colorInput;
	#endif
	
	//Vertical 50/50 angled split
	#if SplitScreenMode == 2
	//Calculate the distance from center
		float distance = ((tex.x - 3.0/8.0) + (tex.y * 0.25));
		distance = saturate(distance - 0.25);
		return distance ? colorInput : tex2D(s0, tex);
	#endif
  	
	//Horizontal 50/50 split
  	#if SplitScreenMode == 3
		return (tex.y < 0.5) ? tex2D(s0, tex) : colorInput;
  #endif
}