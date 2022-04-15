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
	virtual void Possess(CCharacter *pTarget) final; // ������ ĳ���͸� ���� ���ִ� �Լ�
	virtual bool UnPossess() final; // ĳ���� ���� ����
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
	
	// ---- ����� �� ----
private:
	float	fPlanTime = 0.f; // �÷� �Լ� ����ð�
	int		m_iCurAniNum = 0;

	// ---- ����� �� ----
#ifndef _FINAL
public:
	void Debug_Render();
#endif

	virtual _ulong Free(void);
};
