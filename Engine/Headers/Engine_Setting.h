#pragma once

//#define _LEAKCHECK // 릭 체크 하고 싶을때만 켜둘 것

#define _KJW

#define _KJY

#define _KMH

#define _KOH

#define _OSM

#define _PSH


#ifdef _DEBUG
#ifdef _LEAKCHECK
#include "vld.h"
#endif  // _LEAKCHECK  
#endif  // _DEBUG  

