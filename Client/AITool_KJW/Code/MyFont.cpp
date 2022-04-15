#include "stdafx.h"
#include "..\Headers\MyFont.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"
#include "Target_Manager.h"


CMyFont::CMyFont(LPDIRECT3DDEVICE9 pGraphicDev)
	:m_pGraphicDev(pGraphicDev)
{
	m_pGraphicDev->AddRef();
}

HRESULT CMyFont::Ready_Component(_tchar * pComponentTag)
{
	Engine::CComponent*		pComponent = nullptr;

	// For.Transform
	pComponent = m_pTransformCom = (Engine::CTransform*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CTransform");
	if (nullptr == pComponent)
		return E_FAIL;	
	
	pComponent = m_pTextureCom = (Engine::CTexture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, pComponentTag);
	if (nullptr == pComponent)
		return E_FAIL;		

	pComponent = m_pShaderCom = (Engine::CShader*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CShader_Font");
	if (nullptr == pComponent)
		return E_FAIL;	

	pComponent = m_pBufferCom = (Engine::CRect_Texture*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"CRect_Texture");
	if (nullptr == pComponent)
		return E_FAIL;	

	return NOERROR;
}

HRESULT CMyFont::Ready_Font(_tchar * pComponentTag, const _float& fMaxWidthNum, const _float& fMaxHeightNum, const _int& iStartAscllCode, const _int& iEndAscllCode)
{
	if (FAILED(Ready_Component(pComponentTag)))
		return E_FAIL;

	m_tFontInfo.iStartAscllCode = iStartAscllCode;
	m_tFontInfo.iEndAscllCode = iEndAscllCode;
	m_tFontInfo.fMaxHeightNum = fMaxHeightNum;
	m_tFontInfo.fMaxWidthNum = fMaxWidthNum;
	
	return NOERROR;
}

void CMyFont::Set_FontSetting(const _float & fWidth,const _float & fHeight, const _float & fSpace, const _vec4 & vColor,const _float& fDir)
{
	m_tFontInfo.fWidth = fWidth;
	m_tFontInfo.fHeight = fHeight;
	m_tFontInfo.fSpace = fSpace;
	m_tFontInfo.vColor = vColor;
	m_tFontInfo.fDir = fDir;
}

void CMyFont::Draw2DFont(const _float & fX, const _float & fY,const char* strTextFormat ,...)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;
	
	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_tFontInfo.strText, 256, strTextFormat, ap);
	va_end(ap);
	puts(m_tFontInfo.strText);
	
	_int iTextSize = (_int)strlen(m_tFontInfo.strText);

	if (iTextSize >= MAX_PATH)
	{
		MSG_BOX("The text is over 260(MAX_PATH)");
		return;
	}

	pEffect->Begin(nullptr, 0);
	for (_int i = 0; i < iTextSize; i++)
	{	
		SetUp_2DFontConstantTable(pEffect, (_int)(m_tFontInfo.strText[i]),i, fX, fY);
		pEffect->BeginPass(TYPE_2D);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}

	pEffect->End();
}
void CMyFont::Draw3DFont(const _vec3* pScale, const _vec3* pAngle, const _vec3* pPos, const char* strTextFormat, ...)
{
	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;	

	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_tFontInfo.strText, 256, strTextFormat, ap);
	va_end(ap);
	puts(m_tFontInfo.strText);


	_int iTextSize = (_int)strlen(m_tFontInfo.strText);

	if (iTextSize >= MAX_PATH)
	{
		MSG_BOX("The text is over 260(MAX_PATH)");
		return;
	}

	pEffect->Begin(nullptr, 0);

	for (_int i = 0; i < iTextSize; i++)
	{
		SetUp_3DFontConstantTable(pEffect, (_int)(m_tFontInfo.strText[i]), i, pScale, pAngle, pPos );
		pEffect->BeginPass(TYPE_3D);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}

	pEffect->End();

}
void CMyFont::Draw3DFont_NotDepth(const _matrix * pMatrix, const char * strTextFormat, ...)
{

	if (nullptr == m_pBufferCom)
		return;

	if (nullptr == m_pGraphicDev)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();

	if (nullptr == pEffect)
		return;

	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_tFontInfo.strText, 256, strTextFormat, ap);
	va_end(ap);
	puts(m_tFontInfo.strText);


	_int iTextSize = (_int)strlen(m_tFontInfo.strText);

	if (iTextSize >= MAX_PATH)
	{
		MSG_BOX("The text is over 260(MAX_PATH)");
		return;
	}

	pEffect->Begin(nullptr, 0);

	for (_int i = 0; i < iTextSize; i++)
	{
		SetUp_3DFontConstantTable(pEffect, (_int)(m_tFontInfo.strText[i]), i, pMatrix);
		pEffect->BeginPass(TYPE_NOTDEPTH_3D);
		m_pBufferCom->Render_Buffer();
		pEffect->EndPass();
	}

	pEffect->End();


}
HRESULT CMyFont::SetUp_2DFontConstantTable(LPD3DXEFFECT pEffect,const _int& iFontNum, const _int& iCurNum,const _float& fX, const _float& fY)
{
	if (pEffect == nullptr)
		return E_FAIL;

	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;

	D3DXMatrixIdentity(&matView);

	// 직교투영행렬을 만든다.
	D3DXMatrixOrthoLH(&matProj, (_float)BACKSIZEX, (_float)BACKSIZEY, 0.0f, 1.f);
	D3DXMatrixIdentity(&matView);

	matView._11 = m_tFontInfo.fWidth;
	matView._22 = m_tFontInfo.fHeight;
	matView._33 = 1.f;
	matView._41 = fX + (m_tFontInfo.fWidth * iCurNum * m_tFontInfo.fDir)- BACKSIZEX * 0.5f;
	
	matView._41 += m_tFontInfo.fSpace * (_float)iCurNum;

	// 300
	matView._42 = -fY + BACKSIZEY * 0.5f;

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetVector("g_vColor",&m_tFontInfo.vColor);
	
	pEffect->SetFloat("g_fMaxWidthNum", m_tFontInfo.fMaxWidthNum);
	pEffect->SetFloat("g_fMaxHeightNum", m_tFontInfo.fMaxHeightNum);

		
	_vec2 vUVOffset;
	vUVOffset.x = ((iFontNum - m_tFontInfo.iStartAscllCode) % (int)m_tFontInfo.fMaxWidthNum) / m_tFontInfo.fMaxWidthNum;	
	vUVOffset.y = ((iFontNum - m_tFontInfo.iStartAscllCode) / (int)m_tFontInfo.fMaxWidthNum) / m_tFontInfo.fMaxHeightNum;

	pEffect->SetFloatArray("g_vUVOffset", vUVOffset, 2);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");
	
	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CMyFont::SetUp_3DFontConstantTable(LPD3DXEFFECT pEffect, const _int & iFontNum, const _int & iCurNum, const _vec3* pScale, const _vec3* pAngle, const _vec3* pPos)
{
	if (pEffect == nullptr)
		return E_FAIL;

	pEffect->AddRef();

	
	m_pTransformCom->Set_Infomation(CTransform::INFO_SCALE, &_vec3(((m_tFontInfo.fWidth*0.005f)*(pScale->x)), ((m_tFontInfo.fHeight*0.005f)*(pScale->y)), 0.005f*(pScale->z)));
	m_pTransformCom->Set_Infomation(CTransform::INFO_ANGLE, pAngle);

	_vec3 vRight = *m_pTransformCom->Get_WorldRightVec_FORCED();
	D3DXVec3Normalize(&vRight, &vRight); 

	_vec3 vLook = *m_pTransformCom->Get_WorldLookVec_FORCED();
	D3DXVec3Normalize(&vLook, &vLook);

	_float fBufferSize = m_tFontInfo.fWidth*0.005f;

	_vec3 vPos = *pPos + ((vRight*fBufferSize) * (_float)iCurNum * m_tFontInfo.fDir) - (vLook*fBufferSize*0.01f *(_float)iCurNum);

	vPos += (vRight*fBufferSize) * m_tFontInfo.fSpace * (_float)iCurNum;

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &vPos);// + ((vRight*fBufferSize) * m_tFontInfo.fSpace))
		
	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);  // 셋팅된 카메라 x 현재 플레이어 카메라의 View
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
		
	//CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"RenderTarget_Depth", "g_DepthTexture");
	//pEffect->SetMatrix("g_matInvView",&CCamera_Manager::GetInstance()->Get_InverseViewMatrix());

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetVector("g_vColor", &m_tFontInfo.vColor);

	pEffect->SetFloat("g_fMaxWidthNum", m_tFontInfo.fMaxWidthNum);
	pEffect->SetFloat("g_fMaxHeightNum", m_tFontInfo.fMaxHeightNum);

	_vec2 vUVOffset;
	vUVOffset.x = ((iFontNum - m_tFontInfo.iStartAscllCode) % (int)m_tFontInfo.fMaxWidthNum) / m_tFontInfo.fMaxWidthNum;
	vUVOffset.y = ((iFontNum - m_tFontInfo.iStartAscllCode) / (int)m_tFontInfo.fMaxWidthNum) / m_tFontInfo.fMaxHeightNum;

	pEffect->SetFloatArray("g_vUVOffset", vUVOffset, 2);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}
