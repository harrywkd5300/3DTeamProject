texture g_DebugTexture;
sampler DebugSampler = sampler_state
{
	texture = g_DebugTexture;
};

struct PS_IN
{
	float2		vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};
