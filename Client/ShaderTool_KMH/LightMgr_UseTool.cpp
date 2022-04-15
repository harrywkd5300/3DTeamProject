#include "stdafx.h"
#include "LightMgr_UseTool.h"

#include "Component_Manager.h"
#include "Light_Manager.h"

#include "MainFrm.h"
#include "EditBox.h"
#include "EditSheet.h"
#include "ShaderToolMainView.h"
#include "Tool_Light.h"

CLightMgr_UseTool::CLightMgr_UseTool( LPDIRECT3DDEVICE9 _pGraphicDev )
	: Engine::CGameObject( _pGraphicDev )
{
	m_pLightContainer = CLight_Manager::GetInstance()->Get_LightContainer();
}

void CLightMgr_UseTool::Set_LightMode( const LIGHTMODE& _LightMode )
{
	m_eLightMode = _LightMode;
}

void CLightMgr_UseTool::Set_IsShowLightDebug( const _bool & _bIsShowLightDebug )
{
	m_bIsShowLightDebug = _bIsShowLightDebug;
}

HRESULT CLightMgr_UseTool::Get_LightName( _tchar* _pOut, const _tchar * _pGroupTag, const _uint & _iIndex )
{
	_uint iCount = m_vecLightGroupData.size();
	for ( _uint i = 0; i < iCount; ++i )
	{
		if ( 0 == lstrcmp( m_vecLightGroupData[i].szName, _pGroupTag ) )
		{
			if ( m_vecLightGroupData[i].vecIndex.size() <= _iIndex )
				return E_FAIL;

			_uint iIndex = m_vecLightGroupData[i].vecIndex[_iIndex];

			if ( m_vecLightIndexData.size() == iIndex )
				return E_FAIL;

			lstrcpy( _pOut, m_vecLightIndexData[iIndex].szName );
			
			return NOERROR;
		}
	}

	return E_FAIL;
}

