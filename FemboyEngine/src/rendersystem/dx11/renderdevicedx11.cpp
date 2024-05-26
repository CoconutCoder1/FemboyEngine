#include "renderdevicedx11.h"

#include <SDL3/SDL_assert.h>

#include <stdexcept>
#include <array>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace fe::render {

// ===============================================
// DirectX 11 conversion functions
// ===============================================
DXGI_FORMAT ConvertRenderFormat(RenderFormat::Enum format) {
	switch (format)
	{
	case RenderFormat::R8G8B8A8_UNorm:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case RenderFormat::R32_Float:
		return DXGI_FORMAT_R32_FLOAT;

	case RenderFormat::R32G32B32A32_Float:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	default:
		break;
	}

	return DXGI_FORMAT_UNKNOWN;
}

D3D11_USAGE ConvertBufferUsage(BufferUsage::Enum usage) {
	switch (usage)
	{
	case BufferUsage::Default:
		return D3D11_USAGE_DEFAULT;

	case BufferUsage::Immutable:
		return D3D11_USAGE_IMMUTABLE;

	case BufferUsage::Dynamic:
		return D3D11_USAGE_DYNAMIC;

	case BufferUsage::Staging:
		return D3D11_USAGE_STAGING;

	default:
		break;
	}

	SDL_assert(!"Unknown BufferUsage value");
	return D3D11_USAGE_DEFAULT;
}

UINT ConvertBindFlags(uint32_t bindFlags) {
	UINT result = 0;

	if (bindFlags & BindFlags::VertexBuffer)
		result |= D3D11_BIND_VERTEX_BUFFER;

	if (bindFlags & BindFlags::IndexBuffer)
		result |= D3D11_BIND_INDEX_BUFFER;

	if (bindFlags & BindFlags::ConstantBuffer)
		result |= D3D11_BIND_CONSTANT_BUFFER;

	if (bindFlags & BindFlags::DepthStencil)
		result |= D3D11_BIND_DEPTH_STENCIL;

	if (bindFlags & BindFlags::ShaderResource)
		result |= D3D11_BIND_SHADER_RESOURCE;

	if (bindFlags & BindFlags::RenderTarget)
		result |= D3D11_BIND_RENDER_TARGET;

	if (bindFlags & BindFlags::UnorderedAccess)
		result |= D3D11_BIND_UNORDERED_ACCESS;

	return result;
}

D3D_PRIMITIVE_TOPOLOGY ConvertPrimitiveTopology(PrimitiveToplogy::Enum topology) {
	switch (topology)
	{
	case PrimitiveToplogy::TriangleList:
		return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	default:
		break;
	}

	SDL_assert(!"Unknown topology for conversion");
	return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

// ===============================================
// DirectX 11 utility functions
// ===============================================
static void CreateBuffer(ID3D11Device* pDevice, ID3D11Buffer** ppBufferOut, uint32_t sizeInBytes, uint32_t strideInBytes, UINT bindFlags, BufferUsage::Enum usage, const void* pInitData) {
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = bindFlags;
	bd.ByteWidth = sizeInBytes;
	bd.StructureByteStride = strideInBytes;
	bd.CPUAccessFlags = 0;
	bd.Usage = ConvertBufferUsage(usage);

	if (usage == BufferUsage::Dynamic)
		bd.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = pInitData;

	ThrowIfFailed(pDevice->CreateBuffer(&bd, pInitData ? &srd : nullptr, ppBufferOut));
}


// ===============================================
// DirectX 11 render resources implementation
// ===============================================
class Texture2D_Dx11 : public Texture2D {
public:
	Texture2D_Dx11(wrl::ComPtr<ID3D11Texture2D> texturePtr)
		: m_pTexture(texturePtr) {}

	virtual ~Texture2D_Dx11() = default;

	virtual uint32_t Release() {
		return static_cast<uint32_t>(m_pTexture->Release());
	}

	wrl::ComPtr<ID3D11Texture2D> m_pTexture;
};

class RenderTarget_Dx11 : public RenderTarget {
public:
	RenderTarget_Dx11(wrl::ComPtr<ID3D11RenderTargetView> pRTV)
		: m_pRTV(pRTV) {}

	virtual ~RenderTarget_Dx11() = default;

	virtual uint32_t Release() {
		return static_cast<uint32_t>(m_pRTV->Release());
	}

	wrl::ComPtr<ID3D11RenderTargetView> m_pRTV;
};

class Buffer_Dx11 : public Buffer {
public:
	Buffer_Dx11(wrl::ComPtr<ID3D11Buffer> pBuffer)
		: m_pBuffer(pBuffer)
	{
		pBuffer->GetDesc(&m_Desc);
	}

	virtual ~Buffer_Dx11() = default;

	virtual uint32_t Release() {
		return static_cast<uint32_t>(m_pBuffer->Release());
	}

	wrl::ComPtr<ID3D11Buffer> m_pBuffer;
	D3D11_BUFFER_DESC m_Desc;
};

class VertexShader_Dx11 : public VertexShader {
public:
	VertexShader_Dx11(wrl::ComPtr<ID3D11VertexShader> pShader)
		: m_pShader(pShader) {}

	virtual ~VertexShader_Dx11() = default;

	virtual uint32_t Release() {
		return static_cast<uint32_t>(m_pShader->Release());
	}

	wrl::ComPtr<ID3D11VertexShader> m_pShader;
};

class PixelShader_Dx11 : public PixelShader {
public:
	PixelShader_Dx11(wrl::ComPtr<ID3D11PixelShader> pShader)
		: m_pShader(pShader) {}

	virtual ~PixelShader_Dx11() = default;

	virtual uint32_t Release() {
		return static_cast<uint32_t>(m_pShader->Release());
	}

	wrl::ComPtr<ID3D11PixelShader> m_pShader;
};

class InputLayout_Dx11 : public InputLayout {
public:
	InputLayout_Dx11(wrl::ComPtr<ID3D11InputLayout> pLayout)
		: m_pLayout(pLayout) {}

	virtual ~InputLayout_Dx11() = default;

	virtual uint32_t Release() {
		return static_cast<uint32_t>(m_pLayout->Release());
	}

	wrl::ComPtr<ID3D11InputLayout> m_pLayout;
};

// ===============================================
// DirectX 11 render device implementation
// ===============================================
RenderDeviceDx11::~RenderDeviceDx11() {

}

bool RenderDeviceDx11::Initialize(const RenderDeviceParams_t& params) {
	UINT createFlags = 0;

	if (params.enableDebugging)
		createFlags |= D3D11_CREATE_DEVICE_DEBUG;

	ThrowIfFailed(D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE,
		NULL, createFlags,
		nullptr, 0, D3D11_SDK_VERSION,
		&m_pDevice, nullptr, &m_pDeviceContext
	));

	m_pImmediateContext = ScopedPtr<RenderContextDx11>(new RenderContextDx11(m_pDeviceContext));

	return true;
}

