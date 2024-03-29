//#include<stdio.h>
//#include <Windows.h>
//#include <winnt.h>
//#include <atlstr.h>
//
////#include <ShellScalingApi.h>
//
////prototypes
//struct BUTTON {
//	LPCWSTR text;
//	int key_code;
//	RECT rect;
//	COLORREF bk_color;
//};
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//WPARAM setupWindow(HMODULE hModule);
//LRESULT onTouch(HWND hWnd, WPARAM wParam, LPARAM lParam);
//void createButtons(HDC hdc, LPRECT winRect);
//void buttonClicked(int code, BUTTON* btn, HWND hWnd);
//void registerAppBar(HWND hWnd, long left, long top, long width, long height, int pos);
//void unregisterAppBar(HWND hWnd);
//void onMouse(HWND hWnd, int action, WPARAM wParam, LPARAM lParam);
//RECT getTaskBarRect(HWND hWnd);
//
//
////globals
//INPUT _inputs[1] = {};
//int _winWid = 60;
//int BTN_DOWN = 1;
//int BTN_UP = 2;
//int _btnCols = 1;
//
////to add new buttons, define them here.  Also, set the window height to accommodate new buttons
//BUTTON _buttons[] = 
//{ 
//	{ L"🞬", 0},
//	{ L"⎌", VK_OEM_MINUS},
//	{ L"🖵", VK_TAB},
//	{ L"♒", VK_NUMPAD5},
//	{ L"♨", 0x46}, //F key
//
//	//{ L"Redo", 192},
//
//	//{ L"Ctrl", VK_CONTROL},
//	//{ L"Blend", VK_NUMPAD3},
//	//{ L"Pick", VK_MULTIPLY},
//	//{ L"Shft", VK_SHIFT}, 
//	
//
//};		
//
////int _winHgt = (_winWid / _btnCols) * _countof(_buttons);
//
//
////main
////To run as standalone window without console, change Project properties -> Linker -> System -> Sub System -> Windows (/SUBSYSTEM:WINDOWS)
////int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
////{
////	return setupWindow(hInstance);
////}
//
//
////For console support while while coding/debugging
////To make it work, change Project properties -> Linker -> System -> Sub System -> Windows (/SUBSYSTEM:CONSOLE)
//int main()
//{
//	return setupWindow(GetModuleHandleW(0));
//}
//
//
//WPARAM setupWindow(HMODULE hModule) {
//	LPCWSTR szAppName = L"HelloWin";
//	MSG msg;
//	WNDCLASSW wndclass;
//	HMODULE hInstance = hModule;
//
//	wndclass.style = CS_HREDRAW | CS_VREDRAW;
//	wndclass.lpfnWndProc = WndProc;
//	wndclass.cbClsExtra = 0;
//	wndclass.cbWndExtra = 0;
//	wndclass.hInstance = hInstance;
//	wndclass.hIcon = LoadIcon(NULL, IDI_EXCLAMATION);
//	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	wndclass.lpszMenuName = NULL;
//	wndclass.lpszClassName = szAppName;
//
//	if (!RegisterClass(&wndclass))
//	{
//		MessageBoxW(NULL, L"Failed to register window class", szAppName, MB_ICONERROR | MB_OK);
//		return 0;
//	}
//
//	//auto res = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
//	SetProcessDPIAware();
//
//	HWND hDesktop = GetDesktopWindow();
//	RECT desktopRect = {};
//	GetWindowRect(hDesktop, &desktopRect);
//	RECT taskBarRect = getTaskBarRect(hDesktop);
//
//	float dpi = (float) GetDpiForWindow(hDesktop) / 100.0f;
//
//	int taskBarHgt = (taskBarRect.bottom - taskBarRect.top)/dpi;
//	int winLeft = desktopRect.right - _winWid;
//	int winTop = desktopRect.top;
//	int winHgt = desktopRect.bottom - taskBarHgt;
//
//	//HWND hWnd = CreateWindowW(szAppName, L"🠝", WS_SYSMENU | WS_OVERLAPPED, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//	//HWND hWnd = CreateWindowW(szAppName, L"🠝", WS_POPUPWINDOW | WS_VISIBLE, desktopRect.left-1, desktopRect.top, _winWid, desktopRect.bottom, NULL, NULL, hInstance, NULL);
//	//HWND hWnd = CreateWindowW(szAppName, L"🠝", WS_POPUPWINDOW | WS_VISIBLE, desktopRect.right - _winWid, desktopRect.top, _winWid, desktopRect.bottom, NULL, NULL, hInstance, NULL);
//	//createAppBar(hWnd, desktopRect.left, desktopRect.top, _winWid, desktopRect.bottom, ABE_LEFT);
//	//SetWindowPos(hWnd, HWND_TOPMOST, desktopRect.left-0, desktopRect.top, _winWid, desktopRect.bottom, SWP_SHOWWINDOW);
//
//
//	HWND hWnd = CreateWindowW(szAppName, L"🠝", WS_POPUPWINDOW | WS_VISIBLE, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
//	RegisterTouchWindow(hWnd, 0);
//	SetWindowLongW(hWnd, GWL_EXSTYLE, WS_EX_NOACTIVATE | WS_EX_NOACTIVATE);
//
//	registerAppBar(hWnd, winLeft, winTop, desktopRect.right, winHgt, ABE_RIGHT);
//	SetWindowPos(hWnd, HWND_TOPMOST, winLeft, winTop, _winWid, winHgt, SWP_SHOWWINDOW);
//
//	ShowWindow(hWnd, SW_SHOW);
//	UpdateWindow(hWnd);
//
//	while (GetMessageW(&msg, NULL, 0, 0))
//	{
//		TranslateMessage(&msg);
//		DispatchMessage(&msg);
//	}
//
//	return msg.wParam;
//}
//
//
//LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	HDC hdc = {};	
//	PAINTSTRUCT ps;
//	RECT rect;
//
//	switch (message)
//	{
//	case WM_CREATE:
//		break;
//	case WM_PAINT:
//		hdc = BeginPaint(hwnd, &ps);
//		GetClientRect(hwnd, &rect);
//		createButtons(hdc, &rect);
//		EndPaint(hwnd, &ps);
//		break;
//	case WM_LBUTTONDOWN:
//		//SendMessageW(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
//		onMouse(hwnd, WM_LBUTTONDOWN, wParam, lParam);	
//		break;
//	case WM_LBUTTONUP:
//		onMouse(hwnd, WM_LBUTTONUP, wParam, lParam);
//		break;
//	case WM_MOVE:
//		break;
//	//case WM_TOUCH:
//		//onTouch(hwnd, wParam, lParam);
//		//	break;
//	case WM_CLOSE:
//		//if (MessageBoxW(NULL, L"Do you really want to quit?", L"Closing", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
//			DestroyWindow(hwnd);
//		//}
//		break;
//	case WM_DESTROY:
//		unregisterAppBar(hwnd);
//		PostQuitMessage(0);
//		break;
//	default:
//		return DefWindowProc(hwnd, message, wParam, lParam);
//	}
//
//	return 0;
//}
//
//RECT getTaskBarRect(HWND hWnd) {
//	APPBARDATA appBarData = {};
//	appBarData.cbSize = sizeof(appBarData);
//	appBarData.hWnd = hWnd;
//	appBarData.uCallbackMessage = WM_USER;
//
//	SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData);
//	return appBarData.rc;
//}
//
//
//void registerAppBar(HWND hWnd, long left, long top, long width, long height, int pos) {
//	APPBARDATA appBarData = {};
//	appBarData.cbSize = sizeof(appBarData);
//	appBarData.hWnd = hWnd;
//	appBarData.uCallbackMessage = WM_USER;
//
//	// Register the app bar
//	SHAppBarMessage(ABM_NEW, &appBarData);
//
//	// Set the app bar's position and size
//	appBarData.rc.left = left;
//	appBarData.rc.top = top;
//	appBarData.rc.right = width;
//	appBarData.rc.bottom = height;
//
//	// Request an always-on-top, autohide app bar
//	appBarData.uEdge = pos;
//
//	SHAppBarMessage(ABM_SETPOS, &appBarData);
//}
//
//void unregisterAppBar(HWND hWnd)
//{
//	APPBARDATA appBarData;
//	appBarData.cbSize = sizeof(appBarData);
//	appBarData.hWnd = hWnd;
//
//	// Unregister the app bar
//	SHAppBarMessage(ABM_REMOVE, &appBarData);
//}
//
//void createButtons(HDC hdc, LPRECT winRect) {
//	//int btnHgt = winRect->bottom / _countof(_buttons);
//
//	SetTextColor(hdc, COLORREF(0xFFFFFF));
//	SetBkMode(hdc, TRANSPARENT);
//	HFONT hFont = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Impact"));
//	SelectObject(hdc, hFont);
//
//
//	int col = 0;
//	int row = 0;
//
//	int btnWid =  _winWid / _btnCols; // winRect->right / _btnCols;
//	int btnHgt = winRect->bottom / _countof(_buttons); // btnWid;
//
//	for (int i = 0; i < _countof(_buttons); i++)
//	{
//		int btnLeft = 0;
//		int btnRight = 0;
//		int btnTop = 0;
//		int btnBottom = 0;
//
//		
//		if ((i + 1) % _btnCols == 0) { //reached col end
//			btnLeft = btnWid * col;
//			btnRight = btnLeft + btnWid;
//			btnTop = btnHgt * row;
//			btnBottom = btnTop + btnHgt;
//
//			col = 0; //reset col
//			row++; //increment row
//		}
//		else { //not yet reached col end
//			btnLeft = 0;
//			btnRight = btnLeft + btnWid;
//			btnTop = btnHgt * row;
//			btnBottom = btnTop + btnHgt;
//
//			
//			col++; //increment col
//		}
//
//		
//		_buttons[i].rect.left = btnLeft;  //winRect->left;
//		_buttons[i].rect.right = btnRight;  //winRect->right;
//
//		_buttons[i].rect.top = btnTop; // winRect->top + i * btnHgt;
//		_buttons[i].rect.bottom = btnBottom; // _buttons[i].rect.top + btnHgt;
//
//		_buttons[i].bk_color = RGB(10 + i * 20, 0, 0);
//
//
//		HBRUSH brush = CreateSolidBrush(_buttons[i].bk_color);
//		FillRect(hdc, &_buttons[i].rect, brush);
//		DrawTextW(hdc, _buttons[i].text, -1, &_buttons[i].rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//		DeleteObject(brush);
//	}
//
//	DeleteObject(hFont);
//
//	/*for (int i = 0; i < _countof(_buttons); i++)
//	{
//		_buttons[i].rect.left = winRect->left;
//		_buttons[i].rect.right = winRect->right;
//		_buttons[i].rect.top = winRect->top + i * btnHgt;
//		_buttons[i].rect.bottom = _buttons[i].rect.top + btnHgt;
//		_buttons[i].bk_color = RGB(10 + i * 20, 0, 0);
//
//		HBRUSH brush = CreateSolidBrush(_buttons[i].bk_color);
//		FillRect(hdc, &_buttons[i].rect, brush);
//		DrawTextW(hdc, _buttons[i].text, -1, &_buttons[i].rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//		DeleteObject(brush);
//	}*/
//}
//
//void onMouse(HWND hWnd, int action, WPARAM wParam, LPARAM lParam) {
//	POINT p;
//	p.x = LOWORD(lParam);
//	p.y = HIWORD(lParam);
//
//	for (int i = 0; i < _countof(_buttons); i++)
//	{
//		if (p.x > _buttons[i].rect.left && p.x < _buttons[i].rect.right && p.y > _buttons[i].rect.top && p.y < _buttons[i].rect.bottom)
//		{
//			//do something with the button
//			if (action == WM_LBUTTONDOWN)
//				buttonClicked(BTN_DOWN, &_buttons[i], hWnd);
//			else if (action == WM_LBUTTONUP)
//				buttonClicked(BTN_UP, &_buttons[i], hWnd);
//			
//
//			break;
//		}
//	}
//}
//
//LRESULT onTouch(HWND hWnd, WPARAM wParam, LPARAM lParam) {
//	BOOL bHandled = FALSE;
//	UINT cInputs = LOWORD(wParam);
//	PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];
//
//	if (pInputs) {
//		if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
//			for (UINT i = 0; i < cInputs; i++) {
//				TOUCHINPUT ti = pInputs[i];
//				
//				//do something with each touch input entry
//				POINT p;
//				p.x = ti.x / 100; //Windows returns touch coordinates at 100th of coordinates.
//				p.y = ti.y / 100;
//				ScreenToClient(hWnd, &p); //convert screen xy to client xy
//
//
//				for (int j = 0; j < _countof(_buttons); j++)
//				{
//
//					//if (ti.dwFlags & TOUCHEVENTF_DOWN) {
//					//	wprintf(L"btn %d: %d-%d(%d), %d-%d(%d) \n", j, _buttons[j].rect.left, _buttons[j].rect.right, p.x, _buttons[j].rect.top, _buttons[j].rect.bottom, p.y);
//					//}
//					//else if (ti.dwFlags & TOUCHEVENTF_UP)
//					//{
//					//}
//
//
//					if (p.x > _buttons[j].rect.left && p.x < _buttons[j].rect.right && p.y > _buttons[j].rect.top && p.y < _buttons[j].rect.bottom)
//					{
//						if (ti.dwFlags & TOUCHEVENTF_DOWN) {
//							buttonClicked(BTN_DOWN, &_buttons[j], hWnd);
//						}
//						else if (ti.dwFlags & TOUCHEVENTF_UP)
//						{
//							buttonClicked(BTN_UP, &_buttons[j], hWnd);
//						}
//
//						break;
//					}
//				}
//			}
//
//			bHandled = TRUE;
//		}
//		else {
//			/* handle the error here */
//		}
//		delete[] pInputs;
//	}
//	else {
//		/* handle the error here, probably out of memory */
//	}
//	if (bHandled) {
//		// if you handled the message, close the touch input handle and return
//		CloseTouchInputHandle((HTOUCHINPUT)lParam);
//		return 0;
//	}
//	else {
//		// if you didn't handle the message, let DefWindowProc handle it
//		return DefWindowProc(hWnd, WM_TOUCH, wParam, lParam);
//	}
//}
//
//void buttonClicked(int code, BUTTON* btn, HWND hWnd)
//{
//	if (btn->key_code == 0) {
//		SendMessageW(hWnd, WM_CLOSE, 0, 0);
//		return;
//	}
//
//	if (code == BTN_DOWN) {
//		wchar_t caption[20] = L"down - "; //L"🠟 - ";
//		StrCatW(caption, btn->text);
//		SetWindowTextW(hWnd, caption);
//
//		HDC hdc = GetDC(hWnd);
//		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));
//		FillRect(hdc, &btn->rect, brush);
//		DeleteObject(brush);
//		ReleaseDC(hWnd, hdc);
//
//		_inputs[0].type = INPUT_KEYBOARD;
//		_inputs[0].ki.wVk = btn->key_code;
//		_inputs[0].ki.dwFlags = NULL;
//		SendInput(ARRAYSIZE(_inputs), _inputs, sizeof(INPUT));
//	}
//	else if (code == BTN_UP) {
//		wchar_t caption[20] = L"up - "; //L"🠝 - ";
//		StrCatW(caption, btn->text);
//		SetWindowTextW(hWnd, caption);
//
//		RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE);
//
//		_inputs[0].type = INPUT_KEYBOARD;
//		_inputs[0].ki.wVk = btn->key_code;
//		_inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
//		SendInput(ARRAYSIZE(_inputs), _inputs, sizeof(INPUT));
//	}
//}
//
