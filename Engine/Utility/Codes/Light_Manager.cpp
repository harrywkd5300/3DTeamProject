#include "..\Headers\Light_Manager.h"
#include "Light.h"
#include "Debug_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CLight_Manager)

CLight_Manager::CLight_Manager()
{
#ifndef _FINAL
	Tag(L"CLight_Manager", std::bind(&CLight_Manager::Debug_Render, this));
#endif

}

void CLight_Manager::Set_LightGroupActive( const _tchar * pLayerTag, const _bool & bIsActive )
{
	for ( auto& LightGroup : m_LightContainer )
	{
		if ( lstrcmp( LightGroup.pName, pLayerTag ) )
			continue;

		LightGroup.bIsActive = bIsActive;
		break;
	}
}

HRESULT CLight_Manager::Register_Light( CLight * pLight )
{
	if ( nullptr == pLight )
		return E_FAIL;

	pLight->AddRef();
	m_DynamicLightList.push_back( pLight );

	return NOERROR;
}

HRESULT CLight_Manager::Add_Light(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pLayerTag, const D3DLIGHT9 * pLightInfo, const _vec3& _vLightPower)
{
	CLight*		pLight = CLight::Create(pGraphicDev, pLightInfo, _vLightPower);
	if(nullptr == pLight)
		return E_FAIL;

	LIGHTGROUP* pFind = Find_LightGroup( pLayerTag );

	if ( nullptr == pFind )
		m_LightContainer.push_back( LIGHTGROUP( pLayerTag, pLight ) );
	else
	{
		pFind->vecLight.push_back( pLight );
	}

	return NOERROR;
}

