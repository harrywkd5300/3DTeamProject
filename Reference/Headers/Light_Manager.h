#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CLight;
class CFrustum;
typedef struct tagLightGroup
{
	vector<CLight*>			vecLight;
	_tchar*					pName = nullptr;
	_bool					bIsActive = true;

	tagLightGroup(const _tchar* _pName, CLight* _pLight, const _bool& _bIsActive = true)
	{
		if ( nullptr != pName )
			Engine::Safe_Delete_Array( pName );

		_uint iLength = lstrlen( _pName );
		pName = new _tchar[iLength + 1];
		lstrcpy( pName, _pName );

		vecLight.push_back( _pLight );
	}

	tagLightGroup(const _tchar* _pName, const _bool& _bIsActive = true)
	{
		if ( nullptr != pName )
			Engine::Safe_Delete_Array( pName );

		_uint iLength = lstrlen( _pName );
		pName = new _tchar[iLength + 1];
		lstrcpy( pName, _pName );
	}

} LIGHTGROUP;

class DLL_EXPORT CLight_Manager : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
public:
	typedef vector<LIGHTGROUP>			LIGHTCONTAINER;
private:
	explicit CLight_Manager(void);
	virtual ~CLight_Manager() = default;
public:
	void Set_LightGroupActive( const _tchar* pLayerTag, const _bool& bIsActive );
public:
	HRESULT Register_Light( CLight* pLight );
	HRESULT Add_Light(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag, const D3DLIGHT9* pLightInfo, const _vec3& _vLightPower = _vec3( 1.f, 1.f, 1.f ));
	HRESULT LoadLightFile( LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath );
	HRESULT Modify_Light( const _tchar* pLayerTag, const _uint& _iIndex, const D3DLIGHT9* pLightInfo, const _vec3& _vLightPower );
#ifdef LIGHT_DEBUGING
	HRESULT Add_LightGroup(const _tchar* pLayerTag);
	void Render_DebugLight( void );
	_bool CheckPicking( D3DLIGHT9* _pOut, _tchar* _pOutGroupName, _uint* _pOutIndex, const _vec3& _vRayPos, const _vec3& _vRayDir );
	void Set_LightDetailDraw( const _tchar* _pGroupTag, const _uint& _iIndex, const _bool& _bisDetailDraw );
#endif
public:
	void Release_Light( void );
	void Release_Light( const _tchar* pLayerTag );
	void Release_Light( const _tchar* pLayerTag, const _uint& _iIndex );
	void Render_Light( const _matrix& _matViewProj, CFrustum* _pFrustum, LPD3DXEFFECT pEffect);
private:
	LIGHTCONTAINER						m_LightContainer;
	list<CLight*>						m_DynamicLightList;
private:
	LIGHTGROUP*	Find_LightGroup( const _tchar* pLayerTag );
#ifdef USELIGHTTOOL
public:
	LIGHTCONTAINER* Get_LightContainer( void ) { return &m_LightContainer; }
#endif
#ifndef _FINAL
public:
	void Debug_Render();
#endif
public:
	virtual _ulong Free(void);
};

END