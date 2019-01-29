#pragma once
#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class TextureAddShader : public BaseShader
{
public:
	TextureAddShader(ID3D11Device* device, HWND hwnd);
	~TextureAddShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture0, ID3D11ShaderResourceView* texture1);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

