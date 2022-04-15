#include "Shadow.hpp"

VS_OUTSHADOW vs_renderShadow( VS_IN Input )
{
	VS_OUTSHADOW Output = (VS_OUTSHADOW)0.f;

	float4 vPos = mul( Input.vPosition, g_matWorld );

	vPos = mul( vPos, g_matLightVP );

	Output.vPosition = vPos;
	Output.vDepth = vPos.zzzw;

	return Output;
}

PS_OUT ps_renderShadow( PS_INSHADOW Input )
{
	PS_OUT Output = (PS_OUT)0;

	float fDepth = Input.vDepth.z / Input.vDepth.w;

	Output.vColor = float4( fDepth, fDepth * fDepth, 0.f, 1.f );

	return Output;
}

VS_OUTPUT_QUAD VS_pass1(VS_IN Input)
{
	VS_OUTPUT_QUAD Out = (VS_OUTPUT_QUAD)0;

	float4 vPos = mul( Input.vPosition, mul( g_matView, g_matProj ) );

	Out.vPosition = vPos;

	Out.vTexCoord0 = Input.vTexCoord + float2(-0.5f/MAP_SIZE, -0.5f/MAP_SIZE);
	Out.vTexCoord1 = Input.vTexCoord + float2(+0.5f/MAP_SIZE, +0.5f/MAP_SIZE);
	Out.vTexCoord2 = Input.vTexCoord + float2(-0.5f/MAP_SIZE, +0.5f/MAP_SIZE);
	Out.vTexCoord3 = Input.vTexCoord + float2(+0.5f/MAP_SIZE, -0.5f/MAP_SIZE);

	return Out;
}
// -------------------------------------------------------------
// ÇÈ¼¿¼ÎÀÌ´õ
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT_QUAD In) : COLOR
{   
	float4 Color;

	float d0 = tex2D( Sampler_Src, In.vTexCoord0 ) - tex2D( Sampler_Src, In.vTexCoord1 );
	float d1 = tex2D( Sampler_Src, In.vTexCoord2 ) - tex2D( Sampler_Src, In.vTexCoord3 );
	
	Color = d0*d0 + d1*d1;
	
	return Color;
}

// -------------------------------------------------------------
// 3ÆÐ½º:Èå¸®°Ô ¹¶°³±â
// -------------------------------------------------------------

// -------------------------------------------------------------
// Á¤Á¡¼ÎÀÌ´õ
// -------------------------------------------------------------
VS_OUTPUT_QUAD VS_pass2( VS_IN Input )
{
	VS_OUTPUT_QUAD Out = (VS_OUTPUT_QUAD)0;        // Ãâ·Âµ¥ÀÌÅÍ

	float4 vPos = mul( Input.vPosition, mul( g_matView, g_matProj ) );

	Out.vPosition = vPos;

	Out.vTexCoord0 = Input.vTexCoord + float2(-3.0f / MAP_SIZE, -3.0f / MAP_SIZE);
	Out.vTexCoord1 = Input.vTexCoord + float2(-3.0f / MAP_SIZE, -1.0f / MAP_SIZE);
	Out.vTexCoord2 = Input.vTexCoord + float2(-3.0f / MAP_SIZE, 1.0f / MAP_SIZE);
	Out.vTexCoord3 = Input.vTexCoord + float2(-3.0f / MAP_SIZE, 3.0f / MAP_SIZE);
	Out.vTexCoord4 = Input.vTexCoord + float2(-1.0f / MAP_SIZE, -3.0f / MAP_SIZE);
	Out.vTexCoord5 = Input.vTexCoord + float2(-1.0f / MAP_SIZE, -1.0f / MAP_SIZE);
	Out.vTexCoord6 = Input.vTexCoord + float2(-1.0f / MAP_SIZE, 1.0f / MAP_SIZE);
	Out.vTexCoord7 = Input.vTexCoord + float2(-1.0f / MAP_SIZE, 3.0f / MAP_SIZE);

	return Out;
}

// -------------------------------------------------------------
// ÇÈ¼¿¼ÎÀÌ´õ
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT_QUAD In) : COLOR
{
	float4 Color;
	float2 dvu = float2(4.0f / MAP_SIZE, 0);
	
	Color = tex2D( Sampler_Src, In.vTexCoord0 )
	+ tex2D( Sampler_Src, In.vTexCoord1 )
	+ tex2D( Sampler_Src, In.vTexCoord2 )
	+ tex2D( Sampler_Src, In.vTexCoord3 )
	+ tex2D( Sampler_Src, In.vTexCoord4 )
	+ tex2D( Sampler_Src, In.vTexCoord5 )
	+ tex2D( Sampler_Src, In.vTexCoord6 )
	+ tex2D( Sampler_Src, In.vTexCoord7 )
	+ tex2D( Sampler_Src, In.vTexCoord0 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord1 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord2 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord3 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord4 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord5 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord6 + dvu )
	+ tex2D( Sampler_Src, In.vTexCoord7 + dvu )
	;
	
	return 0.7f * Color;
}

technique Default
{
	pass RenderShadow
	{
		zenable = false;
		zwriteenable = false;

		VertexShader = compile vs_3_0 vs_renderShadow();
		PixelShader = compile ps_3_0 ps_renderShadow();
	}

	pass aaa
	{
		zenable = false;
		zwriteenable = false;

		VertexShader = compile vs_3_0 VS_pass1();
		PixelShader = compile ps_3_0 PS_pass1();
	}

	pass bbb
	{
		zenable = false;
		zwriteenable = false;

		VertexShader = compile vs_3_0 VS_pass2();
		PixelShader = compile ps_3_0 PS_pass2();
	}
};