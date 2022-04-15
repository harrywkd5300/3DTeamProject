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
		//>>> ������ AI ���࿡ ������ �ڽ� �׽�ũ�� ��Ÿ�� ��� ��ȸ�� ���߰� true ����  
		//>>> ��� �ڽ��� �������� ��� false ����  
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