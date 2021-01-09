#include "windows_api.h"

#include <d3d11.h>

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                    _In_ int nShowCmd)
{
    const wchar_t class_name[] = L"class name";

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = window_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = class_name;
    wc.hIconSm = nullptr;
    RegisterClassEx(&wc);

    int width = 640, height = 480;

    HWND hwnd = CreateWindowEx(0, class_name, L"test", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
                               CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, nullptr);

    ID3D11Device* d3d11_device;
    D3D_FEATURE_LEVEL feature_level;
    ID3D11DeviceContext* immediate_context;

    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_1};

    HRESULT res_device = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, feature_levels, 1, D3D11_SDK_VERSION,
                                    &d3d11_device, &feature_level, &immediate_context);

    IDXGIDevice* dxgi_device;
    IDXGIAdapter* dxgi_adapter;
    IDXGIFactory* dxgi_factory;
    d3d11_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
    dxgi_device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgi_adapter));
    dxgi_adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgi_factory));


    IDXGISwapChain* swap_chain;
    DXGI_SWAP_CHAIN_DESC swapchain_desriptor = {0};
    swapchain_desriptor.BufferCount = 1;
    swapchain_desriptor.BufferDesc.Width = width;
    swapchain_desriptor.BufferDesc.Height = height;
    swapchain_desriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapchain_desriptor.BufferDesc.RefreshRate.Numerator = 60;
    swapchain_desriptor.BufferDesc.RefreshRate.Denominator = 1;
    swapchain_desriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapchain_desriptor.OutputWindow = hwnd;
    swapchain_desriptor.SampleDesc.Count = 1;
    swapchain_desriptor.SampleDesc.Quality = 0;
    swapchain_desriptor.Windowed = TRUE;
    HRESULT res_swap = dxgi_factory->CreateSwapChain(d3d11_device, &swapchain_desriptor, &swap_chain);

    

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {0};
    BOOL msg_result;
    while ((msg_result = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        if (msg_result == -1)
        {
            return -1;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}