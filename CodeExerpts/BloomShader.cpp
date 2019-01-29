#include "BloomShader.h"

BloomShader::BloomShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"Bloom_vs.cso", L"Bloom_ps.cso");
}
BloomShader::~BloomShader()
{
	if (matrixBuffer) {
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	if (layout) {
		layout->Release();
		layout = 0;
	}
	if (bloomBuffer) {
		bloomBuffer->Release();
		bloomBuffer = 0;
	}

	BaseShader::~BaseShader();
}

void BloomShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* bloomTextures, XMFLOAT2 textureSize, bool horizontal)
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

	result = deviceContext->Map(bloomBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	BloomBufferType* bloomDataPtr = (BloomBufferType*)mappedResource.pData;
	bloomDataPtr->textureSize = XMFLOAT4(textureSize.x, textureSize.y, 0.0f, 0.0f);
	bloomDataPtr->horizontal = horizontal;
	bloomDataPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(bloomBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &bloomBuffer);

	deviceContext->PSSetShaderResources(0, 1, &bloomTextures);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void BloomShader::initShader(WCHAR * vsFilename, WCHAR * psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC bloomBufferDesc;

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
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	bloomBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bloomBufferDesc.ByteWidth = sizeof(BloomBufferType);
	bloomBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bloomBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bloomBufferDesc.MiscFlags = 0;
	bloomBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&bloomBufferDesc, NULL, &bloomBuffer);
}
