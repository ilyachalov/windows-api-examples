﻿// исходный текст программы сохранен в кодировке UTF-8 с сигнатурой

// Output_to_printer_WinGDI_4_AlignMode.cpp
// вывод на принтер с помощью набора функций Windows API с использованием
// функций Windows GDI (graphics device interface — интерфейс графических
// устройств)

// для вывода текста на устройство используется функция TextOut
// (добавляем возможность выравнивания по обеим сторонам, влево, по центру и вправо)

#include <io.h>      // для функции _setmode
#include <fcntl.h>   // для константы _O_U16TEXT
#include <iostream>
#include <windows.h> // для функций WinAPI
using namespace std;

// прототип функции для вывода текста на устройство
BOOL TextOutput(HDC pDC, LPCWSTR str, int align_mode = 0);
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
		L"Пробило 12 часов дня, и майор Щелколобов, обладатель тысячи десятин земли и "
		L"молоденькой жены, высунул свою плешивую голову из-под ситцевого одеяла и громко "
		L"выругался. Вчера, проходя мимо беседки, он слышал, как молодая жена его, майорша "
		L"Каролина Карловна, более чем милостиво беседовала со своим приезжим кузеном, "
		L"называла своего супруга, майора Щелколобова, бараном и с женским легкомыслием "
		L"доказывала, что она своего мужа не любила, не любит и любить не будет за его, "
		L"Щелколобова, тупоумие, мужицкие манеры и наклонность к умопомешательству и "
		L"хроническому пьянству. Такое отношение жены поразило, возмутило и привело в "
		L"сильнейшее негодование майора. Он не спал целую ночь и целое утро. В голове у "
		L"него кипела непривычная работа, лицо горело и было краснее вареного рака; кулаки "
		L"судорожно сжимались, а в груди происходила такая возня и стукотня, какой майор и "
		L"под Карсом не видал и не слыхал. Выглянув из-под одеяла на свет божий и выругавшись,"
		L" он спрыгнул с кровати и, потрясая кулаками, зашагал по комнате.";

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
//     align_mode
//         = 0 (по умолчанию) не заботиться о выравнивании;
//         = 1 выравнять текст по обеим сторонам;
//         = 2 выравнять текст влево;
//         = 3 выравнять текст по центру;
//         = 4 выравнять текст вправо.
BOOL TextOutput(HDC pDC, LPCWSTR str, int align_mode)
{
	// определить шрифт и его настройки
	HFONT hFont;
	int fHeight = -MulDiv(11, GetDeviceCaps(pDC, LOGPIXELSY), 72);
	hFont = CreateFontW(fHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"Arial");
	SelectObject(pDC, hFont);

	// устанавливаем режим выравнивания текста
	int coordX = 100; // координата X (100 пикселей — для выравнивания влево или по обеим сторонам)
	if (align_mode == 2)
		SetTextAlign(pDC, TA_LEFT);
	else if (align_mode == 3)
	{
		coordX = GetDeviceCaps(pDC, HORZRES) / 2;
		SetTextAlign(pDC, TA_CENTER);
	}
	else if (align_mode == 4)
	{
		coordX = GetDeviceCaps(pDC, HORZRES) - 100;
		SetTextAlign(pDC, TA_RIGHT);
	}

	// получим максимально возможную ширину подстроки
	// и вычтем 200 пикселей: чтобы оставить расстояние в 100 пикселей
	// с каждой стороны (зазор между текстом и краем листа)
	int max_width = GetDeviceCaps(pDC, HORZRES) - 200;

	BOOL OK;

	// переберем строку посимвольно и разобьем ее на подстроки,
	// которые будем печатать
	int i = 0;     // текущий номер символа в основной строке
	SIZE sz;       // структура с размерами (ширина, высота) подстроки в пикселях
	int start = i; // начало подстроки (номер символа в основной строке)
	int n = 1;     // номер подстроки
	while (i < wcslen(str))
	{
		// получаем размеры подстроки в пикселях
		OK = GetTextExtentPoint32W(pDC, str + start, i - start + 1, &sz);
		// если ширина подстроки превысила максимальный размер
		// или это последняя подстрока
		if (sz.cx >= max_width ||
			i == wcslen(str) - 1)
		{
			// если это не последняя подстрока
			if (i != wcslen(str) - 1)
			{
				// распечатать подстроку выбранным шрифтом, закончив
				// символом, который еще влазил в максимальную ширину
				
				// если мы находимся в середине слова
				if (*(str + i) != L' ')
					// вернуться к началу слова
					while (*(str + i) != L' ')
						i--;
				// иначе вернуться к предыдущему символу
				else
					i--;

				if (align_mode == 1) // выравнивание текста по обеим сторонам
				{
					// снова получим размеры подстроки, которую собираемся печатать
					// (это нужно потому, что неизвестно, на сколько символов отступили)
					OK = GetTextExtentPoint32W(pDC, str + start, i - start + 1, &sz);
					// подсчитаем число пробелов в этой подстроке
					int k = 0;
					for (int j = start; j < i + 1; j++)
					{
						if (*(str + j) == L' ')
							k++;
					}
					// настроим выравнивание по ширине
					if (k > 0)
						OK = SetTextJustification(pDC, max_width - sz.cx, k);
				}

				// печатаем строку
				OK = TextOutW(pDC, coordX, n * sz.cy, str + start, i - start + 1);
				if (!OK)
				{
					wcout << L"Не получилось вывести на страницу текст!\n";
					return FALSE;
				}

				if (align_mode == 1) // выравнивание текста по обеим сторонам
				{
					// вернем выравнивание по умолчанию
					OK = SetTextJustification(pDC, 0, 0);
				}

				start = i + 1; // переходим к следующей подстроке
				n++;           // увеличим номер подстроки
			}
			else // печать последней подстроки и выход из цикла
			{
				OK = TextOutW(pDC, coordX, n * sz.cy, str + start, i - start + 1);
				if (!OK)
				{
					wcout << L"Не получилось вывести на страницу текст!\n";
					return FALSE;
				}

				break; // выход из цикла
			}
		}
		// переходим к следующему символу основной строки
		i++;
	}

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