#pragma once

#include "Defines.h"
#include "Scene.h"

class CLoading;
class CBack_Logo;
class CScene_Logo : public Engine::CScene
{
private:
	explicit CScene_Logo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Logo() = default;
public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
private:
	list<CLoading*>		m_pLoadingList;
	int					m_pLoadCount = 0;
private:
	HRESULT Ready_LogoComponent(void);
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
private:
	HRESULT Change_Scene(SCENETYPE eSceneID);
	HRESULT Write_CellInfo_Gara(void);

private:
	CBack_Logo* m_pLogo = nullptr;
public:
	static CScene_Logo* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iSceneIdx);
#ifndef _FINAL
public:
	void Debug_Render();
#endif

protected:
	virtual _ulong Free(void) override;
};

