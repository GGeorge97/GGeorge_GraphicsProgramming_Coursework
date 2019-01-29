#include "Macros.h"

Texture2D heightmap0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix[NUM_SHADOWMAPS];
	matrix lightProjectionMatrix[NUM_SHADOWMAPS];
};

cbuffer terrainBuffer : register(b1)
{
	float4 cameraPosition;
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
	float3 worldPosition : TEXCOORD2;
	float4 lightViewPos[NUM_SHADOWMAPS] : TEXCOORD3;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, uvCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvCoord.y);
	float3 vertexPosition = lerp(v1, v2, uvCoord.x);

	float2 texCoord = uvCoord.xy;
	output.tex = texCoord * 4.0f;

	float heightmapColour = heightmap0.SampleLevel(sampler0, texCoord, 0, 0).y;
	vertexPosition.y += heightmapColour * 12;

	float2 leftTex = texCoord + float2(-(planeRes.x / textureRes.x), 0.0f);
	float2 rightTex = texCoord + float2((planeRes.x / textureRes.x), 0.0f);
	float2 upTex = texCoord + float2(0.0f, -(planeRes.y / textureRes.y));
	float2 downTex = texCoord + float2(0.0f, (planeRes.y / textureRes.y));

	float leftTexY = heightmap0.SampleLevel(sampler0, leftTex, 0).y;
	float rightTexY = heightmap0.SampleLevel(sampler0, rightTex, 0).y;
	float upTexY = heightmap0.SampleLevel(sampler0, upTex, 0).y;
	float downTexY = heightmap0.SampleLevel(sampler0, downTex, 0).y;

	float3 tangent = normalize(float3(1.0f, rightTexY - leftTexY, 0.0f));
	float3 bitangent = normalize(float3( 0.0f, downTexY - upTexY, 1.0f));
	output.normal = cross(bitangent, tangent);

	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	for (int i = 0; i < NUM_SHADOWMAPS; i++)
	{
		output.lightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
		output.lightViewPos[i] = mul(output.lightViewPos[i], lightViewMatrix[i]);
		output.lightViewPos[i] = mul(output.lightViewPos[i], lightProjectionMatrix[i]);
	}

	float4 worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.worldPosition = worldPosition;
	output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);

	return output;
} 
