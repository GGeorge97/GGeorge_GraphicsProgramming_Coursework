#pragma once
#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class BloomShader : public BaseShader
{
public:
	BloomShader(ID3D11Device* device, HWND hwnd);
	~BloomShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* bloomTextures, XMFLOAT2 textureSize, bool horizontal);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;

	ID3D11Buffer* bloomBuffer;
	struct BloomBufferType
	{
		XMFLOAT4 textureSize;
		bool horizontal;
		XMFLOAT3 padding;
	};
};

