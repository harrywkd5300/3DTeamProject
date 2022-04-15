
#pragma once

#include "Engine_Defines.h"
#include "Defines.h"
#include "Observer.h"
#include "Character.h"

class CUIObserver : public CObserver
{
private:
	explicit CUIObserver();
	virtual ~CUIObserver();

public: // Current Player
	const CHARINFO*	Get_Current_CharInfo() { return m_pCurrent_CharInfo; }
	PLAYER_TYPE*Get_Current_Type() { return &m_eCurrent_Type; }
	_vec3*		Get_CharPos() { return &m_vCurrent_CharPos; }
	_matrix*	Get_Current_PlayerWorld() { return &m_matCurrent_PlayerWorld; }
	_matrix*	Get_Current_CameraWorldMatrix() { return &m_matCurrent_CameraWorld; }
	_matrix*	Get_Current_CameraInvViewMatrix() { return &m_matCurrent_CameraInvView; }
	CWorldState* Get_Current_WorldState() { return m_pCurrent_WorldState; }

public: // Player
	_matrix* Get_Player_WorldMatrix(PLAYER_TYPE eType) { return &m_pPlayerWorldmatrix[eType]; }
	_bool* Get_PlayerIsDead(PLAYER_TYPE eType) { return &m_IsPlayerDead[eType]; }
	
public:
	virtual void Update(int iMessage, CSubject_Manager::SUBJECTTYPE eType,void* pData); // Notify 가 호출되면 실행될 Update함수.
	
private:
	virtual HRESULT Ready_Observer(void); // 생성시 준비할것이 있다면 

private:
	// 데이터 변수를 저장할 곳 
	// 클래스를 저장시키게 된다면 참조한 클래스가 삭제되었을때의 문제점을 생각해보아야 할 것 같다.	

private: // Current Player Info
	const CHARINFO*	m_pCurrent_CharInfo = nullptr; 
	PLAYER_TYPE m_eCurrent_Type;
	_vec3		m_vCurrent_CharPos = {0.f,0.f,0.f};
	_matrix		m_matCurrent_PlayerWorld;
	_matrix     m_matCurrent_CameraWorld;
	_matrix     m_matCurrent_CameraInvView;
	CWorldState* m_pCurrent_WorldState = nullptr; //포인트를 받은순간부터 관리를 잘해야한다.

private: // Player Info // 4마리
	_matrix m_pPlayerWorldmatrix[4];
	_bool		m_IsPlayerDead[4];
		

private:
	_int		m_iPizzaCount = 0;
	_int		m_iShurikenCount = 0;
	//카메라 타입별로 정보가 다르기때문에 (Relative,Distance 등) 생각을 해보아야할것같다.
	
	//CPlayer* pPlayer; 

	

public:
	static CUIObserver* Create();

public:
	virtual _ulong Free(); // 삭제는 각자 객체에서 해줄것.
};

