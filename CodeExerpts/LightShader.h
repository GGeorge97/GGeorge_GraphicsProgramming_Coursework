#pragma once
#include "DXF.h"
#include "Macros.h"

using namespace std;
using namespace DirectX;

class LightShader : public BaseShader
{
public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, vector<Light*> lights, XMFLOAT3 cameraPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);

	ID3D11SamplerState* sampleState;

	ID3D11Buffer* matrixBuffer;

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
	};
	ID3D11Buffer* attenBuffer;
	struct AttenBufferType
	{
		float constantFactor;
		float linearFactor;
		float quadraticFactor;
		float attenPadding;
	};
	ID3D11Buffer* cameraBuffer;
	struct CameraBufferType
	{
		XMFLOAT4 cameraPosition;
	};

};

