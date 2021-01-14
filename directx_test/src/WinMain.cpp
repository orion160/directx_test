#include "windows_api.h"

#include <d3d11.h>
#include <d3dcompiler.h>

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
    RECT wr = {0};
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;

    AdjustWindowRectEx(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE, 0);

    HWND hwnd =
        CreateWindowEx(0, class_name, L"test", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
                       wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, hInstance, nullptr);

    ID3D11Device* d3d11_device = nullptr;
    D3D_FEATURE_LEVEL feature_level;
    ID3D11DeviceContext* immediate_context = nullptr;
    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_1};

    HRESULT res_device = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, feature_levels, 1,
                                           D3D11_SDK_VERSION, &d3d11_device, &feature_level, &immediate_context);

    IDXGIDevice* dxgi_device = nullptr;
    IDXGIAdapter* dxgi_adapter = nullptr;
    IDXGIFactory* dxgi_factory = nullptr;
    d3d11_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgi_device));
    dxgi_device->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgi_adapter));
    dxgi_adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgi_factory));

    IDXGISwapChain* swap_chain = nullptr;
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

    ID3D11Texture2D* buffer = nullptr;
    HRESULT res_buffer = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer));
    ID3D11RenderTargetView* render_target_view = nullptr;
    d3d11_device->CreateRenderTargetView(buffer, nullptr, &render_target_view);

    ID3D11VertexShader* vertex_shader = nullptr;
    ID3DBlob* blob1 = nullptr;
    D3DReadFileToBlob(L"VertexShader.cso", &blob1);
    d3d11_device->CreateVertexShader(blob1->GetBufferPointer(), blob1->GetBufferSize(), nullptr, &vertex_shader);

    ID3D11PixelShader* pixel_shader = nullptr;
    ID3DBlob* blob2 = nullptr;
    D3DReadFileToBlob(L"PixelShader.cso", &blob2);
    d3d11_device->CreatePixelShader(blob2->GetBufferPointer(), blob2->GetBufferSize(), nullptr, &pixel_shader);

    float vertices[] = {-0.5F, -0.5F, 0.0F, 0.0F, 0.5F, 0.0F, 0.5F, -0.5F, 0.0F};

    D3D11_BUFFER_DESC buffer_descriptor = {0};
    buffer_descriptor.ByteWidth = sizeof(float) * 9;
    buffer_descriptor.Usage = D3D11_USAGE_DEFAULT;
    buffer_descriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_descriptor.CPUAccessFlags = 0;
    buffer_descriptor.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA sub_data = {0};
    sub_data.pSysMem = vertices;

    ID3D11Buffer* output_buffer = nullptr;
    d3d11_device->CreateBuffer(&buffer_descriptor, &sub_data, &output_buffer);

    D3D11_INPUT_ELEMENT_DESC layout_descriptor[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    ID3D11InputLayout* layout = nullptr;
    d3d11_device->CreateInputLayout(layout_descriptor, 1, blob1->GetBufferPointer(), blob1->GetBufferSize(), &layout);

    ShowWindow(hwnd, SW_SHOW);

    MSG msg = {0};
    BOOL msg_result;
    while ((msg_result = GetMessage(&msg, nullptr, 0, 0)) != 0)
    {
        float rgba_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
        immediate_context->ClearRenderTargetView(render_target_view, rgba_color);
        immediate_context->OMSetRenderTargets(1, &render_target_view, nullptr);

        D3D11_VIEWPORT viewport = {0};
        viewport.Width = static_cast<FLOAT>(width);
        viewport.Height = static_cast<FLOAT>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        immediate_context->RSSetViewports(1, &viewport);

        immediate_context->VSSetShader(vertex_shader, nullptr, 0);

        immediate_context->PSSetShader(pixel_shader, nullptr, 0);

        UINT stride = sizeof(float) * 3;
        UINT offset = 0;
        immediate_context->IASetInputLayout(layout);
        immediate_context->IASetVertexBuffers(0, 1, &output_buffer, &stride, &offset);

        immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        immediate_context->Draw(3, 0);

        swap_chain->Present(TRUE, 0);

        if (msg_result == -1)
        {
            return -1;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(1);
    }

    return 0;
}