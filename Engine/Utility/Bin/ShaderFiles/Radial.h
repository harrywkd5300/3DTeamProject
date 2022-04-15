/*------------------------------------------------------------------------------
						RADIAL
------------------------------------------------------------------------------*/
float4 RadialPass(float4 colorInput, float2 tex)
{
    float2 Center   = {0.5, 0.5};
    float Samples   = 128; // max 128   4 8 16 32 64 128 ?
    float4 Radials = 0;
    for(int i = 0; i < Samples; i++)
    {
       float Scale = 1.0 + RadialWidth * (i / (Samples - 1));
       Radials     += tex2D(s0, (tex - 0.5) * Scale + Center );
    }
    Radials /= Samples;
    return lerp(colorInput, Radials, RadialStrength);
}