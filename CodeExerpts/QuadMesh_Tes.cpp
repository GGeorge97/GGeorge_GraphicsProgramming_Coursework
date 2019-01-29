#include "QuadMesh_Tes.h"

QuadMesh_Tes::QuadMesh_Tes(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT2 size)
{
	quadSize = size;
	initBuffers(device);
}

QuadMesh_Tes::~QuadMesh_Tes()
{
	BaseMesh::~BaseMesh();
}

void QuadMesh_Tes::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(TerrainVertexStructure);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}

void QuadMesh_Tes::initBuffers(ID3D11Device * device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = 4;
	indexCount = 4;
	
	TerrainVertexStructure* vertices = new TerrainVertexStructure[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];
	
	vertices[0].position = XMFLOAT3(-quadSize.x, 0.0f, quadSize.y);	// Top left.
	vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);
	
	vertices[1].position = XMFLOAT3(-quadSize.x, 0.0f, -quadSize.y);	// Bottom left.
	vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.0f, 1.0f);
	
	vertices[2].position = XMFLOAT3(quadSize.x, 0.0f, -quadSize.y);	// Bottom right.
	vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	
	vertices[3].position = XMFLOAT3(quadSize.x, 0.0f, quadSize.y);	// Top right.
	vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 1.0f);
	
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	D3D11_BUFFER_DESC vertexBufferDesc = { sizeof(TerrainVertexStructure) * vertexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	vertexData = { vertices, 0 , 0 };
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc = { sizeof(unsigned long) * indexCount, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	indexData = { indices, 0, 0 };
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}