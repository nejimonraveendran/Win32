#include<stdio.h>
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM setupWindow();

//To run as standalone window without console, change Project properties -> Linker -> System -> Sub System -> Windows (/SUBSYSTEM:WINDOWS)
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	return setupWindow(hInstance);
}

//For console support while while coding/debugging
//To make it work, change Project properties -> Linker -> System -> Sub System -> Windows (/SUBSYSTEM:CONSOLE)
int main()
{
    wprintf(L"this is a test\n");
	return setupWindow(GetModuleHandleW(0));
}


WPARAM setupWindow(HMODULE hModule) {
	LPCWSTR szAppName = L"HelloWin";
	MSG msg;
	WNDCLASSW wndclass;
	HMODULE hInstance = hModule;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;


	if (!RegisterClass(&wndclass))
	{
		MessageBoxW(NULL, L"Failed to register window class", szAppName, MB_ICONERROR | MB_OK);
		return 0;
	}

	HWND hwnd = CreateWindowW(szAppName, // window class name
		L"The Hello Program", // window caption
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, // initial x position
		CW_USEDEFAULT, // initial y position
		CW_USEDEFAULT, // initial x size
		CW_USEDEFAULT, // initial y size
		NULL, // parent window handle
		NULL, // window menu handle
		hInstance, // program instance handle
		NULL); // creation parameters

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
		case WM_CREATE:
			wprintf(L"window created\n");
			break;
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			GetClientRect(hwnd, &rect);
			DrawTextW(hdc, L"Hello, Windows", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			EndPaint(hwnd, &ps);
			break;
		case WM_MOVE:
			wprintf(L"window moved\n");
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
