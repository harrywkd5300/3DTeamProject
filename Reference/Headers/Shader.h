#pragma once

#include "Component.h"

BEGIN(Engine)

class DLL_EXPORT CShader : public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9  pGraphicDev);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;
public:
	LPD3DXEFFECT Get_EffectHandle(void) {
		return m_pEffect; }
public:
	HRESULT Ready_Shader(const _tchar* pFilePath);
private:
	LPD3DXEFFECT			m_pEffect = nullptr;

	
public:
	virtual CComponent* Clone(void);
	static CShader* Create(LPDIRECT3DDEVICE9  pGraphicDev, const _tchar* pFilePath);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
	virtual _ulong Free(void);
};

END