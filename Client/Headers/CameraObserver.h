#pragma once

#include "Engine_Defines.h"
#include "Defines.h"
#include "Observer.h"
#include "Character.h"

namespace Engine
{
	class CTransform;
}

class CCameraObserver : public CObserver
{
private:
	explicit CCameraObserver();
	virtual ~CCameraObserver();

public:
	//SHAKINGINFO* Get_ShakingInfo() { return &m_tShakingInfo; }
	CTransform* Get_Current_PlayerTransform() { return m_pCurrent_Transform; }
	const CWorldState* Get_WorldState(PLAYER_TYPE eType) { return m_pWorldState[eType]; }
public:
	virtual void Update(int iMessage, CSubject_Manager::SUBJECTTYPE eType, void* pData); // Notify �� ȣ��Ǹ� ����� Update�Լ�.

private:
	virtual HRESULT Ready_Observer(void); // ������ �غ��Ұ��� �ִٸ� 

private:
	// ������ ������ ������ �� 

	// Ŭ������ �����Ű�� �ȴٸ� ������ Ŭ������ �����Ǿ������� �������� �����غ��ƾ� �� �� ����.
		
	CTransform* m_pCurrent_Transform = nullptr; //����Ʈ�� ������������ ������ ���ؾ��Ѵ�.

										  //ī�޶� Ÿ�Ժ��� ������ �ٸ��⶧���� (Relative,Distance ��) ������ �غ��ƾ��ҰͰ���.

										  //CPlayer* pPlayer; 	
	
	const CWorldState* m_pWorldState[4]; //����Ʈ�� ������������ ������ ���ؾ��Ѵ�.        

	//SHAKINGINFO m_tShakingInfo;

public:
	static CCameraObserver* Create();

public:
	virtual _ulong Free(); // ������ ���� ��ü���� ���ٰ�.
};

