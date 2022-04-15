#include "stdafx.h"
//#include "EffectOBJ_CL.h"
//#include "Component_Manager.h"
//
//CEffectOBJ_CL::CEffectOBJ_CL(LPDIRECT3DDEVICE9 pGraphicDev)
//	: CEffectOBJ(pGraphicDev)
//{
//}
//
//HRESULT CEffectOBJ_CL::Add_Effect(CMyEffect * pNewEffect)
//{
//	if (nullptr == pNewEffect)
//		return E_FAIL;
//
//	if (m_iIndex >= 7)
//		return NOERROR;
//
//	// 원본객체이므로 pNewEffect->AddRef(); 안함
//
//	// 월드 매트릭스의 주소값 전달
//	//pNewEffect->Set_ParentMatrix(m_pTransformCom);
//	//m_listEffects[m_iIndex].push_back(pNewEffect);
//
//	// 컴포넌트, 리소스 준비
//	Ready_EffectComponent(pNewEffect);
//
//	// 이전 Init_EffectList에서 클론하던 작업 이전
//	EFFECT_INFO		tInfo = pNewEffect->Get_EffectInfo();
//
//	_uint iEffectCnt = CRandom::GetInstance()->GetValueF(tInfo.m_fCnt_Min, tInfo.m_fCnt_Max);
//
//	for (_uint i = 0; i < iEffectCnt; ++i)
//	{
//		Engine::CMyEffect*	pClone = (Engine::CMyEffect*)pNewEffect->Clone();
//		pClone->Reset_Info();
//		m_listEffects[m_iIndex].push_back(pClone);
//	}
//
//	Engine::Safe_Release(pNewEffect);
//
//	++m_iIndex;
//
//	return NOERROR;
//}
//
////void CEffectOBJ_CL::Init_EffectList(Engine::CMyEffect * pOriginEffect, _uint & iListIndex)
////{
////	Engine::CMyEffect*	pNewOrigin = (Engine::CMyEffect*)pOriginEffect->Clone();
////	EFFECT_INFO		tInfo = pNewOrigin->Get_EffectInfo(); // 원본
////
////	//Reset_EffectList(iListIndex); // 원본 포함해서 삭제
////
////	_uint iEffectCnt = CRandom::GetInstance()->GetValueF(tInfo.m_fCnt_Min, tInfo.m_fCnt_Max);
////
////	for (_uint i = 0; i < iEffectCnt; ++i)
////	{
////		Engine::CMyEffect*	pClone = (Engine::CMyEffect*)pNewOrigin->Clone();
////		pClone->Reset_Info();
////		m_listEffects[iListIndex].push_back(pClone);
////	}
////
////	Engine::Safe_Release(pNewOrigin);
////}
//
//HRESULT CEffectOBJ_CL::Ready_GameObject(void)
//{
//	if (FAILED(Ready_Component()))
//		return E_FAIL;
//
//	return NOERROR;
//}
//
//_int CEffectOBJ_CL::Update_GameObject(const _float & fTimeDelta)
//{
//	CEffectOBJ::Update_GameObject(fTimeDelta);
//
//	for (auto& ListEffect : m_listEffects)
//	{
//		for (auto& pEffectCom : ListEffect)
//			pEffectCom->Update_GameObject(fTimeDelta);
//	}
//
//	if (nullptr != m_pRendererCom)
//		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_ALPHA, this);
//
//
//	return 0;
//}
//
//void CEffectOBJ_CL::Render_GameObject(void)
//{
//	if (nullptr == m_pShaderCom)
//		return;
//
//	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
//	if (nullptr == pEffect)
//		return;
//
//	pEffect->Begin(nullptr, 0);
//
//	for (auto& ListEffect : m_listEffects)
//	{
//		for (auto& pEffectCom : ListEffect)
//		{
//			pEffectCom->Render_GameObject(pEffect);
//			pEffect->CommitChanges();
//		}
//	}
//
//	pEffect->End();
//}
//
//HRESULT CEffectOBJ_CL::Ready_Component(void)
//{
//	Engine::CComponent*		pComponent = nullptr;
//
//	// For.Transform
//	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CTransform");
//	if (nullptr == pComponent)
//		return E_FAIL;
//	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"CTransform", pComponent));
//	m_pTransformCom->AddRef();
//
//	// For.Renderer
//	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(0, L"CRenderer");
//	if (nullptr == pComponent)
//		return E_FAIL;
//	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CRenderer", pComponent));
//	m_pRendererCom->AddRef();
//
//	// For.Shader
//	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CShader_Effect");
//	if (nullptr == pComponent)
//		return E_FAIL;
//	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"CShader", pComponent));
//	m_pShaderCom->AddRef();
//
//	return NOERROR;
//}
//
//void CEffectOBJ_CL::Reset_EffectList(_uint & iListIndex)
//{
//}
//
//HRESULT CEffectOBJ_CL::Ready_EffectComponent(Engine::CMyEffect* pNewEffect)
//{
//	EFFECT_INFO		tInfo = pNewEffect->Get_EffectInfo();
//	_uint	iEffectType = (EFFECT_INFO::PASS_TYPE)tInfo.m_ePassType;
//
//	Engine::CComponent*		pComponent = nullptr;
//
//	switch (iEffectType)
//	{
//	case EFFECT_INFO::PASS_PARTICLE:
//		pComponent = CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, tInfo.m_szTexturePath[0]);
//		if (nullptr == pComponent)
//			return E_FAIL;
//		dynamic_cast<CMyEffect_Particle*>(pNewEffect)->Set_TexComponent((Engine::CTexture*)pComponent);
//		break;
//	case EFFECT_INFO::PASS_MASKING:
//		for (int i = 0; i < tInfo.m_iTexCnt; ++i)
//		{
//			pComponent = CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, tInfo.m_szTexturePath[i]);
//			if (nullptr == pComponent)
//				return E_FAIL;
//			dynamic_cast<CMyEffect_Buff*>(pNewEffect)->Set_TexComponent((Engine::CTexture*)pComponent, i);
//		}
//		pNewEffect->Reset_Info();
//		break;
//	}
//
//	return NOERROR;
//}
//
//CEffectOBJ_CL * CEffectOBJ_CL::Create(LPDIRECT3DDEVICE9 pGraphicDev)
//{
//	CEffectOBJ_CL*		pInstance = new CEffectOBJ_CL(pGraphicDev);
//
//	if (FAILED(pInstance->Ready_GameObject()))
//	{
//		MSG_BOX("CEffectOBJ_CL Create Failed");
//		Engine::Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//#ifndef _FINAL
//void CEffectOBJ_CL::Debug_Render()
//{
//	CDebug::GetInstance()->Add_DebugText("CEffectOBJ_CL");
//}
//#endif
//_ulong CEffectOBJ_CL::Free(void)
//{
//	_ulong		dwRefCnt = 0;
//
//	dwRefCnt = Engine::CEffectOBJ::Free();
//
//	return dwRefCnt;
//}
