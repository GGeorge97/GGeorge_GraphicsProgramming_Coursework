#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class DepthShader : public BaseShader
{

public:

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);

	ID3D11Buffer* matrixBuffer;
};
