#include <iostream>
#include <math.h>
#include "Paint.h"

using namespace std;

#define N 4 // размер столбцов матриц
#define M 8 // количество точек в фигуре

#define angle 0.0174444444444444

struct square // структура грани 
{
	point P1; //точки
	point P2;
	point P3;
	point P4;
	double mid_z = 0; //среднее значение удаленности для грани

	COLORREF clr = rgbWhite; //цветовая структура
};




void mul_matrix(double fig[M][N], double mass[N][N]);
void scale(double fig[M][N], double); //масштабирование
void rotateX(double fig[M][N], double); // поворот относительно оси х
void rotateY(double fig[M][N], double);
void rotateZ(double fig[M][N], double);
void move(double fig[M][N], int = 0, int = 0, int = 0); //сдвиг фигуры

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Создаём прототип функции окна, которая будет определена ниже

char szProgName[] = "Курсовая работа";

// матрица свободной проекции
double pro[N][N] = { {1, 0, 0, 0},
					 {0, 1, 0, 0},
					 {sqrt(2) / 4, sqrt(2) / 4, 1, 0},
					 {0, 0, 0, 1} };

// координаты точек призмы
double prism[M][N] = { {400, 240, 100, 1},
					   {400, 160, 100, 1},
					   {480, 160, 100, 1},
					   {480, 240, 100, 1},
					   {400, 240, 180, 1},
					   {400, 160, 180, 1},
					   {480, 160, 180, 1},
					   {480, 240, 180, 1} };

// координаты точек параллелограма
double paral[M][N] = { {1000, 240, 100, 1},
					   {1050, 160, 100, 1},
					   {1150, 160, 100, 1},
					   {1100, 240, 100, 1},
					   {1000, 240, 180, 1},
					   {1050, 160, 180, 1},
					   {1150, 160, 180, 1},
					   {1100, 240, 180, 1} };

point light{ 750, -100.0f, 5.0f }; // координаты источника света

double COPY_prism[M][N];//массив копий 
double lilfig_prism[4][2];//массив точек грани
square FIG_prism[6];//массив граней

double COPY_paral[M][N];
double lilfig_paral[4][2];
square FIG_paral[6]; 

