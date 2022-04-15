#include "..\Headers\Transform.h"
#include "Object_Manager.h"

#include "Cell.h"

USING(Engine)

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CTransform", std::bind(&CTransform::Debug_Render, this));
#endif

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)
	, m_matParent(rhs.m_matParent)
{
	Ready_Transform();
#ifndef _FINAL
	Tag(L"CTransform", std::bind(&CTransform::Debug_Render, this));
#endif

}

_matrix CTransform::Get_LocalMatrix()
{
	D3DXQUATERNION		QtRot;
	_matrix				matScale, matRot, matTrans;
	_matrix				matWorld;
	D3DXMatrixIdentity(&matRot);
	D3DXQuaternionRotationYawPitchRoll(&QtRot, D3DXToRadian(m_vInfo[INFO_ANGLE].y), D3DXToRadian(m_vInfo[INFO_ANGLE].x), D3DXToRadian(m_vInfo[INFO_ANGLE].z));
	D3DXMatrixRotationQuaternion(&matRot, &QtRot);
	//D3DXMatrixInverse(&matRot, NULL, &matRot);

	D3DXMatrixScaling(&matScale, m_vInfo[INFO_SCALE].x, m_vInfo[INFO_SCALE].y, m_vInfo[INFO_SCALE].z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POSITION].x, m_vInfo[INFO_POSITION].y, m_vInfo[INFO_POSITION].z);

	matWorld = matScale * matRot * matTrans;

	return matWorld;
}

void CTransform::Clamp_Angle()
{
	if (m_vInfo[INFO_ANGLE].x > 360.f)
		m_vInfo[INFO_ANGLE].x -= 360.f;
	else if (m_vInfo[INFO_ANGLE].x < 0.f)
		m_vInfo[INFO_ANGLE].x += 360.f;

	if (m_vInfo[INFO_ANGLE].y > 360.f)
		m_vInfo[INFO_ANGLE].y -= 360.f;
	else if (m_vInfo[INFO_ANGLE].y < 0.f)
		m_vInfo[INFO_ANGLE].y += 360.f;

	if (m_vInfo[INFO_ANGLE].z > 360.f)
		m_vInfo[INFO_ANGLE].z -= 360.f;
	else if (m_vInfo[INFO_ANGLE].z < 0.f)
		m_vInfo[INFO_ANGLE].z += 360.f;
}

void CTransform::Update_WorldMatrix()
{
	_matrix				matScale, matRot, matTrans;

	D3DXMatrixIdentity(&matRot);
	D3DXQuaternionRotationYawPitchRoll(&m_QtRot, D3DXToRadian(m_vInfo[INFO_ANGLE].y), D3DXToRadian(m_vInfo[INFO_ANGLE].x), D3DXToRadian(m_vInfo[INFO_ANGLE].z));
	D3DXMatrixRotationQuaternion(&matRot, &m_QtRot);
	//D3DXMatrixInverse(&matRot, NULL, &matRot);

	D3DXMatrixScaling(&matScale, m_vInfo[INFO_SCALE].x, m_vInfo[INFO_SCALE].y, m_vInfo[INFO_SCALE].z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POSITION].x, m_vInfo[INFO_POSITION].y, m_vInfo[INFO_POSITION].z);

	m_matWorld = matScale * matRot * matTrans * m_matParent;
	m_matWorld_FORCED = m_matWorld;

	m_bUpdateWorld = true;
	Clamp_Angle();
	//m_matWorld_Render = m_matWorld * m_matParent;
}

void CTransform::Update_WorldMatrix(_matrix * pMatrix)
{
	if (nullptr == pMatrix)
		return;

	_matrix				matScale, matRot, matTrans;

	D3DXMatrixIdentity(&matRot);
	D3DXQuaternionRotationYawPitchRoll(&m_QtRot, D3DXToRadian(m_vInfo[INFO_ANGLE].y), D3DXToRadian(m_vInfo[INFO_ANGLE].x), D3DXToRadian(m_vInfo[INFO_ANGLE].z));
	D3DXMatrixRotationQuaternion(&matRot, &m_QtRot);
	//D3DXMatrixInverse(&matRot, NULL, &matRot);

	D3DXMatrixScaling(&matScale, m_vInfo[INFO_SCALE].x, m_vInfo[INFO_SCALE].y, m_vInfo[INFO_SCALE].z);
	D3DXMatrixTranslation(&matTrans, m_vInfo[INFO_POSITION].x, m_vInfo[INFO_POSITION].y, m_vInfo[INFO_POSITION].z);

	*pMatrix = matScale * matRot * matTrans * m_matParent;
	m_bUpdateWorld = true;
	Clamp_Angle();
}

