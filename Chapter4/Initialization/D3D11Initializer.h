#pragma once
#include <atlcomcli.h>
#include <d3d11.h>

#include "GameTimer.h"

class D3D11Initializer
{
public:
	D3D11Initializer();
	~D3D11Initializer();

	bool InitializeWithDefault(int width, int height, HWND hwnd, bool fullscreen = false, bool use_msaa_4x = false);
	bool Initialize(int width, int height, HWND hwnd, int monitor = 0, bool fullscreen = false, bool use_msaa_4x = false);

	void Render();

	void Uninitialize();

private:
	CComPtr<ID3D11Device>			d3d11_device_;
	CComPtr<ID3D11DeviceContext>	d3d11_device_ctx_;
	CComPtr<IDXGIFactory>			dxgi_factory_;
	CComPtr<IDXGISwapChain>			dxgi_swap_chain_;
	CComPtr<ID3D11RenderTargetView>	d3d11_render_target_view_;
	CComPtr<ID3D11Texture2D>		d3d11_depth_stencil_buffer_;
	CComPtr<ID3D11DepthStencilView>	d3d11_depth_stencil_view_;

	UINT							d3d11_msaa_4x_quality_	= 0;

	GameTimer						timer_;
};

