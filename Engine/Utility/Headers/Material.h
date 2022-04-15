#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CMaterial final : public Engine::CComponent
{
private:
	explicit CMaterial(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMaterial(const CMaterial& rhs);
	virtual ~CMaterial() = default;
public:
	void Get_MaterialInfo(D3DMATERIAL9* pMtrlInfo) {
		*pMtrlInfo = m_MaterialInfo; }
public:
	HRESULT Ready_Material(D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Specular, const _float& fPower);
	void SetUp_OnGraphicDev(void);
private:
	D3DMATERIAL9	m_MaterialInfo;
private:
	virtual CComponent* Clone(void);
public:
	static CMaterial* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Specular, const _float& fPower);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);

};

END