#pragma once
#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class BrightPassShader : public BaseShader
{
public:
	BrightPassShader(ID3D11Device* device, HWND hwnd);
	~BrightPassShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* renderTexture, XMFLOAT4 bloomFactor);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;

	ID3D11Buffer* bloomFactorBuffer;
	struct BloomFactorBufferType
	{
		XMFLOAT4 bloomFactor;
	};
};

