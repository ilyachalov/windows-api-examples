// исходный текст программы сохранен в кодировке UTF-8 с сигнатурой

// PrintDlgEx_defprn.cpp
// получение принтера по умолчанию в операционной системе Windows с помощью
// функции PrintDlgEx из набора функций Windows API
// (диалоговое окно на экране не показываем)

#include <io.h>      // для функции _setmode
#include <fcntl.h>   // для константы _O_U16TEXT
#include <iostream>
#include <windows.h> // для функций WinAPI
using namespace std;

// прототип функции для получения принтера по умолчанию
HRESULT GetDefPrinterEx(HDC* printerDC, LPWSTR* printerName);

int main()
{
	// переключение стандартного потока вывода в формат Юникода
	_setmode(_fileno(stdout), _O_U16TEXT);

	HDC printerDC;      // для контекста устройства (принтера по умолчанию)
	LPWSTR printerName; // для названия принтера по умолчанию

	// вызываем функцию для получения контекста устройства (принтера)
	// и названия принтера по умолчанию
	if (GetDefPrinterEx(&printerDC, &printerName) != S_OK)
	{
		wcout << L"Не удалось получить принтер по умолчанию!\n";
		return -1; // завершаем программу ошибкой
	}

	wcout << L"Принтер по умолчанию:\n " << printerName << endl;

	return 0;
}

// функция для получения принтера по умолчанию
HRESULT GetDefPrinterEx(HDC* printerDC, LPWSTR* printerName)
{
	HRESULT OK = S_FALSE; // переменная-флаг: успех или провал вызова функции

	// определим структуру с данными и значения ее полей для построения
	// диалогового окна
	PRINTDLGEXW printDlgInfo = {}; // всем полям структуры присваиваем значение 0
	printDlgInfo.lStructSize = sizeof(printDlgInfo); // размер структуры в байтах
	printDlgInfo.hwndOwner = GetConsoleWindow();     // дескриптор родительского окна
	printDlgInfo.Flags =
		PD_RETURNDEFAULT // не показывать диалоговое окно
		| PD_RETURNDC    // возвратить контекст устройства (принтера) по умолчанию
		| PD_NOPAGENUMS; // если этот флаг не установить, придется определять поля
						 // структуры printDlgInfo, связанные с диапазоном страниц
	printDlgInfo.nStartPage = START_PAGE_GENERAL; // обязательное поле структуры

	// получим контекст устройства (принтера) в структуру printDlgInfo
	OK = PrintDlgExW(&printDlgInfo);
	if (OK != S_OK) // если получена ошибка,
		return OK;  // завершить функцию ошибкой

	// функция PrintDlgExW возвратила значение S_OK, при этом в структуре printDlgInfo
	// в поле dwResultAction возвращено значение 0 (PD_RESULT_CANCEL) несмотря на то,
	// что мы не показывали диалоговое окно (то есть считается, будто пользователь
	// нажал кнопку "Cansel" ("Отмена") в диалоговом окне)

	// всё, что нам нужно, возвращено функцией PrintDlgExW в структуру printDlgInfo,
	// нужно только извлечь эту информацию:

	// получим из структуры printDlgInfo контекст устройства (принтера)
	*printerDC = printDlgInfo.hDC;

	// получим из структуры printDlgInfo название принтера
	DEVNAMES* pDN = (DEVNAMES*)GlobalLock(printDlgInfo.hDevNames);
	*printerName = (LPWSTR)pDN + pDN->wDeviceOffset;
	GlobalUnlock(printDlgInfo.hDevNames);

	return OK; // завершить функцию успехом
}