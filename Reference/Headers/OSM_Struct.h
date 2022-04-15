#pragma once


namespace Engine
{
	enum Event_Signal
	{
		EVENT_SIGNAL_NONE,
		COLLIDER_ON,
		COLLIDER_OFF,
		SOUND_ON,
		SOUND_OFF,
		EFFECT_ON,
		EFFECT_OFF,
		SPEED_CHANGE,      // 스피드 체인지 -> 애니메이션 스피드 변경
		EVENT_SIGNAL_END,
	};


	enum Collider_Type
	{
		CIRCLE,
		AABB,
		OBB
	};


	typedef struct tag_AniEvent
	{
		union
		{
			_tchar    EventName[64];      // 이벤트 구별 태그 or 비트셋
		};
		
		
		double   fAniPosition = 0.f;   // 이벤트 시작 애니메이션 포지션
		float   fEventSpeed = 0.f;   // 속도 변화 이벤트의 변수값
		Event_Signal pEvnetArr = EVENT_SIGNAL_NONE;      // 이벤트 시그널8
	}ANI_EVENT;

	typedef struct Ani_Status
	{
		int iAni_Index = 0;               // 무슨 애니메이션
		float fAni_Speed = 1.5f;         // 해당 애니메이션 스피드
		double fPeriod_Minus = 0.25f;
		bool bRoot_Move = true;
		int iArrSize = 0;               // 동적 배열 사이즈
		tag_AniEvent* m_AniEvent = nullptr; // 동적 배열
	}ANI_STATUS;


	typedef struct Collider_Status
	{
		Collider_Type eType = CIRCLE;
		_vec3	vMin = { 0.f, 0.f, 0.f };
		_vec3	vMax = { 0.f, 0.f, 0.f };
		_vec3	vOffset = { 0.f, 0.f, 0.f };
		_tchar	BoneName[64];
		_tchar	GroupName[64];

	}COLLIDER_STATUS;



}