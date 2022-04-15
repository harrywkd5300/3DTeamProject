#include "Debug.hpp"

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vColor = tex2D(DebugSampler, In.vTexUV);

	Out.vColor = vColor;

	return Out;
}

technique DefaultDevice
{
	pass Default
	{
		ZwriteEnable = false;

		//AlphaTestEnable = true;
		//AlphaRef = 0x00;
		//AlphaFunc = greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}