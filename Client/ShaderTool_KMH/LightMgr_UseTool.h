#pragma once
#include "Tool_Defines.h"
#include "GameObject.h"

#include "Light_Manager.h"

namespace Engine
{
	class CRenderer;
}

class CTool_Light;
class CLightMgr_UseTool :
	public Engine::CGameObject
{
public:
	typedef struct tagLightGroupData
	{
		_tchar			szName[128] = L"";
		vector<_uint>	vecIndex;

		tagLightGroupData( const _tchar* _pName )
		{
			lstrcpy( szName, _pName );
		}

	} LIGHTGROUPDATA;

	typedef struct tagLightData
	{
		_tchar		szName[128] = L"";
		_tchar		szGroupName[128] = L"";
		D3DLIGHT9	LightInfo;
		LIGHTGROUPDATA*	pLightGroupData = nullptr;
		_vec3		vLightPower = _vec3( 1.f, 1.f, 1.f );

		tagLightData( const _tchar* _pName, const _tchar* _pGroupName, const D3DLIGHT9& _LightInfo )
		{
			lstrcpy( szName, _pName );
			lstrcpy( szGroupName, _pGroupName );
			LightInfo = _LightInfo;
		}

	} LIGHTDATA;
public:
	explicit CLightMgr_UseTool( LPDIRECT3DDEVICE9 _pGraphicDev );
	virtual ~CLightMgr_UseTool() = default;
public:
	void Set_LightMode( const LIGHTMODE& _LightMode );
	void Set_IsShowLightDebug( const _bool& _bIsShowLightDebug );
public:
	HRESULT Get_LightName( _tchar* _pOut, const _tchar* _pGroupTag, const _uint& _iIndex );
public:
	HRESULT Add_Light( const _tchar* _pGroupTag, const _tchar* _pLightName, const D3DLIGHT9* _pLightInfo, const _vec3& _vLightPower );
	HRESULT Add_LightGroup( const _tchar* _pGroupTag );
	HRESULT Delete_Light( void );
	HRESULT Delete_Light( const _tchar* _pGroupTag );
	HRESULT Delete_Light( const _tchar* _pGroupTag, const _uint& _iIndex );
	HRESULT Change_LightGroupName( const _uint& _iIndex, const _tchar* _pGroupTag );
	HRESULT Modify_LightData( const _uint& _iIndex, const D3DLIGHT9* _pLightInfo, const _vec3& _vLightPower );
	HRESULT Save_File( const _tchar* _pFilePath );
	HRESULT Load_File( const _tchar* _pFilePath );
	HRESULT Move_LightGroup( _tchar* _pOutGroupName, const _tchar* _pLightTag, const _bool& _bMoveUpGroup );
public:
	HRESULT Ready_LightMgr( void );
	virtual _int Update_GameObject( const _float& _fTimeDelta );
	virtual void Render_GameObject( void );
private:
	CTool_Light*			m_pLightTool = nullptr;
private:
	CRenderer*				m_pRendererCom = nullptr;
private:
	// 빛 관련 정보..
	_uint					m_iLightTotalCount = 0;	// 생성된 빛 총 개수..
private:
	// 디버그 관련..
	LIGHTMODE				m_eLightMode = LightMode_None;
	_bool					m_bIsShowLightDebug = false;
public:
	// 빛 그룹 관련 정보..
	vector<LIGHTDATA>					m_vecLightIndexData;			// 인덱스별로 분류한 라이트 데이터..
	vector<LIGHTGROUPDATA>				m_vecLightGroupData;			// 그룹별로 분류한 라이트 데이터..
	CLight_Manager::LIGHTCONTAINER*		m_pLightContainer = nullptr;	// CLight_Manager 클래스의 빛을 저장하고 있는 컨테이너의 주소..
public:
	void Update_NoneMode( const _float& _fTimeDelta );
	void Update_LightCreateMode( const _float& _fTimeDelta );
	void Update_LightDeleteMode( const _float& _fTimeDelta );
private:
	CLightMgr_UseTool::LIGHTGROUPDATA* Find_LightGroup( const _tchar* _pGroupTag );
	HRESULT Delete_Light( const _uint& _iIndex );
public:
	static CLightMgr_UseTool* Create( LPDIRECT3DDEVICE9 _pGraphicDev );
private:
	virtual _ulong Free( void ) override;

};

