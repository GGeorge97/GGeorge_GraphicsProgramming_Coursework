#include "Macros.h"

Texture2D texture0 : register(t0);
Texture2D textureBlend : register(t1);
Texture2D depthMapTextureArray[NUM_SHADOWMAPS] : register(t2);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
	float4 ambient[NUM_LIGHTS];
	float4 diffuse[NUM_LIGHTS];
	float4 position[NUM_LIGHTS];
	float4 lightDirection[NUM_LIGHTS];
	float4 lightType[NUM_LIGHTS];
	float4 specularColour[NUM_LIGHTS];
	float4 specularPower[NUM_LIGHTS];
	bool specularEnabled;
	float3 padding;
};

cbuffer AttenBuffer : register(b1)
{
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
	float attenPadding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
	float3 worldPosition : TEXCOORD2;
	float4 lightViewPos[NUM_SHADOWMAPS] : TEXCOORD3;
};

float4 calculateShadow(float4 lightView, Texture2D depthMap, float4 textureCol)
{
	float depthValue;
	float lightDepthValue;
	float shadowMapBias = 0.2f;

	float2 pTexCoord = lightView.xy / lightView.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	if (pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f)
	{
		return textureCol;
	}

	float4 depthFloat;
	depthFloat.x = depthMap.Sample(shadowSampler, pTexCoord).r;
	depthFloat.y = lightView.z / lightView.w;
	depthFloat.y -= shadowMapBias;

	return depthFloat;
}

float calculateAttenuation(float distance)
{
	float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));
	return attenuation;
}

float4 directionalLightDiffuseCalc(float3 direction, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, direction));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 pointLightDiffuseCalc(float4 position, float3 normal, float3 worldPosition, float4 diffuse)
{
	float vectorLength = length(position - worldPosition);
	float lightAttenuation = calculateAttenuation(vectorLength);
	float3 lightDirVec = normalize(position - worldPosition);
	float intensity = saturate(dot(normal, lightDirVec));
	diffuse *= lightAttenuation;
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 spotLightDiffuseCalc(float3 direction, float3 lightDirVec, float diffuse)
{
	float cone = 100.0f;
	float4 colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float3 dirNormal = normalize(direction);

	colour = saturate(pow(max(dot(-lightDirVec, dirNormal), 0.0f), cone) * diffuse);

	return colour;
}

// BLINN-PHONG BRDF
float4 pointLightSpecularCalc(float3 position, float3 normal, float3 worldPos, float3 viewVector, float4 specularColour, float specularPower)
{
	float3 lightDir = normalize(worldPos - position);
	float3 halfway = normalize(normalize(viewVector - lightDir));
	float specularIntensity = pow(max(dot(normal, halfway), 0.0f), specularPower);
	return saturate(specularColour * specularIntensity);
}

//	    COOK-TORRENCE BRDF		\\
|	   Beckmann Distribution	|	
float4 calculateDistribution(float3 halfway, float3 normal, float pI)
{
	const float roughness = 0.1f;
	float D = 1 / (pI * (roughness * roughness) * (pow(dot(normal, halfway), 4))) * exp(((dot(normal, halfway) * dot(normal, halfway)) - 1) / ((roughness * roughness) * (dot(normal, halfway) * dot(normal, halfway))));
	return D;
}
//    Shlick's Approximation	\\
|								|
float calculateFresnel(float3 halfway)
{
	float F = 1 + (1 - 1) * pow(1 - halfway, 5);
	return F;
}
//  Geometric attenuation term	\\
|								|
float3 calculateGeometery(float3 halfway, float3 normal, float3 viewVector, float3 lightDirection)
{
	float y = (2 * (dot(halfway, normal) * dot(viewVector, normal))) / dot(viewVector, halfway);
	float z = (2 * (dot(halfway, normal) * dot(lightDirection, normal))) / dot(viewVector, halfway);
	float3 G = (1.0f, y, z);
	return G;
}
// Calculate specular lighting	\\
|								|
float4 calculateSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower, float pI)
{
	float3 halfway = normalize(lightDirection + viewVector);
	float3 DFG = calculateDistribution(halfway, normal, pI) * calculateFresnel(halfway) * calculateGeometery(halfway, normal, viewVector, lightDirection);
	float specularIntensity = DFG / (pI / (dot(viewVector, normal) * (dot(normal, lightDirection))));
	return saturate(specularColour * specularIntensity);
}
//______________________________\\

float4 main(InputType input) : SV_TARGET
{
	const float pI = 3.14159265358979323846f;
	float4 textureColour = texture0.Sample(sampler0, input.tex);
	float4 rockBlend = textureBlend.Sample(sampler0, input.tex);
	float4 ambientLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specularLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 finalColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float2 depthFloat = float2(0.0f, 0.0f);

	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		ambientLight += ambient[i];
		if (lightType[i].x == 0)		// Directional Light
		{
			depthFloat = calculateShadow(input.lightViewPos[i], depthMapTextureArray[i], textureColour);
			if (depthFloat.y < depthFloat.x)
			{
				for (int j = 0; j < NUM_SHADOWMAPS; j++)
				{
					finalColour += directionalLightDiffuseCalc(-lightDirection[j].xyz, input.normal, diffuse[j]);
				}
			}
		}
		else if (lightType[i].x == 1)	// Point Light
		{
			finalColour += pointLightDiffuseCalc(position[i], input.normal, input.worldPosition, diffuse[i]);
			if (specularEnabled)
				specularLight += pointLightSpecularCalc(position[i].xyz, input.normal, input.worldPosition, input.viewVector, specularColour[i], specularPower[i].x);
		}
		else if (lightType[i].x == 2)	// Spot Light
		{
			float range = 500.0f;
			float3 lightDistanceVec = position[i] - input.worldPosition;
			float lightLength = length(lightDistanceVec);
			if (lightLength > range)
			{
				return (0.0f, 0.0f, 0.0f, 0.0f);
			}
			float lightContactVal = dot(lightDistanceVec, input.normal);
			float lightAttenuation = calculateAttenuation(lightLength);
			float3 lightDirVector = normalize(lightDistanceVec);
			if (lightContactVal > 0.0f)
			{
				finalColour += spotLightDiffuseCalc(-lightDirection[i].xyz, lightDirVector, diffuse[i]);
			}
		}
	}
	ambientLight /= NUM_LIGHTS;
	finalColour += ambientLight;
	finalColour *= textureColour;
	finalColour += specularLight;

	//return float4(input.normal.x, input.normal.y, input.normal.z, 1.0f)
	return saturate(finalColour * rockBlend);
}