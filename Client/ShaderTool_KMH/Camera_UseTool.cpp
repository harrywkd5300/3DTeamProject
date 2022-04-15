#include "stdafx.h"
#include "Camera_UseTool.h"

#include "MainFrm.h"
#include "ShaderToolMainView.h"
#include "EditBox.h"
#include "EditSheet.h"
#include "Tool_Light.h"

#include "Input_Device.h"
#include "Component_Manager.h"

CCamera_UseTool::CCamera_UseTool(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
{
}

void CCamera_UseTool::Set_Position( const _vec3 & _vPosition, const _bool & _bIsTeleport )
{
	if ( _bIsTeleport )
	{
		_vec3 vDir = m_vAt - m_vEye;
		m_vEye = _vPosition;
		m_vAt = m_vEye + vDir;
	}
	else
	{
		m_vDestination = _vPosition;
		m_bGoDestination = true;
	}
}

void CCamera_UseTool::Set_CamRunMode( const _bool & _bIsRunMode )
{
	m_bIsRunMode = _bIsRunMode;
}

void CCamera_UseTool::Set_PlayerCamMode( const _bool & _bIsPlayerCamMode )
{
	m_bIsPlayerCamMode = _bIsPlayerCamMode;
}

void CCamera_UseTool::Set_LookTarget( Engine::CGameObject * _pTarget )
{
	if ( nullptr != _pTarget )
		m_pLookTarget = _pTarget;
}

HRESULT CCamera_UseTool::Ready_GameObject(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	//if (FAILED(Engine::CCamera::Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
	//	return E_FAIL;

	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vFixedUp = *pUp;

	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_matView_Old = m_matView;
	m_matProj_Old = m_matProj;

	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &m_vAt, &m_vFixedUp );
	D3DXMatrixPerspectiveFovLH( &m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar );

	return NOERROR;
}

_int CCamera_UseTool::Update_GameObject(const _float & fTimeDelta)
{
	CEditBox* pEditBox = ((CMainFrame*)AfxGetMainWnd())->Get_EditBox();
	if ( nullptr == pEditBox )
		return 0;
	CEditSheet* pEditSheet = pEditBox->Get_EditSheet();
	if ( nullptr == pEditSheet )
		return 0;
	if ( true == pEditSheet->Get_LightTool()->m_bIsSaveOrLoadState )
		return 0;

	if ( m_bIsPlayerCamMode == false )
	{
		Check_InputState( fTimeDelta );

		if ( true == m_bGoDestination )
		{
			_vec3 vDir = m_vAt - m_vEye;
			D3DXVec3Lerp( &m_vEye, &m_vEye, &m_vDestination, fTimeDelta * 5.f );
			_float fLength = D3DXVec3Length( &(m_vEye - m_vDestination) );
			if ( fLength <= 5.f )
				m_bGoDestination = false;

			m_vAt = m_vEye + vDir;
		}
	}
	else
	{
		_vec3 vTargetLook = *m_pLookTarget->Get_Transform()->Get_WorldLookVec();
		vTargetLook.y = 0.f;

		m_vAt = *m_pLookTarget->Get_Transform()->Get_Infomation( Engine::CTransform::INFO_POSITION );
		m_vEye = m_vAt - vTargetLook * 250.f;
		m_vEye.y += 2.f;
	}

	m_bOperate = true;

	D3DXMatrixLookAtLH( &m_matView, &m_vEye, &m_vAt, &m_vFixedUp );
	D3DXMatrixPerspectiveFovLH( &m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar );

	m_matView_Old = m_matView;
	m_matProj_Old = m_matProj;

	D3DXMatrixInverse(&m_matView_Inverse, nullptr, &m_matView);
	D3DXMatrixInverse(&m_matProj_Inverse, nullptr, &m_matProj);

	m_pGraphicDev->SetTransform( D3DTS_VIEW, &m_matView );
	m_pGraphicDev->SetTransform( D3DTS_PROJECTION, &m_matProj );

	m_vDir = m_vAt - m_vEye;

	m_matWorld = m_matView_Inverse;

	return 0;
}