HRESULT CMyFont::SetUp_3DFontConstantTable(LPD3DXEFFECT pEffect, const _int & iFontNum, const _int & iCurNum, const _matrix * pMatrix)
{
	if (pEffect == nullptr)
		return E_FAIL;

	pEffect->AddRef();

	m_pTransformCom->Set_WorldMatrix_ForEffect(*pMatrix);// + ((vRight*fBufferSize) * m_tFontInfo.fSpace))

	m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);  // 셋팅된 카메라 x 현재 플레이어 카메라의 View
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->SetVector("g_vColor", &m_tFontInfo.vColor);

	pEffect->SetFloat("g_fMaxWidthNum", m_tFontInfo.fMaxWidthNum);
	pEffect->SetFloat("g_fMaxHeightNum", m_tFontInfo.fMaxHeightNum);

	_vec2 vUVOffset;
	vUVOffset.x = ((iFontNum - m_tFontInfo.iStartAscllCode) % (int)m_tFontInfo.fMaxWidthNum) / m_tFontInfo.fMaxWidthNum;
	vUVOffset.y = ((iFontNum - m_tFontInfo.iStartAscllCode) / (int)m_tFontInfo.fMaxWidthNum) / m_tFontInfo.fMaxHeightNum;

	pEffect->SetFloatArray("g_vUVOffset", vUVOffset, 2);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");

	pEffect->CommitChanges();

	Safe_Release(pEffect);

	return NOERROR;
}
CMyFont * CMyFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, _tchar * pComponentTag, const _float& fMaxWidthNum, const _float& fMaxHeightNum)
{
	CMyFont *	pInstance = new CMyFont(pGraphicDev);

	if (FAILED(pInstance->Ready_Font(pComponentTag,fMaxWidthNum,fMaxHeightNum)))
	{
		MSG_BOX("CMyFont Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CMyFont::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pGraphicDev);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	
	return 0;
}
