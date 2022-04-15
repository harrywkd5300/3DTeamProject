
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
	virtual void Update(int iMessage, CSubject_Manager::SUBJECTTYPE eType,void* pData); // Notify �� ȣ��Ǹ� ����� Update�Լ�.
	
private:
	virtual HRESULT Ready_Observer(void); // ������ �غ��Ұ��� �ִٸ� 

private:
	// ������ ������ ������ �� 
	// Ŭ������ �����Ű�� �ȴٸ� ������ Ŭ������ �����Ǿ������� �������� �����غ��ƾ� �� �� ����.	

private: // Current Player Info
	const CHARINFO*	m_pCurrent_CharInfo = nullptr; 
	PLAYER_TYPE m_eCurrent_Type;
	_vec3		m_vCurrent_CharPos = {0.f,0.f,0.f};
	_matrix		m_matCurrent_PlayerWorld;
	_matrix     m_matCurrent_CameraWorld;
	_matrix     m_matCurrent_CameraInvView;
	CWorldState* m_pCurrent_WorldState = nullptr; //����Ʈ�� ������������ ������ ���ؾ��Ѵ�.

private: // Player Info // 4����
	_matrix m_pPlayerWorldmatrix[4];
	_bool		m_IsPlayerDead[4];
		

private:
	_int		m_iPizzaCount = 0;
	_int		m_iShurikenCount = 0;
	//ī�޶� Ÿ�Ժ��� ������ �ٸ��⶧���� (Relative,Distance ��) ������ �غ��ƾ��ҰͰ���.
	
	//CPlayer* pPlayer; 

	

public:
	static CUIObserver* Create();

public:
	virtual _ulong Free(); // ������ ���� ��ü���� ���ٰ�.
};

