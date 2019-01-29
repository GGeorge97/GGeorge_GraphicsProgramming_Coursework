#include "Application.h"

Application::Application()
{
	depthShader = nullptr;
	terrainDepthShader = nullptr;
	terrainTessShader = nullptr;
	waterDepthShader = nullptr;
	waterTessellationShader = nullptr;
	lightShader = nullptr;
	textureShader = nullptr;
	textureAddShader = nullptr;
	brightPassShader = nullptr;
	bloomShader = nullptr;
	terrainPlane = nullptr;
	waterPlane = nullptr;
	sphereMesh = nullptr;
	light0 = nullptr;
	light1 = nullptr;
	light2 = nullptr;
	light3 = nullptr;
}
Application::~Application()
{
	if (depthShader) {
		depthShader = nullptr;
		delete depthShader;
	}
	if (terrainDepthShader) {
		terrainDepthShader = nullptr;
		delete terrainDepthShader;
	}
	if (terrainTessShader) {
		terrainTessShader = nullptr;
		delete terrainTessShader;
	}
	if (waterDepthShader) {
		waterDepthShader = nullptr;
		delete waterDepthShader;
	}
	if (waterTessellationShader) {
		waterTessellationShader = nullptr;
		delete waterTessellationShader;
	}
	if (lightShader) {
		lightShader = nullptr;
		delete lightShader;
	}
	if (textureShader) {
		textureShader = nullptr;
		delete textureShader;
	}
	if (textureAddShader) {
		textureAddShader = nullptr;
		delete textureAddShader;
	}
	if (brightPassShader) {
		brightPassShader = nullptr;
		delete brightPassShader;
	}
	if (bloomShader) {
		bloomShader = nullptr;
		delete bloomShader;
	}
	if (terrainPlane) {
		terrainPlane = nullptr;
		delete terrainPlane;
	}
	if (waterPlane) {
		waterPlane = nullptr;
		delete waterPlane;
	}
	if (sphereMesh) {
		sphereMesh = nullptr;
		delete sphereMesh;
	}
	for (RenderTexture* r : shadowMaps) {
		r = nullptr;
		delete r;
	}
	for (Light* l : lights) {
		l = nullptr;
		delete l;
	}

	BaseApplication::~BaseApplication();
}

void Application::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);


	// Init shaders
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	terrainDepthShader = new TerrainDepthShader(renderer->getDevice(), hwnd);
	terrainTessShader = new Terrain_Tessellation_Shader(renderer->getDevice(), hwnd);
	waterDepthShader = new WaterDepthShader(renderer->getDevice(), hwnd);
	waterTessellationShader = new Water_Tessellation_Shader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	textureAddShader = new TextureAddShader(renderer->getDevice(), hwnd);
	brightPassShader = new BrightPassShader(renderer->getDevice(), hwnd);
	bloomShader = new BloomShader(renderer->getDevice(), hwnd);

	// Load textures
	textureMgr->loadTexture("shrubs", L"../res/shrubs.jpg");
	textureMgr->loadTexture("rock", L"../res/rock.jpg");
	textureMgr->loadTexture("water", L"../res/water.jpg");
	textureMgr->loadTexture("heightmap", L"../res/heightmap.jpg");

	// Init render textures
	sceneTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	brightPassTex = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTex_Final = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTex_H = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTex_V = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTexDownSample_H = new RenderTexture(renderer->getDevice(), screenWidth / 4, screenHeight / 4, SCREEN_NEAR, SCREEN_DEPTH);
	bloomTexDownSample_V = new RenderTexture(renderer->getDevice(), screenWidth / 4, screenHeight / 4, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 4, screenHeight / 4, SCREEN_NEAR, SCREEN_DEPTH);
	upSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	shadowMap0 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMap1 = new RenderTexture(renderer->getDevice(), shadowmapWidth, shadowmapHeight, 0.1f, 100.f);
	shadowMaps.push_back(shadowMap0);
	shadowMaps.push_back(shadowMap1);
	depthMaps[0] = shadowMap0->getShaderResourceView();
	depthMaps[1] = shadowMap1->getShaderResourceView();

	// Init meshes
	screenSizeOrthMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	downsampleOrthMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth / 4, screenHeight / 4);
	terrainPlane = new QuadMesh_Tes(renderer->getDevice(), renderer->getDeviceContext(), XMFLOAT2(35.0f, 35.0f)); 
	waterPlane = new QuadMesh_Tes(renderer->getDevice(), renderer->getDeviceContext(), XMFLOAT2(26.0f, 26.0f));
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	// Init lights
	light0 = new Light;
	light0->setPosition(0.0f, 0.0f, -20.0f);
	light0->setAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	light0->setDiffuseColour(0.0f, 0.5f, 0.0f, 1.0f);
	light0->setDirection(0.6f, -0.1f, 1.0f);
	light0->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);
	light0->setLightTypeID(DIRECTIONAL_L);
	lights.push_back(light0);

	light1 = new Light;
	light1->setPosition(0.0f, 0.0f, 20.f);
	light1->setAmbientColour(0.15f, 0.15f, 0.15f, 1.0f);
	light1->setDiffuseColour(0.5f, 0.0f, 0.0f, 1.0f);
	light1->setDirection(0.6f, -0.1f, -1.0f);
	light1->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);
	light1->setLightTypeID(DIRECTIONAL_L);
	lights.push_back(light1);

	light2 = new Light;
	light2->setPosition(20.0f, 0.0f, -8.0f);
	light2->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light2->setDiffuseColour(1.0f, 1.0f, 0.0f, 1.0f);
	light2->setSpecularPower(500.0f);
	light2->setSpecularColour(1.0f, 1.0f, 0.0f, 1.0f);
	light2->setLightTypeID(POINT_L);
	lights.push_back(light2);

	light3 = new Light;
	light3->setPosition(10.9f, 5.5f, 18.8f);
	light3->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	light3->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light3->setDirection(0.1f, 0.1f, 0.2f);
	light3->setLightTypeID(SPOT_L);
	lights.push_back(light3);
}

