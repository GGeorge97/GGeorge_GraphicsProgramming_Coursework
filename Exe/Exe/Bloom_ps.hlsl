Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer BloomBuffer : register(b0)
{
	float4 textureSize;
	bool horizontal;
	float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float weights[5] = { 0.4f, 0.2f, 0.2f, 0.1f, 0.1f };
    float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float blurRadius = 4;
	int weight = 4;  

	if (horizontal)
	{
		float texelSize = 1.0f / textureSize.x;
		for (int i = -blurRadius; i < blurRadius; ++i)
		{
			colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * i, 0.0f)) * weights[abs(i)];
		}
	}
	else
	{
		float texelSize = 1.0f / textureSize.y;
		for (float i = -blurRadius; i < blurRadius; ++i)
		{
			colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * i)) * weights[abs(i)];
		}
	}
    colour.a = 1.0f;

	return colour;
}
