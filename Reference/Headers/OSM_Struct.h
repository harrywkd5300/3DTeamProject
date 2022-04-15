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
		SPEED_CHANGE,      // ���ǵ� ü���� -> �ִϸ��̼� ���ǵ� ����
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
			_tchar    EventName[64];      // �̺�Ʈ ���� �±� or ��Ʈ��
		};
		
		
		double   fAniPosition = 0.f;   // �̺�Ʈ ���� �ִϸ��̼� ������
		float   fEventSpeed = 0.f;   // �ӵ� ��ȭ �̺�Ʈ�� ������
		Event_Signal pEvnetArr = EVENT_SIGNAL_NONE;      // �̺�Ʈ �ñ׳�8
	}ANI_EVENT;

	typedef struct Ani_Status
	{
		int iAni_Index = 0;               // ���� �ִϸ��̼�
		float fAni_Speed = 1.5f;         // �ش� �ִϸ��̼� ���ǵ�
		double fPeriod_Minus = 0.25f;
		bool bRoot_Move = true;
		int iArrSize = 0;               // ���� �迭 ������
		tag_AniEvent* m_AniEvent = nullptr; // ���� �迭
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