void RenderDeviceDx11::ReleaseResource(RenderResource* pResource) {
	if (pResource->Release() == 0) {
		delete pResource;
	}
}

RenderContext* RenderDeviceDx11::GetImmediateContext() const {
	return m_pImmediateContext.get();
}

RenderContext* RenderDeviceDx11::CreateDeferredContext() {
	return nullptr;
}

SwapChain* RenderDeviceDx11::CreateSwapChain(const SwapChainParams_t& params) {
	SwapChainDx11* pResult = m_pSwapChainList.emplace_back(new SwapChainDx11()).get();

	if (!pResult->Initialize(this, params)) {
		m_pSwapChainList.pop_back();
		return nullptr;
	}

	return pResult;
}

VertexShader* RenderDeviceDx11::CreateVertexShader(const void* pBytecode, size_t bytecodeLength) {
	wrl::ComPtr<ID3D11VertexShader> pShader;

	ThrowIfFailed(m_pDevice->CreateVertexShader(pBytecode, bytecodeLength, nullptr, &pShader));

	return new VertexShader_Dx11(pShader);
}

PixelShader* RenderDeviceDx11::CreatePixelShader(const void* pBytecode, size_t bytecodeLength) {
	wrl::ComPtr<ID3D11PixelShader> pShader;

	ThrowIfFailed(m_pDevice->CreatePixelShader(pBytecode, bytecodeLength, nullptr, &pShader));

	return new PixelShader_Dx11(pShader);
}

InputLayout* RenderDeviceDx11::CreateInputLayout(InputElement_t const* const pElements, uint32_t numElements, const void* pShaderBytecode, size_t bytecodeLength) {
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements(numElements);

	for (uint32_t i = 0; i < numElements; i++) {
		const InputElement_t& elem = pElements[i];

		inputElements[i] = {
			elem.semanticName.c_str(),
			elem.semanticIndex,
			ConvertRenderFormat(elem.format),
			0,
			elem.byteOffset,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		};
	}

	wrl::ComPtr<ID3D11InputLayout> pLayout;

	ThrowIfFailed(m_pDevice->CreateInputLayout(inputElements.data(), numElements, pShaderBytecode, bytecodeLength, &pLayout));

	return new InputLayout_Dx11(pLayout);
}

Buffer* RenderDeviceDx11::CreateVertexBuffer(uint32_t numVertices, uint32_t strideInBytes, BufferUsage::Enum usage, const void* pInitData) {
	wrl::ComPtr<ID3D11Buffer> pBuffer;
	CreateBuffer(m_pDevice.Get(), &pBuffer, numVertices * strideInBytes, strideInBytes, D3D11_BIND_VERTEX_BUFFER, usage, pInitData);

	return new Buffer_Dx11(pBuffer);
}

Texture2D* RenderDeviceDx11::CreateTexture2D(uint32_t width, uint32_t height, RenderFormat::Enum format, const void* pInitData, uint32_t arraySize) {
	D3D11_TEXTURE2D_DESC td = {};
	td.Width = width;
	td.Height = height;
	td.ArraySize = arraySize;
	td.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	td.Format = ConvertRenderFormat(format);
	td.MipLevels = 1;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	
	wrl::ComPtr<ID3D11Texture2D> texturePtr;
	ThrowIfFailed(m_pDevice->CreateTexture2D(&td, nullptr, &texturePtr));

	return new Texture2D_Dx11(texturePtr);
}

