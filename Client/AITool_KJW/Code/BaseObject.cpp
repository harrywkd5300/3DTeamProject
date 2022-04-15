#include "stdafx.h"
#include "..\Headers\BaseObject.h"

#include "Component_Manager.h"
#include "Camera_Manager.h"

CBaseObject::CBaseObject(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	, m_pColliderCom(nullptr), m_pFrustumCom(nullptr), m_pMeshCom(nullptr)
	, m_pRendererCom(nullptr), m_pShaderCom(nullptr)
	, m_isDraw(true)
{
	ZeroMemory(&m_ObjcetData, sizeof(MAPOBJECT));
#ifdef _DEBUG
	Tag(L"CBaseObject", std::bind(&CBaseObject::Debug_Render, this));
#endif
}

HRESULT CBaseObject::Ready_GameObject(MAPOBJECT pData, _bool Combine, _matrix* pWorld)
{
	if (FAILED(Ready_Mesh(pData.ComName)))
		return E_FAIL;

	m_ObjcetData = pData;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	if (!lstrcmp(m_ObjcetData.FileName, L"Bldg_SkyLine_1.X") ||
		!lstrcmp(m_ObjcetData.FileName, L"Bldg_Skyline_3_Night.X") || 
		!lstrcmp(m_ObjcetData.FileName, L"Bldg_Skyline_4.X") ||
		!lstrcmp(m_ObjcetData.FileName, L"Subway_median.X") ||
		!lstrcmp(m_ObjcetData.FileName, L"WH_Roof_2.X") ||
		!lstrcmp(m_ObjcetData.FileName, L"WH_Roof_Floor_2.X") || 
		!lstrcmp(m_ObjcetData.FileName, L"SM_CP_ext_bridge_pipes.X"))
	{
		m_fCullSize = 100.f;
	}
	else if (!lstrcmp(m_ObjcetData.FileName, L"Floor_SidewalkS.X") ||
		     !lstrcmp(m_ObjcetData.FileName, L"Roof_Floor_1_a.X") || 
			 !lstrcmp(m_ObjcetData.FileName, L"Roof_Floor_Ch1.X") ||
			 !lstrcmp(m_ObjcetData.FileName, L"Tatto_Roof_S.X") ||
			 !lstrcmp(m_ObjcetData.FileName, L"Tattoo_Roof_C.X") || 
			 !lstrcmp(m_ObjcetData.FileName, L"Wire.X") ||
		     !lstrcmp(m_ObjcetData.FileName, L"SM_AM_wall2_grate3.X"))
	{
		m_fCullSize = 25.f;
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

	//m_pTransformCom->Update_Component(1.f);

	return NOERROR;
}

_int CBaseObject::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// Add By.KMH - 렌더러에서 삭제안되는 목록에 담아놓고 거기서 프러스텀 검사..
	//if (nullptr != m_pFrustumCom)
	//	m_isDraw = m_pFrustumCom->isIn_Frustum(m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION), m_fCullSize);
	//
	//if (true == m_isDraw)
	//	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

_int CBaseObject::LastUpdate_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::LastUpdate_GameObject(fTimeDelta);

	return 0;
}

void CBaseObject::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ConstantTable(pEffect);

	//if( 만약 그림자를 띄울 수 있는 오브젝트가 아니라면.. )
	//	pEffect->SetTechnique("DefaultRendering");
	//else	// 그림자를 띄울 수 있는 오브젝트라면
	//	pEffect->SetTechnique("ShadowRendering");
	//pEffect->Begin(nullptr, 0);

	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture", m_ObjcetData.bIsAlpha);

	//pEffect->End();
}

HRESULT CBaseObject::Ready_Component(void)
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

HRESULT CBaseObject::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	matWV = matView * matProj;

	matView = CCamera_Manager::GetInstance()->Get_ViewMatrix();
	matProj = CCamera_Manager::GetInstance()->Get_ProjMatrix();



	//m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	//m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CBaseObject::Ready_Mesh(const _tchar * pComname)
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

CBaseObject * CBaseObject::Create(LPDIRECT3DDEVICE9 pGraphicDev, MAPOBJECT pData, _bool Combine, _matrix* pWorld)
{
	CBaseObject *	pInstance = new CBaseObject(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pData, Combine, pWorld)))
	{
		MSG_BOX("CBaseObject Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CBaseObject::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CBaseObject");

	CDebug::GetInstance()->Add_DebugText(L"ComName: %s", m_ObjcetData.ComName);

	CDebug::GetInstance()->Add_DebugText("LocalMatrix[0]: %f %f %f", m_ObjcetData.LocalMatrix.m[0][0], m_ObjcetData.LocalMatrix.m[0][1], m_ObjcetData.LocalMatrix.m[0][2]);
	CDebug::GetInstance()->Add_DebugText("LocalMatrix[1]: %f %f %f", m_ObjcetData.LocalMatrix.m[1][0], m_ObjcetData.LocalMatrix.m[1][1], m_ObjcetData.LocalMatrix.m[1][2]);
	CDebug::GetInstance()->Add_DebugText("LocalMatrix[2]: %f %f %f", m_ObjcetData.LocalMatrix.m[2][0], m_ObjcetData.LocalMatrix.m[2][1], m_ObjcetData.LocalMatrix.m[2][2]);
	CDebug::GetInstance()->Add_DebugText("LocalMatrix[3]: %f %f %f", m_ObjcetData.LocalMatrix.m[3][0], m_ObjcetData.LocalMatrix.m[3][1], m_ObjcetData.LocalMatrix.m[3][2]);


}

_ulong CBaseObject::Free(void)
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
