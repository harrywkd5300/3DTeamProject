#include "..\Headers\EffectOBJ.h"
#include "Component_Manager.h"

USING(Engine)

CEffectOBJ::CEffectOBJ(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTarget(nullptr)
	, m_pTargetTrans(nullptr)
	, m_pMatFrame(nullptr)
	, m_eTransType(EFFECT_INFO::TYPE_END)
	, m_bIsClone(false)
{
	D3DXMatrixIdentity(&m_matParent);

#ifndef _FINAL
	Tag(L"CEffectOBJ", std::bind(&CEffectOBJ::Debug_Render, this));
#endif
}

CEffectOBJ::CEffectOBJ(const CEffectOBJ & rhs)
	: CGameObject(rhs.m_pGraphicDev)
	//, m_pTransformCom(nullptr) // 추후 클라에서 확인해야 함!!!
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pTarget(nullptr)
	, m_pTargetTrans(nullptr)
	, m_pMatFrame(nullptr)
	, m_bIsClone(true)
	, m_pEffectTag(rhs.m_pEffectTag)
{
	//if (nullptr != m_pTransformCom)
	//	m_pTransformCom->AddRef();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->AddRef();
	if (nullptr != m_pShaderCom)
		m_pShaderCom->AddRef();
	Ready_GameObject();

	D3DXMatrixIdentity(&m_matParent);

	for (auto& pEffect : rhs.m_listEffects)
	{
		CMyEffect*	pNewEffect = pEffect->Clone();
		Add_Effect(pNewEffect);
	}

#ifndef _FINAL
	Tag(L"CEffectOBJ", std::bind(&CEffectOBJ::Debug_Render, this));
#endif
}

HRESULT CEffectOBJ::Add_Effect(CMyEffect * pNewEffect)
{
	if (nullptr == pNewEffect)
		return E_FAIL;

	m_listEffects.push_back(pNewEffect);

	return NOERROR;
}

void CEffectOBJ::Reset(void)
{
	for (auto pEffect : m_listEffects)
	{
		pEffect->Reset_Info();
	}

	Set_IsDead(false);

	m_pTarget = nullptr;
	m_pTargetTrans = nullptr;
	m_pMatFrame = nullptr;
	D3DXMatrixIdentity(&m_matParent);
}

HRESULT CEffectOBJ::Ready_GameObject(void)
{
	if (!m_bIsClone)
	{
		if (FAILED(Ready_Component()))
			return E_FAIL;
	}
	else
	{
		Engine::CComponent*		pComponent = nullptr;

		// For.Transform
		pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
		if (nullptr == pComponent)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"CTransform", pComponent));
		m_pTransformCom->AddRef();

	}

	return NOERROR;
}

_int CEffectOBJ::Update_GameObject(const _float & fTimeDelta)
{
	if (m_bIsDead)
		return 1;

#ifndef _FINAL
	float fTime = 0.f;
	bool bErr = false;
	CHECK_TIME_START
#endif


		if (m_pTarget != nullptr)
		{
			if (m_pTarget->Get_IsDead())
			{
				m_pTargetTrans = nullptr;
				m_pMatFrame = nullptr;
				//Set_IsDead(true);
			}
			if (m_pTargetTrans != nullptr)
				//m_matParent = m_pTargetTrans->Get_WorldMatrix_FORCED();
				m_matParent = m_pTargetTrans->Get_WorldMatrix();
		}

	if (m_pMatFrame != nullptr)
		m_matParent = *m_pMatFrame * m_matParent;

	// 스케일 영향력 없애기 위해 노말라이즈
	D3DXVec3Normalize((_vec3*)m_matParent.m[0], (_vec3*)m_matParent.m[0]);
	D3DXVec3Normalize((_vec3*)m_matParent.m[1], (_vec3*)m_matParent.m[1]);
	D3DXVec3Normalize((_vec3*)m_matParent.m[2], (_vec3*)m_matParent.m[2]);

	m_pTransformCom->Set_ParentMatrix(&m_matParent);

	CGameObject::Update_GameObject(fTimeDelta);

	_int	iReturn = 0;

	LISTEFFECTS::iterator iter_begin = m_listEffects.begin();
	LISTEFFECTS::iterator iter_end = m_listEffects.end();

	bool bRunEffect = false;
	for (; iter_begin != iter_end;)
	{
		if ((*iter_begin)->Get_IsDead())
		{
			++iter_begin;
			continue;
		}
		bRunEffect = true;
		(*iter_begin)->Set_ParentMatrix(&m_pTransformCom->Get_WorldMatrix());
		iReturn = (*iter_begin)->Update_GameObject(fTimeDelta);

		if (iReturn == 1)
		{
			(*iter_begin)->Set_IsDead(true);
		}
		else
			++iter_begin;
	}


	if (!bRunEffect)
		Set_IsDead(true);
	else
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);
	}


#ifndef _FINAL
	CHECK_TIME_END(fTime, bErr);
	CDebug::GetInstance()->Add_EffectRunTime(fTime);
#endif

	return 0;
}

void CEffectOBJ::Render_GameObject(LPD3DXEFFECT pEffect)
{

}

HRESULT CEffectOBJ::Ready_Component(void)
{
	Engine::CComponent*		pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"CTransform", pComponent));
	m_pTransformCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CRenderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CShader_Effect");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CShader", pComponent));
	m_pShaderCom->AddRef();

	return NOERROR;
}

CEffectOBJ * CEffectOBJ::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffectOBJ*		pInstance = new CEffectOBJ(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffectOBJ Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}

	return pInstance;
}

CEffectOBJ * CEffectOBJ::Clone(void)
{
	return new CEffectOBJ(*this);
}
#ifndef _FINAL
void CEffectOBJ::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CEffectOBJ");
	CDebug::GetInstance()->Add_DebugText(">>>  Donw (VK_DOWN) / UP (VK_UP)  <<<");
	int Count = 0;


	int listSize = m_listEffects.size();
	CDebug::GetInstance()->Add_DebugText("listSize:%d", listSize);

	for (auto pEffect : m_listEffects)
	{
		if (iStartCount > Count)
		{
			++Count;
			continue;
		}
		CDebug::GetInstance()->Add_DebugText(L"%c) %s", CDebug::DisplayKey[Count - iStartCount], L"CMyEffect");
		//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[Count]))
		if (GetAsyncKeyState(CDebug::Key[Count - iStartCount]) & 0x0001)
		{
			iStartCount = 0;
			pEffect->Set_LeafNode();
			CDebug::GetInstance()->Set_CurDebug(std::bind(&CMyEffect::Debug_Render, pEffect), pEffect);
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x0001)
		{
			iStartCount += 1;
			if (iStartCount >= listSize)
				iStartCount = listSize - 1;
		}
		if (GetAsyncKeyState(VK_UP) & 0x0001)
		{
			iStartCount -= 1;
			if (iStartCount < 0)
				iStartCount = 0;
		}

		++Count;
	}
}
#endif
_ulong CEffectOBJ::Free(void)
{
	_ulong		dwRefCnt = 0;

	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);

	for (auto& pEffectCom : m_listEffects)
		Engine::Safe_Release(pEffectCom);

	m_listEffects.clear();

	dwRefCnt = CGameObject::Free();

	return dwRefCnt;
}