bool Application::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}

	// Retrieve deltatime
	timeBufferData.w += timer->getTime();

	// Update light objects
	light0->setPosition(directional0LightPos.x, directional0LightPos.y, directional0LightPos.z);
	light0->setDirection(directional0LightDir.x, directional0LightDir.y, directional0LightDir.z);
	light0->setDiffuseColour(directional0LightColour.x, directional0LightColour.y, directional0LightColour.z, directional0LightColour.w);
	light0->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	light1->setPosition(directional1LightPos.x, directional1LightPos.y, directional1LightPos.z);
	light1->setDirection(directional1LightDir.x, directional1LightDir.y, directional1LightDir.z);
	light1->setDiffuseColour(directional1LightColour.x, directional1LightColour.y, directional1LightColour.z, directional1LightColour.w);
	light1->generateOrthoMatrix(sceneWidth, sceneHeight, 0.1f, 100.f);

	light2->setPosition(pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
	light2->setDiffuseColour(pointLightColour.x, pointLightColour.y, pointLightColour.z, pointLightColour.w);
	light2->setSpecularColour(pointLightColour.x, pointLightColour.y, pointLightColour.z, pointLightColour.w);

	light3->setPosition(spotLightPosition.x, spotLightPosition.y, spotLightPosition.z);
	light3->setDirection(spotLightDirection.x, spotLightDirection.y, spotLightDirection.z);

	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::render()
{
	// Run depth passes for all directional lights
	for (int i = 0; i < NUM_SHADOWMAPS; i++)
	{
		depthPass(shadowMaps.at(i), lights.at(i));
	}
	// Render scene with lighting
	scenePass();
	renderer->setWireframeMode(false);
	// Render postprocesses
	postProcesses();
	// Render final scene with post processes applied
	finalPass();

	return true;
}

void Application::depthPass(RenderTexture* shadowMap , Light* light)
{
	shadowMap->setRenderTarget(renderer->getDeviceContext());
	shadowMap->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	light->generateViewMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();

	XMMATRIX rotationMatrix;
	XMMATRIX translationMatrix;
	XMMATRIX scaleMatrix;

	if (renderTerrain)
	{
		worldMatrix = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
		terrainPlane->sendData(renderer->getDeviceContext());
		terrainDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture("heightmap"), tessellationFactor, camera->getPosition());
		terrainDepthShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
	}

	if (renderWater)
	{
		worldMatrix = XMMatrixTranslation(5.5f, -1.5f, -1.0f);
		waterPlane->sendData(renderer->getDeviceContext());
		waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, tessellationFactor, timeBufferData, camera->getPosition());
		waterDepthShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
	}

	if (renderSphere)
	{
		translationMatrix = XMMatrixTranslation(-10.0f, 5.0f, 10.0f);
		worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);
		rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, timeBufferData.w, 0.0f);
		worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
		sphereMesh->sendData(renderer->getDeviceContext());
		depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
		depthShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
	}

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void Application::scenePass()
{
	sceneTexture->setRenderTarget(renderer->getDeviceContext());
	sceneTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	XMMATRIX rotationMatrix;
	XMMATRIX translationMatrix;
	XMMATRIX scaleMatrix;

	if (renderTerrain)
	{
		worldMatrix = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
		terrainPlane->sendData(renderer->getDeviceContext());
		terrainTessShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("heightmap"), depthMaps, textureMgr->getTexture("shrubs"), textureMgr->getTexture("rock"), tessellationFactor, lights, camera->getPosition(), false);
		terrainTessShader->render(renderer->getDeviceContext(), terrainPlane->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
	}

	if (renderWater)
	{
		worldMatrix = XMMatrixTranslation(5.5f, -1.5f, -1.0f);
		waterPlane->sendData(renderer->getDeviceContext());
		waterTessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("water"), tessellationFactor, timeBufferData, lights, camera->getPosition(), true);
		waterTessellationShader->render(renderer->getDeviceContext(), waterPlane->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
	}

	if (renderSphere)
	{
		translationMatrix = XMMatrixTranslation(-10.0f, 5.0f, 10.0f);
		worldMatrix = XMMatrixMultiply(worldMatrix, translationMatrix);
		rotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, timeBufferData.w, 0.0f);
		worldMatrix = XMMatrixMultiply(worldMatrix, rotationMatrix);
		sphereMesh->sendData(renderer->getDeviceContext());
		lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("rock"), lights, camera->getPosition());
		lightShader->render(renderer->getDeviceContext(), sphereMesh->getIndexCount());
		worldMatrix = renderer->getWorldMatrix();
	}

	renderer->setBackBufferRenderTarget();
}

