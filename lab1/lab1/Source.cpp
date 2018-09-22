#include <windows.h>
#include <math.h>

#define A_KEY 65
#define W_KEY 87
#define S_KEY 83
#define D_KEY 68
#define ESC_KEY 27
#define LEFT_KEY 37
#define RIGHT_KEY 39

const int distance = 20;
const int width = 100;
const int height = 100;
int angle = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawRectangle(HWND hWnd, INT16 x, INT16 y, bool keyboard);
void KeyDownHandler(HWND hWnd, WPARAM wParam, LPARAM lParam);
void MouseWheelHandler(HWND hWnd, WPARAM wParam, LPARAM lParam);
void Rotate(HWND hWnd, INT16 x, INT16 y, bool right);

HDC hDc;
HBRUSH hBrush;
HDC sourceDC;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex; HWND hWnd; MSG msg;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "Lab1Class";
	wcex.hIconSm = wcex.hIcon;

	RegisterClassEx(&wcex);

	hWnd = CreateWindow("Lab1Class", "Lab1", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	sourceDC = CreateCompatibleDC(GetDC(hWnd));
	RECT rect;
	GetClientRect(hWnd, &rect);
	HBITMAP bm = CreateCompatibleBitmap(GetDC(hWnd), rect.right, rect.bottom);
	SelectObject(sourceDC, bm);
	Rectangle(sourceDC, 0, 0, 50, 50);
	

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		DrawRectangle(hWnd, LOWORD(lParam), HIWORD(lParam), false);
		break;
	case WM_KEYDOWN:
		KeyDownHandler(hWnd, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		MouseWheelHandler(hWnd, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DrawRectangle(HWND hWnd, INT16 x, INT16 y, bool keyboard)
{
	static RECT rect = { 0, 0, 0, 0 };
	InvalidateRect(hWnd, &rect, true);
	UpdateWindow(hWnd);	

	if (keyboard)
	{
		x += rect.left;
		y += rect.top;
	}

	rect = { x, y, x + width, y + height };
	Rectangle(hDc, rect.left, rect.top, rect.right, rect.bottom);
}

void KeyDownHandler(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case A_KEY:
		DrawRectangle(hWnd, -distance, 0, true);
		break;
	case W_KEY:
		DrawRectangle(hWnd, 0, -distance, true);
		break;
	case S_KEY:
		DrawRectangle(hWnd, 0, distance, true);
		break;
	case D_KEY:
		DrawRectangle(hWnd, distance, 0, true);
		break;
	case LEFT_KEY:
		Rotate(hWnd, LOWORD(lParam), HIWORD(lParam), false);
		break;
	case RIGHT_KEY:
		Rotate(hWnd, LOWORD(lParam), HIWORD(lParam), true);
		break;
	case ESC_KEY:
		if (MessageBox(hWnd, "Exit?", "lab1", MB_YESNO) == IDYES)
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	}
}

void MouseWheelHandler(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	int delta = HIWORD(wParam);
	int key = LOWORD(wParam);
	if (key != MK_SHIFT)
		DrawRectangle(hWnd, 0, -delta, true);
	else
		DrawRectangle(hWnd, delta, 0, true);
}

void Rotate(HWND hWnd, INT16 x, INT16 y, bool right)
{
	if (right)
		angle += 5;
	else
		angle -= 5;
	int x0 = x + width / 2;
	int y0 = y + height / 2;

	HDC destDC = GetDC(hWnd);

	XFORM xfm = { 0.0f };
	xfm.eM11 = (float)cos(angle);
	xfm.eM12 = (float)sin(angle);
	xfm.eM21 = -(float)sin(angle);
	xfm.eM22 = (float)cos(angle);
	xfm.eDx = x;
	xfm.eDy = y;


	SetGraphicsMode(destDC, GM_ADVANCED);
	SetWorldTransform(destDC, &xfm);  
	BitBlt(destDC, 0, 0, 50, 50, sourceDC, 0, 0, SRCCOPY);
}