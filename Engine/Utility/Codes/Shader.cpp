#include "..\Headers\Shader.h"

USING(Engine)

CShader::CShader(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
	, m_pEffect(nullptr)
{
#ifndef _FINAL
	Tag(L"CShader", std::bind(&CShader::Debug_Render, this));
#endif

}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
{
	m_pEffect->AddRef();
#ifndef _FINAL
	Tag(L"CShader", std::bind(&CShader::Debug_Render, this));
#endif

}

HRESULT CShader::Ready_Shader(const _tchar * pFilePath)
{
	LPD3DXBUFFER			pErrBuffer = nullptr;

	if (FAILED(D3DXCreateEffectFromFile(m_pGraphicDev, pFilePath, nullptr, nullptr, D3DXSHADER_DEBUG, nullptr, &m_pEffect, &pErrBuffer)))
	{
		MessageBoxA(0, (char*)pErrBuffer->GetBufferPointer(), "Shader Compile Err", MB_OK);
		return E_FAIL;
	}

	if(nullptr != pErrBuffer)
		MessageBoxA(0, (char*)pErrBuffer->GetBufferPointer(), "Shader Compile Warning", MB_OK);

	
	return NOERROR;
}

CComponent * CShader::Clone(void)
{
	return new CShader(*this);
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath)
{
	CShader *		pInstance = new CShader(pGraphicDev);

	if (FAILED(pInstance->Ready_Shader(pFilePath)))
	{
		MSG_BOX("CShader Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}
#ifndef _FINAL
void CShader::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CShader");
}
#endif
_ulong CShader::Free(void)
{
	Safe_Release(m_pEffect);

	return CComponent::Free();
}
