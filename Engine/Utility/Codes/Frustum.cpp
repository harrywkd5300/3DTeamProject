#include "..\Headers\Frustum.h"
#include "Camera_Manager.h"
USING(Engine)

CFrustum::CFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CFrustum", std::bind(&CFrustum::Debug_Render, this));
#endif

}

CFrustum::CFrustum(const CFrustum & rhs)
	: CComponent(rhs)
{
#ifndef _FINAL
	Tag(L"CFrustum", std::bind(&CFrustum::Debug_Render, this));
#endif

}

HRESULT CFrustum::Ready_Frustum(void)
{
	SetUp_PointsInProjection();

	return NOERROR;
}

void CFrustum::Update_Component(const _float & fTimeDelta)
{
	if (!m_bUpdate)
	{
		SetUp_PointsInWorldSpace();
		m_bUpdate = true;
	}
}

void CFrustum::LastUpdate_Component(const _float & fTimeDelta)
{
	m_bUpdate = false;
}


_bool CFrustum::isIn_Frustum(const _vec3 * pPositionInWorldSpace, _float fRadius, bool* pbCheck)
{
	if (nullptr == m_pGraphicDev)
		return false;

	for (size_t i = 0; i < 6; ++i)
	{
		if (fRadius < D3DXPlaneDotCoord(&m_PlaneArr[i], pPositionInWorldSpace))
		{
			if (pbCheck != nullptr)
				*pbCheck = false;

			return false;
		}
	}
	if (pbCheck != nullptr)
		*pbCheck = true;

	return true;
}

_bool CFrustum::isIn_Frustum_Local(const _matrix & matWorld, const _vec3 * pPositionInLocal, _float fRadius)
{
	if (nullptr == m_pGraphicDev)
		return false;

	_vec3 vPosWS = *pPositionInLocal;
	D3DXVec3TransformCoord(&vPosWS, &vPosWS, &matWorld);

	for (size_t i = 0; i < 6; ++i)
	{
		if ( fRadius < D3DXPlaneDotCoord( &m_PlaneArr[i], &vPosWS ) )
			return false;
	}
	return true;
}

void CFrustum::SetUp_PointsInProjection(void)
{
	m_vOriginalPoint[0] = _vec3(-1.f, 1.f, 0.f);
	m_vOriginalPoint[1] = _vec3(1.f, 1.f, 0.f);
	m_vOriginalPoint[2] = _vec3(1.f, -1.f, 0.f);
	m_vOriginalPoint[3] = _vec3(-1.f, -1.f, 0.f);

	m_vOriginalPoint[4] = _vec3(-1.f, 1.f, 1.f);
	m_vOriginalPoint[5] = _vec3(1.f, 1.f, 1.f);
	m_vOriginalPoint[6] = _vec3(1.f, -1.f, 1.f);
	m_vOriginalPoint[7] = _vec3(-1.f, -1.f, 1.f);
}

void CFrustum::SetUp_PointsInWorldSpace(void)
{
	if (nullptr == m_pGraphicDev)
		return;

	_matrix			matView, matProj;

	//  뷰스페이스로 보낸다.
	//CCamera_Manager::GetInstance()->Get_InverseProjMatrix();
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixInverse(&matProj, nullptr, &matProj);
	matProj = CCamera_Manager::GetInstance()->Get_InverseProjMatrix();

	for (size_t i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vOriginalPoint[i], &matProj);

	//  월드스페이스로 보낸다.
	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	//D3DXMatrixInverse(&matView, nullptr, &matView);
	matView = CCamera_Manager::GetInstance()->Get_InverseViewMatrix();

	for (size_t i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &matView);

	// +x, -x
	D3DXPlaneFromPoints(&m_PlaneArr[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
	D3DXPlaneFromPoints(&m_PlaneArr[1], &m_vPoint[4], &m_vPoint[0], &m_vPoint[3]);

	// +y, -y
	D3DXPlaneFromPoints(&m_PlaneArr[2], &m_vPoint[4], &m_vPoint[5], &m_vPoint[1]);
	D3DXPlaneFromPoints(&m_PlaneArr[3], &m_vPoint[3], &m_vPoint[2], &m_vPoint[6]);

	// +z, -z
	D3DXPlaneFromPoints(&m_PlaneArr[4], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
	D3DXPlaneFromPoints(&m_PlaneArr[5], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);
}

CFrustum * CFrustum::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFrustum *		pInstance = new CFrustum(pGraphicDev);

	if (FAILED(pInstance->Ready_Frustum()))
	{
		MSG_BOX("CFrustum Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CFrustum::Clone(void)
{
	this->AddRef();
	return this;
	//return new CFrustum(*this);
}
#ifndef _FINAL
void CFrustum::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CFrustum");

	CDebug::GetInstance()->Add_DebugText("m_vPoint[0] %f", m_vPoint[0].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[1] %f", m_vPoint[1].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[2] %f", m_vPoint[2].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[3] %f", m_vPoint[3].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[4] %f", m_vPoint[4].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[5] %f", m_vPoint[5].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[6] %f", m_vPoint[6].x);
	CDebug::GetInstance()->Add_DebugText("m_vPoint[7] %f", m_vPoint[7].x);

	if (m_bUpdate)
		CDebug::GetInstance()->Add_DebugText("m_bUpdate: true");
	else
		CDebug::GetInstance()->Add_DebugText("m_bUpdate: false");
}
#endif
_ulong CFrustum::Free(void)
{
	return CComponent::Free();
}
