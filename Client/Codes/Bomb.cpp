#include "stdafx.h"
#include "..\Headers\Bomb.h"

#include "Component_Manager.h"
#include "Object_Manager.h"

CBomb::CBomb(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CBomb", std::bind(&CBomb::Debug_Render, this));
#endif
}

HRESULT CBomb::Ready_GameObject(_matrix* pBone_mat, CTransform* pMonster_Transform)
{

	if (FAILED(Ready_Component()))
		return E_FAIL;	
	

	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &_vec3(210.f, 0.f, -90.f));
	_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);
	vPos.x += 2.f;
	vPos.y -= 5.f;
	vPos.z += 3.f;
	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);


	m_pMonster_Transform = pMonster_Transform;
	m_pMonster_Bone_mat = pBone_mat;

	m_vMonster_Look = *m_pMonster_Transform->Get_WorldLookVec();
	m_vMonster_Look.y = 0.f;
	D3DXVec3Normalize(&m_vMonster_Look, &m_vMonster_Look);

	CDebug::GetInstance()->Add_ConsoleDebugText("vMonster_Look %f, %f, %f", m_vMonster_Look.x, m_vMonster_Look.y, m_vMonster_Look.z);

	m_fSpeed = 4.f;
	m_fSpeed_Y = 1.5f;
	m_fTempSpeed_Y = m_fSpeed_Y;
	m_fSpeed_Minus_Y = 1.5f;

	return NOERROR;
}

_int CBomb::Update_GameObject(const _float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (true == m_bThrowing)
	{
		_matrix Monster_mat = m_pMonster_Transform->Get_WorldMatrix();
		_matrix Result_mat = *m_pMonster_Bone_mat * Monster_mat;

		m_Result_mat = Result_mat;

		m_pTransformCom->Set_ParentMatrix(&Result_mat);
	}
	else
	{
		if (true == m_bInit_Angle)
		{
			_matrix mat;
			D3DXMatrixIdentity(&mat);
			m_pTransformCom->Set_ParentMatrix(&mat);

			_vec3 vAngle = _vec3(0.f, 0.f, 0.f);
			m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, &vAngle);

			_vec3 vScale = _vec3(0.01f, 0.01f, 0.01f);
			m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &vScale);

			_vec3 vPos = (_vec3)m_Result_mat.m[3];
			m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);



			_float fAngle = 0.f;

			_vec3 vDefault_Angle = _vec3(0.f, 0.f, -1.f);
			D3DXVec3Normalize(&vDefault_Angle, &vDefault_Angle);

			_float fdot = D3DXVec3Dot(&vDefault_Angle, &m_vMonster_Look);
			fAngle = acosf(fdot);

			fAngle = D3DXToDegree(fAngle);

			if (0 < m_vMonster_Look.x)
				fAngle *= -1;

			m_pTransformCom->Rotation_Y(fAngle);


			m_bInit_Angle = false;
		}
		

		_vec3 vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

		if (m_fTempSpeed_Y * -1.f < m_fSpeed_Y)
		{
			vPos += m_vMonster_Look * m_fSpeed * fTimeDelta;
			vPos.y += m_fSpeed_Y * fTimeDelta * 1.2f;
		}
		else if ((m_fTempSpeed_Y + 1.1f) * -1.f > m_fSpeed_Y)
		{
			//죽는 bool값 적용   Get만들자
			return 1;
		}
		else if (0 >= m_fSpeed_Y)
		{
			vPos += m_vMonster_Look * m_fSpeed * fTimeDelta;
			vPos.y += m_fSpeed_Y * fTimeDelta * 2.2f;
		}
		

		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);

		m_fSpeed_Y -= m_fSpeed_Minus_Y * fTimeDelta * 3.2f;

		

	}


	
	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);

	return 0;
}

void CBomb::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	SetUp_ConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	//pEffect->BeginPass(0);

	//m_pMeshCom->Render_Mesh();
	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture");

	//pEffect->EndPass();
	pEffect->End();

	
}

HRESULT CBomb::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	
	// For.Mesh
	pComponent = m_pMeshCom = (Engine::CMesh_Static*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CMesh_Static_Monster_Bomb_Mask");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();



	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();


	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Mesh");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();


	return NOERROR;
}


HRESULT CBomb::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	Safe_Release(pEffect);

	return NOERROR;
}

CBomb * CBomb::Create(LPDIRECT3DDEVICE9 pGraphicDev, _matrix* pBone_mat, CTransform* pMonster_Transform)
{
	CBomb *	pInstance = new CBomb(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject(pBone_mat, pMonster_Transform)))
	{
		MSG_BOX("CBomb Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CBomb::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CBomb");

}
#endif
_ulong CBomb::Free(void)
{
	_ulong		dwRefCnt = 0;

	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pMeshCom);
	Engine::Safe_Release(m_pShaderCom);

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