void Application::postProcesses()
{
	// Bloom ===========
	brightPassFilter();
	bloom(bloomTex_H, screenSizeOrthMesh, brightPassTex, true);
	bloom(bloomTex_V, screenSizeOrthMesh, bloomTex_H, false);
	for (int i = 0; i < EXTRA_BLUR_PASSES; i++)
	{
		bloom(bloomTex_H, screenSizeOrthMesh, bloomTex_V, true);
		bloom(bloomTex_V, screenSizeOrthMesh, bloomTex_H, false);
	}
	downSample(downSampleTexture, downsampleOrthMesh, bloomTex_V);
	bloom(bloomTexDownSample_H, downsampleOrthMesh, downSampleTexture, true);
	bloom(bloomTexDownSample_V, downsampleOrthMesh, bloomTexDownSample_H, false);
	for (int i = 0; i < EXTRA_BLUR_PASSES; i++)
	{
		bloom(bloomTexDownSample_H, downsampleOrthMesh, bloomTexDownSample_V, true);
		bloom(bloomTexDownSample_V, downsampleOrthMesh, bloomTexDownSample_H, false);
	}
	upSample(upSampleTexture, screenSizeOrthMesh, bloomTexDownSample_V);
	addTextures(bloomTex_Final, screenSizeOrthMesh, upSampleTexture, bloomTex_V);
	// =================
}
void Application::brightPassFilter()
{
	brightPassTex->setRenderTarget(renderer->getDeviceContext());
	brightPassTex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = brightPassTex->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	screenSizeOrthMesh->sendData(renderer->getDeviceContext());
	brightPassShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView(), bloomFactor);
	brightPassShader->render(renderer->getDeviceContext(), screenSizeOrthMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}
void Application::downSample(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex)
{
	outputTex->setRenderTarget(renderer->getDeviceContext());
	outputTex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = outputTex->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	outputMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, inputTex->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), outputMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}
