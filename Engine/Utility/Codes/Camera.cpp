#include "..\Headers\Camera.h"

USING(Engine)

CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_vEye(0.f, 0.f, 0.f)
	, m_vAt(0.f, 0.f, 0.f)
	, m_vFixedUp(0.f, 0.f, 0.f)
	, m_fFovY(0.f)
	, m_fAspect(0.f)
	, m_fNear(0.f)
	, m_fFar(0.f)
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_bOperate(false)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matProj_Old);
	D3DXMatrixIdentity(&m_matView_Old);
	D3DXMatrixIdentity(&m_matProj_Inverse);
	D3DXMatrixIdentity(&m_matView_Inverse);
	m_pInput_Device->AddRef();
#ifndef _FINAL
	Tag(L"CCamera", std::bind(&CCamera::Debug_Render, this));
#endif

}

HRESULT CCamera::Ready_GameObject(const _vec3 * pEye, const _vec3 * pAt, const _vec3 * pUp, const _float & fFovY, const _float & fAspect, const _float & fNear, const _float & fFar)
{
	m_vEye = *pEye;
	m_vAt = *pAt;
	m_vFixedUp = *pUp;

	m_fFovY = fFovY;
	m_fAspect = fAspect;
	m_fNear = fNear;
	m_fFar = fFar;

	m_matView_Old = m_matView;
	m_matProj_Old = m_matProj;
	SetUp_ViewMatrix(&m_matView);
	SetUp_ProjMatrix(&m_matProj);


	m_vDir = m_vAt - m_vEye;
	m_vDir_NotY = m_vDir;
	m_vDir_NotY.y = 0;
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	D3DXVec3Normalize(&m_vDir_NotY, &m_vDir_NotY);
		
	return NOERROR;
}


_int CCamera::Update_GameObject(const _float & fTimeDelta)
{
	CGameObject::Update_GameObject(fTimeDelta);

	m_matView_Old = m_matView;
	m_matProj_Old = m_matProj;

	SetUp_ViewMatrix(&m_matView);
	SetUp_ProjMatrix(&m_matProj);

	m_vDir = m_vAt - m_vEye;
	m_vDir_NotY = m_vDir;
	m_vDir_NotY.y = 0;
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	D3DXVec3Normalize(&m_vDir_NotY, &m_vDir_NotY);

	D3DXMatrixInverse(&m_matView_Inverse, nullptr, &m_matView);
	D3DXMatrixInverse(&m_matProj_Inverse, nullptr, &m_matProj);

	return 0;
}

_int CCamera::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CGameObject::LastUpdate_GameObject(fTimeDelta);
	return 0;
}

void CCamera::SetUp_ViewMatrix(_matrix* pMatrix)
{
	D3DXMatrixIdentity(&m_matView);
	// 뷰스페이스 변환행렬을 만들자.

	// 1. 카메라의 월드변환행렬(상태행렬)을 만들어보자.
	_vec3		vRight, vUp, vLook, vPos;

	// look
	vLook = m_vAt - m_vEye;

	// right
	D3DXVec3Cross(&vRight, &m_vFixedUp, &vLook);

	// up
	D3DXVec3Cross(&vUp, &vLook, &vRight);

	// vPos
	vPos = m_vEye;

	// right
	memcpy((_vec3*)&m_matWorld.m[0][0], D3DXVec3Normalize(&vRight, &vRight), sizeof(_vec3));
	// up
	memcpy((_vec3*)&m_matWorld.m[1][0], D3DXVec3Normalize(&vUp, &vUp), sizeof(_vec3));
	// look
	memcpy((_vec3*)&m_matWorld.m[2][0], D3DXVec3Normalize(&vLook, &vLook), sizeof(_vec3));
	// pos
	memcpy((_vec3*)&m_matWorld.m[3][0], &vPos, sizeof(_vec3));

	D3DXMatrixInverse(&m_matView, nullptr, &m_matWorld);

	if(true == m_bOperate)
		m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);
}

void CCamera::SetUp_ProjMatrix(_matrix * pMatrix)
{
	D3DXMatrixIdentity(&m_matProj);

	m_matProj._11 = (1.f / tanf(m_fFovY * 0.5f)) / m_fAspect;
	m_matProj._22 = 1.f / tanf(m_fFovY * 0.5f);
	m_matProj._33 = m_fFar / (m_fFar - m_fNear);
	m_matProj._43 = (m_fFar * m_fNear) / (m_fFar - m_fNear) * -1.0f;
	m_matProj._34 = 1.f;
	m_matProj._44 = 0.f;

	if (true == m_bOperate)
		m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);
}
#ifndef _FINAL
void CCamera::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CCamera");
	CDebug::GetInstance()->Add_DebugText(m_vEye,"m_vEye");
	CDebug::GetInstance()->Add_DebugText(m_vAt, "m_vAt");
}
#endif
_ulong CCamera::Free(void)
{
	Engine::Safe_Release(m_pInput_Device);

	return Engine::CGameObject::Free();
}
