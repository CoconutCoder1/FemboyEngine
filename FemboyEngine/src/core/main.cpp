#include "rendersystem/rhi.h"
#include "rendersystem/types/floattypes.h"

#include "mathlib/mathlib.h"
#include "mathlib/matrix.h"

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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
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
	
	windowFlags |= SDL_WINDOW_RESIZABLE;

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

struct TestBuffer {
	render::Float4x4 projMat;
	render::Float4x4 viewMat;
};

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
	swapChainParams.isFullscreen = g_GameConfig.isFullscreen;

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

	render::RenderContext* pImmediateContext = g_pDevice->GetImmediateContext();

	render::Buffer* pVB = g_pDevice->CreateVertexBuffer(_countof(vertices), sizeof(Vertex_t), render::BufferUsage::Default, vertices);
	render::Buffer* pCB = g_pDevice->CreateConstantBuffer(sizeof(TestBuffer), render::BufferUsage::Dynamic, nullptr);

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
		float time = static_cast<float>(static_cast<double>(SDL_GetTicks()) * 0.001);

		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT)
				exitGame = true;

			if (ev.type == SDL_EVENT_KEY_DOWN) {
				if (ev.key.keysym.scancode == SDL_SCANCODE_F8) {
					g_pDevice->ReportLiveObjects();
				}
			}

			if (ev.type == SDL_EVENT_WINDOW_EXPOSED || ev.type == SDL_EVENT_WINDOW_RESIZED) {
				g_pSwapChain->ResizeBuffers();

				SDL_GetWindowSize(g_pGameWindow, &g_GameConfig.width, &g_GameConfig.height);
			}
		}

		render::Viewport_t viewport(static_cast<float>(g_GameConfig.width), static_cast<float>(g_GameConfig.height));

		render::RenderTarget* pRenderTarget = g_pSwapChain->GetBackBufferTarget();

		pImmediateContext->ClearRenderTarget(pRenderTarget, { 1.f, 0.f, 0.f, 1.f });
		pImmediateContext->SetRenderTargets(&pRenderTarget, 1);

		pImmediateContext->SetConstantBuffers(render::ShaderStage::Vertex, &pCB, 1);
		pImmediateContext->SetVertexShader(pVS);
		pImmediateContext->SetPixelShader(pPS);

		math::Matrix4x4 projMat = math::MakePerspectiveFovRH(70.f * math::Deg2Rad, 1.f, 0.01f, 100.f);
		math::Matrix4x4 viewMat = math::MakeLookAtRH(math::Vector3(cosf(time * 2.f) * 4.f, 0.f, -3.f), math::Vector3(0.f, 0.f, 0.f), math::Vector3(0.f, 1.f, 0.f));

		TestBuffer* pTestBufferData;
		pImmediateContext->Map(pCB, reinterpret_cast<void**>(&pTestBufferData));

		projMat.Copy(pTestBufferData->projMat.AsArray());
		viewMat.Copy(pTestBufferData->viewMat.AsArray());

		/*pTestBufferData->transform = render::Float4x4(
			render::Float4(1.f, 0.f, 0.f, cosf(time)),
			render::Float4(0.f, 1.f, 0.f, 0.f),
			render::Float4(0.f, 0.f, 1.f, 0.f),
			render::Float4(0.f, 0.f, 0.f, 1.f)
		);*/
		pImmediateContext->Unmap(pCB);

		pImmediateContext->SetViewports(&viewport, 1);
		pImmediateContext->SetPrimtiveTopology(render::PrimitiveToplogy::TriangleList);
		pImmediateContext->SetInputLayout(pInputLayout);

		pImmediateContext->SetVertexBuffer(pVB);
		pImmediateContext->Draw(_countof(vertices), 0);

		g_pSwapChain->Present();
	}
	
	pRHI->RemoveRenderDevice(g_pDevice);

	return 0;
}

}

int main() {
	return fe::EntryPoint();
}