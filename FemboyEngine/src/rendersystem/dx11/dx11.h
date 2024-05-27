#pragma once

#include <d3d11.h>
#include <dxgi1_4.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include <cstdio>
#include <stdexcept>

namespace wrl = Microsoft::WRL;

#define ThrowIfFailed(_expr) \
{ \
	HRESULT hr = _expr; \
	if (FAILED(hr)) { \
		printf("DX11 Error (%s): %s (HRESULT: %x)\n", __FUNCTION__, #_expr, hr); \
		throw std::exception(); \
	} \
}