void CTransform::Add_Angle(D3DXQUATERNION pQuaterRot)
{
	m_QtRot = pQuaterRot;
	double test = pQuaterRot.x*pQuaterRot.y + pQuaterRot.z*pQuaterRot.w;
	if (test > 0.499f) { // singularity at north pole  
		m_vInfo[INFO_ANGLE].y += D3DXToDegree(2 * atan2f(pQuaterRot.x, pQuaterRot.w));
		m_vInfo[INFO_ANGLE].z += D3DXToDegree(D3DX_PI / 2);
		m_vInfo[INFO_ANGLE].x += 0;
	}
	else if (test < -0.499f) { // singularity at south pole  
		m_vInfo[INFO_ANGLE].y += D3DXToDegree (-2 * atan2(pQuaterRot.x, pQuaterRot.w));
		m_vInfo[INFO_ANGLE].z += D3DXToDegree (-D3DX_PI / 2);
		m_vInfo[INFO_ANGLE].x += 0;
	}
	else
	{
		double sqx = pQuaterRot.x*pQuaterRot.x;
		double sqy = pQuaterRot.y*pQuaterRot.y;
		double sqz = pQuaterRot.z*pQuaterRot.z;
		m_vInfo[INFO_ANGLE].y += D3DXToDegree(atan2f(2 * pQuaterRot.y*pQuaterRot.w - 2 * pQuaterRot.x*pQuaterRot.z, float(1 - 2 * sqy - 2 * sqz)));
		m_vInfo[INFO_ANGLE].z += D3DXToDegree(asinf(float(2 * test)));
		m_vInfo[INFO_ANGLE].x += D3DXToDegree(atan2f(2 * pQuaterRot.x*pQuaterRot.w - 2 * pQuaterRot.y*pQuaterRot.z, float(1 - 2 * sqx - 2 * sqz)));
	}
	m_bUpdateWorld = false;
	Clamp_Angle();
}

void CTransform::Set_Angle(D3DXQUATERNION pQuaterRot)
{
	m_QtRot = pQuaterRot;
	double test = pQuaterRot.x*pQuaterRot.y + pQuaterRot.z*pQuaterRot.w;
	if (test > 0.499f) { // singularity at north pole  
		m_vInfo[INFO_ANGLE].y = 2 * atan2f(pQuaterRot.x, pQuaterRot.w);
		m_vInfo[INFO_ANGLE].z = D3DX_PI / 2;
		m_vInfo[INFO_ANGLE].x = 0;
	}
	else if (test < -0.499f) { // singularity at south pole  
		m_vInfo[INFO_ANGLE].y = -2 * atan2(pQuaterRot.x, pQuaterRot.w);
		m_vInfo[INFO_ANGLE].z = -D3DX_PI / 2;
		m_vInfo[INFO_ANGLE].x = 0;
	}
	else
	{
		double sqx = pQuaterRot.x*pQuaterRot.x;
		double sqy = pQuaterRot.y*pQuaterRot.y;
		double sqz = pQuaterRot.z*pQuaterRot.z;
		m_vInfo[INFO_ANGLE].y = D3DXToDegree(atan2f(2 * pQuaterRot.y*pQuaterRot.w - 2 * pQuaterRot.x*pQuaterRot.z, float(1 - 2 * sqy - 2 * sqz)));
		m_vInfo[INFO_ANGLE].z = D3DXToDegree(asinf(float(2 * test)));
		m_vInfo[INFO_ANGLE].x = D3DXToDegree(atan2f(2 * pQuaterRot.x*pQuaterRot.w - 2 * pQuaterRot.y*pQuaterRot.z, float(1 - 2 * sqx - 2 * sqz)));
	}
	m_bUpdateWorld = false;

	Clamp_Angle();
}

void CTransform::Set_Angle(_matrix matRotationWorld)
{
	D3DXQuaternionRotationMatrix(&m_QtRot, &matRotationWorld);
	Set_Angle(m_QtRot);
	m_bUpdateWorld = false;
	
	Clamp_Angle();
}

const _vec3 * CTransform::Get_WorldRightVec_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}
	return (_vec3*)&m_matWorld_FORCED.m[0][0];
}

const _vec3 * CTransform::Get_WorldUpVec_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	return (_vec3*)&m_matWorld_FORCED.m[1][0];
}

const _vec3 * CTransform::Get_WorldLookVec_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	return (_vec3*)&m_matWorld_FORCED.m[2][0];
}

_matrix CTransform::Get_WorldMatrix_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	return m_matWorld_FORCED;
}

_matrix CTransform::Get_WorldMatrix_NotRot_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	_matrix matWorld = m_matWorld_FORCED;

	_vec3	vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	memcpy(&matWorld.m[0][0], &(vRight * D3DXVec3Length((_vec3*)&matWorld.m[0][0])), sizeof(_vec3));
	memcpy(&matWorld.m[1][0], &(vUp * D3DXVec3Length((_vec3*)&matWorld.m[1][0])), sizeof(_vec3));
	memcpy(&matWorld.m[2][0], &(vLook * D3DXVec3Length((_vec3*)&matWorld.m[2][0])), sizeof(_vec3));

	return matWorld;
}

