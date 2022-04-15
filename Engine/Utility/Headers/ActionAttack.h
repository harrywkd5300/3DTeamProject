#pragma once

#include "Engine_Defines.h"
#include "BaseTask.h"

BEGIN(Engine)

class DLL_EXPORT CActionAttack : public CBaseTask{
protected:
	BTList m_ChildList;
	virtual bool operator()(void* agent);

protected:
	CActionAttack();
	virtual ~CActionAttack();

	virtual void OnInitialize() {};
	virtual Status Update() = 0;
	virtual void OnTerminate(Status m_Status) {};

public:
	virtual void Release() = 0;
	Status Tick(_float fTimeDelta);

	virtual std::string Name() = 0;
	virtual void AddChild(CBaseTask* Child) {};
	typedef shared_ptr<CActionAttack> Ptr;


};

END