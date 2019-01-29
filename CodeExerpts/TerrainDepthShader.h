#pragma once
#include "DXF.h"
#include "Macros.h"

using namespace std;
using namespace DirectX;

class TerrainDepthShader : public BaseShader
{
public:
	TerrainDepthShader(ID3D11Device* device, HWND hwnd);
	~TerrainDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* heightMap, XMFLOAT4 tessellationFactor, XMFLOAT3 cameraPos);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename);

	ID3D11SamplerState* sampleState;
	ID3D11Buffer* matrixBuffer;

	ID3D11Buffer* tessellationBuffer;
	struct TessellationBufferType
	{
		XMFLOAT4 tessellationFactor;
		XMFLOAT4 playerCamPos;
	};
	ID3D11Buffer* depthBuffer;
	struct DepthBufferType
	{
		XMFLOAT2 textureRes;
		XMFLOAT2 planeRes;
	};
};