HRESULT CLight_Manager::LoadLightFile( LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath )
{
	HANDLE hFile = CreateFile( pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		CloseHandle( hFile );
		MSG_BOX( "Failed to Load LightFile" );
		return E_FAIL;
	}

	_ulong dwByte = 0;
	_uint iCount = 0;

	ReadFile( hFile, &iCount, sizeof( _uint ), &dwByte, nullptr );

	for ( _uint i = 0; i < iCount; ++i )
	{
		_uint iLength = 0;
		_tchar szGroupName[256] = L"";
		D3DLIGHT9 LightInfo;
		ReadFile( hFile, &iLength, sizeof( _uint ), &dwByte, nullptr );
		ReadFile( hFile, szGroupName, sizeof( _tchar ) * (iLength + 1), &dwByte, nullptr );
		ReadFile( hFile, &LightInfo, sizeof( D3DLIGHT9 ), &dwByte, nullptr );

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

		Add_Light( pGraphicDev, szGroupName, &LightInfo );
	}

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CLight_Manager::Modify_Light( const _tchar * pLayerTag, const _uint& _iIndex, const D3DLIGHT9 * pLightInfo, const _vec3& _vLightPower )
{
	LIGHTGROUP* pFindGroup = Find_LightGroup( pLayerTag );

	if ( nullptr == pFindGroup ||
		 _iIndex >= pFindGroup->vecLight.size() )
		return E_FAIL;
	if ( FAILED( pFindGroup->vecLight[_iIndex]->Ready_Light( pLightInfo, _vLightPower ) ) )
		return E_FAIL;

	return NOERROR;
}

#ifdef LIGHT_DEBUGING
HRESULT CLight_Manager::Add_LightGroup( const _tchar * pLayerTag )
{
	m_LightContainer.push_back( LIGHTGROUP( pLayerTag ) );

	return NOERROR;
}

void CLight_Manager::Render_DebugLight( void )
{
	for ( auto& LightGroup : m_LightContainer )
	{
		_uint iCount = (_uint)LightGroup.vecLight.size();
		for ( _uint i = 0; i < iCount; ++i )
		{
			LightGroup.vecLight[i]->Render_DebugLight();
		}
	}
}
_bool CLight_Manager::CheckPicking( D3DLIGHT9* _pOut, _tchar* _pOutGroupName, _uint* _pOutIndex, const _vec3 & _vRayPos, const _vec3 & _vRayDir )
{
	for ( auto& LightGroup : m_LightContainer )
	{
		_uint iCount = (_uint)LightGroup.vecLight.size();
		for ( _uint i = 0; i < iCount; ++i )
		{
			if ( true == LightGroup.vecLight[i]->CheckPicking( _pOut, _vRayPos, _vRayDir ) )
			{
				lstrcpy( _pOutGroupName, LightGroup.pName );
				*_pOutIndex = i;
				return true;
			}
		}
	}

	return false;
}
void CLight_Manager::Set_LightDetailDraw( const _tchar * _pGroupTag, const _uint & _iIndex, const _bool & _bisDetailDraw )
{
	LIGHTGROUP* pFind = Find_LightGroup( _pGroupTag );

	if ( pFind->vecLight.size() <= _iIndex )
		return;

	pFind->vecLight[_iIndex]->Set_IsDrawDetail( _bisDetailDraw );
}
#endif

void CLight_Manager::Release_Light( void )
{
	for ( auto& Light : m_LightContainer )
	{
		for ( _uint i = 0; i < Light.vecLight.size(); ++i )
			Engine::Safe_Release( Light.vecLight[i] );
		Engine::Safe_Delete_Array( Light.pName );
	}
	m_LightContainer.clear();
}

void CLight_Manager::Release_Light( const _tchar * pLayerTag )
{
	for ( auto iter = m_LightContainer.begin(); iter != m_LightContainer.end(); )
	{
		if ( lstrcmp( pLayerTag, iter->pName ) )
		{
			++iter;
			continue;
		}

		for ( _uint i = 0; i < iter->vecLight.size(); ++i )
			Engine::Safe_Release( iter->vecLight[i] );

		Engine::Safe_Delete_Array( iter->pName );

		iter = m_LightContainer.erase( iter );
	}
}

void CLight_Manager::Release_Light( const _tchar * pLayerTag, const _uint & _iIndex )
{
	for ( auto iter = m_LightContainer.begin(); iter != m_LightContainer.end(); )
	{
		if ( lstrcmp( pLayerTag, iter->pName ) )
		{
			++iter;
			continue;
		}

		if ( iter->vecLight.size() <= _iIndex )
			return;

		Engine::Safe_Release( iter->vecLight[_iIndex] );
		iter->vecLight.erase( iter->vecLight.begin() + _iIndex );

		if ( iter->vecLight.empty() )
		{
			Engine::Safe_Delete_Array( iter->pName );

			iter = m_LightContainer.erase( iter );
		}

		return;
	}
}

void CLight_Manager::Render_Light(const _matrix& _matViewProj, CFrustum* _pFrustum, LPD3DXEFFECT pEffect)
{
#ifndef _FINAL
	_float fTime = 0.f;
	_bool bErrasd = false;

	_uint iLightTotalCount = 0;

	CHECK_TIME_START
#endif

	for ( auto& Light : m_LightContainer )
	{
		if ( false == Light.bIsActive )
			continue;

#ifndef _FINAL
		iLightTotalCount += Light.vecLight.size();
#endif

		_uint iCount = (_uint)Light.vecLight.size();
		for ( _uint i = 0; i < iCount; ++i )
		{
			//if ( Light.vecLight[i]->ActOcculsionTest( _pFrustum, _matViewProj ) )
				Light.vecLight[i]->Render_Light( pEffect );
		}
	}

	// 동적으로 등록한 라이트들 렌더..
	for ( auto& Light : m_DynamicLightList )
	{
		Light->Render_Light( pEffect );
		Engine::Safe_Release( Light );	// 썼으면 Add 했던 레퍼런스 카운트 다시 없애기..
	}
	m_DynamicLightList.clear();	// 리스트 클리어 작업..

#ifndef _FINAL
	CHECK_TIME_END( fTime, bErrasd );
	//CDebug::GetInstance()->Add_LightRunTimeData( fTime, (_uint)(iLightTotalCount + m_DynamicLightList.size()) );
#endif
}

LIGHTGROUP* CLight_Manager::Find_LightGroup( const _tchar * pLayerTag )
{
	for ( auto& LightGroup : m_LightContainer )
	{
		if ( 0 == lstrcmp( pLayerTag, LightGroup.pName ) )
			return &LightGroup;
	}

	return nullptr;
}

#ifndef _FINAL
void CLight_Manager::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CLight_Manager");

}
#endif
_ulong CLight_Manager::Free(void)
{
	for ( auto& Light : m_LightContainer )
	{
		for ( _uint i = 0; i < Light.vecLight.size(); ++i )
			Engine::Safe_Release( Light.vecLight[i] );
		Engine::Safe_Delete_Array( Light.pName );
	}
	m_LightContainer.clear();

	return _ulong();
}