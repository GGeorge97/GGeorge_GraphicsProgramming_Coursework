#include "Macros.h"

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer waterBuffer : register(b1)
{
	float4 cameraPosition;
	float2 textureRes;
	float2 planeRes;
}

cbuffer TimeBuffer : register(b2)
{
	float time;
	float height;
	float frequency;
	float speed;
};

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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
	float3 worldPosition : TEXCOORD2;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, uvCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvCoord.y);
	float3 vertexPosition = lerp(v1, v2, uvCoord.x);

	vertexPosition.y += height * (sin((frequency * vertexPosition.z) + (speed * time)));

	float2 texCoord = uvCoord.xy;
	texCoord.x = texCoord.x + ((time * speed) / 100);
	output.tex = texCoord;

	float3 tangent = normalize(patch[0].position - patch[1].position);
	float3 bitangent = normalize(patch[0].position - patch[3].position);
	output.normal = cross(bitangent, tangent);

	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	float4 worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.worldPosition = worldPosition;
	output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);

	return output;
}

