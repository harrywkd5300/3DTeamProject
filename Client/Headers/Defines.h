#pragma once

#include "../Default/stdafx.h"
#include "../../Reference/Headers/Engine_Setting.h"

using namespace std;

#ifdef _DEBUG

#include <stdlib.h> 

#endif

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")
#pragma comment (lib, "dxguid.lib")

#ifdef _DEBUG
#ifdef _LEAKCHECK
//#include "vld.h"
//#pragma comment(linker,"/entry:wWinMainCRTStartup /subsystem:console")
#endif  // _LEAKCHECK  
#endif  // _DEBUG 


#include "Constant.h"


extern HWND g_hWnd;
extern HINSTANCE g_hInst;