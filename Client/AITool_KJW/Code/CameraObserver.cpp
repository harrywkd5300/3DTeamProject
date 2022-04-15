#include "stdafx.h"
#include "..\Headers\CameraObserver.h"

#include "Subject_Manager.h"

#include "Component_Manager.h"

#include "Player.h"
#include "Camera.h"

CCameraObserver::CCameraObserver()
{
	
	for (_int i = 0; i < 4; i++)
	{
		m_pWorldState[i] = nullptr;
	}
	


}


CCameraObserver::~CCameraObserver()
{
}

void CCameraObserver::Update(int iMessage, CSubject_Manager::SUBJECTTYPE eType, void * pData)
{
	list<void*>* pDataList = CSubject_Manager::GetInstance()->GetDataList(iMessage, eType);

	if (pDataList == NULL)
		return;

	list<void*>::iterator iter;
	iter = find(pDataList->begin(), pDataList->end(), pData);

	if (iter == pDataList->end())
		return;

	switch (iMessage)
	{
	case MESSAGE_CURRENT_PLAYER_DATA:
		m_pCurrent_Transform = ((CPlayer*)(*iter))->Get_Transform();
		break;
	case MESSAGE_PLAYER_DATA:
		m_pWorldState[((CPlayer*)(*iter))->Get_Current_Type()] = &((CPlayer*)(*iter))->Get_Info().m_WorldState;		
		break;
	}
}

HRESULT CCameraObserver::Ready_Observer(void)
{

	return NOERROR;
}

CCameraObserver * CCameraObserver::Create()
{
	CCameraObserver *	pInstance = new CCameraObserver;

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX("CCameraObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
_ulong CCameraObserver::Free()
{
	return CObserver::Free();
}
