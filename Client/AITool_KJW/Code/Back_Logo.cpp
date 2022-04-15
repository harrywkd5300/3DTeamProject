#include "stdafx.h"
#include "..\Headers\Back_Logo.h"

#include "Component_Manager.h"
#include "MyFont.h"

CBack_Logo::CBack_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	//, m_pTransformCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pShaderCom(nullptr)
{
#ifndef _FINAL
	Tag(L"CBack_Logo", std::bind(&CBack_Logo::Debug_Render, this));
#endif

}

HRESULT CBack_Logo::Ready_GameObject(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;
		

	m_fX = BACKSIZEX / 2.f;
	m_fY = BACKSIZEY / 6.f;
	m_fSizeX = BACKSIZEX / 1.5f;
	m_fSizeY = BACKSIZEY / 1.7f;


	m_fKeyX = BACKSIZEX / 2.f + BACKSIZEX/8;
	m_fKeyY = (BACKSIZEY / 5.f) * 3.5f;
	m_fKeySizeX = 41.5;
	m_fKeySizeY = 41;


	m_fBackX = BACKSIZEX / 2.f;
	m_fBackY = BACKSIZEY / 2.f;
	m_fBackSizeX = BACKSIZEX;
	m_fBackSizeY = BACKSIZEY;

	m_fShuriKenX = 0 + BACKSIZEX / 20;
	m_fShuriKenY = BACKSIZEY - BACKSIZEY / 10;
	m_fShuriKenSizeX = 100.f;
	m_fShuriKenSizeY = 100.f;


	return NOERROR;
}

_int CBack_Logo::Update_GameObject(const _float & fTimeDelta)
{
	//m_pTransformCom->Rotation_Y(45.0f, fTimeDelta);
		
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	
	Set_AlphaMotion(fTimeDelta);

	m_fShuriKenAngle -= fTimeDelta * 200.f;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_PRIORITY, this);

	return 0;
}

void CBack_Logo::Render_GameObject(void)
{
	if (nullptr == m_pBufferCom
		|| nullptr == m_pShaderCom)
		return;	

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return ;

	pEffect->AddRef();

	SetUp_OnContantTable_BackScreen(pEffect);
		
	pEffect->Begin(nullptr, 0);	
	pEffect->BeginPass(4);	
	
	m_pBufferCom->Render_Buffer();

	SetUp_OnContantTable(pEffect);

	m_pBufferCom->Render_Buffer();

	if (m_bRenderEnterKey == true)
	{
		SetUp_OnContantTable_TextureKey(pEffect);
		m_pBufferCom->Render_Buffer();
	}	
	else
	{
		SetUp_OnContantTable_BackShuriKen(pEffect);
		m_pBufferCom->Render_Buffer();

		SetUp_OnContantTable_ShuriKen(pEffect);
		m_pBufferCom->Render_Buffer();
	}

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	Render_Font();
}

void CBack_Logo::Set_AlphaMotion(const _float & fTimeDelta)
{
	if (m_bRenderEnterKey == true)
	{
		if (m_IsChangeAlpha == true)
			m_fAlpha -= fTimeDelta;
		else
			m_fAlpha += fTimeDelta;


		if (m_fAlpha < 0)
		{
			m_IsChangeAlpha = false;
			m_fAlpha = 0;
		}
		else if (m_fAlpha > 1.f)
		{
			m_IsChangeAlpha = true;
			m_fAlpha = 1.f;
		}
	}
		

	if (m_IsChangeFontAlpha == true)
		m_fFontAlpha -= fTimeDelta;
	else
		m_fFontAlpha += fTimeDelta;


	if (m_fFontAlpha < 0)
	{
		m_IsChangeFontAlpha = false;
		m_fFontAlpha = 0;
	}
	else if (m_fFontAlpha > 1.f)
	{
		m_IsChangeFontAlpha = true;
		m_fFontAlpha = 1.f;
	}


}
void CBack_Logo::Render_Font()
{
	if (m_bRenderEnterKey == false)
	{
		m_pFont->Set_FontSetting(84.f / 2.f,100.f / 2.f, 0.f, _vec4(1.f, 1.f, 1.f, m_fFontAlpha));
		m_pFont->Draw2DFont(BACKSIZEX / 10, BACKSIZEY - BACKSIZEY / 10, "Loading...");
	}
	else
	{
		m_pFont->Set_FontSetting(84.f / 2.f,100.f / 2.f, 0.f, _vec4(0.f, 1.f, 0.f, m_fAlpha));
		m_pFont->Draw2DFont(m_fKeyX - (m_fKeySizeX + 50 * 5), m_fKeyY, "Select");
	}
		
}
HRESULT CBack_Logo::Ready_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->AddRef();

	// For.Buffers
	pComponent = m_pBufferCom = (Engine::CRect_Texture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CRect_Texture");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->AddRef();

	// For.Texture
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_BackLogo");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->AddRef();

	// For.Texture
	pComponent = m_pKeyTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_KeyIcon");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_KeyTexture", pComponent));
	m_pKeyTextureCom->AddRef();

	// For.Texture
	pComponent = m_pBackTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Texture_UpsellScreen");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_BackTexture", pComponent));
	m_pBackTextureCom->AddRef();

	// For.Texture
	pComponent = m_pShuriKenTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTexture_ShuriKen");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_ShuriKenTexture", pComponent));
	m_pShuriKenTextureCom->AddRef();

	// For.Renderer
	pComponent = m_pRendererCom = (Engine::CRenderer*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC,  L"CRenderer");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Renderer", pComponent));
	m_pRendererCom->AddRef();

	// For.Shader
	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Component_Shader_UI");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->AddRef();


	m_pFont = CMyFont::Create(m_pGraphicDev, L"CTexture_MarioFont", 17.f, 6.f);

	return NOERROR;
}

