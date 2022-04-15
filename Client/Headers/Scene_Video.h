#pragma once

#include "Defines.h"
#include "Scene.h"

class CLoading;
class CBack_Logo;
class CScene_Video : public Engine::CScene
{
private:
	explicit CScene_Video(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Video() = default;
public:

	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
	
private:
	HRESULT Ready_Video(void);
	HRESULT Ready_LogoComponent(void);	
private:
	HRESULT Change_Scene(SCENETYPE eSceneID);

private:
	HWND			m_hVideo;
public:
	static CScene_Video* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iSceneIdx);
#ifndef _FINAL
public:
	void Debug_Render();
#endif

protected:
	virtual _ulong Free(void) override;
};