_matrix CTransform::Get_WorldMatrix_NotScale_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	_matrix matWorld = m_matWorld_FORCED;

	_vec3	vRight(1.f, 0.f, 0.f), vUp(0.f, 1.f, 0.f), vLook(0.f, 0.f, 1.f);

	D3DXVec3Normalize((_vec3*)matWorld.m[0], (_vec3*)matWorld.m[0]);
	D3DXVec3Normalize((_vec3*)matWorld.m[1], (_vec3*)matWorld.m[1]);
	D3DXVec3Normalize((_vec3*)matWorld.m[2], (_vec3*)matWorld.m[2]);

	return matWorld;
}

_matrix CTransform::Get_RotationMatrix_FORCED()
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}
	_matrix matWorld = m_matWorld_FORCED;

	_vec3	vPosition(0.f, 0.f, 0.f);

	memcpy(&matWorld.m[3][0], &(vPosition), sizeof(_vec3));

	return matWorld;
}

_float CTransform::Compute_Distance(const _matrix * pWorldMatrix) const
{
	_vec3	vDistance = *(_vec3*)&pWorldMatrix->m[INFO_POSITION][0] - m_vInfo[INFO_POSITION];

	return D3DXVec3Length(&vDistance);
}

void CTransform::Move_OnBuffer(Engine::CVIBuffer* pTargetBuffer)
{
	if (nullptr == pTargetBuffer)
		return;

	tuple<_uint, _uint, _float>	VertexCnt = pTargetBuffer->Get_VertexCntXZItv();

	const _vec3*	pVertexPos = pTargetBuffer->Get_VertexPos();

	_vec3			vPosition;
	memcpy(&vPosition, &m_vInfo[INFO_POSITION], sizeof(_vec3));

	_int		iCurrentIdx = (_int(vPosition.z) / _int(get<2>(VertexCnt))) * (get<0>(VertexCnt)) + (_int(vPosition.x) / _int(get<2>(VertexCnt)));

	_int		iIndex[4];
	iIndex[0] = iCurrentIdx + get<0>(VertexCnt);
	iIndex[1] = iCurrentIdx + get<0>(VertexCnt) + 1;
	iIndex[2] = iCurrentIdx + 1;
	iIndex[3] = iCurrentIdx;

	_float		fRatioX = (vPosition.x - pVertexPos[iIndex[0]].x) / get<2>(VertexCnt);
	_float		fRatioZ = (pVertexPos[iIndex[0]].z - vPosition.z) / get<2>(VertexCnt);

	_float		fY = 0.f;

	// 오른쪽 위 삼각형내
	if (fRatioX > fRatioZ)
		fY = pVertexPos[iIndex[0]].y + (pVertexPos[iIndex[1]].y - pVertexPos[iIndex[0]].y) * fRatioX + ((pVertexPos[iIndex[2]]).y - (pVertexPos[iIndex[1]]).y) * fRatioZ;

	// 왼쪽 아래 삼각형내
	else
		fY = pVertexPos[iIndex[0]].y + (pVertexPos[iIndex[3]].y - pVertexPos[iIndex[0]].y) * fRatioZ + ((pVertexPos[iIndex[2]]).y - (pVertexPos[iIndex[3]]).y) * fRatioX;

	m_vInfo[INFO_POSITION].y = fY;

	m_bUpdateWorld = false;
}

void CTransform::Move_OnNavigation(Engine::CNavigation * pNavigation)
{
	if (nullptr == pNavigation)
		return;

	_vec3			vPosition;
	memcpy(&vPosition, &m_vInfo[INFO_POSITION], sizeof(_vec3));


	CCell*		pCell = pNavigation->Get_CurVecCell();

	_vec3 vPoint[3];
	vPoint[0] = *pCell->Get_Point(CCell::POINT_A);
	vPoint[1] = *pCell->Get_Point(CCell::POINT_B);
	vPoint[2] = *pCell->Get_Point(CCell::POINT_C);


	D3DXPLANE tCellPlane;

	D3DXPlaneFromPoints(&tCellPlane, &vPoint[0], &vPoint[1], &vPoint[2]);

	//ax+by+cz+d = 0; ->  ax+cz+d / b= y;


	_float fY = (tCellPlane.a * vPosition.x + tCellPlane.c * vPosition.z + tCellPlane.d) / tCellPlane.b;


	//m_matWorld.m[3][1] = -fY;

	m_vInfo[INFO_POSITION].y = -fY;

	m_bUpdateWorld = false;
}

