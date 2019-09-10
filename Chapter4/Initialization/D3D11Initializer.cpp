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
#define RETURN_FALSE_IF_FAILED(HR) if (FAILED(HR)) {return false;}
	RETURN_FALSE_IF_FAILED(hr);

	hr = d3d11_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &d3d11_msaa_4x_quality_);
	RETURN_FALSE_IF_FAILED(hr);

	CComPtr<IDXGIDevice> dxgi_device;
	hr = d3d11_device_->QueryInterface(&dxgi_device);
	RETURN_FALSE_IF_FAILED(hr);

	CComPtr<IDXGIAdapter> dxgi_adapter;
	hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter);
	RETURN_FALSE_IF_FAILED(hr);

	hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)& dxgi_factory_);
	RETURN_FALSE_IF_FAILED(hr);

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
	RETURN_FALSE_IF_FAILED(hr);
	
	CComPtr<ID3D11Texture2D> back_buffer;
	hr = dxgi_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)& back_buffer);
	RETURN_FALSE_IF_FAILED(hr);

	hr = d3d11_device_->CreateRenderTargetView(back_buffer, nullptr, &d3d11_render_target_view_);
	RETURN_FALSE_IF_FAILED(hr);

	D3D11_TEXTURE2D_DESC depth_stencil_desc = { 0 };
	depth_stencil_desc.Width = width;
	depth_stencil_desc.Height = height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (use_msaa_4x)
	{
		depth_stencil_desc.SampleDesc.Count = 4;
		depth_stencil_desc.SampleDesc.Quality = d3d11_msaa_4x_quality_ - 1;
	}
	else
	{
		depth_stencil_desc.SampleDesc.Count = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
	}
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	hr = d3d11_device_->CreateTexture2D(&depth_stencil_desc, nullptr, &d3d11_depth_stencil_buffer_);
	RETURN_FALSE_IF_FAILED(hr);

	hr = d3d11_device_->CreateDepthStencilView(d3d11_depth_stencil_buffer_, nullptr, &d3d11_depth_stencil_view_);
	RETURN_FALSE_IF_FAILED(hr);

	ID3D11RenderTargetView* render_target_views = d3d11_render_target_view_;
	d3d11_device_ctx_->OMSetRenderTargets(1, &render_target_views, d3d11_depth_stencil_view_);

	D3D11_VIEWPORT viewport = { 0 };
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;
	d3d11_device_ctx_->RSSetViewports(1, &viewport);
	return true;
}

bool D3D11Initializer::Initialize(int width, int height, HWND hwnd, int monitor, bool fullscreen, bool use_msaa_4x)
{
	HRESULT hr = S_OK;
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgi_factory_);
	RETURN_FALSE_IF_FAILED(hr);

	CComPtr<IDXGIAdapter> dxgi_adapter;
	hr = dxgi_factory_->EnumAdapters(monitor, &dxgi_adapter);
	RETURN_FALSE_IF_FAILED(hr);

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
	RETURN_FALSE_IF_FAILED(hr);

	hr = d3d11_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &d3d11_msaa_4x_quality_);
	RETURN_FALSE_IF_FAILED(hr);

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
	RETURN_FALSE_IF_FAILED(hr);

	CComPtr<ID3D11Texture2D> back_buffer;
	hr = dxgi_swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
	RETURN_FALSE_IF_FAILED(hr);

	hr = d3d11_device_->CreateRenderTargetView(back_buffer, nullptr, &d3d11_render_target_view_);
	RETURN_FALSE_IF_FAILED(hr);

	D3D11_TEXTURE2D_DESC depth_stencil_desc = { 0 };
	depth_stencil_desc.Width = width;
	depth_stencil_desc.Height = height;
	depth_stencil_desc.MipLevels = 1;
	depth_stencil_desc.ArraySize = 1;
	depth_stencil_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (use_msaa_4x)
	{
		depth_stencil_desc.SampleDesc.Count = 4;
		depth_stencil_desc.SampleDesc.Quality = d3d11_msaa_4x_quality_ - 1;
	}
	else
	{
		depth_stencil_desc.SampleDesc.Count = 1;
		depth_stencil_desc.SampleDesc.Quality = 0;
	}
	depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_stencil_desc.CPUAccessFlags = 0;
	depth_stencil_desc.MiscFlags = 0;
	hr = d3d11_device_->CreateTexture2D(&depth_stencil_desc, nullptr, &d3d11_depth_stencil_buffer_);
	RETURN_FALSE_IF_FAILED(hr);
	
	hr = d3d11_device_->CreateDepthStencilView(d3d11_depth_stencil_buffer_, nullptr, &d3d11_depth_stencil_view_);
	RETURN_FALSE_IF_FAILED(hr);

	ID3D11RenderTargetView* render_target_views = d3d11_render_target_view_;
	d3d11_device_ctx_->OMSetRenderTargets(1, &render_target_views, d3d11_depth_stencil_view_);

	D3D11_VIEWPORT viewport = { 0 };
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;
	d3d11_device_ctx_->RSSetViewports(1, &viewport);

	return true;
}

void D3D11Initializer::Render()
{
	if (!d3d11_device_ctx_)
	{
		return;
	}

	const FLOAT blue_color[4] = {0.0f, 0.0f, 1.0f, 1.0f};
	d3d11_device_ctx_->ClearRenderTargetView(d3d11_render_target_view_, blue_color);
	d3d11_device_ctx_->ClearDepthStencilView(d3d11_depth_stencil_view_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	dxgi_swap_chain_->Present(0, 0);
}

void D3D11Initializer::Uninitialize()
{
	d3d11_render_target_view_.Release();
	d3d11_depth_stencil_view_.Release();
	d3d11_depth_stencil_buffer_.Release();

	d3d11_device_ctx_.Release();
	d3d11_device_.Release();
	dxgi_swap_chain_.Release();
	dxgi_factory_.Release();
}
