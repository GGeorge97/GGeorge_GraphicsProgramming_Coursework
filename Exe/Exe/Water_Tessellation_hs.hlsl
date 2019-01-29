cbuffer TessellationBuffer : register (b0)
{
	float4 tessellationFactor;
	float4 playerCamPos;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float3 distanceVec = playerCamPos - (inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4;
	float eighthLength = length(distanceVec) / 8; // Take an eighth of the length to reduce the impact of the distance tessellation
	float tesFactor = 64;

	output.edges[0] = clamp(tesFactor - eighthLength, tessellationFactor.y, tessellationFactor.z);
	output.edges[1] = clamp(tesFactor - eighthLength, tessellationFactor.y, tessellationFactor.z);
	output.edges[2] = clamp(tesFactor - eighthLength, tessellationFactor.y, tessellationFactor.z);
	output.edges[3] = clamp(tesFactor - eighthLength, tessellationFactor.y, tessellationFactor.z);

	output.inside[0] = clamp(tesFactor - eighthLength, tessellationFactor.y, tessellationFactor.z);
	output.inside[1] = clamp(tesFactor - eighthLength, tessellationFactor.y, tessellationFactor.z);

	return output;
}


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;

	output.position = patch[pointId].position;
	output.normal = patch[pointId].normal;
	output.tex = patch[pointId].tex;

	return output;
}