/*------------------------------------------------------------------------------
						CUSTOM SHADER
------------------------------------------------------------------------------*/

/********************
Following values are available to use in this custom shader file:

USE_CUSTOM_SELECT 		selection switch that can be used as a toggle within the shader, values 0 & 1

CustomValue1 						integer variable with values 1 to 9   				
CustomValue2 						integer variable with values 1 to 9
CustomValue3 						integer variable with values 1 to 9
CustomValue4 						integer variable with values 1 to 9
CustomValue5 					    integer variable with values 1 to 9
CustomValue6 						integer variable with values 1 to 9

*********************/

float4 CustomPass(float4 colorInput, float2 tex)
{
		  return colorInput;
} 