void Application::bloom(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex, bool horizontal)
{
	float textureSizeX = (float)outputTex->getTextureWidth();
	float textureSizeY = (float)outputTex->getTextureHeight();
	XMFLOAT2 textureSize = XMFLOAT2(textureSizeX, textureSizeY);

	outputTex->setRenderTarget(renderer->getDeviceContext());
	outputTex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = outputTex->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	outputMesh->sendData(renderer->getDeviceContext());
	bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, inputTex->getShaderResourceView(), textureSize, horizontal);
	bloomShader->render(renderer->getDeviceContext(), outputMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}
void Application::upSample(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex)
{
	outputTex->setRenderTarget(renderer->getDeviceContext());
	outputTex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = outputTex->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	outputMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, inputTex->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), outputMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}
void Application::addTextures(RenderTexture* outputTex, OrthoMesh* outputMesh, RenderTexture* inputTex0, RenderTexture* inputTex1)
{
	outputTex->setRenderTarget(renderer->getDeviceContext());
	outputTex->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = outputTex->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	outputMesh->sendData(renderer->getDeviceContext());
	textureAddShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, inputTex0->getShaderResourceView(), inputTex1->getShaderResourceView());
	textureAddShader->render(renderer->getDeviceContext(), outputMesh->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

void Application::finalPass()	
{
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();

	if (showBrightpassTex)
	{
		renderer->setZBuffer(false);
		screenSizeOrthMesh->sendData(renderer->getDeviceContext());
		textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, brightPassTex->getShaderResourceView());
		textureShader->render(renderer->getDeviceContext(), screenSizeOrthMesh->getIndexCount());
		renderer->setZBuffer(true);
	}
	else
	{
		renderer->setZBuffer(false);
		screenSizeOrthMesh->sendData(renderer->getDeviceContext());
		textureAddShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneTexture->getShaderResourceView(), bloomTex_Final->getShaderResourceView());
		textureAddShader->render(renderer->getDeviceContext(), screenSizeOrthMesh->getIndexCount());
		renderer->setZBuffer(true);
	}

	gui();

	renderer->endScene();
}

void Application::gui()
{
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Display Brightpass Texture", &showBrightpassTex);


	if (ImGui::CollapsingHeader("Display Objects"))
	{
		ImGui::Checkbox("Render Terrain", &renderTerrain);
		ImGui::Checkbox("Render Water", &renderWater);
		ImGui::Checkbox("Render Sphere", &renderSphere);
	}
	if (ImGui::CollapsingHeader("Water Settings"))
	{
		ImGui::InputFloat4("Wave Sd|Ht|Fq|Ti: ", (float*)&timeBufferData);
	}
	if (ImGui::CollapsingHeader("Post Process Settings"))
	{
		ImGui::SliderFloat("Bloom Cutoff: ", (float*)&bloomFactor.x, 0.0f, 1.0f, "%.1f", 1.0f);
		ImGui::SliderFloat("Bloom Glare: ", (float*)&bloomFactor.y, 0.0f, 1.0f, "%.1f", 1.0f);
	}
	if (ImGui::CollapsingHeader("Light Settings"))
	{
		if (ImGui::TreeNode("Directional Light 0"))
		{
			ImGui::SliderFloat3("Position: ", (float*)&directional0LightPos, -50, 50, "%.1f", 1.0f);
			ImGui::SliderFloat3("Direction: ", (float*)&directional0LightDir, -1.0f, 1.0f, "%.1f", 1.0f);
			ImGui::InputFloat4("Colour: ", (float*)&directional0LightColour);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Directional Light 1"))
		{
			ImGui::SliderFloat3("Position: ", (float*)&directional1LightPos, -50, 50, "%.1f", 1.0f);
			ImGui::SliderFloat3("Direction: ", (float*)&directional1LightDir, -1.0f, 1.0f, "%.1f", 1.0f);
			ImGui::InputFloat4("Colour: ", (float*)&directional1LightColour);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Point Light"))
		{
			ImGui::SliderFloat3("Position: ", (float*)&pointLightPosition, -50, 50, "%.1f", 1.0f);
			ImGui::InputFloat4("Colour: ", (float*)&pointLightColour);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Spot Light"))
		{
			ImGui::SliderFloat3("Position: ", (float*)&spotLightPosition, -50, 50, "%.1f", 1.0f);
			ImGui::SliderFloat3("Direction: ", (float*)&spotLightDirection, -1.0, 1.0, "%.1f", 1.0f);
			ImGui::TreePop();
		}
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
