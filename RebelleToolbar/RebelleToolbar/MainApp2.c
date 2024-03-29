#include<stdio.h>
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>

//constants
#define _APPBAR_CALLBACK_MSG (WM_USER + 1010)
#define _CONTROLTYPE_BUTTON 100
#define _CONTROLTYPE_SLIDER 101

typedef enum
{
	btnClose = 1,
	btnSize = 2,
	btnUndo = 3,
	btnTablet = 4,
	btnDry = 5,
	btnFastDry = 6,
	btnPaint = 7,
	btnBlend = 8,
	btnErase = 9,
	btnLoadingOpacity = 10,
	btnWaterOiliness = 11,
	btnPressure = 12

} ControlId;

typedef struct 
{
	ControlId id;
	LPCWSTR text;
	UINT controlType; 
	RECT rect;
	COLORREF backColor;
	COLORREF contactColor;
	COLORREF foreColor;
	HWND hWndParent;
	TOOLINFOW toolTip;
	INT btnKeyCode;
	INT sliderUpKeyCode;
	INT sliderDownKeyCode;
} Control; 


//prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM setupWindow();
LRESULT onTouch(HWND hWnd, WPARAM wParam, LPARAM lParam);
void registerAppBar(HWND hWnd, int pos);
void unregisterAppBar(HWND hWnd);
void onMouse(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void onPointer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
RECT getTaskBarRect(HWND hWnd);
void onAppBarCallback(hWnd, message, wParam, lParam);
void buildControls(HWND hwndParent);
void updateControlColor(Control* ctrl, COLORREF color);
void setControlTooltip(Control* ctrl);
void simulateKeyDown(WORD btnKeyCode);
void simulateKeyUp(WORD btnKeyCode);
void depressControl(Control* ctrl);
void releaseControl(Control* ctrl);
void slideControl(Control* ctrl, POINT curPoint);

//globals
INPUT _inputs[1];
int _appBarThickness = 50;
int _appBarPos = ABE_RIGHT;
HINSTANCE _hInstance;
BOOL _isAppBarRegistered = FALSE;
APPBARDATA _appBarData;
HWND _hwndTooltip = NULL;
int _sliderResolution = 10;
POINT _prevSliderPoint;
int _lastPointerDownBtnId = 0;


Control _controls[] =
{
	{.id = btnClose , .text = L"🞬", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = 0, .toolTip = {.lpszText = L"Close", .hwnd = NULL, .uId = 0} },
	
	{.id = btnSize, .text = L"⚪", .controlType = _CONTROLTYPE_SLIDER, .btnKeyCode = -1, .sliderUpKeyCode = VK_OEM_6, .sliderDownKeyCode = VK_OEM_4, .toolTip = {.lpszText = L"Size", .hwnd = NULL, .uId = 0 } },
	{.id = btnLoadingOpacity, .text = L"◑", .controlType = _CONTROLTYPE_SLIDER, .btnKeyCode = -1, .sliderUpKeyCode = VK_OEM_7, .sliderDownKeyCode = VK_OEM_1, .toolTip = {.lpszText = L"Loading/Opacity", .hwnd = NULL, .uId = 0 } },
	{.id = btnWaterOiliness, .text = L"◒", .controlType = _CONTROLTYPE_SLIDER, .btnKeyCode = -1, .sliderUpKeyCode = VK_OEM_PLUS, .sliderDownKeyCode = VK_BACK, .toolTip = {.lpszText = L"Water/Oiliness", .hwnd = NULL, .uId = 0 } },
	{.id = btnPressure, .text = L"◎", .controlType = _CONTROLTYPE_SLIDER, .btnKeyCode = -1, .sliderUpKeyCode = 'U', .sliderDownKeyCode = 'P', .toolTip = {.lpszText = L"Pressure", .hwnd = NULL, .uId = 0 } },

	{.id = btnUndo, .text = L"⎌", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = VK_OEM_MINUS, .toolTip = {.lpszText = L"Undo", .hwnd = NULL, .uId = 0 } },
	{.id = btnTablet, .text = L"🖵", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = VK_TAB, .toolTip = {.lpszText = L"Tablet Mode", .hwnd = NULL, .uId = 0 } },
	{.id = btnDry, .text = L"♒", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = VK_NUMPAD5, .toolTip = {.lpszText = L"Dry", .hwnd = NULL, .uId = 0 } },
	{.id = btnFastDry, .text = L"♨", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = 0x46, .toolTip = {.lpszText = L"Fast Dry", .hwnd = NULL, .uId = 0 } }, //F key
	{.id = btnPaint, .text = L"🖌️", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = VK_DIVIDE, .toolTip = {.lpszText = L"Paint", .hwnd = NULL, .uId = 0 } },
	{.id = btnBlend, .text = L"✍", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = VK_NUMPAD3, .toolTip = {.lpszText = L"Blend", .hwnd = NULL, .uId = 0 } },
	{.id = btnErase, .text = L"◪", .controlType = _CONTROLTYPE_BUTTON, .btnKeyCode = VK_DECIMAL, .toolTip = {.lpszText = L"Erase", .hwnd = NULL, .uId = 0 } },

};


////To make it work, change Project properties -> Linker -> System -> Sub System -> Windows (/SUBSYSTEM:CONSOLE)
int main()
{
	_hInstance = GetModuleHandle(NULL);
	return setupWindow();
}


//To run as standalone window without console, change Project properties -> Linker -> System -> Sub System -> Windows (/SUBSYSTEM:WINDOWS)
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	_hInstance = hInstance;
	return setupWindow();
}


