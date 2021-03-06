#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "EngineProperties.h"
#include "System.h"
#include "Light.h"
#include "Light2D.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "Renderer.h"
#include "PostProcessStack.h"
#include "Shader.h"

#include <Events/Event.h>

#include <vector>
#include <map>

class RenderSystem : public System {

	std::vector<Light*> lights;
	std::vector<Light*> casters;

	std::vector<Light2D*> lights2D;
	std::vector<Light2D*> casters2D;

	TextureData depthTextureData;

	Framebuffer* depthFBO[MAX_LIGHTS];
	mat4f lightSpaceMatrices[MAX_LIGHTS];

	std::vector<Camera*> cameras;
	std::vector<Camera*> rawCameras;
	std::vector<Camera*> fbCameras;

	std::vector<Renderer*> renderers;
	PostProcessStack* postProccessing;

	float scaleFactor;
	vec2i windowSize;
	Framebuffer* mainFBO;
	Shader* fbShader;

	static unsigned VAO;

public:

	~RenderSystem() override;

	void Initialize() override;
	void Update(float const& dt) override;

	void Start() override;
	void Stop() override;

private:

	void LightActiveHandler(Events::Event* event);
	void LightShadowHanlder(Events::Event* event);

	void Light2DActiveHandler(Events::Event* event);
	void Light2DShadowHanlder(Events::Event* event);

	void CameraActiveHandler(Events::Event* event);
	void CameraDepthHandler(Events::Event* event);
	void CameraFramebufferHandler(Events::Event* event);
	void CameraUseProcessHandler(Events::Event* event);

	void ResizeHandler(Events::Event* event);

	void DepthRender();
	void FBRender();
	void Render();
	void RenderLowRes();
	void Render(RendererData const& data);
	void RawRender();
	
};

#endif
