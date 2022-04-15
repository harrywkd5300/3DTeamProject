#include "stdafx.h"
#include "..\Headers\MoveObject.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "Input_Device.h"

CMoveObject::CMoveObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pColliderCom(nullptr), m_pFrustumCom(nullptr), m_pMeshCom(nullptr)
	, m_pRendererCom(nullptr), m_pShaderCom(nullptr)
	, m_isDraw(true)
{
	ZeroMemory(&m_ObjcetData, sizeof(MAPOBJECT));
#ifdef _DEBUG
	Tag(L"CMoveObject", std::bind(&CMoveObject::Debug_Render, this));
#endif
}

HRESULT CMoveObject::Ready_GameObject(MAPOBJECT pData, _bool Combine, _matrix* pWorld)
{
	if (FAILED(Ready_Mesh(pData.ComName)))
		return E_FAIL;

	m_ObjcetData = pData;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	if (!lstrcmp(m_ObjcetData.FileName, L"Tunnel_segment_Long.X"))
	{
		m_fCullSize = 800.f;
	}
	else
	{
		m_fCullSize = 10.f;
	}

	if (Combine || nullptr == pWorld)
	{
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &pData.vPosition);
		m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &pData.vScale);
		m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &pData.vRotation);
	}
	else
	{
		m_pTransformCom->Set_WorldMatrix_ForEffect(*pWorld);
	}

	m_fMoveSpeed = 1.f;

	return NOERROR;
}

_int CMoveObject::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	if (!lstrcmp(m_ObjcetData.FileName, L"Tunnel_segment_Long.X") ||
		!lstrcmp(m_ObjcetData.FileName, L"Light_Red.X") ||
		!lstrcmp(m_ObjcetData.FileName, L"Light_Yellow.X"))
		Move_Tunnel(fTimeDelta);
	else if (!lstrcmp(m_ObjcetData.FileName, L"Sub_Traincar.X"))
		Move_Subway(fTimeDelta);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// Add By.KMH - 렌더러에서 삭제안되는 목록에 담아놓고 거기서 프러스텀 검사..
	//if (nullptr != m_pFrustumCom)
	//	m_isDraw = m_pFrustumCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), m_fCullSize);
	//
	//if (true == m_isDraw)
	//	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CMoveObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CMoveObject::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ConstantTable(pEffect);

	//pEffect->Begin(nullptr, 0);

	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture", m_ObjcetData.bIsAlpha);

	//pEffect->End();
}

HRESULT CMoveObject::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pColliderCom->ReSizing(CCollider::TYPE_AABB, m_pMeshCom->Get_Min(), m_pMeshCom->Get_Max(), m_pTransformCom);
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Frustum
	pComponent = m_pFrustumCom = (Engine::CFrustum*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CFrustum");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Frustum", pComponent));
	m_pFrustumCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

HRESULT CMoveObject::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	matWV = matView * matProj;

	matView = CCamera_Manager::GetInstance()->Get_ViewMatrix();
	matProj = CCamera_Manager::GetInstance()->Get_ProjMatrix();


	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CMoveObject::Ready_Mesh(const _tchar * pComname)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Shader
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, pComname);
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();

	return NOERROR;
}

void CMoveObject::Move_Tunnel(const _float & fTimeDelta)
{
	if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_NUMPAD5))
	{
		if (CInput_Device::GetInstance()->Get_DIKeyState(DIK_1))
			m_bEnd = true;
	}

	if (m_bStart)
	{
		m_fMoveSpeed += fTimeDelta;
		if (m_fMoveSpeed >= 30.f)
		{
			m_bStart = false;
			m_fMoveSpeed = 30.f;
		}
	}

	if (m_bEnd)
	{
		m_fMoveSpeed -= 3.f * fTimeDelta;
		if (m_fMoveSpeed <= 0.f)
		{
			m_bEnd = false;
			m_fMoveSpeed = 0.f;
		}
	}

	_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

	if (vPos.x <= -(m_fTunnelSize * 2.f))
	{
		vPos.x += m_fTunnelSize * 4.f;
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);
	}

	vPos.x -= m_fMoveSpeed * fTimeDelta;

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);
}

void CMoveObject::Move_Subway(const _float & fTimeDelta)
{
	if (m_bShake)
	{
		if (!m_bShakeEnd)
		{
			if (m_bShakeRight)
			{
				m_fShakePower += 30.f * fTimeDelta;
				if (m_fShakePower > 2.f)
					m_bShakeRight = false;
			}
			else
			{
				m_fShakePower -= 30.f * fTimeDelta;
				if (m_fShakePower < -2.f)
					m_bShakeEnd = true;
			}
		}
		else
		{
			m_fShakePower += 30.f * fTimeDelta;
			if (m_fShakePower > 0.f)
			{
				m_fShakePower = 0.f;
				++m_fShakeCnt;
				m_bShakeRight = true;
				m_bShakeEnd = false;
			}
		}

		if (2.f == m_fShakeCnt)
		{
			m_bShake = false;
			m_fShakeCnt = 0.f;
		}

		_vec3 vShake = *m_pTransformCom->Get_Infomation(CTransform::INFO_ANGLE);

		vShake.z = m_fShakePower;

		m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &vShake);
	}

	// Shake 쿨타임
	if (!m_bShake)
	{
		m_fShake += 1.f * fTimeDelta;
		if (m_fShake > 4.f)
		{
			m_fShake = 0.f;
			m_bShake = true;
		}
	}
}

CMoveObject * CMoveObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, MAPOBJECT pData, _bool Combine, _matrix* pWorld)
{
	CMoveObject *	pInstance = new CMoveObject(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pData, Combine, pWorld)))
	{
		MSG_BOX("CMoveObject Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoveObject::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CMoveObject");

	CDebug::GetInstance()->Add_DebugText(L"ComName: %s", m_ObjcetData.ComName);

	CDebug::GetInstance()->Add_DebugText(m_ObjcetData.LocalMatrix, "m_ObjcetData.LocalMatrix");

	CDebug::GetInstance()->Add_DebugText(L"MoveSpeed: %f", m_fMoveSpeed);
}

_ulong CMoveObject::Free(void)
{
	_ulong		dwRefCnt = 0;

	dwRefCnt = Engine::Safe_Release(m_pColliderCom);
	dwRefCnt = Engine::Safe_Release(m_pRendererCom);
	dwRefCnt = Engine::Safe_Release(m_pFrustumCom);
	dwRefCnt = Engine::Safe_Release(m_pMeshCom);
	dwRefCnt = Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
