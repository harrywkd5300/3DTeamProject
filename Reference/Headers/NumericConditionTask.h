#pragma once

#include "Engine_Defines.h"
#include "BaseTask.h"

BEGIN(Engine)

template <class T>
class DLL_EXPORT CNumericConditionTask : public CBaseTask {

public:
	CNumericConditionTask(bool(T::*func)(), bool check)
		: m_Func(func), m_Check(check)
	{}
	CNumericConditionTask(bool(T::* const func)() const, bool check)
		: m_Func(reinterpret_cast<bool(T::* const)()>(func)), m_Check(check)
	{}

	virtual bool operator()(void* agent)
	{
		T* obj = (T*)agent;
		if ((obj->*m_Func)() == m_Check)
			return true;
		else
			return false;
	};

protected:
	virtual void OnInitialize() {};
	virtual Status Update() = 0;
	virtual void OnTerminate(Status Status) {};
	virtual EStatus Update() = 0;
	virtual void OnTerminate(EStatus Status) {};
	virtual EStatus Update() = 0;
	virtual void OnTerminate(EStatus EStatus) {};

public:
	virtual void Release() = 0;
	Status Tick(_float fTimeDelta);
	FORCEINLINE Status GetStatus() { return Status; }
	EStatus Tick(_float fTimeDelta);
	FORCEINLINE EStatus GetStatus() { return Status; }
	EStatus Tick(_float fTimeDelta);
	FORCEINLINE EStatus GetStatus() { return EStatus; }

	FORCEINLINE void Reset() { Status = Status::Invalid; }
	FORCEINLINE void Abort() { OnTerminate(Status::Aborted); Status = Status::Aborted; }
	FORCEINLINE void Reset() { Status = EStatus::Invalid; }
	FORCEINLINE void Abort() { OnTerminate(EStatus::Aborted); Status = EStatus::Aborted; }
	FORCEINLINE void Reset() { EStatus = EStatus::Invalid; }
	FORCEINLINE void Abort() { OnTerminate(EStatus::Aborted); EStatus = EStatus::Aborted; }

	FORCEINLINE bool IsTerminate() { return Status == Status::Success || Status == Status::Failure; }
	FORCEINLINE bool IsRunning() { return Status == Status::Running; }
	FORCEINLINE bool IsSuccess() { return Status == Status::Success; }
	FORCEINLINE bool IsFailuer() { return Status == Status::Failure; }
	FORCEINLINE bool IsTerminate() { return Status == EStatus::Success || Status == EStatus::Failure; }
	FORCEINLINE bool IsRunning() { return Status == EStatus::Running; }
	FORCEINLINE bool IsSuccess() { return Status == EStatus::Success; }
	FORCEINLINE bool IsFailuer() { return Status == EStatus::Failure; }
	FORCEINLINE bool IsTerminate() { return EStatus == EStatus::Success || EStatus == EStatus::Failure; }
	FORCEINLINE bool IsRunning() { return EStatus == EStatus::Running; }
	FORCEINLINE bool IsSuccess() { return EStatus == EStatus::Success; }
	FORCEINLINE bool IsFailuer() { return EStatus == EStatus::Failure; }

	virtual std::string Name() = 0;
	virtual void AddChild(CBaseTask* Child) {};

};

END