int WINAPI WinMain(HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	HWND hWnd; //Дескриптор HWND - уникальный номер экземпляра окна программы
	MSG lpMsg;
	WNDCLASS w; //создаём экземпляр структуры WNDCLASS

	// И начинаем её заполнять
	w.lpszClassName = szProgName; //имя программы
	w.hInstance = hInstance; //идентификатор текущего приложения
	w.lpfnWndProc = WndProc; //указатель на функцию окна
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //загрузка курсора
	w.hIcon = 0;
	w.lpszMenuName = 0;
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //цвет фона окна
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	//Если не удалось зарегистрировать класс окна - выходим
	if (!RegisterClass(&w))
	{
		return 0;
	}

	//Создадим окно в памяти, заполнив аргументы CreateWindow
	hWnd = CreateWindow(szProgName,
		"Курсовая работа",
		WS_OVERLAPPEDWINDOW, //Стиль окна - перекрывающий
		0, //положение окна на экране по х
		0, //положение по у
		1500, //ширина
		800, //высота
		(HWND)NULL, //идентификатор родительского окна
		(HMENU)NULL, //идентификатор меню
		(HINSTANCE)hInstance, //идентификатор экземпляра программы
		(HINSTANCE)NULL); //отсутствие дополнительных параметров

	ShowWindow(hWnd, nCmdShow); // Выводим окно на экран
	UpdateWindow(hWnd); // Обновим содержимое окна

	//Цикл обработки сообщений
	while (GetMessage(&lpMsg, NULL, 0, 0)) // Получаем сообщение из очереди
	{
		TranslateMessage(&lpMsg); // Преобразует сообщения клавиш в символы
		DispatchMessage(&lpMsg); // Передаёт сообщение соответствующей функции окна
	}
	return(lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) // Функция окна
{
	HDC hdc; //создаём контекст устройства
	PAINTSTRUCT ps; //создаём экземпляр структуры графического вывода

	//Цикл обработки сообщений
	switch (messg)
	{
	case WM_PAINT: // сообщение рисования

		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				COPY_prism[i][j] = prism[i][j]; 
			}
		}

		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				COPY_paral[i][j] = paral[i][j];
			}
		}

		mul_matrix(COPY_prism, pro); // перемножаем координаты призмы на матрицу проекции и все заносим в COPY
		mul_matrix(COPY_paral, pro);

		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		// призма 12 ребер
		Line(hdc, COPY_prism[0][0], COPY_prism[0][1], COPY_prism[1][0], COPY_prism[1][1], rgbBlack);
		Line(hdc, COPY_prism[1][0], COPY_prism[1][1], COPY_prism[2][0], COPY_prism[2][1], rgbBlack);
		Line(hdc, COPY_prism[2][0], COPY_prism[2][1], COPY_prism[3][0], COPY_prism[3][1], rgbBlack);
		Line(hdc, COPY_prism[3][0], COPY_prism[3][1], COPY_prism[0][0], COPY_prism[0][1], rgbBlack);

		Line(hdc, COPY_prism[4][0], COPY_prism[4][1], COPY_prism[5][0], COPY_prism[5][1], rgbBlack);
		Line(hdc, COPY_prism[5][0], COPY_prism[5][1], COPY_prism[6][0], COPY_prism[6][1], rgbBlack);
		Line(hdc, COPY_prism[6][0], COPY_prism[6][1], COPY_prism[7][0], COPY_prism[7][1], rgbBlack);
		Line(hdc, COPY_prism[7][0], COPY_prism[7][1], COPY_prism[4][0], COPY_prism[4][1], rgbBlack);

		Line(hdc, COPY_prism[1][0], COPY_prism[1][1], COPY_prism[5][0], COPY_prism[5][1], rgbBlack);
		Line(hdc, COPY_prism[4][0], COPY_prism[4][1], COPY_prism[0][0], COPY_prism[0][1], rgbBlack);

		Line(hdc, COPY_prism[2][0], COPY_prism[2][1], COPY_prism[6][0], COPY_prism[6][1], rgbBlack);
		Line(hdc, COPY_prism[7][0], COPY_prism[7][1], COPY_prism[3][0], COPY_prism[3][1], rgbBlack);

		//вершины , одна грань
		FIG_prism[0].P1.x = COPY_prism[0][0];
		FIG_prism[0].P1.y = COPY_prism[0][1];
		FIG_prism[0].P1.z = prism[0][2];
		FIG_prism[0].P2.x = COPY_prism[1][0];
		FIG_prism[0].P2.y = COPY_prism[1][1];
		FIG_prism[0].P2.z = prism[1][2];
		FIG_prism[0].P3.x = COPY_prism[2][0];
		FIG_prism[0].P3.y = COPY_prism[2][1];
		FIG_prism[0].P3.z = prism[2][2];
		FIG_prism[0].P4.x = COPY_prism[3][0];
		FIG_prism[0].P4.y = COPY_prism[3][1];
		FIG_prism[0].P4.z = prism[3][2];
		FIG_prism[0].clr = rgbBlue;

		FIG_prism[1].P1.x = COPY_prism[4][0];
		FIG_prism[1].P1.y = COPY_prism[4][1];
		FIG_prism[1].P1.z = prism[4][2];
		FIG_prism[1].P2.x = COPY_prism[5][0];
		FIG_prism[1].P2.y = COPY_prism[5][1];
		FIG_prism[1].P2.z = prism[5][2];
		FIG_prism[1].P3.x = COPY_prism[6][0];
		FIG_prism[1].P3.y = COPY_prism[6][1];
		FIG_prism[1].P3.z = prism[6][2];
		FIG_prism[1].P4.x = COPY_prism[7][0];
		FIG_prism[1].P4.y = COPY_prism[7][1];
		FIG_prism[1].P4.z = prism[7][2];
		FIG_prism[1].clr = rgbRed;

		FIG_prism[2].P1.x = COPY_prism[0][0];
		FIG_prism[2].P1.y = COPY_prism[0][1];
		FIG_prism[2].P1.z = prism[0][2];
		FIG_prism[2].P2.x = COPY_prism[1][0];
		FIG_prism[2].P2.y = COPY_prism[1][1];
		FIG_prism[2].P2.z = prism[1][2];
		FIG_prism[2].P3.x = COPY_prism[5][0];
		FIG_prism[2].P3.y = COPY_prism[5][1];
		FIG_prism[2].P3.z = prism[5][2];
		FIG_prism[2].P4.x = COPY_prism[4][0];
		FIG_prism[2].P4.y = COPY_prism[4][1];
		FIG_prism[2].P4.z = prism[4][2];
		FIG_prism[2].clr = rgbCyan;

		FIG_prism[3].P1.x = COPY_prism[2][0];
		FIG_prism[3].P1.y = COPY_prism[2][1];
		FIG_prism[3].P1.z = prism[2][2];
		FIG_prism[3].P2.x = COPY_prism[3][0];
		FIG_prism[3].P2.y = COPY_prism[3][1];
		FIG_prism[3].P2.z = prism[3][2];
		FIG_prism[3].P3.x = COPY_prism[7][0];
		FIG_prism[3].P3.y = COPY_prism[7][1];
		FIG_prism[3].P3.z = prism[7][2];
		FIG_prism[3].P4.x = COPY_prism[6][0];
		FIG_prism[3].P4.y = COPY_prism[6][1];
		FIG_prism[3].P4.z = prism[6][2];
		FIG_prism[3].clr = rgbGreen;

		FIG_prism[4].P1.x = COPY_prism[1][0];
		FIG_prism[4].P1.y = COPY_prism[1][1];
		FIG_prism[4].P1.z = prism[1][2];
		FIG_prism[4].P2.x = COPY_prism[2][0];
		FIG_prism[4].P2.y = COPY_prism[2][1];
		FIG_prism[4].P2.z = prism[2][2];
		FIG_prism[4].P3.x = COPY_prism[6][0];
		FIG_prism[4].P3.y = COPY_prism[6][1];
		FIG_prism[4].P3.z = prism[6][2];
		FIG_prism[4].P4.x = COPY_prism[5][0];
		FIG_prism[4].P4.y = COPY_prism[5][1];
		FIG_prism[4].P4.z = prism[5][2];
		FIG_prism[4].clr = rgbOlive;

		FIG_prism[5].P1.x = COPY_prism[4][0];
		FIG_prism[5].P1.y = COPY_prism[4][1];
		FIG_prism[5].P1.z = prism[4][2];
		FIG_prism[5].P2.x = COPY_prism[0][0];
		FIG_prism[5].P2.y = COPY_prism[0][1];
		FIG_prism[5].P2.z = prism[0][2];
		FIG_prism[5].P3.x = COPY_prism[3][0];
		FIG_prism[5].P3.y = COPY_prism[3][1];
		FIG_prism[5].P3.z = prism[3][2];
		FIG_prism[5].P4.x = COPY_prism[7][0];
		FIG_prism[5].P4.y = COPY_prism[7][1];
		FIG_prism[5].P4.z = prism[7][2];
		FIG_prism[5].clr = rgbMagenta;

		// ищем среднее значение удаленности для граней
		//считаем глубину грани и зарисовываем с дальней от меня 
		for (int q = 0; q < 6; q++)
		{
			FIG_prism[q].mid_z = (FIG_prism[q].P1.z + FIG_prism[q].P2.z + FIG_prism[q].P3.z + FIG_prism[q].P4.z) / 4;
		}

		for (int q = 0; q < 5; q++)
		{
			for (int v = 0; v < 5 - q; v++)
			{
				if (FIG_prism[v].mid_z > FIG_prism[v + 1].mid_z)
				{
					square TMP = FIG_prism[v];
					FIG_prism[v] = FIG_prism[v + 1];
					FIG_prism[v + 1] = TMP;
				}
			}
		}

		for (int q = 0; q < 6; q++)
		{
			//точки грани перезаписываются в новый массив
			lilfig_prism[0][0] = FIG_prism[q].P1.x;
			lilfig_prism[0][1] = FIG_prism[q].P1.y;

			lilfig_prism[1][0] = FIG_prism[q].P2.x;
			lilfig_prism[1][1] = FIG_prism[q].P2.y;

			lilfig_prism[2][0] = FIG_prism[q].P3.x;
			lilfig_prism[2][1] = FIG_prism[q].P3.y;

			lilfig_prism[3][0] = FIG_prism[q].P4.x;
			lilfig_prism[3][1] = FIG_prism[q].P4.y;

			PolyScan(hdc, lilfig_prism, FIG_prism[q].clr);
		}


		for (int i = 0; i < M; i++) // ф-ия отвечает за тень
		{

			COPY_prism[i][0] = COPY_prism[i][0] + (0.50f * 800 - COPY_prism[i][1]) * (COPY_prism[i][0] - light.x) / (COPY_prism[i][1] - light.y);

			COPY_prism[i][2] = prism[i][2] + (0.50f * 800 - COPY_prism[i][1]) * (COPY_prism[i][2] - light.z) / (COPY_prism[i][1] - light.y);

			COPY_prism[i][1] = 0.90f * 800 - COPY_prism[i][2] * 0.8f;
		}
		
		FIG_prism[0].P1.x = COPY_prism[0][0];
		FIG_prism[0].P1.y = COPY_prism[0][1];
		FIG_prism[0].P2.x = COPY_prism[1][0];
		FIG_prism[0].P2.y = COPY_prism[1][1];
		FIG_prism[0].P3.x = COPY_prism[2][0];
		FIG_prism[0].P3.y = COPY_prism[2][1];
		FIG_prism[0].P4.x = COPY_prism[3][0];
		FIG_prism[0].P4.y = COPY_prism[3][1];

		FIG_prism[1].P1.x = COPY_prism[4][0];
		FIG_prism[1].P1.y = COPY_prism[4][1];
		FIG_prism[1].P2.x = COPY_prism[5][0];
		FIG_prism[1].P2.y = COPY_prism[5][1];
		FIG_prism[1].P3.x = COPY_prism[6][0];
		FIG_prism[1].P3.y = COPY_prism[6][1];
		FIG_prism[1].P4.x = COPY_prism[7][0];
		FIG_prism[1].P4.y = COPY_prism[7][1];

		FIG_prism[2].P1.x = COPY_prism[0][0];
		FIG_prism[2].P1.y = COPY_prism[0][1];
		FIG_prism[2].P2.x = COPY_prism[1][0];
		FIG_prism[2].P2.y = COPY_prism[1][1];
		FIG_prism[2].P3.x = COPY_prism[5][0];
		FIG_prism[2].P3.y = COPY_prism[5][1];
		FIG_prism[2].P4.x = COPY_prism[4][0];
		FIG_prism[2].P4.y = COPY_prism[4][1];

		FIG_prism[3].P1.x = COPY_prism[2][0];
		FIG_prism[3].P1.y = COPY_prism[2][1];
		FIG_prism[3].P2.x = COPY_prism[3][0];
		FIG_prism[3].P2.y = COPY_prism[3][1];
		FIG_prism[3].P3.x = COPY_prism[7][0];
		FIG_prism[3].P3.y = COPY_prism[7][1];
		FIG_prism[3].P4.x = COPY_prism[6][0];
		FIG_prism[3].P4.y = COPY_prism[6][1];

		FIG_prism[4].P1.x = COPY_prism[1][0];
		FIG_prism[4].P1.y = COPY_prism[1][1];
		FIG_prism[4].P2.x = COPY_prism[2][0];
		FIG_prism[4].P2.y = COPY_prism[2][1];
		FIG_prism[4].P3.x = COPY_prism[6][0];
		FIG_prism[4].P3.y = COPY_prism[6][1];
		FIG_prism[4].P4.x = COPY_prism[5][0];
		FIG_prism[4].P4.y = COPY_prism[5][1];

		FIG_prism[5].P1.x = COPY_prism[4][0];
		FIG_prism[5].P1.y = COPY_prism[4][1];
		FIG_prism[5].P2.x = COPY_prism[0][0];
		FIG_prism[5].P2.y = COPY_prism[0][1];
		FIG_prism[5].P3.x = COPY_prism[3][0];
		FIG_prism[5].P3.y = COPY_prism[3][1];
		FIG_prism[5].P4.x = COPY_prism[7][0];
		FIG_prism[5].P4.y = COPY_prism[7][1];
		
		for (int q = 0; q < 6; q++)
		{
			lilfig_prism[0][0] = FIG_prism[q].P1.x;
			lilfig_prism[0][1] = FIG_prism[q].P1.y;

			lilfig_prism[1][0] = FIG_prism[q].P2.x;
			lilfig_prism[1][1] = FIG_prism[q].P2.y;

			lilfig_prism[2][0] = FIG_prism[q].P3.x;
			lilfig_prism[2][1] = FIG_prism[q].P3.y;

			lilfig_prism[3][0] = FIG_prism[q].P4.x;
			lilfig_prism[3][1] = FIG_prism[q].P4.y;

			PolyScan(hdc, lilfig_prism);

			Line(hdc, lilfig_prism[0][0], lilfig_prism[0][1], lilfig_prism[1][0], lilfig_prism[1][1]);
			Line(hdc, lilfig_prism[1][0], lilfig_prism[1][1], lilfig_prism[2][0], lilfig_prism[2][1]);
			Line(hdc, lilfig_prism[2][0], lilfig_prism[2][1], lilfig_prism[3][0], lilfig_prism[3][1]);
			Line(hdc, lilfig_prism[3][0], lilfig_prism[3][1], lilfig_prism[0][0], lilfig_prism[0][1]);
		}

		// параллелограм 
		Line(hdc, COPY_paral[0][0], COPY_paral[0][1], COPY_paral[1][0], COPY_paral[1][1]);
		Line(hdc, COPY_paral[1][0], COPY_paral[1][1], COPY_paral[2][0], COPY_paral[2][1]);
		Line(hdc, COPY_paral[2][0], COPY_paral[2][1], COPY_paral[3][0], COPY_paral[3][1]);
		Line(hdc, COPY_paral[3][0], COPY_paral[3][1], COPY_paral[0][0], COPY_paral[0][1]);

		Line(hdc, COPY_paral[4][0], COPY_paral[4][1], COPY_paral[5][0], COPY_paral[5][1]);
		Line(hdc, COPY_paral[5][0], COPY_paral[5][1], COPY_paral[6][0], COPY_paral[6][1]);
		Line(hdc, COPY_paral[6][0], COPY_paral[6][1], COPY_paral[7][0], COPY_paral[7][1]);
		Line(hdc, COPY_paral[7][0], COPY_paral[7][1], COPY_paral[4][0], COPY_paral[4][1]);


		Line(hdc, COPY_paral[0][0], COPY_paral[0][1], COPY_paral[4][0], COPY_paral[4][1]);
		Line(hdc, COPY_paral[1][0], COPY_paral[1][1], COPY_paral[5][0], COPY_paral[5][1]);

		Line(hdc, COPY_paral[2][0], COPY_paral[2][1], COPY_paral[6][0], COPY_paral[6][1]);
		Line(hdc, COPY_paral[3][0], COPY_paral[3][1], COPY_paral[7][0], COPY_paral[7][1]);

		FIG_paral[0].P1.x = COPY_paral[0][0];
		FIG_paral[0].P1.y = COPY_paral[0][1];
		FIG_paral[0].P1.z = paral[0][2];
		FIG_paral[0].P2.x = COPY_paral[1][0];
		FIG_paral[0].P2.y = COPY_paral[1][1];
		FIG_paral[0].P2.z = paral[1][2];
		FIG_paral[0].P3.x = COPY_paral[2][0];
		FIG_paral[0].P3.y = COPY_paral[2][1];
		FIG_paral[0].P3.z = paral[2][2];
		FIG_paral[0].P4.x = COPY_paral[3][0];
		FIG_paral[0].P4.y = COPY_paral[3][1];
		FIG_paral[0].P4.z = paral[3][2];
		FIG_paral[0].clr = rgbBlue;

		FIG_paral[1].P1.x = COPY_paral[3][0];
		FIG_paral[1].P1.y = COPY_paral[3][1];
		FIG_paral[1].P1.z = paral[3][2];
		FIG_paral[1].P2.x = COPY_paral[2][0];
		FIG_paral[1].P2.y = COPY_paral[2][1];
		FIG_paral[1].P2.z = paral[2][2];
		FIG_paral[1].P3.x = COPY_paral[6][0];
		FIG_paral[1].P3.y = COPY_paral[6][1];
		FIG_paral[1].P3.z = paral[6][2];
		FIG_paral[1].P4.x = COPY_paral[7][0];
		FIG_paral[1].P4.y = COPY_paral[7][1];
		FIG_paral[1].P4.z = paral[7][2];
		FIG_paral[1].clr = rgbGreen;

		FIG_paral[2].P1.x = COPY_paral[7][0];
		FIG_paral[2].P1.y = COPY_paral[7][1];
		FIG_paral[2].P1.z = paral[7][2];
		FIG_paral[2].P2.x = COPY_paral[6][0];
		FIG_paral[2].P2.y = COPY_paral[6][1];
		FIG_paral[2].P2.z = paral[6][2];
		FIG_paral[2].P3.x = COPY_paral[5][0];
		FIG_paral[2].P3.y = COPY_paral[5][1];
		FIG_paral[2].P3.z = paral[5][2];
		FIG_paral[2].P4.x = COPY_paral[4][0];
		FIG_paral[2].P4.y = COPY_paral[4][1];
		FIG_paral[2].P4.z = paral[4][2];
		FIG_paral[2].clr = rgbRed;

		FIG_paral[3].P1.x = COPY_paral[5][0];
		FIG_paral[3].P1.y = COPY_paral[5][1];
		FIG_paral[3].P1.z = paral[5][2];
		FIG_paral[3].P2.x = COPY_paral[6][0];
		FIG_paral[3].P2.y = COPY_paral[6][1];
		FIG_paral[3].P2.z = paral[6][2];
		FIG_paral[3].P3.x = COPY_paral[2][0];
		FIG_paral[3].P3.y = COPY_paral[2][1];
		FIG_paral[3].P3.z = paral[2][2];
		FIG_paral[3].P4.x = COPY_paral[1][0];
		FIG_paral[3].P4.y = COPY_paral[1][1];
		FIG_paral[3].P4.z = paral[1][2];
		FIG_paral[3].clr = rgbOlive;

		FIG_paral[4].P1.x = COPY_paral[5][0];
		FIG_paral[4].P1.y = COPY_paral[5][1];
		FIG_paral[4].P1.z = paral[5][2];
		FIG_paral[4].P2.x = COPY_paral[4][0];
		FIG_paral[4].P2.y = COPY_paral[4][1];
		FIG_paral[4].P2.z = paral[4][2];
		FIG_paral[4].P3.x = COPY_paral[0][0];
		FIG_paral[4].P3.y = COPY_paral[0][1];
		FIG_paral[4].P3.z = paral[0][2];
		FIG_paral[4].P4.x = COPY_paral[1][0];
		FIG_paral[4].P4.y = COPY_paral[1][1];
		FIG_paral[4].P4.z = paral[1][2];
		FIG_paral[4].clr = rgbCyan;

		FIG_paral[5].P1.x = COPY_paral[4][0];
		FIG_paral[5].P1.y = COPY_paral[4][1];
		FIG_paral[5].P1.z = paral[4][2];
		FIG_paral[5].P2.x = COPY_paral[7][0];
		FIG_paral[5].P2.y = COPY_paral[7][1];
		FIG_paral[5].P2.z = paral[7][2];
		FIG_paral[5].P3.x = COPY_paral[3][0];
		FIG_paral[5].P3.y = COPY_paral[3][1];
		FIG_paral[5].P3.z = paral[3][2];
		FIG_paral[5].P4.x = COPY_paral[0][0];
		FIG_paral[5].P4.y = COPY_paral[0][1];
		FIG_paral[5].P4.z = paral[0][2];
		FIG_paral[5].clr = rgbMagenta;

		// ищем среднее значение удаленности для граней
		for (int q = 0; q < 6; q++)
		{
			FIG_paral[q].mid_z = (FIG_paral[q].P1.z + FIG_paral[q].P2.z + FIG_paral[q].P3.z + FIG_paral[q].P4.z) / 4;
		}

		for (int q = 0; q < 5; q++)
		{
			for (int v = 0; v < 5 - q; v++)
			{
				if (FIG_paral[v].mid_z > FIG_paral[v + 1].mid_z)
				{
					square TMP_pyramid = FIG_paral[v];
					FIG_paral[v] = FIG_paral[v + 1];
					FIG_paral[v + 1] = TMP_pyramid;
				}
			}
		}

		for (int q = 0; q < 6; q++)
		{
			lilfig_paral[0][0] = FIG_paral[q].P1.x;
			lilfig_paral[0][1] = FIG_paral[q].P1.y;

			lilfig_paral[1][0] = FIG_paral[q].P2.x;
			lilfig_paral[1][1] = FIG_paral[q].P2.y;

			lilfig_paral[2][0] = FIG_paral[q].P3.x;
			lilfig_paral[2][1] = FIG_paral[q].P3.y;

			lilfig_paral[3][0] = FIG_paral[q].P4.x;
			lilfig_paral[3][1] = FIG_paral[q].P4.y;

			PolyScan(hdc, lilfig_paral, FIG_paral[q].clr);

		}


		for (int i = 0; i < M; i++)
		{

			COPY_paral[i][0] = COPY_paral[i][0] + (0.50f * 800 - COPY_paral[i][1]) * (COPY_paral[i][0] - light.x) / (COPY_paral[i][1] - light.y);

			COPY_paral[i][2] = paral[i][2] + (0.50f * 800 - COPY_paral[i][1]) * (COPY_paral[i][2] - light.z) / (COPY_paral[i][1] - light.y);

			COPY_paral[i][1] = 0.90f * 800 - COPY_paral[i][2] * 0.8f;
		}

		FIG_paral[0].P1.x = COPY_paral[0][0];
		FIG_paral[0].P1.y = COPY_paral[0][1];
		FIG_paral[0].P2.x = COPY_paral[1][0];
		FIG_paral[0].P2.y = COPY_paral[1][1];
		FIG_paral[0].P3.x = COPY_paral[2][0];
		FIG_paral[0].P3.y = COPY_paral[2][1];
		FIG_paral[0].P4.x = COPY_paral[3][0];
		FIG_paral[0].P4.y = COPY_paral[3][1];

		FIG_paral[1].P1.x = COPY_paral[4][0];
		FIG_paral[1].P1.y = COPY_paral[4][1];
		FIG_paral[1].P2.x = COPY_paral[5][0];
		FIG_paral[1].P2.y = COPY_paral[5][1];
		FIG_paral[1].P3.x = COPY_paral[6][0];
		FIG_paral[1].P3.y = COPY_paral[6][1];
		FIG_paral[1].P4.x = COPY_paral[7][0];
		FIG_paral[1].P4.y = COPY_paral[7][1];

		FIG_paral[2].P1.x = COPY_paral[0][0];
		FIG_paral[2].P1.y = COPY_paral[0][1];
		FIG_paral[2].P2.x = COPY_paral[1][0];
		FIG_paral[2].P2.y = COPY_paral[1][1];
		FIG_paral[2].P3.x = COPY_paral[5][0];
		FIG_paral[2].P3.y = COPY_paral[5][1];
		FIG_paral[2].P4.x = COPY_paral[4][0];
		FIG_paral[2].P4.y = COPY_paral[4][1];

		FIG_paral[3].P1.x = COPY_paral[2][0];
		FIG_paral[3].P1.y = COPY_paral[2][1];
		FIG_paral[3].P2.x = COPY_paral[3][0];
		FIG_paral[3].P2.y = COPY_paral[3][1];
		FIG_paral[3].P3.x = COPY_paral[7][0];
		FIG_paral[3].P3.y = COPY_paral[7][1];
		FIG_paral[3].P4.x = COPY_paral[6][0];
		FIG_paral[3].P4.y = COPY_paral[6][1];

		FIG_paral[4].P1.x = COPY_paral[1][0];
		FIG_paral[4].P1.y = COPY_paral[1][1];
		FIG_paral[4].P2.x = COPY_paral[2][0];
		FIG_paral[4].P2.y = COPY_paral[2][1];
		FIG_paral[4].P3.x = COPY_paral[6][0];
		FIG_paral[4].P3.y = COPY_paral[6][1];
		FIG_paral[4].P4.x = COPY_paral[5][0];
		FIG_paral[4].P4.y = COPY_paral[5][1];

		FIG_paral[5].P1.x = COPY_paral[4][0];
		FIG_paral[5].P1.y = COPY_paral[4][1];
		FIG_paral[5].P2.x = COPY_paral[0][0];
		FIG_paral[5].P2.y = COPY_paral[0][1];
		FIG_paral[5].P3.x = COPY_paral[3][0];
		FIG_paral[5].P3.y = COPY_paral[3][1];
		FIG_paral[5].P4.x = COPY_paral[7][0];
		FIG_paral[5].P4.y = COPY_paral[7][1];

		for (int q = 0; q < 6; q++)
		{
			lilfig_paral[0][0] = FIG_paral[q].P1.x;
			lilfig_paral[0][1] = FIG_paral[q].P1.y;

			lilfig_paral[1][0] = FIG_paral[q].P2.x;
			lilfig_paral[1][1] = FIG_paral[q].P2.y;

			lilfig_paral[2][0] = FIG_paral[q].P3.x;
			lilfig_paral[2][1] = FIG_paral[q].P3.y;

			lilfig_paral[3][0] = FIG_paral[q].P4.x;
			lilfig_paral[3][1] = FIG_paral[q].P4.y;

			PolyScan(hdc, lilfig_paral);

			Line(hdc, lilfig_paral[0][0], lilfig_paral[0][1], lilfig_paral[1][0], lilfig_paral[1][1]);
			Line(hdc, lilfig_paral[1][0], lilfig_paral[1][1], lilfig_paral[2][0], lilfig_paral[2][1]);
			Line(hdc, lilfig_paral[2][0], lilfig_paral[2][1], lilfig_paral[3][0], lilfig_paral[3][1]);
			Line(hdc, lilfig_paral[3][0], lilfig_paral[3][1], lilfig_paral[0][0], lilfig_paral[0][1]);
		}

		//заканчиваем рисовать
		ValidateRect(hWnd, NULL); // обновляем окно
		EndPaint(hWnd, &ps); // закончили рисовать
		break;

	case WM_KEYDOWN: // Обработка нажатия клавиши
		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		switch (wParam)
		{
		case VK_LEFT: // Стрелка влево
			move(prism, -5);
			move(paral, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_RIGHT: //  Стрелка вправо
			move(prism, 5);
			move(paral, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_UP: // Стрелка вверх
			move(prism, 0, -5);
			move(paral, 0, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_DOWN: // Стрелка вниз
			move(prism, 0, 5);
			move(paral, 0, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0xba: //  ;-уменьшение
			scale(prism, 0.9);
			scale(paral, 0.9);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0xde: // '-увеличение
			scale(prism, 1.1);
			scale(paral, 1.1);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x58: //X
			rotateX(prism, angle);
			rotateX(paral, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x59: // Y
			rotateY(prism, angle);
			rotateY(paral, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x5A: //  Z
			rotateZ(prism, angle);
			rotateZ(paral, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		default:
			break;
		}


		EndPaint(hWnd, &ps);

		break;

	case WM_DESTROY: // сообщение выхода - разрушение окна
		PostQuitMessage(0); // Посылаем сообщение выхода с кодом 0 - нормальное завершение
		break;

	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam)); //очищаем очередь приложения
	}
	return 0;
}



void mul_matrix(double fig[M][N], double mass[N][N]) // ф-ия перемножения матриц
{
	double res[M][N] = { 0,0,0,0 };
	for (int k = 0; k < M; k++)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res[k][i] += fig[k][j] * mass[j][i];
			}
		}
	}
	for (int k = 0; k < M; k++)
	{
		for (int i = 0; i < N; i++)
		{
			fig[k][i] = res[k][i];
		}
	}
}

