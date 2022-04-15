#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class DLL_EXPORT CBaseTask {

protected:
	std::function<T> m_Func;

public:
	typedef shared_ptr<CBaseTask> Ptr;
	virtual bool operator()(void* agent) = 0;

protected:
	CBaseTask();
	virtual ~CBaseTask();

	virtual void OnInitialize() {};
	virtual Status Update() = 0;
	virtual void OnTerminate(Status m_Status) {};

public:
	virtual void Release() = 0;
	Status Tick(_float fTimeDelta);
	FORCEINLINE Status GetStatus() { return m_Status; }

	FORCEINLINE void Reset() { m_Status = Status::Invalid; }
	FORCEINLINE void Abort() { OnTerminate(Status::Aborted); m_Status = Status::Aborted; }

	FORCEINLINE bool IsTerminate() { return m_Status == Status::Success || m_Status == Status::Failure; }
	FORCEINLINE bool IsRunning() { return m_Status == Status::Running; }
	FORCEINLINE bool IsSuccess() { return m_Status == Status::Success; }
	FORCEINLINE bool IsFailuer() { return m_Status == Status::Failure; }

	virtual std::string Name() = 0;
	virtual void AddChild(CBaseTask* Child) {};

protected:
	Status m_Status;    
	
};

typedef std::vector<CBaseTask::Ptr> BTList;

END