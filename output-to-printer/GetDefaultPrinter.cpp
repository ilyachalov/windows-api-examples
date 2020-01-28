// исходный текст программы сохранен в кодировке UTF-8 с сигнатурой

// GetDefaultPrinter.cpp
// получение названия принтера по умолчанию в операционной системе Windows с помощью
// функции GetDefaultPrinter из набора функций Windows API

#include <io.h>      // для функции _setmode
#include <fcntl.h>   // для константы _O_U16TEXT
#include <iostream>
#include <windows.h> // для функций WinAPI
using namespace std;

// прототип функции для получения названия принтера по умолчанию
BOOL GetDefPrinter(LPWSTR* printerName);

int main()
{
	// переключение стандартного потока вывода в формат Юникода
	_setmode(_fileno(stdout), _O_U16TEXT);
	
	LPWSTR printerName; // для названия принтера по умолчанию

	// вызываем функцию для получения названия принтера по умолчанию
	if (!GetDefPrinter(&printerName))
	{
		wcout << L"Не удалось получить принтер по умолчанию!\n";
		return -1; // завершаем программу ошибкой
	}

	wcout << L"Принтер по умолчанию:\n " << printerName << endl;

	return 0;
}

// функция для получения названия принтера по умолчанию
BOOL GetDefPrinter(LPWSTR* printerName)
{
	DWORD n; // количество символов (включая нулевой символ), требуемое под
			 // массив-буфер для размещения названия принтера по умолчанию

	// 1-й вызов: получим количество символов n
	BOOL OK = GetDefaultPrinterW(NULL, &n);

	// выделим память под массив-буфер для размещения названия принтера
	*printerName = NULL;
	if (n > 0)
		*printerName = new WCHAR[n];

	// 2-й вызов: получим название принтера по умолчанию
	if (*printerName != NULL)
		OK = GetDefaultPrinterW(*printerName, &n);

	return OK;
}