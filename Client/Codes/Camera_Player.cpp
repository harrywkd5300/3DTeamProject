#include "stdafx.h"
#include "..\Headers\Camera_Player.h"
#include "Component_Manager.h"
#include "Subject_Manager.h"
#include "CameraObserver.h"

CCamera_Player::CCamera_Player(LPDIRECT3DDEVICE9 pGraphicDev)
	:CCamera(pGraphicDev)
	,m_fCamSpeed(10.f)
	,m_vAngle(0.f,0.f,0.f)	
	, m_vCurAngle(0.f,0.f,0.f)
	,m_fDistance(1.2f)
    ,m_vRelative(0.f,0.f,0.f)
    ,m_vEndEye(0.f, 0.f, 0.f)
    ,m_vEndAt(0.f, 0.f, 0.f)  
    ,m_IsDebugMode(false)
    ,m_eCurState(MODE_END)
    ,m_ePreState(MODE_PLAYER)   
    ,m_fInverseY(1.f)
    ,m_fMouse_SensitivityX(5.f)
    ,m_fMouse_SensitivityY(5.f)
	,m_vMoveLook(0.f,0.f,0.f)
	,m_pSubject_Manager(CSubject_Manager::GetInstance())
{
#ifndef _FINAL
	Tag(L"CCamera_Player", std::bind(&CCamera_Player::Debug_Render, this));
#endif

	//ZeroMemory(&m_tShakingInfo,sizeof(SHAKINGINFO));
	m_pSubject_Manager->AddRef();
}
HRESULT CCamera_Player::Set_Player(CTransform* pPlayerTransform)
{
	if (nullptr == pPlayerTransform)
		return E_FAIL;

	if (m_pPlayerTransform != nullptr)	
		Safe_Release(m_pPlayerTransform);
			
	m_pPlayerTransform = pPlayerTransform;

	m_pPlayerTransform->AddRef();

	return NOERROR;
}
HRESULT CCamera_Player::Ready_Observer()
{
	m_pObserver = CCameraObserver::Create();
	CSubject_Manager::GetInstance()->AddObserver(m_pObserver, CSubject_Manager::TYPE_STATIC);

	return NOERROR;
}
HRESULT CCamera_Player::Ready_GameObject(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	if (FAILED(Engine::CCamera::Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
		return E_FAIL;

	if (FAILED(Ready_Observer()))
		return E_FAIL;

	m_eCurState = MODE_PLAYER;

	/*m_pSubject_Manager->AddData(MESSAGE_CAMERA_DATA, CSubject_Manager::TYPE_STATIC, this);
	m_pSubject_Manager->Notify(MESSAGE_CAMERA_DATA, CSubject_Manager::TYPE_STATIC, this);*/

	CSubject_Manager::GetInstance()->AddData(MESSAGE_CAMERA_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);
	return NOERROR;
}

//CTriger(함수포인터,충돌속성);
//함수포인터();
//CTriger(std::bind(Set_CurCamera, 카메라주소), Player);


_int CCamera_Player::Update_GameObject(const _float & fTimeDelta)
{

	int iExit = 0;

	if (nullptr == m_pInput_Device)
		return -1;

	if(m_IsMouseFix == true)
		Fix_MousePointer(BACKSIZEX >> 1, BACKSIZEY >> 1);
	
#ifndef _FINAL
	if (m_pInput_Device->Get_DownKeyState(DIK_O)) //디버그 전용 카메라
	{
		m_IsDebugMode = !m_IsDebugMode;

		if (m_IsDebugMode == true)
			m_eCurState = MODE_DEBUG;
		else
			m_eCurState = MODE_PLAYER;
	}

	if (m_pInput_Device->Get_DownKeyState(DIK_P)) //디버그 전용 카메라
	{
		m_IsMouseFix = !m_IsMouseFix;
	}

	
	//if (m_pInput_Device->Get_DIKeyState(DIK_LCONTROL))
	//{
	//	if (m_pInput_Device->Get_DownKeyState(DIK_K))
	//	{
	//		m_tShakingInfo.fShakingHeight += 1000;
	//	}
	//	if (m_pInput_Device->Get_DownKeyState(DIK_L))
	//	{
	//		m_tShakingInfo.fShakingHeight -= 1000;
	//	}
	//	if (m_pInput_Device->Get_DownKeyState(DIK_H))
	//	{
	//		m_tShakingInfo.fShakingHeightX += 1000;
	//	}
	//	if (m_pInput_Device->Get_DownKeyState(DIK_J))
	//	{
	//		m_tShakingInfo.fShakingHeightX -= 1000;
	//	}
	//	if (m_pInput_Device->Get_DownKeyState(DIK_N))
	//	{
	//		m_tShakingInfo.fShakingRange += 0.001f;
	//	}
	//	if (m_pInput_Device->Get_DownKeyState(DIK_M))
	//	{
	//		m_tShakingInfo.fShakingRange -= 0.001f;
	//	}
	//}


	//_float fVariation = (sin(-13 * D3DX_PI * 0.5f * (m_tShakingInfo.fShakingTime + 1)) * pow(2, -10 * m_tShakingInfo.fShakingTime) - 0.05f);
	//
	if (m_pInput_Device->Get_DIKeyState(DIK_LCONTROL))
	{
		if (m_pInput_Device->Get_DownKeyState(DIK_K))
		{
			m_fA -= 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_L))
		{
			m_fA += 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_H))
		{
			m_fB -= 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_J))
		{
			m_fB += 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_N))
		{
			m_fTimeA -= 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_M))
		{
			m_fTimeA += 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_V))
		{
			m_fC -= 0.1f;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_B))
		{
			m_fC += 0.1f;
		}
	}


