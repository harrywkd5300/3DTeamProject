// Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.


//#include <vld.h>

#include "stdafx.h"
#include "Client.h"

#include "../../Reference/Headers/Engine_Defines.h"

//#include "vld.h"

#include "../Headers/MainApp.h"

#include "Timer.h"
#include "Timer_Manager.h"

#include "Frame.h"
#include "Frame_Manager.h"

#include "Input_Device.h"
#include "Debug_Manager.h"




#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE g_hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
HWND g_hWnd;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.
	CMainApp*		pMainApp = nullptr;

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	pMainApp = CMainApp::Create();
	if (nullptr == pMainApp)
		return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;
	
	CTimer_Manager*		pTimer_Manager = CTimer_Manager::GetInstance();
	pTimer_Manager->AddRef();

	CTimer* pTimer = nullptr;

	// for.TimerDefault
	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return FALSE;
	if(FAILED(pTimer_Manager->Add_Timer(L"Timer_Default", pTimer)))
		return FALSE;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return FALSE;
	if (FAILED(pTimer_Manager->Add_Timer(L"Timer_60", pTimer)))
		return FALSE;
	//pTimer_Manager->Set_MainGameTimer(L"Timer_60");

	CFrame_Manager*	pFrame_Manager = CFrame_Manager::GetInstance();
	pFrame_Manager->AddRef();
	
	// For.Frame:60	
	if (FAILED(pFrame_Manager->Add_Frame(L"Frame_60", 60.f)))
		return FALSE;
	

	Engine::CInput_Device*		pInput_Device = Engine::CInput_Device::GetInstance();
	if (nullptr == pInput_Device)
		return FALSE;
	if (FAILED(pInput_Device->Ready_Input_Device(g_hInst, g_hWnd)))
		return FALSE;


    // �⺻ �޽��� �����Դϴ�.
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{			
			if (nullptr == pMainApp
				|| nullptr == pTimer_Manager
				|| nullptr == pInput_Device)
				break;

			pTimer_Manager->Compute_Timer(L"Timer_Default");
			_float fTimeDelta_Default = pTimer_Manager->Get_TimeDelta(L"Timer_Default");

#ifndef _FINAL
			CDebug::GetInstance()->Set_SystemTimeDelta(fTimeDelta_Default);
			CDebug::GetInstance()->Update_GameSpeed();
			fTimeDelta_Default = fTimeDelta_Default * CDebug::GetInstance()->Get_GameUpdateTime();
			CDebug::GetInstance()->Update_DebugList();
#endif
			
			if (pFrame_Manager->Permit_Call(L"Frame_60", fTimeDelta_Default))
			{
				pTimer_Manager->Compute_Timer(L"Timer_60");
				_float fTimeDelta = pTimer_Manager->Get_TimeDelta(L"Timer_60");				
#ifndef _FINAL
				fTimeDelta = fTimeDelta * CDebug::GetInstance()->Get_GameUpdateTime();
				CDebug::GetInstance()->Set_GameTimeDelta(fTimeDelta);
				CDebug::GetInstance()->BeginUpdate();
#endif
				pInput_Device->Check_InputState();
				pInput_Device->Set_TimeDelta(fTimeDelta);

				pMainApp->Update_MainApp(fTimeDelta);
				pMainApp->Render_MainApp();
#ifndef _FINAL
				CDebug::GetInstance()->EndUpdate();
#endif
			}
#ifndef _FINAL
				CDebug::GetInstance()->Render_DebugList();
#endif
		}
	}

	Engine::_ulong dwRefCnt = 0;

	Engine::Safe_Release(pTimer_Manager);
	Engine::Safe_Release(pFrame_Manager);
	

	if (dwRefCnt = Engine::Safe_Release(pMainApp))
	{
		MSG_BOX("pMainApp Release Failed");
		return FALSE;
	}
    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.


   RECT		rcWindow = { 0, 0, BACKSIZEX, BACKSIZEY };

   AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, TRUE);


   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP | WS_VISIBLE,
	   CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

   

   if (!hWnd)
   {
      return FALSE;
   }

   g_hWnd = hWnd;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	//case WM_KEYDOWN:
	//	if (wParam == VK_ESCAPE)
	//	{
	//		DestroyWindow(hWnd);
	//		break;
	//	}
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
