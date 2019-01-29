#include "BrightPassShader.h"

BrightPassShader::BrightPassShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"BrightPass_vs.cso", L"BrightPass_ps.cso");
}
BrightPassShader::~BrightPassShader()
{
	if (matrixBuffer) {
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout) {
		layout->Release();
		layout = 0;
	}
	if (bloomFactorBuffer) {
		bloomFactorBuffer->Release();
		bloomFactorBuffer = 0;
	}

	BaseShader::~BaseShader();
}

void BrightPassShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, XMFLOAT4 bloomFactor)
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
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	result = deviceContext->Map(bloomFactorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	BloomFactorBufferType* bloomFactorDataPtr = (BloomFactorBufferType*)mappedResource.pData;
	deviceContext->Unmap(bloomFactorBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &bloomFactorBuffer);
	bloomFactorDataPtr->bloomFactor = bloomFactor;
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void BrightPassShader::initShader(WCHAR * vsFilename, WCHAR * psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC bloomFactorBufferDesc;

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
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	bloomFactorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bloomFactorBufferDesc.ByteWidth = sizeof(BloomFactorBufferType);
	bloomFactorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bloomFactorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bloomFactorBufferDesc.MiscFlags = 0;
	bloomFactorBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&bloomFactorBufferDesc, NULL, &bloomFactorBuffer);
}