#endif 
	
	//Check_State(); //현재모드 Check

	//if (m_bOperate == true) // 야매 -> 나중에 수정해야함.
	//	m_eCurState = MODE_PLAYER;
	//if (m_bOperate == false)
	//	m_eCurState = MODE_NOTPLAY; 


	Update_Mode(fTimeDelta); // 모드별 이동 변화량 

	if (m_IsDebugMode != true)
		Check_InputState(fTimeDelta); // Key입력
	else
		DebugCamera_InputState(fTimeDelta);
	
	iExit = Engine::CCamera::Update_GameObject(fTimeDelta);

	m_vDir = m_vEndAt - m_vEndEye;
	m_vDir_NotY = m_vDir;
	m_vDir_NotY.y = 0;

	//if(m_bOperate == true)
	//m_pSubject_Manager->Notify(MESSAGE_CAMERA_DATA, CSubject_Manager::TYPE_STATIC, this);

	return iExit;	
}

_int CCamera_Player::LastUpdate_GameObject(const _float & fTimeDelta)
{	
	return CCamera::LastUpdate_GameObject(fTimeDelta);
}

void CCamera_Player::DebugCamera_InputState(const _float & fTimeDelta)
{	

	if (m_pInput_Device->Get_DIKeyState(DIK_E))
	{
		m_vAt = *m_pPlayerTransform->Get_Infomation(CTransform::INFO_POSITION);

		_vec3 vLook = *m_pPlayerTransform->Get_WorldLookVec();

		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye = m_vAt + vLook * m_fDistance ;
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_W))
	{
		// 카메라의 월드행렬
		_vec3			vLook;
		memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));
		_vec3		vMove = *D3DXVec3Normalize(&vLook, &vLook) * m_fCamSpeed * fTimeDelta;
		m_vEye += vMove;
		m_vAt += vMove;
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_S))
	{
		// 카메라의 월드행렬
		_vec3			vLook;
		memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));
		_vec3		vMove = *D3DXVec3Normalize(&vLook, &vLook) * -m_fCamSpeed * fTimeDelta;
		m_vEye += vMove;
		m_vAt += vMove;
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_A))
	{
		// 카메라의 월드행렬
		_vec3			vRight;
		memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

		_vec3		vMove = *D3DXVec3Normalize(&vRight, &vRight) * -m_fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if (m_pInput_Device->Get_DIKeyState(DIK_D))
	{
		// 카메라의 월드행렬
		_vec3			vRight;
		memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

		_vec3		vMove = *D3DXVec3Normalize(&vRight, &vRight) * m_fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	_long		 Move = 0;

	_vec3		vLook;
	memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));

	if (Move = m_pInput_Device->Get_DIMouseMove(Engine::CInput_Device::DIMM_Y))
	{
		_matrix			matRot;

		_vec3			vRight;

		memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

		D3DXMatrixRotationAxis(&matRot, &vRight, Move * 0.1f * fTimeDelta);

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}

	if (Move = m_pInput_Device->Get_DIMouseMove(Engine::CInput_Device::DIMM_X))
	{
		_matrix			matRot;

		_vec3			vUp(0.f, 1.f, 0.f);

		memcpy(&vUp, &m_matWorld.m[1][0], sizeof(_vec3));

		D3DXMatrixRotationAxis(&matRot, &vUp, Move * 0.1f * fTimeDelta);

		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vAt = m_vEye + vLook;
	}
}
void CCamera_Player::Check_InputState(const _float & fTimeDelta)
{	
	if (m_pInput_Device->Get_DownKeyState(DIK_Q))
	{
		m_tShakingInfo.IsShaking = true;
	}
	

	if (m_eCurState == MODE_PLAYER)
	{
		_long		 Move = 0;	

		if (Move = m_pInput_Device->Get_DIMouseMove(Engine::CInput_Device::DIMM_Y))
		{	
			m_vAngle.x += Move * fTimeDelta * m_fMouse_SensitivityY * m_fInverseY;
		}
		if (Move = m_pInput_Device->Get_DIMouseMove(Engine::CInput_Device::DIMM_X))
		{					
			m_fAngleSpeed = Move * fTimeDelta * m_fMouse_SensitivityX;

			if (m_fAngleSpeed > 30.f)
				m_fAngleSpeed = 30.f;
			else if (m_fAngleSpeed < -30.f)
				m_fAngleSpeed = -30.f;
			
			m_vAngle.y += m_fAngleSpeed;
		}			


		if (m_vAngle.x > 50.f)
			m_vAngle.x = 50.f;
		else if(m_vAngle.x < -10.f)
			m_vAngle.x = -10.f;


		D3DXVec3Lerp(&m_vCurAngle, &m_vCurAngle, &m_vAngle, fTimeDelta * 10.f);

		
		if (m_pInput_Device->Get_DownKeyState(DIK_LBRACKET))
		{
			m_fMouse_SensitivityX--;
			m_fMouse_SensitivityY--;
		}
		if (m_pInput_Device->Get_DownKeyState(DIK_RBRACKET))
		{
			m_fMouse_SensitivityX--;
			m_fMouse_SensitivityY++;
		}

	}
}
void CCamera_Player::Fix_MousePointer(const _uint & iX, const _uint & iY)
{
	POINT		pt = { (long)iX, (long)iY };

	ClientToScreen(g_hWnd, &pt);

	SetCursorPos(pt.x, pt.y);

	ShowCursor(false);
}

