#include "Terrain_Tessellation_Shader.h"

Terrain_Tessellation_Shader::Terrain_Tessellation_Shader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"Terrain_Tessellation_vs.cso", L"Terrain_Tessellation_hs.cso", L"Terrain_Tessellation_ds.cso", L"Terrain_Tessellation_ps.cso");
}
Terrain_Tessellation_Shader::~Terrain_Tessellation_Shader()
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
	if (lightBuffer) {
		lightBuffer->Release();
		lightBuffer = 0;
	}
	if (attenBuffer) {
		attenBuffer->Release();
		attenBuffer = 0;
	}
	if (terrainBuffer) {
		terrainBuffer->Release();
		terrainBuffer = 0;
	}

	BaseShader::~BaseShader();
}

void Terrain_Tessellation_Shader::setShaderParameters(ID3D11DeviceContext* deviceContext,
														const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix,
														ID3D11ShaderResourceView* heightMap,
														ID3D11ShaderResourceView* depthMaps[NUM_SHADOWMAPS],
														ID3D11ShaderResourceView* textureShrub,
														ID3D11ShaderResourceView* textureRock,
														XMFLOAT4 tessellationFactor,
														vector<Light*> lights,
														XMFLOAT3 cameraPos,
														bool specEnabled)
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
	for (int i = 0; i < NUM_SHADOWMAPS; i++)
	{
		matDataPtr->lightView[i] = XMMatrixTranspose(lights.at(i)->getViewMatrix());
		matDataPtr->lightProjection[i] = XMMatrixTranspose(lights.at(i)->getOrthoMatrix());
	}
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	result = deviceContext->Map(tessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TessellationBufferType* tesDataPtr = (TessellationBufferType*)mappedResource.pData;
	tesDataPtr->tessellationFactor = tessellationFactor;
	tesDataPtr->playerCamPos = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	deviceContext->Unmap(tessellationBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessellationBuffer);

	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBufferType* lightDataPtr = (LightBufferType*)mappedResource.pData;
	for (int i = 0; i < NUM_LIGHTS; i++)
	{
		lightDataPtr->ambient[i] = lights.at(i)->getAmbientColour();
		lightDataPtr->diffuse[i] = lights.at(i)->getDiffuseColour();
		lightDataPtr->position[i] = XMFLOAT4(lights.at(i)->getPosition().x, lights.at(i)->getPosition().y, lights.at(i)->getPosition().z, 1.0f);
		lightDataPtr->lightDirection[i] = XMFLOAT4(lights.at(i)->getDirection().x, lights.at(i)->getDirection().y, lights.at(i)->getDirection().z, 1.0f);
		lightDataPtr->lightTypeID[i] = XMFLOAT4(lights.at(i)->getLightTypeID(), 0.0f, 0.0f, 0.0f);
		lightDataPtr->specularColour[i] = lights.at(i)->getSpecularColour();
		lightDataPtr->specularPower[i] = XMFLOAT4(lights.at(i)->getSpecularPower(), 0.0f, 0.0f, 0.0f);
	}
	lightDataPtr->specularEnabled = specEnabled;
	lightDataPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
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

	result = deviceContext->Map(terrainBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TerrainBufferType* terrainDataPtr = (TerrainBufferType*)mappedResource.pData;
	terrainDataPtr->cameraPosition = XMFLOAT4(cameraPos.x, cameraPos.y, cameraPos.z, 0.0f);
	terrainDataPtr->textureRes = XMFLOAT2(600.0f, 600.0f);
	terrainDataPtr->planeRes = XMFLOAT2(30.0f, 30.0f);
	deviceContext->Unmap(terrainBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &terrainBuffer);

	deviceContext->DSSetShaderResources(0, 1, &heightMap);
	deviceContext->DSSetSamplers(0, 1, &textureSampler);

	deviceContext->PSSetShaderResources(0, 1, &textureShrub);
	deviceContext->PSSetSamplers(0, 1, &textureSampler);

	deviceContext->PSSetShaderResources(1, 1, &textureRock);
	deviceContext->PSSetSamplers(0, 1, &textureSampler);

	deviceContext->PSSetShaderResources(2, 2, depthMaps);
	deviceContext->PSSetSamplers(1, 1, &shadowmapSampler);
}

void Terrain_Tessellation_Shader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC tessellationBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC attenBufferDesc;
	D3D11_BUFFER_DESC terrainBufferDesc;

	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

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
	renderer->CreateSamplerState(&samplerDesc, &textureSampler);

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &shadowmapSampler);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	tessellationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessellationBufferDesc.ByteWidth = sizeof(TessellationBufferType);
	tessellationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessellationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessellationBufferDesc.MiscFlags = 0;
	tessellationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessellationBufferDesc, NULL, &tessellationBuffer);

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

	terrainBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	terrainBufferDesc.ByteWidth = sizeof(TerrainBufferType);
	terrainBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	terrainBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	terrainBufferDesc.MiscFlags = 0;
	terrainBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&terrainBufferDesc, NULL, &terrainBuffer);
}

void Terrain_Tessellation_Shader::initShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	initShader(vsFilename, psFilename);

	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}
