
#include <cassert>

#include "WindowUtils.h"
#include "D3D.h"
#include "D3DUtil.h"

using namespace std;

//all windows apps have these handles
HINSTANCE ghAppInst = 0;
HWND      ghMainWnd = 0;
bool      gAppPaused = false;
bool      gMinimized = false;
bool      gMaximized = false;
bool      gResizing = false;
std::string gMainWndCaption;
int gClientWidth;
int gClientHeight;

bool GetIsPaused()
{
	return gAppPaused;
}

HINSTANCE GetAppInst()
{
	return ghAppInst;
}

HWND GetMainWnd()
{
	return ghMainWnd;
}

void GetClientExtents(int& width, int& height)
{
	width = gClientWidth;
	height = gClientHeight;
}


LRESULT DefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// sent when the window is activated or deactivated.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			gAppPaused = true;
		}
		else
		{
			gAppPaused = false;
		}
		return 0;

	// sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		gClientWidth = LOWORD(lParam);
		gClientHeight = HIWORD(lParam);
		if (gd3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				gAppPaused = true;
				gMinimized = true;
				gMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				gAppPaused = false;
				gMinimized = false;
				gMaximized = true;
				gpOnResize(gClientWidth, gClientHeight);
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (gMinimized)
				{
					gAppPaused = false;
					gMinimized = false;
					gpOnResize(gClientWidth, gClientHeight);
				}

				// Restoring from maximized state?
				else if (gMaximized)
				{
					gAppPaused = false;
					gMaximized = false;
					gpOnResize(gClientWidth, gClientHeight);
				}
				else if (gResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here, wait for it to finish 
				}
				else 
				{
					// API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					gpOnResize(gClientWidth, gClientHeight);
				}
			}
		}
		return 0;

	// sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		gAppPaused = true;
		gResizing = true;
		return 0;

	// sent when the user releases the resize bars.
	case WM_EXITSIZEMOVE:
		gAppPaused = false;
		gResizing = false;
		gpOnResize(gClientWidth, gClientHeight);
		return 0;

	// sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// sent when a menu is active and the user presses a key
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

	// prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}





bool InitMainWindow(int width, int height, HINSTANCE hInstance, const std::string& appName, WNDPROC mssgHandler)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	assert(mssgHandler);
	gMainWndCaption = appName;
	ghAppInst = hInstance;
	gClientWidth = width;
	gClientHeight = height;

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = mssgHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = ghAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = "D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, gClientWidth, gClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int w = R.right - R.left;
	int h = R.bottom - R.top;

	ghMainWnd = CreateWindow("D3DWndClassName", gMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, 0, 0, ghAppInst, 0);
	if (!ghMainWnd)
	{
		MessageBox(0, "CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(ghMainWnd, SW_SHOW);
	UpdateWindow(ghMainWnd);

	return true;
}

int Run(void(*pUpdate)(float), void(*pRender)(float))
{
	MSG msg = { 0 };
	assert(pUpdate && pRender);

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	double secondsPerCount = 1.0 / (double)countsPerSec;

	float deltaTime = 0;
	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			if (1)//!gAppPaused)
			{
				//keep the app rendering even when not in focus
				__int64 time1;
				QueryPerformanceCounter((LARGE_INTEGER*)&time1);

				if (!gAppPaused)
					pUpdate(deltaTime);
				pRender(deltaTime);

				__int64 time2;
				QueryPerformanceCounter((LARGE_INTEGER*)&time2);
				deltaTime = (float)((time2 - time1)*secondsPerCount);
				if (!gAppPaused)
					AddSecToClock(deltaTime);
				Sleep(0);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

