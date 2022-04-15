#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Subject_Manager.h"

BEGIN(Engine)

class DLL_EXPORT CObserver : public CBase
{
protected:
	explicit CObserver();
	virtual ~CObserver() = default;

public:
	virtual void Update(int iMessage, CSubject_Manager::SUBJECTTYPE eType,void* pData)PURE; //SubjectManager에서 Notifiy시 실행 할 함수
	virtual HRESULT Ready_Observer(void)PURE; // 생성시 준비시킬 변수및 함수
public:
	virtual _ulong Free();

protected:
	CSubject_Manager*		m_pSubject_Manager = nullptr;;
};

END