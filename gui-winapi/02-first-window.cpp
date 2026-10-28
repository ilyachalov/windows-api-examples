// Исходный код этой программы написан в кодировке UTF-8 (без BOM).
// При компиляции из командной строки с помощью MSVC (cl.exe) используйте
// параметр "/utf-8".

#define UNICODE
#include <windows.h>

#pragma comment(lib, "user32.lib") // для разных функций WinAPI
#pragma comment(lib, "gdi32.lib")  // для CreateSolidBrush

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // прототип

// точка входа
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    LPCWSTR CLASS_NAME{ L"MainWindow" };

    // 1. Создание и регистрация класса окна
    WNDCLASS wc{};
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(230, 230, 230));
    RegisterClass(&wc);

    // 2. Создание окна и показ его на экране
    HWND hWnd{};
    hWnd = CreateWindowEx(0, CLASS_NAME, L"Моё первое окно с WinAPI",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 400,
        NULL, NULL, hInstance, NULL);
    if (hWnd == NULL) return 0;
    ShowWindow(hWnd, nCmdShow);

    // 3. Цикл обработки сообщений
    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// функция окна для обработки сообщений
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}
