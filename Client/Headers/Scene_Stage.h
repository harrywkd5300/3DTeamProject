#pragma once

#include "Defines.h"
#include "Scene.h"

class CScene_Stage : public Engine::CScene
{
private:
	explicit CScene_Stage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Stage() = default;
public:
	virtual HRESULT Ready_Scene(void) override;
	virtual _int Update_Scene(const _float& fTimeDelta) override;
	virtual void Render_Scene(void) override;
private:
	HRESULT Ready_LightInfo(void);
	HRESULT Ready_Decal(void);
	HRESULT Ready_Layer_CCTVCamera(const _tchar* pLayerTag, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp);
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Effect(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UI(const _tchar * pLayerTag);
	HRESULT Ready_Layer_StaticObject(const _tchar* pLayerTag);

	

public:
	static CScene_Stage * Create(LPDIRECT3DDEVICE9 pGraphicDev, const _uint iSceneIdx);
#ifndef _FINAL
public:
	void Debug_Render();
#endif

protected:
	virtual _ulong Free(void) override;
};

