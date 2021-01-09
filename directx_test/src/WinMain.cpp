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

    HWND hwnd = CreateWindowEx(0, class_name, L"test", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 640, 480,
                               nullptr, nullptr, hInstance, nullptr);

    ID3D11Device* d3d11_device;
    D3D_FEATURE_LEVEL feature_level;
    ID3D11DeviceContext* immediate_context;

    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_1};

    HRESULT res = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, feature_levels, 1, D3D11_SDK_VERSION,
                                    &d3d11_device, &feature_level, &immediate_context);

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