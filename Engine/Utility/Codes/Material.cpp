#include "..\Headers\Material.h"

USING(Engine)

CMaterial::CMaterial(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CMaterial", std::bind(&CMaterial::Debug_Render, this));
#endif

}

CMaterial::CMaterial(const CMaterial & rhs)
	: CComponent(rhs)
	, m_MaterialInfo(rhs.m_MaterialInfo)
{
#ifndef _FINAL
	Tag(L"CMaterial", std::bind(&CMaterial::Debug_Render, this));
#endif

}


HRESULT CMaterial::Ready_Material(D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Specular, const _float& fPower)
{
	ZeroMemory(&m_MaterialInfo, sizeof(D3DMATERIAL9));

	m_MaterialInfo.Diffuse = Diffuse;
	m_MaterialInfo.Ambient = Ambient;
	m_MaterialInfo.Specular = Specular;
	m_MaterialInfo.Power = fPower;

	return NOERROR;
}

void CMaterial::SetUp_OnGraphicDev(void)
{
	m_pGraphicDev->SetMaterial(&m_MaterialInfo);
}

CComponent * CMaterial::Clone(void)
{
	return new CMaterial(*this);
}

CMaterial * CMaterial::Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Specular, const _float& fPower)
{
	CMaterial *		pInstance = new CMaterial(pGraphicDev);

	if (FAILED(pInstance->Ready_Material(Diffuse, Ambient, Specular, fPower)))
	{
		MSG_BOX("CMaterial Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CMaterial::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CMaterial");
	CDebug::GetInstance()->Add_DebugText("m_MaterialInfo.Diffuse(rgba): %f %f %f %f", m_MaterialInfo.Diffuse.r, m_MaterialInfo.Diffuse.g, m_MaterialInfo.Diffuse.b, m_MaterialInfo.Diffuse.a);
	CDebug::GetInstance()->Add_DebugText("m_MaterialInfo.Ambient(rgba): %f %f %f %f", m_MaterialInfo.Ambient.r, m_MaterialInfo.Ambient.g, m_MaterialInfo.Ambient.b, m_MaterialInfo.Ambient.a);
	CDebug::GetInstance()->Add_DebugText("m_MaterialInfo.Specular(rgba): %f %f %f %f", m_MaterialInfo.Specular.r, m_MaterialInfo.Specular.g, m_MaterialInfo.Specular.b, m_MaterialInfo.Specular.a);
	CDebug::GetInstance()->Add_DebugText("m_MaterialInfo.Emissive(rgba): %f %f %f %f", m_MaterialInfo.Emissive.r, m_MaterialInfo.Emissive.g, m_MaterialInfo.Emissive.b, m_MaterialInfo.Emissive.a);
	CDebug::GetInstance()->Add_DebugText("m_MaterialInfo.Power: %f ", m_MaterialInfo.Power);
}
#endif
_ulong CMaterial::Free(void)
{
	return CComponent::Free();
}

