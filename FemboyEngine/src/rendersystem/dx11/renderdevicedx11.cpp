#include "renderdevicedx11.h"

#include <SDL3/SDL_assert.h>

#include <stdexcept>
#include <array>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

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

	case RenderFormat::R32G32B32_Float:
		return DXGI_FORMAT_R32G32B32_FLOAT;

	case RenderFormat::R32G32B32A32_Float:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	default:
		break;
	}

	SDL_assert(!"Unknown RenderFormat value");
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
static void CreateBuffer(ID3D11Device* pDevice, D3D11_BUFFER_DESC* pDescOut, ID3D11Buffer** ppBufferOut, uint32_t sizeInBytes, uint32_t strideInBytes, UINT bindFlags, BufferUsage::Enum usage, const void* pInitData) {
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

	if (pDescOut) {
		*pDescOut = bd;
	}
}

static uint32_t GetRefCount(IUnknown* pUnknown) {
	pUnknown->AddRef();
	return pUnknown->Release();
}

// ===============================================
// DirectX 11 render resources implementation
// ===============================================
#define ReleaseDx11_ResourceImpl(_resourcePtr) \
virtual uint32_t Release() { \
uint32_t refCount = static_cast<uint32_t>(_resourcePtr->Release()); \
if (refCount == 0) { \
	_resourcePtr.Detach(); \
} \
return refCount; \
}

class Texture2D_Dx11 : public Texture2D {
public:
	Texture2D_Dx11(wrl::ComPtr<ID3D11Texture2D> texturePtr)
		: m_pTexture(texturePtr) {}

	virtual ~Texture2D_Dx11() = default;

	ReleaseDx11_ResourceImpl(m_pTexture);

	wrl::ComPtr<ID3D11Texture2D> m_pTexture;
};

class RenderTarget_Dx11 : public RenderTarget {
public:
	RenderTarget_Dx11(wrl::ComPtr<ID3D11RenderTargetView> pRTV)
		: m_pRTV(pRTV) {}

	virtual ~RenderTarget_Dx11() = default;

	ReleaseDx11_ResourceImpl(m_pRTV);

	wrl::ComPtr<ID3D11RenderTargetView> m_pRTV;
};

class Buffer_Dx11 : public Buffer {
public:
	Buffer_Dx11(wrl::ComPtr<ID3D11Buffer> pBuffer, const D3D11_BUFFER_DESC& desc)
		: m_pBuffer(pBuffer), m_Desc(desc)
	{
	}

	virtual ~Buffer_Dx11() = default;

	ReleaseDx11_ResourceImpl(m_pBuffer);

	wrl::ComPtr<ID3D11Buffer> m_pBuffer;
	D3D11_BUFFER_DESC m_Desc;
};

class VertexShader_Dx11 : public VertexShader {
public:
	VertexShader_Dx11(wrl::ComPtr<ID3D11VertexShader> pShader)
		: m_pShader(pShader) {}

	virtual ~VertexShader_Dx11() = default;

	ReleaseDx11_ResourceImpl(m_pShader);

	wrl::ComPtr<ID3D11VertexShader> m_pShader;
};

class PixelShader_Dx11 : public PixelShader {
public:
	PixelShader_Dx11(wrl::ComPtr<ID3D11PixelShader> pShader)
		: m_pShader(pShader) {}

	virtual ~PixelShader_Dx11() = default;

	ReleaseDx11_ResourceImpl(m_pShader);

	wrl::ComPtr<ID3D11PixelShader> m_pShader;
};

class InputLayout_Dx11 : public InputLayout {
public:
	InputLayout_Dx11(wrl::ComPtr<ID3D11InputLayout> pLayout)
		: m_pLayout(pLayout) {}

	virtual ~InputLayout_Dx11() = default;

	ReleaseDx11_ResourceImpl(m_pLayout);

	wrl::ComPtr<ID3D11InputLayout> m_pLayout;
};

// ===============================================
// DirectX 11 render device implementation
// ===============================================
static wrl::ComPtr<IDXGIDebug1> pDebugInterface;