HRESULT CLightMgr_UseTool::Add_Light( const _tchar* _pGroupTag, const _tchar* _pLightName, const D3DLIGHT9* _pLightInfo, const _vec3& _vLightPower )
{
	LIGHTDATA LightData = LIGHTDATA( _pLightName, _pGroupTag, *_pLightInfo );
	
	LIGHTGROUPDATA* pFindGroup = Find_LightGroup( _pGroupTag );
	if ( nullptr == pFindGroup )
	{
		LIGHTGROUPDATA LightGroup( _pGroupTag );
		LightGroup.vecIndex.push_back( m_iLightTotalCount );
		m_vecLightGroupData.push_back( LightGroup );
		LightData.pLightGroupData = &m_vecLightGroupData.back();
		LightData.vLightPower = _vLightPower;
	}
	else
	{
		pFindGroup->vecIndex.push_back( m_iLightTotalCount );
		LightData.pLightGroupData = pFindGroup;
		LightData.vLightPower = _vLightPower;
	}

	m_vecLightIndexData.push_back( LightData );

	CLight_Manager::GetInstance()->Add_Light( m_pGraphicDev, _pGroupTag, _pLightInfo, _vLightPower );
	++m_iLightTotalCount;

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Add_LightGroup( const _tchar * _pGroupTag )
{
	LIGHTGROUPDATA LightGroup( _pGroupTag );
	m_vecLightGroupData.push_back( LightGroup );

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Delete_Light( void )
{
	_uint iCount = m_vecLightGroupData.size();
	for ( _uint i = 0; i < iCount; ++i )
	{
		Delete_Light( m_vecLightGroupData[i].szName );
	}

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Delete_Light( const _tchar * _pGroupTag )
{
	LIGHTGROUPDATA* pFindGroup = Find_LightGroup( _pGroupTag );

	if ( nullptr == pFindGroup )
		return E_FAIL;

	_uint iCount = pFindGroup->vecIndex.size();
	for ( size_t i = 0; i < iCount; i++ )
	{
		_uint iIndex = pFindGroup->vecIndex[i];
		if ( FAILED( Delete_Light( iIndex ) ) )
			return E_FAIL;
	}

	iCount = m_vecLightGroupData.size();
	for ( _uint i = 0; i < iCount; ++i )
	{
		if ( 0 == lstrcmp( m_vecLightGroupData[i].szName, _pGroupTag ) )
		{
			m_vecLightGroupData.erase( m_vecLightGroupData.begin() + i );
			return NOERROR;
		}
	}

	return E_FAIL;
}

HRESULT CLightMgr_UseTool::Delete_Light( const _tchar * _pGroupTag, const _uint & _iIndex )
{
	LIGHTGROUPDATA* pFindGroup = Find_LightGroup( _pGroupTag );

	if ( nullptr == pFindGroup )
		return E_FAIL;

	if ( pFindGroup->vecIndex.size() <= _iIndex )
		return E_FAIL;

	_uint iIndex = pFindGroup->vecIndex[_iIndex];

	if ( FAILED( Delete_Light( iIndex ) ) )
		return E_FAIL;

	pFindGroup->vecIndex.erase( pFindGroup->vecIndex.begin() + _iIndex );
	for ( _uint i = 0; i < pFindGroup->vecIndex.size(); ++i )
	{
		if ( pFindGroup->vecIndex[i] > iIndex )
			--pFindGroup->vecIndex[i];
	}

	return E_FAIL;
}

HRESULT CLightMgr_UseTool::Change_LightGroupName( const _uint & _iIndex, const _tchar * _pGroupTag )
{
	

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Modify_LightData( const _uint& _iIndex, const D3DLIGHT9* _pLightInfo, const _vec3& _vLightPower )
{
	if ( m_vecLightIndexData.size() <= _iIndex )
		return E_FAIL;

	LIGHTGROUPDATA* pFindGroup = Find_LightGroup( m_vecLightIndexData[_iIndex].szGroupName );

	if ( nullptr != pFindGroup )
	{
		_uint iCount = pFindGroup->vecIndex.size();
		for ( _uint i = 0; i < iCount; ++i )
		{
			if ( pFindGroup->vecIndex[i] == _iIndex )
			{
				CLight_Manager::GetInstance()->Modify_Light( m_vecLightIndexData[_iIndex].szGroupName, i, _pLightInfo, _vLightPower );
				m_vecLightIndexData[_iIndex].LightInfo = *_pLightInfo;

				return NOERROR;
			}
		}
	}

	return E_FAIL;
}

HRESULT CLightMgr_UseTool::Save_File( const _tchar * _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
		AfxMessageBox( L"Failed to Save LightFile" );
		return E_FAIL;
	}

	_ulong dwByte = 0;
	_uint iCount = m_vecLightIndexData.size();
	_vec3 vLightPower( 0.f, 0.f, 0.f );

	WriteFile( hFile, &iCount, sizeof( _uint ), &dwByte, nullptr );

	for ( _uint i = 0; i < iCount; ++i )
	{
		_uint iLength = lstrlen( m_vecLightIndexData[i].szGroupName );
		WriteFile( hFile, &iLength, sizeof( _uint ), &dwByte, nullptr );
		WriteFile( hFile, m_vecLightIndexData[i].szGroupName, sizeof( _tchar ) * (iLength + 1), &dwByte, nullptr );
		WriteFile( hFile, &m_vecLightIndexData[i].LightInfo, sizeof( D3DLIGHT9 ), &dwByte, nullptr );
		WriteFile( hFile, &m_vecLightIndexData[i].vLightPower, sizeof( _vec3 ), &dwByte, nullptr );
	}

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Load_File( const _tchar * _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
		AfxMessageBox( L"Failed to Load LightFile" );
		return E_FAIL;
	}

	m_vecLightIndexData.clear();
	m_vecLightGroupData.clear();

	_ulong dwByte = 0;
	_uint iCount = 0;
	_vec3 vLightPower( 0.f, 0.f, 0.f );

	ReadFile( hFile, &iCount, sizeof( _uint ), &dwByte, nullptr );

	for ( _uint i = 0; i < iCount; ++i )
	{
		_uint iLength = 0;
		_tchar szGroupName[256] = L"";
		D3DLIGHT9 LightInfo;
		ReadFile( hFile, &iLength, sizeof( _uint ), &dwByte, nullptr );
		ReadFile( hFile, szGroupName, sizeof( _tchar ) * (iLength + 1), &dwByte, nullptr );
		ReadFile( hFile, &LightInfo, sizeof( D3DLIGHT9 ), &dwByte, nullptr );
		ReadFile( hFile, &vLightPower, sizeof( _vec3 ), &dwByte, nullptr );

		_tchar szName[256] = L"";
		switch ( LightInfo.Type )
		{
			case D3DLIGHT_DIRECTIONAL:
				swprintf_s( szName, L"Directional%d", iCount );
				break;
			case D3DLIGHT_POINT:
				swprintf_s( szName, L"Point%d", iCount );
				break;
			case D3DLIGHT_SPOT:
				swprintf_s( szName, L"Spot%d", iCount );
				break;
		}

		Add_Light( szGroupName, szName, &LightInfo, vLightPower );
	}

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Move_LightGroup( _tchar* _pOutGroupName, const _tchar* _pLightTag, const _bool& _bMoveUpGroup )
{
	// 라이트 그룹을 움직이려면 ?????
	// 1. 라이트 그룹이 있는 곳을 찾는다..
	// 2. 라이트 그룹에서 현재 라이트가 몇 번째 있는지 찾는다..
	// 3. 라이트 그룹에서 현재 라이트를 뺀다..
	// 4. 이동하려는 라이트 그룹에 추가..
	for ( _uint i = 0; i < m_iLightTotalCount; ++i )
	{
		if ( 0 == lstrcmp( m_vecLightIndexData[i].szName, _pLightTag ) )
		{
			LIGHTDATA*		pCurLight = &m_vecLightIndexData[i];
			LIGHTGROUPDATA* pCurGroup = nullptr;
			LIGHTGROUPDATA*	pMoveGroup = nullptr;

			_uint jLength = 0;
			_bool bFind = false;

			jLength = m_vecLightGroupData.size() - (1 - _bMoveUpGroup);
			for ( size_t j = 0 + _bMoveUpGroup; j < jLength; ++j )
			{
				if ( 0 == lstrcmp( m_vecLightGroupData[j].szName, pCurLight->szGroupName ) )
				{
					bFind = true;
					pCurGroup = &m_vecLightGroupData[j];
					if ( _bMoveUpGroup )
						pMoveGroup = &m_vecLightGroupData[j - 1];
					else
						pMoveGroup = &m_vecLightGroupData[j + 1];
					break;
				}
			}

			if ( false == bFind )
				return E_FAIL;

			jLength = pCurGroup->vecIndex.size();
			for ( _uint j = 0; j < jLength; ++j )
			{
				_uint iIndex = pCurGroup->vecIndex[j];
				if ( 0 == lstrcmp( m_vecLightIndexData[iIndex].szName, _pLightTag ) )
				{
					pCurGroup->vecIndex.erase( pCurGroup->vecIndex.begin() + j );
					CLight_Manager::GetInstance()->Release_Light( pCurLight->szGroupName, j );
					break;
				}
			}

			lstrcpy( _pOutGroupName, pMoveGroup->szName );
			lstrcpy( pCurLight->szGroupName, pMoveGroup->szName );
			pMoveGroup->vecIndex.push_back( i );

			CLight_Manager::GetInstance()->Add_Light( m_pGraphicDev, pMoveGroup->szName, &m_vecLightIndexData[i].LightInfo );

			break;
		}
	}

	return NOERROR;
}

HRESULT CLightMgr_UseTool::Ready_LightMgr( void )
{
	m_pRendererCom = dynamic_cast<CRenderer*>(CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" ));
	if ( nullptr == m_pRendererCom )
		return E_FAIL;

	m_pLightTool = ((CMainFrame*)AfxGetMainWnd())->Get_EditBox()->Get_EditSheet()->Get_LightTool();
	if ( nullptr == m_pLightTool )
		return E_FAIL;

	return NOERROR;
}

_int CLightMgr_UseTool::Update_GameObject( const _float & _fTimeDelta )
{
	if ( nullptr == m_pRendererCom )
		return -1;
	if ( FAILED( m_pRendererCom->Add_RenderGroup( Engine::CRenderer::RENDER_UI, this ) ) )
		return -1;

	if ( ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->m_bMouseInClient == false )
		return 0;

	return 0;
}

void CLightMgr_UseTool::Render_GameObject( void )
{
	if ( false == m_bIsShowLightDebug )
		return;

#ifdef LIGHT_DEBUGING
	CLight_Manager::GetInstance()->Render_DebugLight();
#endif
}

void CLightMgr_UseTool::Update_NoneMode( const _float & _fTimeDelta )
{
}

void CLightMgr_UseTool::Update_LightCreateMode( const _float & _fTimeDelta )
{
	//if ( CInput_Device::GetInstance()->Get_DownMouseState( CInput_Device::DIM_LBUTTON ) )
	//	m_pLightTool->Add_Light();
}

void CLightMgr_UseTool::Update_LightDeleteMode( const _float & _fTimeDelta )
{
}

CLightMgr_UseTool::LIGHTGROUPDATA * CLightMgr_UseTool::Find_LightGroup( const _tchar * _pGroupTag )
{
	for ( auto& LightGroup : m_vecLightGroupData )
	{
		if ( lstrcmp( LightGroup.szName, _pGroupTag ) == 0 )
			return &LightGroup;
	}

	return nullptr;
}

HRESULT CLightMgr_UseTool::Delete_Light( const _uint & _iIndex )
{
	if ( m_vecLightIndexData.size() <= _iIndex )
		return E_FAIL;

	//if ( 0 != lstrcmp( m_vecLightIndexData[_iIndex].szGroupName, m_vecLightGroupData[0].szName ) )
	{
		for ( _uint i = _iIndex; i < m_iLightTotalCount; ++i )
		{
			if ( 0 == lstrcmp( m_vecLightIndexData[_iIndex].szGroupName, m_vecLightIndexData[i].szGroupName ) )
				continue;

			_uint jLength = m_vecLightGroupData.size();
			for ( _uint j = 0; j < jLength; ++j )
			{
				if ( 0 == lstrcmp( m_vecLightIndexData[i].szGroupName, m_vecLightGroupData[j].szName ) )
				{
					_uint kLength = m_vecLightGroupData[j].vecIndex.size();
					for ( _uint k = 0; k < kLength; ++k )
					{
						_uint iIndex = m_vecLightGroupData[j].vecIndex[k];
						if ( iIndex == i )
						{
							--m_vecLightGroupData[j].vecIndex[k];
							break;
						}
					}

					break;
				}
			}
		}
	}

	m_vecLightIndexData.erase( m_vecLightIndexData.begin() + _iIndex );
	--m_iLightTotalCount;

	return NOERROR;
}

CLightMgr_UseTool * CLightMgr_UseTool::Create( LPDIRECT3DDEVICE9 _pGraphicDev )
{
	CLightMgr_UseTool* pInstance = new CLightMgr_UseTool( _pGraphicDev );

	if ( FAILED( pInstance->Ready_LightMgr() ) )
	{
		CREATE_FAIL( pInstance, "CObject_ToolUse" );
	}

	return pInstance;
}

_ulong CLightMgr_UseTool::Free( void )
{
	Engine::Safe_Release( m_pRendererCom );

	return Engine::CGameObject::Free();
}