void CCamera_Player::Check_State()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
			case MODE_PLAYER:			
				break;
			case MODE_NOTPLAY:
				break;
			case MODE_UI:
				break;
		}

		m_ePreState = m_eCurState;
	}
}

void CCamera_Player::Update_Mode(const _float & fTimeDelta)
{
	switch (m_eCurState)
	{
	case CCamera_Player::MODE_PLAYER:
		Player_Mode(fTimeDelta);
		break;
	case CCamera_Player::MODE_NOTPLAY:
		NotPlay_Mode(fTimeDelta);
		break;
	case CCamera_Player::MODE_UI:
		UI_Mode(fTimeDelta);
		break;
	}
	
	if(m_IsDebugMode == false)
	Move_Camera(fTimeDelta);
		
	if (true == m_tShakingInfo.IsShaking)
		Shake_Mode(fTimeDelta);
	else
		m_tShakingInfo.fShakingTime = 0;

}
void CCamera_Player::Shake_Mode(const _float& fTimeDelta)
{
	if (nullptr == m_pPlayerTransform)
		return;
		
	//_float fVariation = (sin(-13 * D3DX_PI * 0.5f * (m_tShakingInfo.fShakingTime + 1)) * pow(2, -10 * m_tShakingInfo.fShakingTime) - 0.05f) / 2;

	_float fVariation = (sin(m_fA * D3DX_PI * 0.5f * (m_tShakingInfo.fShakingTime + m_fTimeA)) * pow(2, m_fB * m_tShakingInfo.fShakingTime)) / 2.f;		
			
	_float fVariationX = sin(D3DXToRadian(m_tShakingInfo.fShakingTime * m_tShakingInfo.fShakingHeightX)) * powf(0.5f, m_tShakingInfo.fShakingTime) * m_tShakingInfo.fShakingRange; //4000 // 9000
			
	m_tShakingInfo.fShakingTime += fTimeDelta;
	 
	_vec3 vUp = *m_pPlayerTransform->Get_WorldUpVec();

	_vec3 vRight = *(_vec3*)&m_matWorld.m[0];
	D3DXVec3Normalize(&vUp, &vUp);
	D3DXVec3Normalize(&vRight, &vRight);		
		
	m_vEye += vUp * fVariation+ vRight * fVariationX;		 // EndEye , EndAt이 잘된다면 변경
	m_vAt += vUp * fVariation+ vRight * -fVariationX;       //m_vAt += vUp * -fVariation + vRight * -fVariationX;

	if (m_tShakingInfo.fShakingTime > m_tShakingInfo.fShakingMaxTime)
	{		
		if (fabsf(fVariation) < fTimeDelta)
			m_tShakingInfo.IsShaking = false;
	}
	
}
void CCamera_Player::Player_Mode(const _float& fTimeDelta)
{	
		// 플레이어와 일정거리 이상 카메라를 떨어뜨린다.
		// 카메라는 플레이어를 중심으로 회전한다. 

	if (nullptr == m_pPlayerTransform)
		return;	

	Set_Distance(fTimeDelta);

	_matrix matRot;
	MATRIX_ROTATIONXYZ(&matRot, m_vCurAngle.x, m_vCurAngle.y, m_vCurAngle.z);

	_vec3 vDir = *(_vec3*)&matRot.m[2][0];	

	_vec3 vTargetPos = *m_pPlayerTransform->Get_Infomation(CTransform::INFO_POSITION);
	_vec3 vTargetUp = *m_pPlayerTransform->Get_WorldUpVec();

	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vTargetUp, &vTargetUp);

	m_vRelative = vTargetUp * m_fRelative;
		
	m_vEndAt = vTargetPos + m_vRelative;
	m_vAtDir = vDir;

}
void CCamera_Player::NotPlay_Mode(const _float & fTimeDelta)
{
	if (nullptr == m_pPlayerTransform)
		return;

	_matrix matRot;
	MATRIX_ROTATIONXYZ(&matRot, m_vAngle.x, m_vAngle.y, m_vAngle.z);
	_vec3 vDir = *(_vec3*)&matRot.m[2][0];
	_vec3 vTargetPos = *m_pPlayerTransform->Get_Infomation(CTransform::INFO_POSITION);
	_vec3 vTargetUp = *m_pPlayerTransform->Get_WorldUpVec();

	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vTargetUp, &vTargetUp);

	m_vRelative = vTargetUp * m_fRelative;
	m_vEndAt = vTargetPos + m_vRelative;
	m_vEndEye = m_vAt - vDir * m_fNotPlayDistance;

	_vec3 vMoveLook = m_vEndAt - m_vEndEye;

	vMoveLook.y = 0;

	D3DXVec3Normalize(&m_vMoveLook, &vMoveLook);
}
void CCamera_Player::UI_Mode(const _float& fTimeDelta)
{

}
void CCamera_Player::Move_Camera(const _float & fTimeDelta)
{		
	_vec3 vMove;
	_vec3 vEyeMove;

	m_vAt = m_vEndAt;
	
	m_vEndEye = m_vEndAt - m_vAtDir * m_fDistance;	// m_fDistance 를 이동할때와 이동하지 않을때 의 차이를 두어야할것같다.
	
	m_vEye = m_vEndEye;

	_vec3 vMoveLook = m_vEndAt - m_vEndEye;  // 수정
	vMoveLook.y = 0;
	D3DXVec3Normalize(&m_vMoveLook, &vMoveLook);
}
void CCamera_Player::Set_Distance(const _float& fTimeDelta)
{
	const CWorldState* pWorldState = m_pObserver->Get_WorldState(m_ePlayerType);

	if (pWorldState == nullptr)
		return;

	if (pWorldState != nullptr)
	{

		if (m_bOperate == false)
		{
			m_fMaxDistance = 2.5f;
			m_fDistance = 2.5f;
		}
		else if (pWorldState->getVariable(CH_STATE_SPRINT))
		{
			m_fMaxDistance = 2.f;
						
			_float fCurDistance = m_fMaxDistance - m_fDistance ;
			
			//m_fDistance += fTimeDelta* 5.f;

			m_fDistance += fTimeDelta * fCurDistance * 5.f;

			if (m_fDistance >= m_fMaxDistance)
				m_fDistance = m_fMaxDistance;
			
		}	
		else
		{
			if (m_fMaxDistance == 2.5f)
			{
				_float fCurDistance = m_fDistance - m_fMinDistance ;

				m_fDistance -= fTimeDelta * fCurDistance * 15.f; // 20.f

				if (m_fDistance <= m_fMinDistance)
				{
					m_fDistance = m_fMinDistance;
					m_fMaxDistance = 1.2f;
				}
					
			}
			else
			{
				m_fMaxDistance = 1.2f;


				_float fCurDistance = m_fDistance - m_fMinDistance;

				m_fDistance -= fTimeDelta * fCurDistance * 2.f;

				if (m_fDistance <= m_fMinDistance)
					m_fDistance = m_fMinDistance;
			}
			
		}
			
	}	

}
CCamera_Player * CCamera_Player::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CCamera_Player *	pInstance = new CCamera_Player(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
	{
		MSG_BOX("CCamera_Player Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CCamera_Player::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CCamera_Player");
	CDebug::GetInstance()->Add_DebugText(m_tShakingInfo.fShakingRange,"m_fShakingRange");
	CDebug::GetInstance()->Add_DebugText(m_tShakingInfo.fShakingHeight, "m_fShakingHeight");
	CDebug::GetInstance()->Add_DebugText(m_tShakingInfo.fShakingHeightX, "m_fShakingHeightX");


	CDebug::GetInstance()->Add_DebugText(m_fA, "m_fA");
	CDebug::GetInstance()->Add_DebugText(m_fB, "m_fB");
	CDebug::GetInstance()->Add_DebugText(m_fC, "m_fC");
	CDebug::GetInstance()->Add_DebugText(m_fTimeA, "m_fTimeA");

}
#endif
_ulong CCamera_Player::Free(void)
{	
	CSubject_Manager::GetInstance()->RemoveData(MESSAGE_CAMERA_DATA, CSubject_Manager::SUBJECTTYPE::TYPE_STATIC, this);

	Safe_Release(m_pObserver);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pSubject_Manager);
	return Engine::CCamera::Free();
}
