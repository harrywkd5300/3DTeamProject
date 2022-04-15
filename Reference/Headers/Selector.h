#pragma once

#include "Engine_Defines.h"
#include "CompositeTask.h"

BEGIN(Engine)

class DLL_EXPORT CSelector : public CCompositeTask {
protected:
	class DoSelect
	{
	protected:
		void *m_Agent;

	public:
		DoSelect(void *agent) : m_Agent(agent) {}

		bool operator() (CBaseTask::Ptr &task)
		{
			return (*task)(m_Agent);
		}
	};

	virtual bool operator()(void* agent)
	{
		//>>> 최초의 AI 실행에 성공한 자식 테스크가 나타날 경우 순회를 멈추고 true 리턴  
		//>>> 모든 자식이 실패했을 경우 false 리턴  
		if (std::find_if(m_ChildList.begin(), m_ChildList.end(), DoSelect(agent)) == m_ChildList.end())
			return false;

		return true;
	};

protected:
	CSelector();
	virtual ~CSelector();

	virtual void OnInitialize() {};
	virtual Status Update() = 0;
	virtual void OnTerminate(Status m_Status) {};

public:
	virtual void Release() = 0;
	Status Tick(_float fTimeDelta);

	virtual std::string Name() = 0;
	virtual void AddChild(CBaseTask* Child) {};
	typedef shared_ptr<CSelector> Ptr;


};

END