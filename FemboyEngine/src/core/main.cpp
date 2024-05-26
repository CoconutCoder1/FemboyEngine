#include "rendersystem/rhi.h"

#include "fstdlib/pointers.h"

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <SDL3/SDL_system.h>
#include <SDL3/SDL_assert.h>

#pragma comment(lib, "SDL3.lib")

namespace fe {

struct GameConfig_t {
	int32_t width;
	int32_t height;
	bool isFullscreen;
	bool isBorderless;
};

struct Vertex_t {
	float x, y, z;

	Vertex_t() {
		x = y = z = 0.f;
	}

	Vertex_t(float x, float y, float z)
		: x(x), y(y), z(z) {}
};

static SDL_Window* g_pGameWindow = NULL;

static GameConfig_t g_GameConfig;

static render::RenderDevice* g_pDevice = nullptr;
static render::SwapChain* g_pSwapChain = nullptr;

static void* GetSDLWindowHandle_Win32() {
	SDL_PropertiesID windowPropertyId = SDL_GetWindowProperties(g_pGameWindow);
	void* pWindowHandle = SDL_GetProperty(windowPropertyId, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

	return pWindowHandle;
}

static bool CreateGameWindow() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return false;
	}

	SDL_WindowFlags windowFlags = 0;

	if (g_GameConfig.isFullscreen) {
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}
	if (g_GameConfig.isBorderless) {
		windowFlags |= SDL_WINDOW_BORDERLESS;
	}

	g_pGameWindow = SDL_CreateWindow("FemboyEngine", g_GameConfig.width, g_GameConfig.height, windowFlags);

	return true;
}

static void LoadGameConfig() {
	g_GameConfig.isFullscreen = false;
	g_GameConfig.isBorderless = false;

	g_GameConfig.width = 800;
	g_GameConfig.height = 600;
}

static bool CompileShader(const std::string& shaderFile, render::VertexShader** ppVertexShader, render::PixelShader** ppPixelShader, render::InputLayout** ppInputLayout, const render::InputElement_t* const pInputElements, uint32_t numElements) {
	ScopedPtr<render::ShaderBytecode> pVSCode, pPSCode;

	render::ShaderCompiler* pShaderCompiler = g_pDevice->GetShaderCompiler();

	if (!pShaderCompiler->CompileShaderFile(shaderFile, render::ShaderStage::Vertex, pVSCode)) {
		return false;
	}

	if (!pShaderCompiler->CompileShaderFile(shaderFile, render::ShaderStage::Pixel, pPSCode)) {
		return false;
	}

	*ppVertexShader = g_pDevice->CreateVertexShader(pVSCode->GetData(), pVSCode->GetSize());
	*ppPixelShader = g_pDevice->CreatePixelShader(pPSCode->GetData(), pPSCode->GetSize());
	*ppInputLayout = g_pDevice->CreateInputLayout(pInputElements, numElements, pVSCode->GetData(), pVSCode->GetSize());

	return true;
}

int EntryPoint() {
	LoadGameConfig();

	if (!CreateGameWindow()) {
		printf("Creating game window failed.\n");
		return -1;
	}

	ScopedPtr<render::RHI> pRHI = ScopedPtr<render::RHI>(new render::RHI(render::GraphicsAPI::DirectX11));

	render::RenderDeviceParams_t deviceParams;
	deviceParams.enableDebugging = true;

	g_pDevice = pRHI->CreateRenderDevice(deviceParams);
	if (!g_pDevice) {
		printf("Creating render device failed.\n");
		return -1;
	}

	render::SwapChainParams_t swapChainParams;
	swapChainParams.bufferCount = 2;
	swapChainParams.pOutputWindow = GetSDLWindowHandle_Win32();
	swapChainParams.width = 0;
	swapChainParams.height = 0;

	g_pSwapChain = g_pDevice->CreateSwapChain(swapChainParams);
	
	if (!g_pSwapChain) {
		printf("Creating swap chain for the game window.\n");
		return -1;
	}

	g_pSwapChain->SetSyncInterval(1U);

	Vertex_t vertices[] = {
		Vertex_t(-0.5f, -0.5f, 0.f),
		Vertex_t(0.f, 0.5f, 0.f),
		Vertex_t(0.5f, -0.5f, 0.f),
	};

	render::Viewport_t viewport(static_cast<float>(g_GameConfig.width), static_cast<float>(g_GameConfig.height));

	render::RenderContext* pImmediateContext = g_pDevice->GetImmediateContext();
	render::RenderTarget* pRenderTarget = g_pDevice->CreateRenderTarget(g_pSwapChain->GetBackBuffer());

	render::Buffer* pVB = g_pDevice->CreateVertexBuffer(_countof(vertices), sizeof(Vertex_t), render::BufferUsage::Default, vertices);

	render::VertexShader* pVS;
	render::PixelShader* pPS;
	render::InputLayout* pInputLayout;

	render::InputElement_t inputElems[] = {
		render::InputElement_t("POSITION", 0, render::RenderFormat::R32G32B32_Float, 0)
	};

	if (!CompileShader("src/hlsl/lit.hlsl", &pVS, &pPS, &pInputLayout, inputElems, _countof(inputElems))) {
		printf("Failed to compile shader\n");
		return -1;
	}

	bool exitGame = false;

	while (!exitGame) {
		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT)
				exitGame = true;
		}

		pImmediateContext->ClearRenderTarget(pRenderTarget, { 1.f, 0.f, 0.f, 1.f });
		pImmediateContext->SetRenderTargets(&pRenderTarget, 1);

		pImmediateContext->SetInputLayout(pInputLayout);
		pImmediateContext->SetVertexShader(pVS);
		pImmediateContext->SetPixelShader(pPS);

		pImmediateContext->SetViewports(&viewport, 1);
		pImmediateContext->SetPrimtiveTopology(render::PrimitiveToplogy::TriangleList);

		pImmediateContext->SetVertexBuffer(pVB);
		pImmediateContext->Draw(_countof(vertices), 0);

		g_pSwapChain->Present();
	}

	return 0;
}

}

int main() {
	return fe::EntryPoint();
}