WPARAM setupWindow() {
	LPCWSTR szAppName = L"AppBar";
	MSG msg;
	WNDCLASSW wndclass;
	HMODULE hInstance = _hInstance;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_EXCLAMATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBoxW(NULL, L"Failed to register window class", szAppName, MB_ICONERROR | MB_OK);
		return 0;
	}

	SetProcessDPIAware();

	HWND hWnd = CreateWindowW(szAppName, L"🠝", WS_POPUP | WS_BORDER, 0, 0, 0, 0, NULL, NULL, hInstance, NULL); 
	SetWindowLongW(hWnd, GWL_EXSTYLE, WS_EX_NOACTIVATE );

	RegisterTouchWindow(hWnd, 0);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		registerAppBar(hWnd, _appBarPos);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		buildControls(hWnd);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		onMouse(hWnd, message, wParam, lParam);
		break;
	case WM_POINTERENTER:
	case WM_POINTERDOWN:
	case WM_POINTERUP:
	case WM_POINTERUPDATE:
	case WM_POINTERLEAVE:
		onPointer(hWnd, message, wParam, lParam);
		break;
	case WM_DISPLAYCHANGE:
		unregisterAppBar(hWnd);
		registerAppBar(hWnd, _appBarPos);
		break;
	case WM_CLOSE:
		if (MessageBoxW(hWnd, L"Do you really want to quit?", L"Closing", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES) {
			DestroyWindow(hWnd);
		}
		break;
	case _APPBAR_CALLBACK_MSG:
		onAppBarCallback(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		unregisterAppBar(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

RECT getTaskBarRect(HWND hWnd) {
	APPBARDATA appBarData;
	appBarData.cbSize = sizeof(appBarData);
	appBarData.hWnd = hWnd;
	appBarData.uCallbackMessage = WM_USER;

	SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData);
	return appBarData.rc;
}

void registerAppBar(HWND hWnd, int pos) {
	int screenWid = GetSystemMetrics(SM_CXSCREEN);
	int screenHgt = GetSystemMetrics(SM_CYSCREEN);

	APPBARDATA appBarData;

	appBarData.cbSize = sizeof(appBarData);
	appBarData.hWnd = hWnd;
	appBarData.uCallbackMessage = _APPBAR_CALLBACK_MSG;
	_isAppBarRegistered = SHAppBarMessage(ABM_NEW, &appBarData);

	appBarData.uEdge = pos;
	switch (appBarData.uEdge)
	{
	case ABE_RIGHT:
		appBarData.rc.left = screenWid - _appBarThickness;
		appBarData.rc.right = screenWid;
		appBarData.rc.top = 0;
		appBarData.rc.bottom = screenHgt;
		break;
	case ABE_LEFT:
		appBarData.rc.left = 0;
		appBarData.rc.right = _appBarThickness;
		appBarData.rc.top = 0;
		appBarData.rc.bottom = screenHgt;
		break;
	case ABE_TOP:
		appBarData.rc.left = 0;
		appBarData.rc.right = screenWid;
		appBarData.rc.top = 0;
		appBarData.rc.bottom = _appBarThickness;
		break;
	default:
		break;
	}

	SHAppBarMessage(ABM_QUERYPOS, &appBarData);
	SHAppBarMessage(ABM_SETPOS, &appBarData);
	_appBarData = appBarData;

}

void unregisterAppBar(HWND hWnd)
{
	APPBARDATA appBarData;
	appBarData.cbSize = sizeof(appBarData);
	appBarData.hWnd = hWnd;

	_isAppBarRegistered =  !SHAppBarMessage(ABM_REMOVE, &appBarData);
}

void buildControls(HWND hwndParent)
{
	RECT parentRect;
	GetClientRect(hwndParent, &parentRect);


	for (int i = 0; i < _countof(_controls); i++)
	{
		byte r = (rand() % 50) + 1;
		byte g = (rand() % 50) + 1;
		byte b = (rand() % 50) + 1;

		_controls[i].hWndParent = hwndParent;
		_controls[i].backColor = RGB(r, g, b); // RGB(0, 10 + i * 20, 0);
		_controls[i].foreColor = RGB(255, 255, 255);
		_controls[i].contactColor = RGB(70, 70, 70);

		if (_appBarPos == ABE_LEFT || _appBarPos == ABE_RIGHT) 
		{
			int hgt = parentRect.bottom / _countof(_controls);
			_controls[i].rect.left = parentRect.left;
			_controls[i].rect.right = parentRect.right;
			_controls[i].rect.top = parentRect.top + (i * hgt);
			_controls[i].rect.bottom = _controls[i].rect.top +  hgt;
		}
		else if (_appBarPos == ABE_TOP)
		{
			int wid = parentRect.right / _countof(_controls);
			_controls[i].rect.left = parentRect.left + (i * wid);
			_controls[i].rect.right = _controls[i].rect.left + wid;
			_controls[i].rect.top = parentRect.top;
			_controls[i].rect.bottom = parentRect.bottom;
		}

		updateControlColor(&_controls[i], _controls[i].backColor);
		setControlTooltip(&_controls[i]);

	}
}

void updateControlColor(Control* ctrl, COLORREF color) {
	//set background color
	HDC hdc = GetDC(ctrl->hWndParent);
	HBRUSH hBrush = CreateSolidBrush(color); 
	FillRect(hdc, &ctrl->rect, hBrush);

	//set text font and color
	SetTextColor(hdc, ctrl->foreColor);
	SetBkMode(hdc, TRANSPARENT);
	HFONT hFont = CreateFontW(48, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Impact");
	SelectObject(hdc, hFont);

	//draw text
	DrawTextW(hdc, ctrl->text, -1, &ctrl->rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	DeleteObject(hFont); 
	DeleteObject(hBrush); 
	ReleaseDC(ctrl->hWndParent, hdc);
}



void setControlTooltip(Control* ctrl)
{
	ctrl->toolTip.cbSize = sizeof(TOOLINFOW) - sizeof(void*);
	ctrl->toolTip.uFlags =  TTF_SUBCLASS;
	ctrl->toolTip.hinst = _hInstance;

	//create one tooltip window and reuse for all controls
	if (_hwndTooltip == NULL) {
		_hwndTooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, ctrl->hWndParent, NULL, _hInstance, NULL); 

		SetWindowPos(_hwndTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); 
	}


	if (ctrl->toolTip.hwnd == NULL) //if not set, add new tooltip
	{
		ctrl->toolTip.hwnd = ctrl->hWndParent;
		ctrl->toolTip.rect = ctrl->rect;

		SendMessageW(_hwndTooltip, TTM_ADDTOOLW, 0, (LPARAM)(LPTOOLINFOW)&ctrl->toolTip);
		int delay = 100;
		SendMessageW(_hwndTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELONG(delay, 0));

	}
	else //if set, reposition as needed for each control
	{
		ctrl->toolTip.rect = ctrl->rect;

		//wprintf(L"rect: %d, %d, %d, %d\n", ctrl->rect.left, ctrl->rect.top, ctrl->rect.right, ctrl->rect.bottom);

		SendMessageW(_hwndTooltip, TTM_NEWTOOLRECTW, 0, (LPARAM)(LPTOOLINFOW)&ctrl->toolTip);

	}
	 
}

void onAppBarCallback(hWnd, message, wParam, lParam) {
	APPBARDATA appBarData;
	appBarData.cbSize = sizeof(appBarData);
	appBarData.hWnd = hWnd;

	//APPBARDATA* abd;

	//switch (lParam)
	switch (wParam)
	{
		case ABN_FULLSCREENAPP:
			break;
		case ABN_POSCHANGED:
			SetWindowPos(hWnd, HWND_TOPMOST, _appBarData.rc.left, _appBarData.rc.top,
				_appBarData.uEdge == ABE_LEFT || _appBarData.uEdge == ABE_RIGHT ? _appBarThickness : _appBarData.rc.right, //width
				_appBarData.rc.bottom, SWP_SHOWWINDOW);
			break;
		case ABN_STATECHANGE:
			break;
		case ABN_WINDOWARRANGE:
			break;
		default:
			break;
	}
}

void onMouse(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
	POINT p;
	p.x = GET_X_LPARAM(lParam); 
	p.y = GET_Y_LPARAM(lParam);


	for (int i = 0; i < _countof(_controls); i++)
	{
		if (p.x > _controls[i].rect.left && p.x < _controls[i].rect.right && p.y > _controls[i].rect.top && p.y < _controls[i].rect.bottom)
		{
			if(message == WM_LBUTTONDOWN)
			{
				depressControl(&_controls[i]);
				break;
			}
			
			if (message == WM_LBUTTONUP)
			{
				releaseControl(&_controls[i]);
				break;
			}

		}
	}
}


void onPointer(HWND hWnd, int message, WPARAM wParam, LPARAM lParam) {
		
	POINT p;
	p.x = GET_X_LPARAM(lParam);
	p.y = GET_Y_LPARAM(lParam);
	int x_original = p.x;
	int y_original = p.y;

	ScreenToClient(hWnd, &p); //convert screen xy to client xy

	for (int i = 0; i < _countof(_controls); i++)
	{
		if (p.x > _controls[i].rect.left && p.x < _controls[i].rect.right && p.y > _controls[i].rect.top && p.y < _controls[i].rect.bottom)
		{
			if (message == WM_POINTERENTER)
			{
				SetCursorPos(x_original, y_original);
				SendMessageW(_hwndTooltip, TTM_POPUP, NULL, NULL);
				break;
			}
			
			if (message == WM_POINTERDOWN)
			{
				_lastPointerDownBtnId = _controls[i].id;

				_prevSliderPoint.x = p.x;
				_prevSliderPoint.y = p.y;

				depressControl(&_controls[i]);
				break;
			}

			else if (message == WM_POINTERUP)
			{
				releaseControl(&_controls[i]);
				break;
			}

			if (message == WM_POINTERUPDATE)
			{
				if (IS_POINTER_INCONTACT_WPARAM(wParam)) 
				{
					slideControl(&_controls[i], p);
				}
				
				break;
			}
			
		}
	}
}


void slideControl(Control* ctrl, POINT curPoint) 
{
	if (ctrl->controlType != _CONTROLTYPE_SLIDER) 
	{
		return;
	}


	//SetCursorPos(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

	if (_appBarPos == ABE_LEFT || _appBarPos == ABE_RIGHT)
	{
		//track vertical movement
		if (curPoint.y > _prevSliderPoint.y) { //down
			if (curPoint.y - _prevSliderPoint.y > _sliderResolution)
			{
				//wprintf(L"%s down, y: %d \n", ctrl->toolTip.lpszText, curPoint.y);

				if(ctrl->id == _lastPointerDownBtnId)
				{
					simulateKeyDown(ctrl->sliderDownKeyCode);
					simulateKeyUp(ctrl->sliderDownKeyCode);

					_prevSliderPoint.x = curPoint.x;
					_prevSliderPoint.y = curPoint.y;

				}
			}
		}
		else if (curPoint.y < _prevSliderPoint.y) { //up
			if (_prevSliderPoint.y - curPoint.y > _sliderResolution)
			{
				//wprintf(L"%s up, y: %d \n", ctrl->toolTip.lpszText, curPoint.y);
				if (ctrl->id == _lastPointerDownBtnId)
				{
					simulateKeyDown(ctrl->sliderUpKeyCode);
					simulateKeyUp(ctrl->sliderUpKeyCode);

					_prevSliderPoint.x = curPoint.x;
					_prevSliderPoint.y = curPoint.y;
				}

			}
		}
	}
	else if (_appBarPos == ABE_TOP)
	{
		//track horizontal movement
		if (curPoint.x > _prevSliderPoint.x) { //up

			if (curPoint.x - _prevSliderPoint.x > _sliderResolution)
			{
				//wprintf(L"%s up, x: %d \n", ctrl->toolTip.lpszText, curPoint.x);

				if (ctrl->id == _lastPointerDownBtnId)
				{
					simulateKeyDown(ctrl->sliderUpKeyCode);
					simulateKeyUp(ctrl->sliderUpKeyCode);

					_prevSliderPoint.x = curPoint.x;
					_prevSliderPoint.y = curPoint.y;
				}
			}
		}
		else if (curPoint.x < _prevSliderPoint.x) { //down

			if (_prevSliderPoint.x - curPoint.x > _sliderResolution)
			{
				//wprintf(L"%s down, x: %d \n", ctrl->toolTip.lpszText, curPoint.x);

				if (ctrl->id == _lastPointerDownBtnId)
				{

					simulateKeyDown(ctrl->sliderDownKeyCode);
					simulateKeyUp(ctrl->sliderDownKeyCode);

					_prevSliderPoint.x = curPoint.x;
					_prevSliderPoint.y = curPoint.y;

				}
			}
		}
	}

}

LRESULT onTouch(HWND hWnd, WPARAM wParam, LPARAM lParam) {
	BOOL bHandled = FALSE;
	UINT cInputs = LOWORD(wParam);
	PTOUCHINPUT pInputs = (PTOUCHINPUT) malloc(cInputs * sizeof(TOUCHINPUT));

	if (pInputs) {
		if (GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, sizeof(TOUCHINPUT))) {
			for (UINT i = 0; i < cInputs; i++) {
				TOUCHINPUT ti = pInputs[i];

				//do something with each touch input entry
				POINT p;
				p.x = ti.x / 100; //Windows returns touch coordinates at 100th of coordinates.
				p.y = ti.y / 100;
				ScreenToClient(hWnd, &p); //convert screen xy to client xy


				for (int j = 0; j < _countof(_controls); j++)
				{
					if (p.x > _controls[j].rect.left && p.x < _controls[j].rect.right && p.y > _controls[j].rect.top && p.y < _controls[j].rect.bottom)
					{
						if (ti.dwFlags & TOUCHEVENTF_DOWN) {
							//buttonClicked(_BTN_DOWN, &_buttons[j], hWnd);
						}
						else if (ti.dwFlags & TOUCHEVENTF_UP)
						{
							//buttonClicked(_BTN_UP, &_buttons[j], hWnd);
						}

						break;
					}
				}
			}

			bHandled = TRUE;
		}
		else {
			/* handle the error here */
		}

		free(pInputs);
	}
	else {
		/* handle the error here, probably out of memory */
	}


	if (bHandled) {
		// if you handled the message, close the touch input handle and return
		CloseTouchInputHandle((HTOUCHINPUT)lParam);
		return 0;
	}
	else {
		// if you didn't handle the message, let DefWindowProc handle it
		return DefWindowProc(hWnd, WM_TOUCH, wParam, lParam);
	}
}

void depressControl(Control* ctrl) {
	updateControlColor(ctrl, ctrl->contactColor);

	if (ctrl->id == btnClose) {
		return;
	}

	if (ctrl->controlType == _CONTROLTYPE_BUTTON)
	{
		simulateKeyDown(ctrl->btnKeyCode);
	}

}

void releaseControl(Control* ctrl) {
	updateControlColor(ctrl, ctrl->backColor);

	if (ctrl->id == btnClose) {
		SendMessageW(ctrl->hWndParent, WM_CLOSE, 0, 0);
		return;
	}

	if (ctrl->controlType == _CONTROLTYPE_BUTTON)
	{
		simulateKeyUp(ctrl->btnKeyCode);
	}

}

void simulateKeyDown(WORD btnKeyCode)
{
	INPUT inputs[1];
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = btnKeyCode;
	inputs[0].ki.dwFlags = NULL;
	SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

}

void simulateKeyUp(WORD btnKeyCode)
{
	INPUT inputs[1];
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = btnKeyCode;
	inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

}

void simulateKeyPress(WORD btnKeyCode)
{
	INPUT inputs[4];
	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = VK_LCONTROL; //VK_NUMPAD1;
	inputs[0].ki.dwFlags = NULL;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = VK_NUMPAD1;
	inputs[1].ki.dwFlags = NULL;

	inputs[2].type = INPUT_KEYBOARD;
	inputs[2].ki.wVk = VK_NUMPAD1;
	inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;

	inputs[3].type = INPUT_KEYBOARD;
	inputs[3].ki.wVk = VK_LCONTROL;
	inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