RenderDeviceDx11::RenderDeviceDx11() {

}

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

	m_pShaderCompiler = ScopedPtr<ShaderCompiler>(new ShaderCompilerDx11());
	m_pImmediateContext = ScopedPtr<RenderContextDx11>(new RenderContextDx11(m_pDeviceContext));

	if (params.enableDebugging && !pDebugInterface) {
		ThrowIfFailed(DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), reinterpret_cast<void**>(pDebugInterface.GetAddressOf())));
	}

	return true;
}

void RenderDeviceDx11::Shutdown() {
	m_pSwapChainList.clear();
	m_pImmediateContext.reset();

	m_pDeviceContext.Reset();

	RenderResourceIterator_t* pResourceIter = m_RenderResourceList.pHead;
	while (pResourceIter) {
		while (ReleaseResourceEx(pResourceIter->pResource, false) > 0);
		pResourceIter = pResourceIter->pNext;
	}
	m_RenderResourceList.Clear();

	m_pDevice.Reset();
}

uint32_t RenderDeviceDx11::ReleaseResource(RenderResource* pResource) {
	return ReleaseResourceEx(pResource, true);
}

void RenderDeviceDx11::ReportLiveObjects() {
	ReportLiveObjectsD3D11();
}

void RenderDeviceDx11::ReportLiveObjectsD3D11() {
	if (pDebugInterface) {
		pDebugInterface->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_ALL);
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

	return RegisterResource(new VertexShader_Dx11(pShader));
}

PixelShader* RenderDeviceDx11::CreatePixelShader(const void* pBytecode, size_t bytecodeLength) {
	wrl::ComPtr<ID3D11PixelShader> pShader;

	ThrowIfFailed(m_pDevice->CreatePixelShader(pBytecode, bytecodeLength, nullptr, &pShader));

	return RegisterResource(new PixelShader_Dx11(pShader));
}

InputLayout* RenderDeviceDx11::CreateInputLayout(InputElement_t const* const pElements, uint32_t numElements, const void* pShaderBytecode, size_t bytecodeLength) {
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements(numElements);

	for (uint32_t i = 0; i < numElements; i++) {
		const InputElement_t& elem = pElements[i];

		inputElements[i] = {
			elem.semanticName.data(),
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

	return RegisterResource(new InputLayout_Dx11(pLayout));
}

Buffer* RenderDeviceDx11::CreateConstantBuffer(uint32_t bufferSize, BufferUsage::Enum usage, const void* pInitData) {
	wrl::ComPtr<ID3D11Buffer> pBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	CreateBuffer(m_pDevice.Get(), &bufferDesc, &pBuffer, bufferSize, 4, D3D11_BIND_CONSTANT_BUFFER, usage, pInitData);

	return RegisterResource(new Buffer_Dx11(pBuffer, bufferDesc));
}

Buffer* RenderDeviceDx11::CreateVertexBuffer(uint32_t numVertices, uint32_t strideInBytes, BufferUsage::Enum usage, const void* pInitData) {
	wrl::ComPtr<ID3D11Buffer> pBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	CreateBuffer(m_pDevice.Get(), &bufferDesc, &pBuffer, numVertices * strideInBytes, strideInBytes, D3D11_BIND_VERTEX_BUFFER, usage, pInitData);

	return RegisterResource(new Buffer_Dx11(pBuffer, bufferDesc));
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

	return RegisterResource(new Texture2D_Dx11(texturePtr));
}

RenderTarget* RenderDeviceDx11::CreateRenderTarget(Texture2D* pTexture) {
	Texture2D_Dx11* pTextureDx11 = reinterpret_cast<Texture2D_Dx11*>(pTexture);

	wrl::ComPtr<ID3D11RenderTargetView> pRTV;
	ThrowIfFailed(m_pDevice->CreateRenderTargetView(pTextureDx11->m_pTexture.Get(), nullptr, &pRTV));

	return RegisterResource(new RenderTarget_Dx11(pRTV));
}

ShaderCompiler* RenderDeviceDx11::GetShaderCompiler() const {
	return m_pShaderCompiler.get();
}

ID3D11Device* RenderDeviceDx11::D3D11Device() const {
	return m_pDevice.Get();
}

// ===============================================
// DirectX 11 swap chain implementation
// ===============================================
SwapChainDx11::~SwapChainDx11() {
	ReleaseBackBufferResource();
}

bool SwapChainDx11::Initialize(RenderDevice* pDevice, const SwapChainParams_t& params) {
	RenderDeviceDx11* pDeviceDx11 = static_cast<RenderDeviceDx11*>(pDevice);

	m_SwapChainParams = params;
	m_pDevice = pDeviceDx11;

	ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_pFactory.GetAddressOf())));

	DXGI_SWAP_CHAIN_DESC scd = {};
	scd.BufferCount = params.bufferCount;
	scd.SampleDesc.Count = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.Windowed = (!params.isFullscreen);
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

	CreateBackBufferResource();

	return true;
}