HRESULT CBack_Logo::SetUp_OnContantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, float(BACKSIZEX), float(BACKSIZEY), 0.0f, 1.f);

	_vec3		vPoint(0.f, 0.f, 0.f);
	_vec3		vPoint1(float(BACKSIZEX), float(BACKSIZEY), 0.f);
	_vec3		vPoint2(float(BACKSIZEX) * -1.f, float(BACKSIZEY) * -1.f, 0.f);
	_vec3		vResult(0.f, 0.f, 0.f);
	
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fSizeX;
	matView._22 = m_fSizeY;
	matView._33 = 1.f;
	matView._41 = m_fX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", 1.f);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture");

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CBack_Logo::SetUp_OnContantTable_TextureKey(LPD3DXEFFECT pEffect)
{	
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, float(BACKSIZEX), float(BACKSIZEY), 0.0f, 1.f);
	
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fKeySizeX;
	matView._22 = m_fKeySizeY;
	matView._33 = 1.f;
	matView._41 = m_fKeyX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fKeyY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fAlpha);


	m_pKeyTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture",41);

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CBack_Logo::SetUp_OnContantTable_BackScreen(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, float(BACKSIZEX), float(BACKSIZEY), 0.0f, 1.f);

	_vec3		vPoint(0.f, 0.f, 0.f);
	_vec3		vPoint1(float(BACKSIZEX), float(BACKSIZEY), 0.f);
	_vec3		vPoint2(float(BACKSIZEX) * -1.f, float(BACKSIZEY) * -1.f, 0.f);
	_vec3		vResult(0.f, 0.f, 0.f);

	D3DXMatrixIdentity(&matView);

	matView._11 = m_fBackSizeX;
	matView._22 = m_fBackSizeY;
	matView._33 = 1.f;
	matView._41 = m_fBackX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fBackY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", 1);


	m_pBackTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture");	

	pEffect->CommitChanges();
	Safe_Release(pEffect);

	return NOERROR;
}

HRESULT CBack_Logo::SetUp_OnContantTable_ShuriKen(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->Rotation_Z(m_fShuriKenAngle, 1);
	
	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, float(BACKSIZEX), float(BACKSIZEY), 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fShuriKenSizeX;
	matView._22 = m_fShuriKenSizeY;
	matView._33 = 1.f;
	matView._41 = m_fShuriKenX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fShuriKenY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", 1);
	
	m_pShuriKenTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture");
	pEffect->CommitChanges();
	Safe_Release(pEffect);
	return NOERROR;
}

HRESULT CBack_Logo::SetUp_OnContantTable_BackShuriKen(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->Rotation_Z(m_fShuriKenAngle, 1);

	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, float(BACKSIZEX), float(BACKSIZEY), 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_fShuriKenSizeX * 0.5f;
	matView._22 = m_fShuriKenSizeY * 0.5f;
	matView._33 = 1.f;
	matView._41 = m_fShuriKenX - BACKSIZEX * 0.5f;

	// 300
	matView._42 = -m_fShuriKenY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fAlpha", m_fShuriKenAlpha);

	m_pShuriKenTextureCom->SetUp_OnShader(pEffect, "g_UIFixTexture",1);
	pEffect->CommitChanges();
	Safe_Release(pEffect);
	return NOERROR;
}

CBack_Logo * CBack_Logo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBack_Logo *	pInstance = new CBack_Logo(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CBack_Logo Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CBack_Logo::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CBack_Logo");

}
#endif
_ulong CBack_Logo::Free(void)
{
	_ulong		dwRefCnt = 0;

	
	Safe_Release(m_pFont);
	Safe_Release(m_pShuriKenTextureCom);
	Engine::Safe_Release(m_pShaderCom);
	//Engine::Safe_Release(m_pTransformCom);
	Engine::Safe_Release(m_pTextureCom);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pBufferCom);
	
	Engine::Safe_Release(m_pKeyTextureCom);
	Engine::Safe_Release(m_pBackTextureCom);
	

	dwRefCnt = Engine::CGameObject::Free();

	return dwRefCnt;
}
