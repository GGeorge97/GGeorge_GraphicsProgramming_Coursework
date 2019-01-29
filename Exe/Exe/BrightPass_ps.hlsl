Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer BloomFactorBuffer : register(b0)
{
	float4 bloomFactor;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	const float3 relativeLuminance = float3(0.2125f, 0.7154f, 0.0721f);
	const float luminanceCutoff = bloomFactor.x;
	const float glare = bloomFactor.y;

	float4 textureColour = shaderTexture.Sample(SampleType, input.tex);

	float luminance = dot(relativeLuminance, textureColour.xyz);
	float finalLuminance = max(0.0, luminance - luminanceCutoff) * glare;
	textureColour.xyz = textureColour.xyz * (finalLuminance / (luminance + 0.00001f));
	textureColour.a = 1.0f;

	return saturate(textureColour);
}

