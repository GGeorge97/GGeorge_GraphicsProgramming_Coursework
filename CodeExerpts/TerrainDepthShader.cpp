#include "TerrainDepthShader.h"

TerrainDepthShader::TerrainDepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"TerrainDepth_vs.cso", L"Terrain_Tessellation_hs.cso", L"TerrainDepth_ds.cso", L"Depth_ps.cso");
}
TerrainDepthShader::~TerrainDepthShader()
{
	if (sampleState) {
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer) {
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout) {
		layout->Release();
		layout = 0;
	}
	if (tessellationBuffer) {
		tessellationBuffer->Release();
		tessellationBuffer = 0;
	}
	if (depthBuffer) {
		depthBuffer->Release();
		depthBuffer = 0;
	}

	BaseShader::~BaseShader();
}

void TerrainDepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* heightMap, XMFLOAT4 tessellationFactor, XMFLOAT3 cameraPos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* matDataPtr = (MatrixBufferType*)mappedResource.pData;
	matDataPtr->world = tworld;
	matDataPtr->view = tview;
	matDataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	result = deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TessellationBufferType* tesDataPtr = (TessellationBufferType*)mappedResource.pData;
	tesDataPtr->tessellationFactor = tessellationFactor;
	tesDataPtr->playerCamPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	deviceContext->Unmap(tessellationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);

	result = deviceContext->Map(depthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	DepthBufferType* depthDataPtr = (DepthBufferType*)mappedResource.pData;
	depthDataPtr->textureRes = XMFLOAT2(600.0f, 600.0f);
	depthDataPtr->planeRes = XMFLOAT2(35.0f, 35.0f);
	deviceContext->Unmap(depthBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &depthBuffer);

	deviceContext->DSSetShaderResources(0, 1, &heightMap);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
}

void TerrainDepthShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC depthDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC attenBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;

	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationBuffer);

	depthDesc.Usage = D3D11_USAGE_DYNAMIC;
	depthDesc.ByteWidth = sizeof(DepthBufferType);
	depthDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	depthDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	depthDesc.MiscFlags = 0;
	depthDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&depthDesc, NULL, &depthBuffer);
}

void TerrainDepthShader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	initShader(vsFilename, psFilename);

	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}
