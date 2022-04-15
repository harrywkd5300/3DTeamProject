#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "MyEffect.h" // By.KJY
#include "MyEffect_Buff.h" // By.KJY
#include "MyEffect_Particle.h" // By.KJY
#include "MyFX.h"
#include "EffectOBJ.h"

BEGIN(Engine)

class DLL_EXPORT CEffect_Manager : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	explicit CEffect_Manager();
	virtual ~CEffect_Manager() = default;

public:
	HRESULT Init_Manager(LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT Load_Effect(const _tchar* pFileFullPath);
	HRESULT Load_Effect_Resource(const _tchar* pFileFullPath);

public:
	bool	Use_Effect(const _uint & eSceneID, const _tchar* pEffectTag, CGameObject* pTarget, D3DXMATRIX* pBoneMatrix);
	bool	Use_Effect(const _uint & eSceneID, const _tchar* pEffectTag, _matrix matWorld);
	bool	Use_Effect(const _uint & eSceneID, const _tchar* pEffectTag, _vec3 vPosition);

public:
	CEffectOBJ* Create_Effect(const _tchar* pEffectTag);

	void		Update(const _float& fTimeDelta);
	void		LastUpdate(const _float& fTimeDelta);

private:
	bool	Delete_Effect(CEffectOBJ* pEffect);
private:
	LPDIRECT3DDEVICE9				m_pGraphicDev = nullptr;

	list<CEffectOBJ*>								m_EffectOBJList;
	typedef list<CEffectOBJ*>						EFFECTOBJLIST;

	map<const _tchar*, EFFECTOBJLIST >				m_RecycleEffectOBJMap;
	typedef map<const _tchar*, EFFECTOBJLIST>		EFFECTOBJMAP;
	//map<const _tchar*, EFFECTOBJLIST >				m_EffectOBJMap;
	

	map<const _tchar*, CEffectOBJ* >				m_OriginEffectOBJMap;
	typedef map<const _tchar*, CEffectOBJ*>			ORIGIN_EFFECTOBJMAP;

private:
	//_uint				m_iEffectSize = 0;
	_uint				m_iEffectOriginSize = 0;

public:
#ifndef _FINAL
	void Debug_Render();
	int iStartCount = 0;
	int iDebugMenu = 0;
#endif
public:
	HRESULT Release_CurEffectList();
private:
	virtual _ulong Free(void) final;


};

END