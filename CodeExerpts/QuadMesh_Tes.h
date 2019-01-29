#pragma once
#include "DXF.h"

using namespace DirectX;

class QuadMesh_Tes : public BaseMesh
{
public:
	QuadMesh_Tes(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT2 resolution);
	~QuadMesh_Tes();

	void sendData(ID3D11DeviceContext* deviceContext);

protected:
	void initBuffers(ID3D11Device* device);

	struct TerrainVertexStructure
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	XMFLOAT2 quadSize;
};