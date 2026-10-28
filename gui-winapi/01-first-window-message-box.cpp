// Исходный код этой программы написан в кодировке UTF-8 (без BOM).
// При компиляции из командной строки с помощью MSVC (cl.exe) используйте
// параметр "/utf-8".

#define UNICODE
#include <windows.h>

#pragma comment(lib, "user32.lib") // для MessageBox

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    MessageBox(NULL,

        L"Hello, world!\n"
        "Простая программа с графическим интерфейсом!\n"
        "Приветствие по-китайски: 你好!\n"
        "Эмодзи: 😂🤣🧐😱😲",

        L"Приложение с GUI на WinAPI",
        MB_OK | MB_ICONINFORMATION);

    return 0;
}
