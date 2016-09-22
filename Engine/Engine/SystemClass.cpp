#include "SystemClass.h"

SystemClass::SystemClass() : myStandardScreenWidth(800), myStandardScreenHeight(600)
{
	myInput = nullptr;
	myGraphics = nullptr;
}

SystemClass::SystemClass(const SystemClass& aOther) : myStandardScreenWidth(800), myStandardScreenHeight(600)
{
}

SystemClass::~SystemClass()
{
	/*You will also notice I don't do any object clean up in the class destructor.
	I instead do all my object clean up in the Shutdown function you will see further down.
	The reason being is that I don't trust it to be called.
	Certain windows functions like ExitThread() are known for not calling
	your class destructors resulting in memory leaks.
	You can of course call safer versions of these functions now but
	I'm just being careful when programming on windows.*/
}

bool SystemClass::Initalize()
{
	int screenWidth = 0;
	int screenHeight = 0;
	InitializeWindows(screenWidth, screenHeight);

	myInput = new InputClass;
	myInput->Initialize();

	myGraphics = new GraphicsClass;
	return myGraphics->Initialize(screenWidth, screenHeight, myHwnd);
}

void SystemClass::Shutdown()
{
	if (myGraphics)
	{
		myGraphics->Shutdown();
		delete myGraphics;
		myGraphics = nullptr;
	}

	if (myInput)
	{
		delete myInput;
		myInput = nullptr;
	}

	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done = false;

	SecureZeroMemory(&msg, sizeof(MSG));

	while (!done)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT || !Frame())
		{
			done = true;
		}
	}
}

bool SystemClass::Frame()
{
	if (myInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	return myGraphics->Frame();
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND aHwnd, UINT aUmessage, WPARAM aWparam, LPARAM aLparam)
{
	switch (aUmessage)
	{
	case WM_KEYDOWN:
		myInput->KeyDown((unsigned int)aWparam);
		return 0;
	case WM_KEYUP:
		myInput->KeyUp((unsigned int)aWparam);
		return 0;
	default:
		return DefWindowProc(aHwnd, aUmessage, aWparam, aLparam);
	}
}

void SystemClass::InitializeWindows(int& aScreenWidth, int& aScreenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX;
	int posY;

	ApplicationHandle = this;
	myHInstance = GetModuleHandle(nullptr);
	myApplicationName = L"Engine";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = myHInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = myApplicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	aScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	aScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)aScreenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)aScreenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = 0;
		posY = 0;
	}
	else
	{
		aScreenWidth = myStandardScreenWidth;
		aScreenHeight = myStandardScreenHeight;

		posX = (GetSystemMetrics(SM_CXSCREEN) - aScreenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - aScreenHeight) / 2;
	}

	myHwnd = CreateWindowEx
		(WS_EX_APPWINDOW, myApplicationName, myApplicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, aScreenWidth, aScreenHeight,
			nullptr, nullptr, myHInstance, nullptr);

	ShowWindow(myHwnd, SW_SHOW);
	SetForegroundWindow(myHwnd);
	SetFocus(myHwnd);

	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	DestroyWindow(myHwnd);
	myHwnd = nullptr;

	UnregisterClass(myApplicationName, myHInstance);
	myHInstance = nullptr;

	ApplicationHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND aHwnd, UINT aUmessage, WPARAM aWparam, LPARAM aLparam)
{
	if (aUmessage == WM_DESTROY || aUmessage == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}

	return ApplicationHandle->MessageHandler(aHwnd, aUmessage, aWparam, aLparam);
}