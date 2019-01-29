#include "Macros.h"

Texture2D heightmap0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer depthBuffer : register(b1)
{
	float2 textureRes;
	float2 planeRes;
}

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, uvCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvCoord.y);
	float3 vertexPosition = lerp(v1, v2, uvCoord.x);

	float2 texCoord = uvCoord.xy;

	float4 textureColour = heightmap0.SampleLevel(sampler0, texCoord, 0, 0);
	vertexPosition.y = textureColour.y * 12;

	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.depthPosition = output.position;

	return output;
}
