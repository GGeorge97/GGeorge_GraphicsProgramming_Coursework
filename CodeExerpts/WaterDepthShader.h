#pragma once
#include "DXF.h"
#include "Macros.h"

using namespace std;
using namespace DirectX;

class WaterDepthShader : public BaseShader
{
public:
	WaterDepthShader(ID3D11Device* device, HWND hwnd);
	~WaterDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, XMFLOAT4 tessellationFactor, XMFLOAT4 timeBufferData, XMFLOAT3 cameraPos);

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
	ID3D11Buffer* timeBuffer;
	struct TimeBufferType
	{
		float time;
		float height;
		float frequency;
		float speed;
	};
};