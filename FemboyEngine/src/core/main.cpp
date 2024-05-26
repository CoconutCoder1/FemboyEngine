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

struct Vertex {
	float x, y, z;

	Vertex() {
		x = y = z = 0.f;
	}

	Vertex(float x, float y, float z)
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

	Vertex vertices[] = {
		Vertex(-0.5f, -0.5f, 0.f),
		Vertex(0.f, 0.5f, 0.f),
		Vertex(0.5f, -0.5f, 0.f),
	};

	render::RenderContext* pImmediateContext = g_pDevice->GetImmediateContext();
	render::RenderTarget* pRenderTarget = g_pDevice->CreateRenderTarget(g_pSwapChain->GetBackBuffer());

	render::Buffer* pVB = g_pDevice->CreateVertexBuffer(_countof(vertices), sizeof(Vertex), render::BufferUsage::Default, vertices);

	bool exitGame = false;

	while (!exitGame) {
		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT)
				exitGame = true;
		}

		pImmediateContext->ClearRenderTarget(pRenderTarget, { 1.f, 0.f, 0.f, 1.f });

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