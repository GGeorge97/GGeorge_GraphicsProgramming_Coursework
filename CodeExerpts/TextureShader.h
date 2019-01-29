#pragma once
#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class TextureShader : public BaseShader
{
public:
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

