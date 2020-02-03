// исходный текст программы сохранен в кодировке UTF-8 с сигнатурой

// Output_to_printer_WinGDI_2_DrawText.cpp
// вывод на принтер с помощью набора функций Windows API с использованием
// функций Windows GDI (graphics device interface — интерфейс графических
// устройств)

// для вывода текста на устройство используется функция DrawText

#include <io.h>      // для функции _setmode
#include <fcntl.h>   // для константы _O_U16TEXT
#include <iostream>
#include <windows.h> // для функций WinAPI
using namespace std;

// прототип функции для вывода текста на устройство
BOOL TextOutput(HDC pDC, LPCWSTR str);
// прототип функции для получения названия принтера по умолчанию
BOOL GetDefPrinter(LPWSTR* printerName);
// прототип функции для посылки задания на печать на принтер
BOOL toPrinter(LPCWSTR str);

int main()
{
	// переключение стандартного потока вывода в формат Юникода
	_setmode(_fileno(stdout), _O_U16TEXT);

	// тестовые данные для вывода на принтер
	const wchar_t* s1 =
		L"НЕВЕЗЕНИЕ\n"
		L"\n"
		L"\tЯ проснулся от жестокой боли во всем теле. Я открыл глаза и увидел медсестру,"
		L" стоящую у моей койки.\n"
		L"\t— Мистер Фуджима, — сказала она, — вам повезло, вам удалось выжить после"
		L" бомбардировки Хиросимы два дня назад. Но теперь вы в госпитале, вам больше"
		L" ничего не угрожает.\n"
		L"\tЧуть живой от слабости, я спросил:\n"
		L"\t— Где я?\n"
		L"\t— В Нагасаки, — ответила она.\n"
		L"\n"
		L"(Alan E. Mayer, рассказ «Bad Luck»."
		L"\nНа конкурс коротких рассказов из 55 слов.)";

	// вызов функции для вывода строки на принтер по умолчанию
	if (!toPrinter(s1)) // если при вызове случилась ошибка
		exit(-1);       // завершаем программу ошибкой

	wcout << L"Вывод файла на принтер выполнен успешно!\n";

	// если принтер по умолчанию — виртуальный принтер Microsoft XPS Document Writer,
	// то данные выводятся в файл «Распечатанный документ.xps» текущего каталога
	// (у меня получалось, что текущий каталог — это каталог, в котором находится
	// исполняемый файл программы)

	return 0;
}

// функция для вывода текста на устройство
BOOL TextOutput(HDC pDC, LPCWSTR str)
{
	// определить шрифт и его настройки
	HFONT hFont;
	int fHeight = -MulDiv(11, GetDeviceCaps(pDC, LOGPIXELSY), 72);
	hFont = CreateFontW(fHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
	SelectObject(pDC, hFont);

	// распечатать текст выбранным шрифтом в прямоугольнике
	RECT rc = { 100, 100, GetDeviceCaps(pDC, HORZRES) - 100,
		GetDeviceCaps(pDC, VERTRES) - 100 };
	int OK = DrawTextW(pDC, str, wcslen(str), &rc,
		DT_WORDBREAK      // если строка достигает правого края страницы, выполнять
		                  // перенос на следующую строку (перенос не может быть
		                  // в середине слова, только между слов)
		| DT_EXPANDTABS); // каждый символ табуляции расширять в 8 пробелов
	if (!OK)
		return FALSE;

	return TRUE;
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

// функция для посылки задания на печать на принтер
BOOL toPrinter(LPCWSTR str)
{
	LPWSTR printerName; // для названия принтера по умолчанию
	// вызываем функцию для получения названия принтера по умолчанию
	if (!GetDefPrinter(&printerName))
	{
		wcout << L"Не удалось получить принтер по умолчанию!\n";
		return FALSE; // завершаем программу ошибкой
	}

	HDC printerDC; // контекст устройства (принтера)
	// получим контекст устройства (принтера), которое выбрал пользователь
	printerDC = CreateDCW(L"WINSPOOL", printerName, NULL, NULL);
	// если контекст устройства получен успешно
	if (printerDC != NULL)
	{
		DOCINFOW docinfo; // информация о документе, который будем печатать
		docinfo.cbSize = sizeof(docinfo);      // размер структуры с документом
		docinfo.lpszDocName = L"Мой документ"; // название документа
		if (wcscmp(printerName,
			L"Microsoft XPS Document Writer") == 0)
			docinfo.lpszOutput = L"Распечатанный документ.xps";
		else
			docinfo.lpszOutput = NULL;
		docinfo.lpszDatatype = NULL;
		docinfo.fwType = 0;

		int idJob; // номер задания, отправляемого на принтер
		// проинформируем спулер печати о начале печати нового документа
		idJob = StartDocW(printerDC, &docinfo);
		if (idJob > 0)
		{
			int result;
			// начать печать страницы
			result = StartPage(printerDC);
			if (result > 0)
			{
				// распечатать текст
				BOOL OK = TextOutput(printerDC, str);
				if (!OK)
				{
					wcout << L"Не получилось вывести на страницу текст!\n";
					return FALSE;
				}

				// закончить печать страницы
				EndPage(printerDC);
			}
			else
			{
				wcout << L"Не получилось начать печать страницы!\n";
				return FALSE;
			}
			// проинформируем спулер печати об окончании печати документа
			EndDoc(printerDC);
		}
		else
		{
			wcout << L"Не получилось поставить новый документ в очередь печати!\n";
			return FALSE;
		}

		// удалить контекст устройства (принтера), так как он нам больше не нужен
		DeleteDC(printerDC);
	}
	// контекст устройства не получен
	else
	{
		wcout << L"Не удалось получить контекст устройства (принтера)!\n";
		return FALSE;
	}

	return TRUE;
}