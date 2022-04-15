#pragma once

#include "Engine_Defines.h"
#include "CompositeTask.h"

BEGIN(Engine)

class DLL_EXPORT CSequence : public CCompositeTask {
protected:
	class DoSequence
	{
	protected:
		void *m_Agent;

	public:
		DoSequence(void *agent) : m_Agent(agent) {}

		bool operator() (CBaseTask::Ptr &task)
		{
			return !(*task)(m_Agent);
		}
	};

	virtual bool operator()(void* agent)
	{
		//>>> �ڽ� �� �ϳ��� ������ ��� false ����    
		//>>> ��� �ڽ��� ���� ����  ��� true ����    
		if (std::find_if(m_ChildList.begin(), m_ChildList.end(), DoSequence(agent)) != m_ChildList.end())
			return false;

		return true;
	};

protected:
	CSequence();
	virtual ~CSequence();

	virtual void OnInitialize() {};
	virtual Status Update() = 0;
	virtual void OnTerminate(Status m_Status) {};

public:
	virtual void Release() = 0;
	Status Tick(_float fTimeDelta);

	virtual std::string Name() = 0;
	virtual void AddChild(CBaseTask* Child) {};
	typedef shared_ptr<CSequence> Ptr;


};

END