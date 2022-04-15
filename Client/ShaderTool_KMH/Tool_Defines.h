#pragma once

#include "../Default/stdafx.h"

using namespace std; 

extern HWND g_hWnd;

const unsigned short TOOLCX = 600;
const unsigned short WINCX = 1300;
const unsigned short WINCY = 900;

const float CAMRUNPOWER = 5.f;		// 카메라 런모드시  카메라스피드 * CAMRUNPOWER
const float YSPEEDPLUS = 2.f;		// Y 상승량 증가 감소값..
const float CAMSPEEDPLUS = 10.f;	// 카메라 스피드 변경값..

const unsigned int LIGHTSAVEINTERVAL = 5;	// 라이트 정보를 저장할 간격 ( LIGHTSAVEINTERVAL 개수만큼 생성할 때 마다 저장함 )..

enum SCENEID { SCENE_TOOL, TOOLSCENE_END };
enum LIGHTMODE { LightMode_None, LightMode_CreateLight, LightMode_End };
enum CHARACTERKIND { Character_Donatello, Character_Leronard, Character_Michelangelo, Character_Raphael, Character_KinfeThug, Character_SlimThug, Character_BigThug, Character_FootSoldier, Character_End };

enum MAPKIND { MapKind_Stage1, MapKind_Stage2, MapKind_Stage3, MapKind_BossStage, MapKind_End };