RenderTarget* RenderDeviceDx11::CreateRenderTarget(Texture2D* pTexture) {
	Texture2D_Dx11* pTextureDx11 = reinterpret_cast<Texture2D_Dx11*>(pTexture);

	wrl::ComPtr<ID3D11RenderTargetView> pRTV;
	ThrowIfFailed(m_pDevice->CreateRenderTargetView(pTextureDx11->m_pTexture.Get(), nullptr, &pRTV));

	return new RenderTarget_Dx11(pRTV);
}

ID3D11Device* RenderDeviceDx11::D3D11Device() const {
	return m_pDevice.Get();
}

// ===============================================
// DirectX 11 swap chain implementation
// ===============================================
bool SwapChainDx11::Initialize(RenderDevice* pDevice, const SwapChainParams_t& params) {
	RenderDeviceDx11* pDeviceDx11 = static_cast<RenderDeviceDx11*>(pDevice);

	m_SwapChainParams = params;

	ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_pFactory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = params.bufferCount;
	scd.SampleDesc.Count = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.Windowed = TRUE;
	scd.OutputWindow = reinterpret_cast<HWND>(params.pOutputWindow);
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	if (scd.BufferCount > 1) {
		scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	}
	else {
		scd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	}

	wrl::ComPtr<IDXGISwapChain> pSwapChain;
	ThrowIfFailed(m_pFactory->CreateSwapChain(pDeviceDx11->D3D11Device(), &scd, &pSwapChain));

	ThrowIfFailed(pSwapChain.As(&m_pSwapChain));

	wrl::ComPtr<ID3D11Texture2D> texturePtr;
	ThrowIfFailed(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(texturePtr.GetAddressOf())));

	m_pBackBufferList = ScopedPtr<Texture2D>(new Texture2D_Dx11(texturePtr));

	return true;
}

void SwapChainDx11::Present() {
	m_pSwapChain->Present(m_SyncInterval, NULL);
}

uint32_t SwapChainDx11::GetSyncInterval() const {
	return m_SyncInterval;
}

void SwapChainDx11::SetSyncInterval(uint32_t syncInterval) {
	m_SyncInterval = syncInterval;
}

Texture2D* SwapChainDx11::GetBackBuffer() {
	return m_pBackBufferList.get();
}

uint32_t SwapChainDx11::GetNumBackBuffers() const {
	return m_SwapChainParams.bufferCount;
}

// ===============================================
// DirectX 11 render context implementation
// ===============================================
RenderContextDx11::RenderContextDx11(wrl::ComPtr<ID3D11DeviceContext> pContext)
	: m_pContext(pContext) {}

void RenderContextDx11::SetViewports(const Viewport_t* const ppViewports, uint32_t numViewports) {
	std::array<D3D11_VIEWPORT, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE> viewports;

	for (uint32_t i = 0; i < numViewports; i++) {
		const Viewport_t& vp = ppViewports[i];

		viewports[i] = {
			vp.x, vp.y,
			vp.width, vp.height,
			vp.minDepth, vp.maxDepth
		};
	}

	m_pContext->RSSetViewports(numViewports, viewports.data());
}

void RenderContextDx11::ClearRenderTarget(const RenderTarget* pRenderTarget, const std::array<float, 4>& clearColor) {
	const RenderTarget_Dx11* pRenderTargetDx11 = reinterpret_cast<const RenderTarget_Dx11*>(pRenderTarget);

	m_pContext->ClearRenderTargetView(pRenderTargetDx11->m_pRTV.Get(), clearColor.data());
}

void RenderContextDx11::SetRenderTargets(const RenderTarget** ppRenderTargets, uint32_t numRenderTargets) {
	std::array<ID3D11RenderTargetView*, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT> pRenderTargetList;

	for (uint32_t i = 0; i < numRenderTargets; i++) {
		const RenderTarget_Dx11* pRenderTargetDx11 = reinterpret_cast<const RenderTarget_Dx11*>(ppRenderTargets[i]);

		pRenderTargetList[i] = pRenderTargetDx11->m_pRTV.Get();
	}

	m_pContext->OMSetRenderTargets(numRenderTargets, pRenderTargetList.data(), nullptr);
}

void RenderContextDx11::SetPrimtiveTopology(PrimitiveToplogy::Enum topology) {
	m_pContext->IASetPrimitiveTopology(ConvertPrimitiveTopology(topology));
}

void RenderContextDx11::SetVertexBuffer(Buffer* pVertexBuffer) {
	Buffer_Dx11* pBufferDx11 = reinterpret_cast<Buffer_Dx11*>(pVertexBuffer);

	const UINT offset = 0;

	m_pContext->IASetVertexBuffers(0, 1, pBufferDx11->m_pBuffer.GetAddressOf(), &pBufferDx11->m_Desc.StructureByteStride, &offset);
}

void RenderContextDx11::Draw(uint32_t vertexCount, uint32_t startVertexLocation) {
	m_pContext->Draw(vertexCount, startVertexLocation);
}

}