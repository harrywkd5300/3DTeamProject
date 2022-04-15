#pragma once

#include "Controller.h"

BEGIN(Engine)


class DLL_EXPORT CPlayerController : public CController
{
protected:
	explicit CPlayerController(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CPlayerController(const CPlayerController& rhs);
	virtual ~CPlayerController() = default;
public:
	HRESULT Ready_Controller(void);
	virtual void Update_Component(const _float& fTimeDelta);
	virtual void LastUpdate_Component(const _float& fTimeDelta);
	
public:
	static CPlayerController* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
	virtual _ulong Free(void);

public:
	//MOVE_FORWARD
	//MOVE_RIGHT
	//TURN
	//LOOKUP
	//JUMP
	//PARKOUR


};

END