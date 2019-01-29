#include "LightShader.h"

LightShader::LightShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"Light_vs.cso", L"FullLighting_ps.cso");
}
LightShader::~LightShader()
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
	if (lightBuffer) {
		lightBuffer->Release();
		lightBuffer = 0;
	}
	if (attenBuffer) {
		attenBuffer->Release();
		attenBuffer = 0;
	}
	if (cameraBuffer) {
		cameraBuffer->Release();
		cameraBuffer = 0;
	}

	BaseShader::~BaseShader();
}

void LightShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture, vector<Light*> lights, XMFLOAT3 cameraPos)
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

	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBufferType* lightDataPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		lightDataPtr->ambient[i] = lights.at(i)->getAmbientColour();
		lightDataPtr->diffuse[i] = lights.at(i)->getDiffuseColour();
		lightDataPtr->position[i] = XMFLOAT4(lights.at(i)->getPosition().x, lights.at(i)->getPosition().y, lights.at(i)->getPosition().z, 0.0f);
		lightDataPtr->lightDirection[i] = XMFLOAT4(lights.at(i)->getDirection().x, lights.at(i)->getDirection().y, lights.at(i)->getDirection().z, 0.0f);
		lightDataPtr->lightTypeID[i] = XMFLOAT4(lights.at(i)->getLightTypeID(), 0.0f, 0.0f, 0.0f);
		lightDataPtr->specularColour[i] = lights.at(i)->getSpecularColour();
		lightDataPtr->specularPower[i] = XMFLOAT4(lights.at(i)->getSpecularPower(), 0.0f, 0.0f, 0.0f);
	}
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	deviceContext->Map(attenBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	AttenBufferType* attenDataPtr = (AttenBufferType*)mappedResource.pData;
	attenDataPtr->constantFactor = 0.02f;
	attenDataPtr->linearFactor = 0.125f;
	attenDataPtr->quadraticFactor = 0.0f;
	attenDataPtr->attenPadding = 0.0f;
	deviceContext->Unmap(attenBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &attenBuffer);

	result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CameraBufferType* camDataPtr = (CameraBufferType*)mappedResource.pData;
	camDataPtr->cameraPosition = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	deviceContext->Unmap(cameraBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &cameraBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void LightShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
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
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	attenBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	attenBufferDesc.ByteWidth = sizeof(AttenBufferType);
	attenBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	attenBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	attenBufferDesc.MiscFlags = 0;
	attenBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&attenBufferDesc, NULL, &attenBuffer);

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
}
