#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>

#include "bcdx11.h"

int cleanup();
int create_device(HWND hWnd);
void cleanup_device();
void create_render_target();
void cleanup_render_target();
void resize(unsigned int width, unsigned int height);
LRESULT WINAPI window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

ID3D11Device* device = 0;
ID3D11DeviceContext* context = 0;
IDXGISwapChain* swap_chain = 0;
ID3D11RenderTargetView* render_target_view = 0;

unsigned int window_width = 1280;
unsigned int window_height = 720;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	WNDCLASSEX window_class = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, window_proc, 0, 0, GetModuleHandle(0), LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW), 0, 0, TEXT("KaffeeWindow"), 0 };
	RegisterClassEx(&window_class);

	HWND hwnd = CreateWindow(window_class.lpszClassName, TEXT(""), WS_OVERLAPPEDWINDOW, 0, 0, window_width, window_height, 0, 0, hInstance, 0);

	if (!create_device(hwnd))
	{
		UnregisterClass(window_class.lpszClassName, window_class.hInstance);
		return cleanup();
	}

	if (bcdx11::initialize(device, context))
	{
		return cleanup();
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float color[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		context->OMSetRenderTargets(1, &render_target_view, 0);
		context->ClearRenderTargetView(render_target_view, color);

		factory::get_context()->push_command({ 0.0f, 0.0f, 1920.0f, 1080.0f });

		factory::set_font(1);
		factory::get_context()->text({ 50.0f, 50.f }, 1.0f, "The quick brown fox jumps over the lazy dog!", 0xFFFFFFFF, strlen("The quick brown fox jumps over the lazy dog!"));

		bcdx11::render();

		swap_chain->Present(1, 0);
	}

	cleanup();

	DestroyWindow(hwnd);
	UnregisterClass(window_class.lpszClassName, window_class.hInstance);

	return 0;
}

LRESULT WINAPI window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
	case WM_CHAR:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			window_width = (UINT)LOWORD(lParam);
			window_height = (UINT)HIWORD(lParam);
			resize(window_width, window_height);
		}
		break;
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int cleanup()
{
	bcdx11::shutdown();
	cleanup_device();

	return 0;
}

int create_device(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 2;
	desc.BufferDesc.Width = 0;
	desc.BufferDesc.Height = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = hWnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT flags = 0;
	D3D_FEATURE_LEVEL feature_level;
	D3D_FEATURE_LEVEL feature_levels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

	if (D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, feature_levels, 2, D3D11_SDK_VERSION, &desc, &swap_chain, &device, &feature_level, &context) != S_OK)
	{
		return 0;
	}

	create_render_target();

	return 1;
}

void cleanup_device()
{
	cleanup_render_target();

	if (swap_chain)
	{
		swap_chain->Release();
		swap_chain = 0;
	}

	if (context)
	{
		context->Release();
		context = 0;
	}

	if (device)
	{
		device->Release();
		device = 0;
	}
}

void create_render_target()
{
	ID3D11Texture2D* back_buffer;
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	device->CreateRenderTargetView(back_buffer, 0, &render_target_view);
	back_buffer->Release();
}

void cleanup_render_target()
{
	if (render_target_view)
	{
		render_target_view->Release();
		render_target_view = 0;
	}
}

void resize(unsigned int width, unsigned int height)
{
	if (device != 0)
	{
		cleanup_render_target();
		swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		create_render_target();
		bcdx11::resize(width, height);
	}
}