double aver(double fig[M][N], int cnt)
{
	double average = 0;
	for (int i = 0; i < 4; i++)
	{
		average += fig[i][cnt];
	}
	return average / 4;
}


void move(double fig[M][N], int dx, int dy, int dz)
{
	double mover[N][N] = { {1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {0, 0, 1, 0},
						 {dx, dy, dz, 1} };
	mul_matrix(fig, mover);
}



void rotateX(double fig[M][N], double angl)
{
	double y = 0, z = 0;
	y = aver(fig, 1);
	z = aver(fig, 2);
	double Angle[N][N] = { {1, 0, 0, 0},
						   {0 , cos(angl), sin(angl), 0},
						   {0, -sin(angl), cos(angl), 0},
						   {0, y * (1 - cos(angl)) + z * sin(angl), z * (1 - cos(angl)) - y * sin(angl), 1} };
	mul_matrix(fig, Angle);
}



void rotateY(double fig[M][N], double angl)
{
	double x = 0, z = 0;
	x = aver(fig, 0);
	z = aver(fig, 2);
	double Angle[N][N] = { {cos(angl), 0, -sin(angl), 0},
						   {0, 1, 0, 0},
						   {sin(angl), 0, cos(angl), 0},
						   {x * (1 - cos(angl)) - z * sin(angl), 0, z * (1 - cos(angl)) + x * sin(angl), 1} };
	mul_matrix(fig, Angle);
}



void rotateZ(double fig[M][N], double angl)
{
	double x = 0, y = 0;
	x = aver(fig, 0);
	y = aver(fig, 1);
	double Angle[N][N] = { {cos(angl), sin(angl), 0, 0},
						   { -sin(angl), cos(angl), 0, 0},
						   {0, 0, 1, 0},
						   {x * (1 - cos(angl)) + y * sin(angl), y * (1 - cos(angl)) - x * sin(angl), 0, 1} };
	mul_matrix(fig, Angle);
}


void scale(double fig[M][N], double S)
{
	double x = 0, y = 0, z = 0;
	x = aver(fig, 0);
	y = (fig, 1);
	z = aver(fig, 2);
	double	Sx_Sy[N][N] = { {S,0,0,0},
			  {0,S,0,0},
			  {0,0,S,0},
			  {x * (1 - S),y * (1 - S),z * (1 - S),1} };
	mul_matrix(fig, Sx_Sy);
}
