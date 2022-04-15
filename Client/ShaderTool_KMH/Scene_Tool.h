#pragma once
#include "Tool_Defines.h"
#include "Scene.h"

class CScene_Tool :
	public Engine::CScene
{
private:
	explicit CScene_Tool( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CScene_Tool() = default;
public:
	virtual HRESULT Ready_Scene( void ) override;
	virtual _int Update_Scene( const _float& _fTimeDelta ) override;
	virtual _int LastUpdate_Scene( const _float& _fTimeDelta ) override;
	virtual void Render_Scene( void ) override;
private:
	HRESULT Ready_Layer_Camera( const _tchar* _pLayerTag, const _vec3* _pEye, const _vec3* _pAt, const _vec3* _pUp,
								const _float& _fFovY = D3DXToRadian(60.f), const _float& _fAspect = WINCX / WINCY, const _float& _fNear = 0.2f, const _float& _fFar = 1000.f );
	HRESULT Ready_Layer_Terrain( const _tchar* _pLayerTag );
	HRESULT Ready_Layer_Object( const _tchar* _pLayerTag );
public:
	static CScene_Tool* Create( LPDIRECT3DDEVICE9 _pGraphicDev );
private:
	virtual _ulong Free( void ) override;
};