void SwapChainDx11::ResizeBuffers() {
	ReleaseBackBufferResource();

	m_pSwapChain->ResizeBuffers(m_SwapChainParams.bufferCount, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	CreateBackBufferResource();
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
	return m_pBackBuffer.get();
}

RenderTarget* SwapChainDx11::GetBackBufferTarget() {
	return m_pRenderTarget;
}

uint32_t SwapChainDx11::GetNumBackBuffers() const {
	return m_SwapChainParams.bufferCount;
}

void SwapChainDx11::CreateBackBufferResource() {
	wrl::ComPtr<ID3D11Texture2D> texturePtr;
	ThrowIfFailed(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(texturePtr.GetAddressOf())));

	m_pBackBuffer = ScopedPtr<Texture2D>(new Texture2D_Dx11(texturePtr));
	m_pRenderTarget = m_pDevice->CreateRenderTarget(m_pBackBuffer.get());
}

void SwapChainDx11::ReleaseBackBufferResource() {
	if (m_pRenderTarget) {
		SDL_assert(m_pDevice->ReleaseResource(m_pRenderTarget) == 0);
		m_pBackBuffer = nullptr;
		m_pRenderTarget = nullptr;
	}
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

void RenderContextDx11::Map(Buffer* pBuffer, void** ppData) {
	Buffer_Dx11* pBufferDx11 = reinterpret_cast<Buffer_Dx11*>(pBuffer);

	D3D11_MAPPED_SUBRESOURCE mapped;
	ThrowIfFailed(m_pContext->Map(pBufferDx11->m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped));

	*ppData = mapped.pData;
}

void RenderContextDx11::Unmap(Buffer* pBuffer) {
	Buffer_Dx11* pBufferDx11 = reinterpret_cast<Buffer_Dx11*>(pBuffer);

	m_pContext->Unmap(pBufferDx11->m_pBuffer.Get(), 0);
}

void RenderContextDx11::SetVertexShader(VertexShader* pVertexShader) {
	VertexShader_Dx11* pVertexShaderDx11 = reinterpret_cast<VertexShader_Dx11*>(pVertexShader);

	m_pContext->VSSetShader(pVertexShaderDx11->m_pShader.Get(), nullptr, 0);
}

void RenderContextDx11::SetPixelShader(PixelShader* pVertexShader) {
	PixelShader_Dx11* pPixelShaderDx11 = reinterpret_cast<PixelShader_Dx11*>(pVertexShader);

	m_pContext->PSSetShader(pPixelShaderDx11->m_pShader.Get(), nullptr, 0);
}

void RenderContextDx11::SetConstantBuffers(ShaderStage::Enum shaderStage, Buffer const* const* ppBuffers, uint32_t numBuffers) {
	std::array<ID3D11Buffer*, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT> pConstantBuffers;

	for (uint32_t i = 0; i < numBuffers; i++) {
		const Buffer_Dx11* pBufferDx11 = reinterpret_cast<const Buffer_Dx11*>(ppBuffers[i]);

		pConstantBuffers[i] = pBufferDx11->m_pBuffer.Get();
	}

	switch (shaderStage)
	{
	case ShaderStage::Vertex:
		m_pContext->VSSetConstantBuffers(0, numBuffers, pConstantBuffers.data());
		break;

	case ShaderStage::Pixel:
		m_pContext->PSSetConstantBuffers(0, numBuffers, pConstantBuffers.data());
		break;

	default:
		SDL_assert(!"Tried to bind constant buffer to unknown shader stage");
		break;
	}
}

void RenderContextDx11::ClearRenderTarget(const RenderTarget* pRenderTarget, const std::array<float, 4>& clearColor) {
	const RenderTarget_Dx11* pRenderTargetDx11 = reinterpret_cast<const RenderTarget_Dx11*>(pRenderTarget);

	m_pContext->ClearRenderTargetView(pRenderTargetDx11->m_pRTV.Get(), clearColor.data());
}

void RenderContextDx11::SetRenderTargets(RenderTarget const* const* ppRenderTargets, uint32_t numRenderTargets) {
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

void RenderContextDx11::SetInputLayout(InputLayout* pInputLayout) {
	InputLayout_Dx11* pInputLayoutDx11 = reinterpret_cast<InputLayout_Dx11*>(pInputLayout);

	m_pContext->IASetInputLayout(pInputLayoutDx11->m_pLayout.Get());
}

void RenderContextDx11::SetVertexBuffer(Buffer* pVertexBuffer) {
	Buffer_Dx11* pBufferDx11 = reinterpret_cast<Buffer_Dx11*>(pVertexBuffer);

	const UINT offset = 0;

	m_pContext->IASetVertexBuffers(0, 1, pBufferDx11->m_pBuffer.GetAddressOf(), &pBufferDx11->m_Desc.StructureByteStride, &offset);
}

void RenderContextDx11::Draw(uint32_t vertexCount, uint32_t startVertexLocation) {
	m_pContext->Draw(vertexCount, startVertexLocation);
}

// ===============================================
// DirectX 11 shader compiler implementation
// ===============================================
class ShaderBytecodeDx11 : public ShaderBytecode {
public:
	ShaderBytecodeDx11(wrl::ComPtr<ID3DBlob> pBytecode)
		: m_pBytecode(pBytecode) {}

	virtual ~ShaderBytecodeDx11() = default;

	virtual void* GetData() const {
		return m_pBytecode->GetBufferPointer();
	}

	virtual size_t GetSize() const {
		return m_pBytecode->GetBufferSize();
	}

	wrl::ComPtr<ID3DBlob> m_pBytecode;
};

#define MAKE_HLSL_VERSION(major, minor) major ## minor

std::string GetShaderTarget(uint8_t targetVersion) {
	return std::to_string(targetVersion / 10) + "_" + std::to_string(targetVersion % 10);
}

static const uint8_t hlslVersion = MAKE_HLSL_VERSION(5, 0);
static const std::string hlslVersionTarget = GetShaderTarget(hlslVersion);

bool ShaderCompilerDx11::CompileShaderSource(const std::string& source, ShaderStage::Enum stage, ScopedPtr<ShaderBytecode>& pBytecode) {
	std::string entry = "main";
	std::string target = "";

	pBytecode = nullptr;
	
	if (stage == ShaderStage::Vertex) {
		entry = "VSMain";
		target = "vs_" + hlslVersionTarget;
	}
	else if (stage == ShaderStage::Pixel) {
		entry = "PSMain";
		target = "ps_" + hlslVersionTarget;
	}

	wrl::ComPtr<ID3DBlob> pCode;
	wrl::ComPtr<ID3DBlob> pError;

	HRESULT hr = D3DCompile(source.data(), source.size(), nullptr, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry.c_str(), target.c_str(), D3DCOMPILE_DEBUG, 0, &pCode, &pError);
	if (FAILED(hr)) {
		if (pError) {
			printf("HLSL Compile Error (HRESULT: %x): %s\n", hr, reinterpret_cast<char*>(pError->GetBufferPointer()));
		}
		return false;
	}

	pBytecode = ScopedPtr<ShaderBytecode>(new ShaderBytecodeDx11(pCode));

	return true;
}

bool ShaderCompilerDx11::CompileShaderFile(const std::string& sourceFile, ShaderStage::Enum stage, ScopedPtr<ShaderBytecode>& pBytecode) {
	wrl::ComPtr<ID3DBlob> pShaderSource;

	pBytecode = nullptr;

	std::wstring wideFilePath(sourceFile.begin(), sourceFile.end());

	HRESULT hr = D3DReadFileToBlob(wideFilePath.c_str(), &pShaderSource);
	if (FAILED(hr)) {
		if (hr == D3D11_ERROR_FILE_NOT_FOUND) {
			printf("Could read HLSL source file for compiling: %s\n", sourceFile.c_str());
		}
		return false;
	}

	std::string shaderSourceStr(reinterpret_cast<char*>(pShaderSource->GetBufferPointer()), pShaderSource->GetBufferSize());

	return CompileShaderSource(shaderSourceStr, stage, pBytecode);
}

}