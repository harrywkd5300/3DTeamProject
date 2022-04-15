#pragma once

#include "Defines.h"
#include "Controller.h"


class CAIController : public CController
{
protected:
	explicit CAIController(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAIController(const CAIController& rhs);
	virtual ~CAIController() = default;
public:
	HRESULT Ready_Controller(void);
	virtual void Update_Component(const _float& fTimeDelta);
	virtual void LastUpdate_Component(const _float& fTimeDelta);
	
public:
	static CAIController* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
private:
	CInput_Device*	m_pInput = nullptr;



public: 
	virtual void Possess(CCharacter *pTarget) final; // 빙의할 캐릭터를 설정 해주는 함수
	virtual bool UnPossess() final; // 캐릭터 설정 해제
public:
	//virtual EStatus Attack_Forward_Dash();
	//virtual EStatus Move_Forward();
	//virtual EStatus Move_Right();


	//MOVE_FORWARD
	//MOVE_RIGHT
	//TURN
	//LOOKUP
	//JUMP
	//PARKOUR
	
	// ---- 디버그 용 ----
private:
	float	fPlanTime = 0.f; // 플랜 함수 수행시간
	int		m_iCurAniNum = 0;

	// ---- 디버그 용 ----
#ifndef _FINAL
public:
	void Debug_Render();
#endif

	virtual _ulong Free(void);
};
