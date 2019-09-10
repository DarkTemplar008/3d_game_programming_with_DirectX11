#include "pch.h"
#include "D3D11Initializer.h"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")

D3D11Initializer::D3D11Initializer()
{

}

D3D11Initializer::~D3D11Initializer()
{

}

bool D3D11Initializer::InitializeWithDefault(
	int width, 
	int height, 
	HWND hwnd, 
	bool fullscreen/* = false*/, 
	bool use_msaa_4x /*= false*/)
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL feature_level;
	hr = D3D11CreateDevice(NULL, 
		D3D_DRIVER_TYPE_HARDWARE, 
		NULL, 
		0, 
		NULL, 
		0, 
		D3D11_SDK_VERSION, 
		&d3d11_device_, 
		&feature_level, 
		&d3d11_device_ctx_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = d3d11_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &d3d11_msaa_4x_quality_);
	if (FAILED(hr))
	{
		return false;
	}

	CComPtr<IDXGIDevice> dxgi_device;
	hr = d3d11_device_->QueryInterface(&dxgi_device);
	if (FAILED(hr))
	{
		return false;
	}

	CComPtr<IDXGIAdapter> dxgi_adapter;
	hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter);
	if (FAILED(hr))
	{
		return false;
	}

	hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgi_factory_);
	if (FAILED(hr))
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
	swap_chain_desc.BufferDesc.Width = width;
	swap_chain_desc.BufferDesc.Height = height;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (use_msaa_4x)
	{
		swap_chain_desc.SampleDesc.Count = 4;
		swap_chain_desc.SampleDesc.Quality = d3d11_msaa_4x_quality_ - 1;
	}
	else
	{
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
	}
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.Windowed = !fullscreen;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;
	hr = dxgi_factory_->CreateSwapChain(d3d11_device_, &swap_chain_desc, &dxgi_swap_chain_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool D3D11Initializer::Initialize(int width, int height, HWND hwnd, int monitor, bool fullscreen, bool use_msaa_4x)
{
	HRESULT hr = S_OK;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgi_factory_);
	if (FAILED(hr))
	{
		return false;
	}

	CComPtr<IDXGIAdapter> dxgi_adapter;
	hr = dxgi_factory_->EnumAdapters(monitor, &dxgi_adapter);
	if (FAILED(hr))
	{
		return false;
	}

	D3D_FEATURE_LEVEL feature_level;
	hr = D3D11CreateDevice(dxgi_adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&d3d11_device_,
		&feature_level,
		&d3d11_device_ctx_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = d3d11_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &d3d11_msaa_4x_quality_);
	if (FAILED(hr))
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
	swap_chain_desc.BufferDesc.Width = width;
	swap_chain_desc.BufferDesc.Height = height;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	if (use_msaa_4x)
	{
		swap_chain_desc.SampleDesc.Count = 4;
		swap_chain_desc.SampleDesc.Quality = d3d11_msaa_4x_quality_ - 1;
	}
	else
	{
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
	}
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.Windowed = !fullscreen;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;
	hr = dxgi_factory_->CreateSwapChain(d3d11_device_, &swap_chain_desc, &dxgi_swap_chain_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void D3D11Initializer::Uninitialize()
{
	d3d11_device_ctx_.Release();
	d3d11_device_.Release();
	dxgi_swap_chain_.Release();
	dxgi_factory_.Release();
}
