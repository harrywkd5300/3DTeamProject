#include "stdafx.h"
#include "..\Headers\UIObserver.h"
#include "Subject_Manager.h"

#include "Component_Manager.h"

#include "Player.h"
#include "Camera.h"

CUIObserver::CUIObserver()
	:m_vCurrent_CharPos(0.f,0.f,0.f)
	,m_eCurrent_Type(PLAYER_TYPE::PLAYER_None)
	,m_pCurrent_WorldState(nullptr)
{
	D3DXMatrixIdentity(&m_matCurrent_CameraWorld);
	D3DXMatrixIdentity(&m_matCurrent_CameraInvView);

	for (_int i = 0; i < 4; i++)
	{
		D3DXMatrixIdentity(&m_pPlayerWorldmatrix[i]);
		m_IsPlayerDead[i] = false;
	}

}


CUIObserver::~CUIObserver()
{
}

void CUIObserver::Update(int iMessage,CSubject_Manager::SUBJECTTYPE eType,void * pData)
{	

	list<void*>* pDataList = nullptr;
	list<void*>::iterator iter;
	if (MESSAGE_PIZZA_DATA != iMessage || MESSAGE_SHURIKEN_DATA != iMessage)
	{
		pDataList = CSubject_Manager::GetInstance()->GetDataList(iMessage, eType);

		if (pDataList == NULL)
			return;

		iter = find(pDataList->begin(), pDataList->end(), pData);

		if (iter == pDataList->end())
			return;
	}		

	switch (iMessage)
	{
	case MESSAGE_CURRENT_PLAYER_DATA:
		m_pCurrent_CharInfo = &((CPlayer*)(*iter))->Get_Info();
		m_vCurrent_CharPos = *((CPlayer*)(*iter))->Get_Transform()->Get_Infomation(CTransform::INFO_POSITION);
		m_eCurrent_Type = ((CPlayer*)(*iter))->Get_Current_Type();
		m_matCurrent_PlayerWorld = ((CPlayer*)(*iter))->Get_Transform()->Get_WorldMatrix_FORCED();
		break;
	case MESSAGE_PLAYER_DATA:
		m_pPlayerWorldmatrix[((CPlayer*)(*iter))->Get_Current_Type()] = ((CPlayer*)(*iter))->Get_Transform()->Get_WorldMatrix_FORCED();
		m_IsPlayerDead[((CPlayer*)(*iter))->Get_Current_Type()] = ((CPlayer*)(*iter))->Get_IsDead();
		break;
	case MESSAGE_CAMERA_DATA:
		m_matCurrent_CameraWorld = ((CCamera*)(*iter))->Get_WorldMatrix();
		m_matCurrent_CameraInvView = ((CCamera*)(*iter))->Get_InverseViewMatrix();
		break;
	case MESSAGE_STATE_DATA:
		m_pCurrent_WorldState = ((CWorldState*)(*iter));
		break;
	case MESSAGE_PIZZA_DATA:
		m_iPizzaCount += *((_int*)(*iter));
		break;
	case MESSAGE_SHURIKEN_DATA:
		m_iShurikenCount += *((_int*)(*iter));
		break;
	case MESSAGE_COMBO_DATA:

		break;
	}
}

HRESULT CUIObserver::Ready_Observer(void)
{		
	return NOERROR;
}

CUIObserver * CUIObserver::Create()
{
	CUIObserver *	pInstance = new CUIObserver;

	if (FAILED(pInstance->Ready_Observer()))
	{
		MSG_BOX("CUIObserver Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
_ulong CUIObserver::Free()
{
	return CObserver::Free();
}
