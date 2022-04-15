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
	virtual void Update(int iMessage, CSubject_Manager::SUBJECTTYPE eType, void* pData); // Notify 가 호출되면 실행될 Update함수.

private:
	virtual HRESULT Ready_Observer(void); // 생성시 준비할것이 있다면 

private:
	// 데이터 변수를 저장할 곳 

	// 클래스를 저장시키게 된다면 참조한 클래스가 삭제되었을때의 문제점을 생각해보아야 할 것 같다.
		
	CTransform* m_pCurrent_Transform = nullptr; //포인트를 받은순간부터 관리를 잘해야한다.

										  //카메라 타입별로 정보가 다르기때문에 (Relative,Distance 등) 생각을 해보아야할것같다.

										  //CPlayer* pPlayer; 	
	
	const CWorldState* m_pWorldState[4]; //포인트를 받은순간부터 관리를 잘해야한다.        

	//SHAKINGINFO m_tShakingInfo;

public:
	static CCameraObserver* Create();

public:
	virtual _ulong Free(); // 삭제는 각자 객체에서 해줄것.
};

