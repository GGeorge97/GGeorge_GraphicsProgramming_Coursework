#pragma once
#include "DXF.h"
#include "Macros.h"

using namespace std;
using namespace DirectX;

class Terrain_Tessellation_Shader : public BaseShader
{
public:
	Terrain_Tessellation_Shader(ID3D11Device* device, HWND hwnd);
	~Terrain_Tessellation_Shader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* heightMap, ID3D11ShaderResourceView* depthMaps[NUM_SHADOWMAPS], ID3D11ShaderResourceView* textureShrub, ID3D11ShaderResourceView* textureRock, XMFLOAT4 tessellationFactor, vector<Light*> lights, XMFLOAT3 cameraPos, bool specEnabled);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

	ID3D11SamplerState* textureSampler;
	ID3D11SamplerState* shadowmapSampler;

	ID3D11Buffer* matrixBuffer;
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView[NUM_SHADOWMAPS];
		XMMATRIX lightProjection[NUM_SHADOWMAPS];
	};
	ID3D11Buffer* tessellationBuffer;
	struct TessellationBufferType
	{
		XMFLOAT4 tessellationFactor;
		XMFLOAT4 playerCamPos;
	};
	ID3D11Buffer* lightBuffer;
	struct LightBufferType
	{
		XMFLOAT4 ambient[NUM_LIGHTS];
		XMFLOAT4 diffuse[NUM_LIGHTS];
		XMFLOAT4 position[NUM_LIGHTS];
		XMFLOAT4 lightDirection[NUM_LIGHTS];
		XMFLOAT4 lightTypeID[NUM_LIGHTS];
		XMFLOAT4 specularColour[NUM_LIGHTS];
		XMFLOAT4 specularPower[NUM_LIGHTS];
		bool specularEnabled;
		XMFLOAT3 padding;
	};
	ID3D11Buffer* attenBuffer;
	struct AttenBufferType
	{
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
		float attenPadding;
	};
	ID3D11Buffer* terrainBuffer;
	struct TerrainBufferType
	{
		XMFLOAT4 cameraPosition;
		XMFLOAT2 textureRes;
		XMFLOAT2 planeRes;
	};
};

