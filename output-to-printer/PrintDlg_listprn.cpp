// исходный текст программы сохранен в кодировке UTF-8 с сигнатурой

// PrintDlg_listprn.cpp
// получение принтера из списка в операционной системе Windows с помощью
// функции PrintDlg из набора функций Windows API
// (по выбору пользователя из диалогового окна)

#include <io.h>      // для функции _setmode
#include <fcntl.h>   // для константы _O_U16TEXT
#include <iostream>
#include <windows.h> // для функций WinAPI
using namespace std;

// прототип функции для получения принтера из списка
BOOL GetPrinterFromDlg(HDC* printerDC, LPWSTR* printerName);

int main()
{
	// переключение стандартного потока вывода в формат Юникода
	_setmode(_fileno(stdout), _O_U16TEXT);

	HDC printerDC;      // для контекста устройства (принтера)
	LPWSTR printerName; // для названия принтера

	// вызываем функцию для получения контекста устройства (принтера)
	// и названия принтера
	if (!GetPrinterFromDlg(&printerDC, &printerName))
	{
		wcout << L"Не удалось получить принтер!\n";
		return -1; // завершаем программу ошибкой
	}

	wcout << L"Вы выбрали принтер:\n " << printerName << endl;

	return 0;
}

// функция для получения принтера из списка
BOOL GetPrinterFromDlg(HDC* printerDC, LPWSTR* printerName)
{
	BOOL OK = FALSE; // переменная-флаг: успех или провал вызова функции

	// определим структуру с данными и значения ее полей для построения
	// диалогового окна
	PRINTDLGW printDlgInfo = {}; // всем полям структуры присваиваем значение 0
	printDlgInfo.lStructSize = sizeof(printDlgInfo); // размер структуры в байтах
	printDlgInfo.Flags =
		PD_RETURNDC; // возвратить контекст устройства (принтера)

	// получим контекст устройства (принтера) в структуру printDlgInfo
	OK = PrintDlgW(&printDlgInfo);
	if (!OK)       // если получена ошибка,
		return OK; // завершить функцию ошибкой

	// всё, что нам нужно, возвращено функцией PrintDlgW в структуру printDlgInfo,
	// нужно только извлечь эту информацию:

	// получим из структуры printDlgInfo контекст устройства (принтера)
	*printerDC = printDlgInfo.hDC;

	// получим из структуры printDlgInfo название принтера
	DEVNAMES* pDN = (DEVNAMES*)GlobalLock(printDlgInfo.hDevNames);
	*printerName = (LPWSTR)pDN + pDN->wDeviceOffset;
	GlobalUnlock(printDlgInfo.hDevNames);

	return OK; // завершить функцию успехом
}