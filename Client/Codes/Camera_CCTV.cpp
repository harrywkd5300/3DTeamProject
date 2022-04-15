#include "stdafx.h"
#include "..\Headers\Camera_CCTV.h"


#include "Component_Manager.h"
#include "Input_Device.h"
#include "Camera_Manager.h"

#include "CameraObserver.h"
#include "Subject_Manager.h"

CCamera_CCTV::CCamera_CCTV(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCamera(pGraphicDev)
{
}
HRESULT CCamera_CCTV::Set_Target(CTransform * pTargetTransform)
{
	if (nullptr == pTargetTransform)
		return E_FAIL;

	if (m_pTargetTransform != nullptr)
		Safe_Release(m_pTargetTransform);

	m_pTargetTransform = m_pTargetTransform;

	m_pTargetTransform->AddRef();

	return NOERROR;
}

HRESULT CCamera_CCTV::Ready_Component()
{
	return NOERROR;
}
HRESULT CCamera_CCTV::Ready_Observer()
{
	m_pObserver = CCameraObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}

HRESULT CCamera_CCTV::Ready_GameObject(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	if (FAILED(Engine::CCamera::Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;

	return NOERROR;
}

_int CCamera_CCTV::Update_GameObject(const _float & fTimeDelta)
{
	_int iExit = 0;

	if (nullptr == m_pInput_Device)
		return -1;
	
	m_pTargetTransform = m_pObserver->Get_Current_PlayerTransform();

	if (m_pTargetTransform != nullptr)
	{
		_vec3 vPos = *m_pTargetTransform->Get_Infomation(CTransform::INFO_POSITION);

		_vec3 vUp = { 0.f,1.f,0.f };

		m_vRelative = vUp * m_fRelative;

		vPos += m_vRelative;

		m_vAt = vPos;
	}

	if (CInput_Device::GetInstance()->Get_DownMouseState(CInput_Device::DIM_MBUTTON)) //디버그 전용 카메라
	{
		CCamera_Manager::GetInstance()->Set_IsChangeCamera(false);
		CCamera_Manager::GetInstance()->Set_CurrentCamera(this);
	}
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_LCONTROL)) //디버그 전용 카메라
	{
		if (CInput_Device::GetInstance()->Get_DownMouseState(CInput_Device::DIM_MBUTTON)) //디버그 전용 카메라
		{
			CCamera_Manager::GetInstance()->Set_IsChangeCamera(true);
		}
		
	}

	iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	if (m_bIsDead == true)
		iExit = 1;

	return iExit;
}


CCamera_CCTV * CCamera_CCTV::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CCamera_CCTV *	pInstance = new CCamera_CCTV(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
	{
		MSG_BOX("CCamera_CCTV Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CCamera_CCTV::Free(void)
{	
	Safe_Release(m_pObserver);
	//Safe_Release(m_pTargetTransform);
	return Engine::CCamera::Free();
}