HRESULT CTransform::Ready_Transform(void)
{
	D3DXQuaternionIdentity(&m_QtRot);

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matParent);
	
	m_vInfo[INFO_POSITION] = _vec3(0.f, 0.f, 0.f);
	m_vInfo[INFO_SCALE] = _vec3(1.f, 1.f, 1.f);
	m_vInfo[INFO_ANGLE] = _vec3(0.f, 0.f, 0.f);

	m_bUpdateWorld = false;

	return NOERROR;
}

void CTransform::SetUp_OnGraphicDev()
{
	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_matWorld));
}

void CTransform::SetUp_OnGraphicDev_FORCED(void)
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	m_pGraphicDev->SetTransform(D3DTS_WORLD, &(m_matWorld_FORCED));
}

void CTransform::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	if (nullptr == pEffect)
		return;
	pEffect->SetMatrix(pConstantName, &m_matWorld);
}


void CTransform::SetUp_OnShader_FORCED(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	if (nullptr == pEffect)
		return;

	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix(&m_matWorld_FORCED);
	}

	pEffect->SetMatrix(pConstantName, &m_matWorld_FORCED);
}

void CTransform::Update_Component(const _float & fTimeDelta)
{
	if (!m_bUpdateWorld) // 값 변경이후 계산된 적 없다면
	{
		Update_WorldMatrix();
	}
	else // 계산된 적 있다면
	{
		m_matWorld = m_matWorld_FORCED;
	}
}

CComponent * CTransform::Clone(void)
{
	return new CTransform(*this);
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTransform *		pInstance = new CTransform(pGraphicDev);

	if (FAILED(pInstance->Ready_Transform()))
	{
		MSG_BOX("CTransform Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CTransform::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CTransform");

	CDebug::GetInstance()->Add_DebugText("m_QtRot: %f %f %f %f", m_QtRot.x, m_QtRot.y, m_QtRot.z, m_QtRot.w);
	CDebug::GetInstance()->Add_DebugText("m_vInfo[INFO_POSITION]: %f %f %f", m_vInfo[INFO_POSITION].x, m_vInfo[INFO_POSITION].y, m_vInfo[INFO_POSITION].z);
	CDebug::GetInstance()->Add_DebugText("m_vInfo[INFO_SCALE]: %f %f %f", m_vInfo[INFO_SCALE].x, m_vInfo[INFO_SCALE].y, m_vInfo[INFO_SCALE].z);
	CDebug::GetInstance()->Add_DebugText("m_vInfo[INFO_ANGLE]: %f %f %f", m_vInfo[INFO_ANGLE].x, m_vInfo[INFO_ANGLE].y, m_vInfo[INFO_ANGLE].z);

	CDebug::GetInstance()->Add_DebugText("m_matWorld[0]: %f %f %f", m_matWorld.m[0][0], m_matWorld.m[0][1], m_matWorld.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("m_matWorld[1]: %f %f %f", m_matWorld.m[1][0], m_matWorld.m[1][1], m_matWorld.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("m_matWorld[2]: %f %f %f", m_matWorld.m[2][0], m_matWorld.m[2][1], m_matWorld.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("m_matWorld[3]: %f %f %f", m_matWorld.m[3][0], m_matWorld.m[3][1], m_matWorld.m[3][2]);

	CDebug::GetInstance()->Add_DebugText("m_matParent[0]: %f %f %f", m_matParent.m[0][0], m_matParent.m[0][1], m_matParent.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("m_matParent[1]: %f %f %f", m_matParent.m[1][0], m_matParent.m[1][1], m_matParent.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("m_matParent[2]: %f %f %f", m_matParent.m[2][0], m_matParent.m[2][1], m_matParent.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("m_matParent[3]: %f %f %f", m_matParent.m[3][0], m_matParent.m[3][1], m_matParent.m[3][2]);

	CDebug::GetInstance()->Add_DebugText("m_matWorld_FORCED[0]: %f %f %f", m_matWorld_FORCED.m[0][0], m_matWorld_FORCED.m[0][1], m_matWorld_FORCED.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("m_matWorld_FORCED[1]: %f %f %f", m_matWorld_FORCED.m[1][0], m_matWorld_FORCED.m[1][1], m_matWorld_FORCED.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("m_matWorld_FORCED[2]: %f %f %f", m_matWorld_FORCED.m[2][0], m_matWorld_FORCED.m[2][1], m_matWorld_FORCED.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("m_matWorld_FORCED[3]: %f %f %f", m_matWorld_FORCED.m[3][0], m_matWorld_FORCED.m[3][1], m_matWorld_FORCED.m[3][2]);

	if(m_bUpdateWorld)
		CDebug::GetInstance()->Add_DebugText("m_bUpdateWorld: true");
	else
		CDebug::GetInstance()->Add_DebugText("m_bUpdateWorld: false");
}
#endif
_ulong CTransform::Free(void)
{

	return CComponent::Free();
}
