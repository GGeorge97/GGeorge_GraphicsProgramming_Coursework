#ifndef _APP_H
#define _APP_H

#include <windows.h>
#include "DXF.h"
#include "DepthShader.h"
#include "TerrainDepthShader.h"
#include "WaterDepthShader.h"
#include "Terrain_Tessellation_Shader.h"
#include "Water_Tessellation_Shader.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "TextureAddShader.h"
#include "BrightPassShader.h"
#include "BloomShader.h"
#include "QuadMesh_Tes.h"

class Application : public BaseApplication
{
public:

	Application();
	~Application();

	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);
	bool frame();

protected:
	bool render();
	void depthPass(RenderTexture* shadowMap, Light* light);
	void scenePass();
	void postProcesses();
		// Bloom ===============
		void brightPassFilter();
		void downSample(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex);
		void bloom(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex, bool horizontal);
		void upSample(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex);
		void addTextures(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex0, RenderTexture* inputTex1);
		// =====================
	void finalPass();
	void gui();

private:
	vector<Light*> lights;
	Light* light0;
	Light* light1;
	Light* light2;
	Light* light3;

	DepthShader* depthShader;
	TerrainDepthShader* terrainDepthShader;
	WaterDepthShader* waterDepthShader;
	Terrain_Tessellation_Shader* terrainTessShader;
	Water_Tessellation_Shader* waterTessellationShader;
	LightShader* lightShader;
	TextureShader* textureShader;
	TextureAddShader* textureAddShader;
	BrightPassShader* brightPassShader;
	BloomShader* bloomShader;

	RenderTexture* sceneTexture;
	RenderTexture* brightPassTex;
	RenderTexture* bloomTex_H;
	RenderTexture* bloomTex_V;
	RenderTexture* bloomTexDownSample_H;
	RenderTexture* bloomTexDownSample_V;
	RenderTexture* bloomTex_Final;
	RenderTexture* downSampleTexture;
	RenderTexture* upSampleTexture;
	vector<RenderTexture*> shadowMaps;
	ID3D11ShaderResourceView* depthMaps[2];
	RenderTexture* shadowMap0;
	RenderTexture* shadowMap1;

	OrthoMesh* screenSizeOrthMesh;
	OrthoMesh* downsampleOrthMesh;
	OrthoMesh* shadowMap0Mesh;
	OrthoMesh* shadowMap1Mesh;

	QuadMesh_Tes* terrainPlane;
	QuadMesh_Tes* waterPlane;
	SphereMesh* sphereMesh;

	XMFLOAT4 tessellationFactor = XMFLOAT4(0.0f, 32.0f, 64.0f, 0.0f);	// Detail Falloff, Minimum Tes, Maximum Tes;
	XMFLOAT4 timeBufferData = XMFLOAT4(0.25f, 0.2f, 1.0f, 0.0f);		// Speed, Height, Freq, Time
	XMFLOAT4 bloomFactor = XMFLOAT4(0.9f, 0.1f, 0.0f, 0.0f);			// Cutoff, Glare

	XMFLOAT3 pointLightPosition = XMFLOAT3(20.0f, 0.0f, -8.0f);
	XMFLOAT4 pointLightColour = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT3 spotLightPosition = XMFLOAT3(10.9f, 5.5f, 18.8f);
	XMFLOAT3 spotLightDirection = XMFLOAT3(0.1f, 0.1f, 0.2f);
	XMFLOAT3 directional0LightPos = XMFLOAT3(0.0f, 0.0f, -20.0f);
	XMFLOAT3 directional0LightDir = XMFLOAT3(0.6f, -0.2f, 1.0f);
	XMFLOAT4 directional0LightColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT3 directional1LightPos = XMFLOAT3(0.0f, 0.0f, 20.f);
	XMFLOAT3 directional1LightDir = XMFLOAT3(0.6f, -0.2f, -1.0f);
	XMFLOAT4 directional1LightColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Init scene variables
	int shadowmapWidth = 2048;
	int shadowmapHeight = 2048;
	int sceneWidth = 100;
	int sceneHeight = 100;
	bool showBrightpassTex = false;
	bool renderTerrain = true;
	bool renderWater = true;
	bool renderSphere = true;
};

#endif