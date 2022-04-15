#pragma once

#include "Engine_Setting.h" // 각종 세팅

#include <stdlib.h> 

#include <windows.h>
#include <stdio.h>

#include <vector>
#include <list>
#include <map>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <functional>
#include <memory>
#include <unordered_map>

#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
//#pragma comment(lib, "dinput8.lib")
//#pragma comment(lib, "dxguid.lib")

#pragma	warning (disable : 4251)
#pragma	warning (disable : 4805)

//#ifdef _DEBUG
//#ifdef _LEAKCHECK
//#include "vld.h"
//#endif  // _LEAKCHECK  
//#endif  // _DEBUG  

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Func.h"
#include "Engine_Functor.h"
#include "Engine_Struct.h"

#include "WorldState_Enum.h"
#include "KJW_Struct.h"
#include "KMH_Struct.h"
#include "KOH_Struct.h"
#include "OSM_Struct.h"
#include "KJY_Struct.h"
#include "PSH_Struct.h"

//#include "Random.h"

using namespace Engine;
using namespace std;