void CCamera_UseTool::Check_InputState( const _float & fTimeDelta )
{
	if ( false == ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->m_bMouseInClient )
		return;

	static _bool bIsMouseRotation = false;

	CInput_Device* pInput_Device = CInput_Device::GetInstance();

	if ( pInput_Device->Get_DownMouseState( CInput_Device::DIM_MBUTTON ) )
	{
		if ( bIsMouseRotation == false )
			m_iCount = 2;
		bIsMouseRotation = ((bIsMouseRotation == true) ? false : true);
	}

	_float fCamSpeed = ((m_bIsRunMode) ? m_fCamSpeed * CAMRUNPOWER : m_fCamSpeed);

	if ( pInput_Device->Get_DIKeyState( DIK_W ) )
	{
		// 카메라의 월드행렬
		_vec3			vLook;
		memcpy( &vLook, &m_matWorld.m[2][0], sizeof( _vec3 ) );

		_vec3		vMove = *D3DXVec3Normalize( &vLook, &vLook ) * fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if ( pInput_Device->Get_DIKeyState( DIK_S ) )
	{
		// 카메라의 월드행렬
		_vec3			vLook;
		memcpy( &vLook, &m_matWorld.m[2][0], sizeof( _vec3 ) );

		_vec3		vMove = *D3DXVec3Normalize( &vLook, &vLook ) * -fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if ( pInput_Device->Get_DIKeyState( DIK_A ) )
	{
		// 카메라의 월드행렬
		_vec3			vRight;
		memcpy( &vRight, &m_matWorld.m[0][0], sizeof( _vec3 ) );

		_vec3		vMove = *D3DXVec3Normalize( &vRight, &vRight ) * -fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if ( pInput_Device->Get_DIKeyState( DIK_D ) )
	{
		// 카메라의 월드행렬
		_vec3			vRight;
		memcpy( &vRight, &m_matWorld.m[0][0], sizeof( _vec3 ) );

		_vec3		vMove = *D3DXVec3Normalize( &vRight, &vRight ) * fCamSpeed * fTimeDelta;

		m_vEye += vMove;
		m_vAt += vMove;
	}

	if ( true == bIsMouseRotation )
	{
		Fix_MousePointer( WINCX >> 1, WINCY >> 1 );

		_long		 MoveX = 0;
		_long		 MoveY = 0;

		_vec3		vLook;
		memcpy( &vLook, &m_matWorld.m[2][0], sizeof( _vec3 ) );

		MoveX = pInput_Device->Get_DIMouseMove( CInput_Device::DIMM_Y );
		MoveY = pInput_Device->Get_DIMouseMove( CInput_Device::DIMM_X );

		if ( m_iCount > 0 )
		{
			--m_iCount;
			return;
		}

		if ( MoveX )
		{
			_matrix			matRot;

			_vec3			vRight;

			memcpy( &vRight, &m_matWorld.m[0][0], sizeof( _vec3 ) );

			D3DXMatrixRotationAxis( &matRot, &vRight, MoveX * m_fRotSpeed * 0.1f * fTimeDelta );

			D3DXVec3TransformNormal( &vLook, &vLook, &matRot );

			m_vAt = m_vEye + vLook;
		}

		if ( MoveY )
		{
			_matrix			matRot;

			_vec3			vUp( 0.f, 1.f, 0.f );

			memcpy( &vUp, &m_matWorld.m[1][0], sizeof( _vec3 ) );

			D3DXMatrixRotationAxis( &matRot, &vUp, MoveY * m_fRotSpeed * 0.1f * fTimeDelta );

			D3DXVec3TransformNormal( &vLook, &vLook, &matRot );

			m_vAt = m_vEye + vLook;
		}
	}
}

void CCamera_UseTool::Fix_MousePointer(const _uint & iX, const _uint & iY)
{
	POINT			pt = { (long)iX, (long)iY };

	ClientToScreen(g_hWnd, &pt);

	SetCursorPos(pt.x, pt.y);

	//ShowCursor(false);

}

CCamera_UseTool * CCamera_UseTool::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	CCamera_UseTool *	pInstance = new CCamera_UseTool(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pEye, pAt, pUp, fFovY, fAspect, fNear, fFar)))
	{
		MSG_BOX("CCamera_UseTool Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CCamera_UseTool::Free(void)
{
	return Engine::CCamera::Free();
}

