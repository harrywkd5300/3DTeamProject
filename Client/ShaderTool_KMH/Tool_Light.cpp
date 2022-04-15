// Tool_Light.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "Tool_Light.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "ShaderToolMainView.h"
#include "EditBox.h"
#include "EditSheet.h"
#include "Tool_Shader.h"

#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Camera_Manager.h"

#include "LightMgr_UseTool.h"
#include "Terrain_ToolUse.h"
#include "Camera_UseTool.h"
#include "BaseObject_UseTool.h"

// CTool_Light 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTool_Light, CPropertyPage)

CTool_Light::CTool_Light()
	: CPropertyPage(IDD_TOOL_LIGHT)
{

}

CTool_Light::~CTool_Light()
{
	for ( size_t i = 0; i < m_vecLightGroupItem.size(); ++i )
		Engine::Safe_Delete_Array( m_vecLightGroupItem[i].first );

	for ( size_t i = 0; i < m_vecLightGroupName.size(); ++i )
		Engine::Safe_Delete_Array( m_vecLightGroupName[i] );

	for ( size_t i = 0; i < m_vecLightItem.size(); ++i )
		Engine::Safe_Delete_Array( m_vecLightItem[i].first );

	Engine::Safe_Delete_Array( m_ppObjects );
}

void CTool_Light::Update_Tool( const _float & _fTimeDelta )
{
	if ( true == m_bIsSaveOrLoadState )
		return;

	m_fActiveTimeAcc += _fTimeDelta;

	if ( false == m_bIsSelectColor )
	{
		Update_StrData();
		Update_SliderData();

		KeyCheck( _fTimeDelta );

		if ( ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->m_bMouseInClient )
		{
			Update_LightData( _fTimeDelta );
		}

		Update_SelectLight();

		m_pLightMgr->Update_GameObject( _fTimeDelta );
	}

	if ( Get_BtnCheck( Btn_DiffuseSpecularSamePower ) == TRUE )
		m_SliderArr[Slider_LightSpecularA].EnableWindow( FALSE );
	else
		m_SliderArr[Slider_LightSpecularA].EnableWindow( TRUE );
}

_bool CTool_Light::Compute_PickingPos( _vec3 * _pOut )
{
	for ( _uint i = 0; i < m_iObjectCount; ++i )
	{
		if ( true == m_ppObjects[i]->PickingCheck( _pOut, m_pPickingCom ) )
			return true;
	}

	if ( false == m_pPickingCom->Picking_ToBuffer( g_hWnd, _pOut, m_pTerrainBufferCom, nullptr ) )
		return false;

	return true;
}

void CTool_Light::Set_BtnCheck( const BUTTONKIND & _eBtnKind, const BOOL & _bCheck )
{
	m_BtnArr[_eBtnKind].SetCheck( _bCheck );

	switch ( _eBtnKind )
	{
		case CTool_Light::Btn_YPosPickingPos:
			break;
		case CTool_Light::Btn_LightDebug:
			break;
		case CTool_Light::Btn_LightPicking:
			break;
		case CTool_Light::Btn_CamRunMode:
			m_pCamera->Set_CamRunMode( (_bCheck == FALSE) ? false : true );
			break;
		case CTool_Light::Btn_LightOnOff:
			m_pRendererCom->Set_ShaderOptionOnOff( CRenderer::ShaderOption_Light, ((_bCheck == FALSE) ? false : true) );
			break;
		case CTool_Light::Btn_LightGroupOnOff:
			if ( m_CurSelectLightGroupItem.first == nullptr )
			{
				m_BtnArr[_eBtnKind].SetCheck( FALSE );
				Set_BtnEnable( _eBtnKind, FALSE );
			}
			else
				CLight_Manager::GetInstance()->Set_LightGroupActive( m_CurSelectLightGroupItem.first, ((_bCheck == FALSE) ? false : true) );
			break;
	}
}

void CTool_Light::Set_BtnEnable( const BUTTONKIND & _eBtnKind, const BOOL & _bCheck )
{
	m_BtnArr[_eBtnKind].EnableWindow( _bCheck );
}

BOOL CTool_Light::Get_BtnCheck( const BUTTONKIND & _eBtnKind )
{
	return m_BtnArr[_eBtnKind].GetCheck();
}

void CTool_Light::Modify_StageType( const MAPKIND & _eMapKind )
{
	Load_ObjectData( _eMapKind );

	m_eMapKind = _eMapKind;
}

void CTool_Light::Add_LightGroup( void )
{
	UpdateData( TRUE );

	_tchar* pName = new _tchar[128];

	if ( 0 == lstrcmp( m_StrArr[Str_LightGroupName], L"" ) )
	{
		_tchar szName[128] = L"";
		swprintf_s( szName, L"LightGroup%d", m_iDefaultLightGroupCount++ );
		lstrcpy( pName, szName );
	}
	else
		lstrcpy( pName, m_StrArr[Str_LightGroupName] );

	HTREEITEM hItem = m_TreeCtrl_LightGroup.InsertItem( pName, m_RootItem, TVI_LAST );
	m_vecLightGroupItem.push_back( make_pair( pName, hItem ) );

	m_pLightMgr->Add_LightGroup( pName );
	CLight_Manager::GetInstance()->Add_LightGroup( pName );
}

void CTool_Light::Add_LightGroup( const _tchar * _pName )
{
	_tchar* pName = new _tchar[128];

	lstrcpy( pName, _pName );

	HTREEITEM hItem = m_TreeCtrl_LightGroup.InsertItem( pName, m_RootItem, TVI_LAST );
	m_vecLightGroupItem.push_back( make_pair( pName, hItem ) );

	m_pLightMgr->Add_LightGroup( pName );
	CLight_Manager::GetInstance()->Add_LightGroup( pName );
}

void CTool_Light::Add_Light( void )
{
	if ( nullptr == m_pLightMgr )
	{
		AfxMessageBox( L"m_pLightMgr is nullptr" );
		return;
	}

	_vec3 vPos( 0.f, 0.f, 0.f );
	if ( true == Compute_PickingPos( &vPos ) )
	{
		UpdateData( TRUE );

		m_StrArr[Str_LightPosX].Format( L"%f", vPos.x );
		m_StrArr[Str_LightPosZ].Format( L"%f", vPos.z );

		if ( m_BtnArr[Btn_YPosPickingPos].GetCheck() != 0 )
		{
			m_StrArr[Str_LightPosY].Format( L"%f", vPos.y );
			m_fCurY = vPos.y;
		}
		else
			m_StrArr[Str_LightPosY].Format( L"%f", m_fCurY );

		UpdateData( FALSE );
	}

	D3DLIGHT9 LightInfo;

	Compute_LightInfo( LightInfo );

	_vec3 vLightPower( _ttof( m_StrArr[Str_LightDiffuseA] ), _ttof( m_StrArr[Str_LightAmbientA] ), _ttof( m_StrArr[Str_LightSpecularA] ) );

	_tchar szName[128] = L"";

	switch ( LightInfo.Type )
	{
		case D3DLIGHT_DIRECTIONAL:
			swprintf_s( szName, L"Directional%d", m_iLightCount++ );
			break;
		case D3DLIGHT_POINT:
			swprintf_s( szName, L"Point%d", m_iLightCount++ );
			break;
		case D3DLIGHT_SPOT:
			swprintf_s( szName, L"Spot%d", m_iLightCount++ );
			break;
	}

	if ( lstrcmp( m_StrArr[Str_LightGroupName], L"" ) == 0 )
	{
		m_pLightMgr->Add_Light( L"Static", szName, &LightInfo, vLightPower );
		InsertLightToTreeItem( szName, L"Static" );
	}
	else
	{
		m_pLightMgr->Add_Light( m_StrArr[Str_LightGroupName], szName, &LightInfo, vLightPower );
		InsertLightToTreeItem( szName, m_StrArr[Str_LightGroupName] );
	}

	//--m_iAddCountAcc;
	//if ( m_iAddCountAcc <= 0 )
	//{
	//	_tchar szName[256] = L"";
	//	swprintf_s( szName, L"./BackUpFile/SaveFileBackUp%d.dat", m_iAutoSaveCountAcc++ );
	//	if ( FAILED( m_pLightMgr->Save_File( szName ) ) )
	//	{
	//		AfxMessageBox( L"Auto Save Failed" );
	//		return;
	//	}
	//
	//	m_iAddCountAcc = LIGHTSAVEINTERVAL;
	//}
}

void CTool_Light::Compute_LightInfo( D3DLIGHT9& _LightInfo )
{
	UpdateData( TRUE );

	_int iLightType = m_Combo_LightType.GetCurSel();

	if ( iLightType == -1 )
		return;
	
	// Get Light Position..
	_vec3 vPos( _ttof( m_StrArr[Str_LightPosX] ), _ttof( m_StrArr[Str_LightPosY] ), _ttof( m_StrArr[Str_LightPosZ] ) );

	// Get Light Direction..
	_vec3 vDir( _ttof( m_StrArr[Str_LightDirX] ), _ttof( m_StrArr[Str_LightDirY] ), _ttof( m_StrArr[Str_LightDirZ] ) );

	// Get Light Etc Data..
	_float fMinAngle = _ttof( m_StrArr[Str_LightMinAngle] );
	_float fMaxAngle = _ttof( m_StrArr[Str_LightMaxAngle] );
	_float fRange = _ttof( m_StrArr[Str_LightRange] );

	// Get Light Color..
	D3DXCOLOR Diffuse = D3DXCOLOR( 
		_ttof( m_StrArr[Str_LightDiffuseR] ), _ttof( m_StrArr[Str_LightDiffuseG] ), _ttof( m_StrArr[Str_LightDiffuseB] ), 1.f );
	D3DXCOLOR Ambient = D3DXCOLOR( 
		_ttof( m_StrArr[Str_LightAmbientR] ), _ttof( m_StrArr[Str_LightAmbientG] ), _ttof( m_StrArr[Str_LightAmbientB] ), 1.f );
	D3DXCOLOR Specular = D3DXCOLOR( 
		_ttof( m_StrArr[Str_LightSpecularR] ), _ttof( m_StrArr[Str_LightSpecularG] ), _ttof( m_StrArr[Str_LightSpecularB] ), 1.f );

	// Modify LightInfoData..
	_LightInfo.Type = (D3DLIGHTTYPE)(iLightType + 1);
	_LightInfo.Position = vPos;
	_LightInfo.Direction = vDir;
	_LightInfo.Phi = fMinAngle;
	_LightInfo.Theta = fMaxAngle;
	_LightInfo.Range = fRange;
	_LightInfo.Diffuse = Diffuse;
	_LightInfo.Ambient = Ambient;
	_LightInfo.Specular = Specular;
}

void CTool_Light::Setting_MFCBox( D3DLIGHT9 & _LightInfo )
{
	UpdateData( TRUE );

	_int iLightType = _LightInfo.Type - 1;

	// Get Light Position..
	m_StrArr[Str_LightPosX].Format( L"%f", _LightInfo.Position.x );
	m_StrArr[Str_LightPosY].Format( L"%f", _LightInfo.Position.y );
	m_StrArr[Str_LightPosZ].Format( L"%f", _LightInfo.Position.z );

	m_StrArr[Str_LightDirX].Format( L"%f", _LightInfo.Direction.x );
	m_StrArr[Str_LightDirY].Format( L"%f", _LightInfo.Direction.y );
	m_StrArr[Str_LightDirZ].Format( L"%f", _LightInfo.Direction.z );

	m_StrArr[Str_LightMinAngle].Format( L"%f", _LightInfo.Phi );
	m_StrArr[Str_LightMaxAngle].Format( L"%f", _LightInfo.Phi );
	m_StrArr[Str_LightRange].Format( L"%f", _LightInfo.Range );

	m_StrArr[Str_LightDiffuseR].Format( L"%f", _LightInfo.Diffuse.r );
	m_StrArr[Str_LightDiffuseG].Format( L"%f", _LightInfo.Diffuse.g );
	m_StrArr[Str_LightDiffuseB].Format( L"%f", _LightInfo.Diffuse.b );

	m_StrArr[Str_LightAmbientR].Format( L"%f", _LightInfo.Ambient.r );
	m_StrArr[Str_LightAmbientG].Format( L"%f", _LightInfo.Ambient.g );
	m_StrArr[Str_LightAmbientB].Format( L"%f", _LightInfo.Ambient.b );

	m_StrArr[Str_LightSpecularR].Format( L"%f", _LightInfo.Specular.r );
	m_StrArr[Str_LightSpecularG].Format( L"%f", _LightInfo.Specular.g );
	m_StrArr[Str_LightSpecularB].Format( L"%f", _LightInfo.Specular.b );

	m_StrArr[Str_LightDiffuseA].Format( L"%f", _LightInfo.Attenuation0 );
	m_StrArr[Str_LightAmbientA].Format( L"%f", _LightInfo.Attenuation1 );
	m_StrArr[Str_LightSpecularA].Format( L"%f", _LightInfo.Attenuation2 );

	_float fColor[3] = { 0.f };

	fColor[0] = _LightInfo.Attenuation0;
	fColor[1] = _LightInfo.Attenuation1;
	fColor[2] = _LightInfo.Attenuation2;

	m_SliderArr[Slider_LightDiffuseA].SetPos( (_int)(fColor[0] * m_fColorMul) );
	m_SliderArr[Slider_LightAmbientA].SetPos( (_int)(fColor[1] * m_fColorMul) );
	m_SliderArr[Slider_LightSpecularA].SetPos( (_int)(fColor[2] * m_fColorMul) );

	UpdateData( FALSE );
}

void CTool_Light::KeyCheck( const _float & _fTimeDelta )
{
	_bool bMouseIsClient = ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->m_bMouseInClient;
	CInput_Device* pInput_Device = CInput_Device::GetInstance();

	m_fTimeAcc += _fTimeDelta;

	// 마우스가 클라이언트 ( 다이렉트 창 ) 안에 있어야만 검사하는 키들..
	if ( bMouseIsClient == true )
	{
		if ( pInput_Device->Get_DIKeyState( DIK_LCONTROL ) )
		{
			// --------------------------- LightMode 변경 키 체크.. --------------------------- //
			if ( pInput_Device->Get_DIKeyState( DIK_E ) )
			{
				m_Radio_LightMode = LightMode_None;
				Set_BtnCheck( Btn_LightDebug, true );
				Set_BtnCheck( Btn_LightPicking, true );
			}
			if ( pInput_Device->Get_DIKeyState( DIK_R ) )
			{
				m_Radio_LightMode = LightMode_CreateLight;
				Set_BtnCheck( Btn_LightPicking, false );
			}

			m_pLightMgr->Set_LightMode( (LIGHTMODE)m_Radio_LightMode );
			m_pLightMgr->Set_IsShowLightDebug( ((Get_BtnCheck( Btn_LightDebug ) != FALSE) ? true : false) );
			UpdateData( FALSE );

			// --------------------------- 순간이동 키 체크.. --------------------------- //
			if ( pInput_Device->Get_DownMouseState( CInput_Device::DIM_LBUTTON ) )
			{
				_vec3 vPos;
				if ( true == Compute_PickingPos( &vPos ) )
				{
					m_pCamera->Set_Position( vPos, false );
				}
			}

			// --------------------------- Y 상승량 키 체크.. --------------------------- //
			if ( pInput_Device->Get_DIKeyState( DIK_Z ) )
			{
				m_fYSpeed += YSPEEDPLUS * _fTimeDelta;
			}

			// --------------------------- 저장 버튼 키 체크.. --------------------------- //
			if ( pInput_Device->Get_DownKeyState( DIK_D ) )
			{
				m_bIsSaveOrLoadState = true;
				Save_Data();
			}

			// --------------------------- 로드 버튼 키 체크.. --------------------------- //
			if ( pInput_Device->Get_DownKeyState( DIK_F ) )
			{
				m_bIsSaveOrLoadState = true;
				Load_Data();
			}

			// --------------------------- 라이트 그룹 생성 키 체크.. --------------------------- //
			if ( pInput_Device->Get_DownKeyState( DIK_Q ) )
			{
				Add_LightGroup();
			}
		}
		else
		{
			if ( pInput_Device->Get_DownMouseState( CInput_Device::DIM_LBUTTON ) )
			{
				// --------------------------- 빛 생성.. --------------------------- //
				Add_Light();
			}
			if ( pInput_Device->Get_DownMouseState( CInput_Device::DIM_RBUTTON ) )
			{
				// --------------------------- 빛 피킹.. --------------------------- //
				CheckPickingLight();
			}
			// --------------------------- Y 상승량 키 체크.. --------------------------- //

			if ( pInput_Device->Get_DIKeyState( DIK_Z ) )
			{
				m_fYSpeed -= YSPEEDPLUS * _fTimeDelta;
			}
		}

		// --------------------------- 카메라 Run모드 체크.. --------------------------- //
		Set_BtnCheck( Btn_CamRunMode, ((pInput_Device->Get_DIKeyState( DIK_Q )) ? TRUE : FALSE) );

		if ( pInput_Device->Get_DIKeyState( DIK_F1 ) )
		{
			if ( pInput_Device->Get_DownKeyState( DIK_F3 ) )
			{
				Set_BtnCheck( Btn_YPosPickingPos, ((Get_BtnCheck( Btn_YPosPickingPos ) != FALSE) ? FALSE : TRUE) );
			}
		}

		if ( pInput_Device->Get_DownKeyState( DIK_DELETE ) )
		{
			// --------------------------- 빛 삭제.. --------------------------- //
			if ( m_Radio_LightMode == LightMode_None )
			{
				Delete_Light();
			}
		}

		// --------------------------- 빛 디버그 키 체크.. --------------------------- //
		if ( pInput_Device->Get_DownKeyState( DIK_1 ) )
		{
			Set_BtnCheck( Btn_LightDebug, ((Get_BtnCheck( Btn_LightDebug ) == FALSE) ? TRUE : FALSE) );
		}

		// --------------------------- 빛 On/Off 키 체크.. --------------------------- //
		if ( pInput_Device->Get_DownKeyState( DIK_2 ) )
		{
			Set_BtnCheck( Btn_LightOnOff, ((Get_BtnCheck( Btn_LightOnOff ) == FALSE) ? TRUE : FALSE) );
		}

		// --------------------------- 빛 타입 변경.. --------------------------- //
		if ( pInput_Device->Get_DownKeyState( DIK_GRAVE ) )
		{
			_int iSelect = m_Combo_LightType.GetCurSel();
			++iSelect;
			if ( iSelect >= 3 )
				iSelect = 0;
			m_Combo_LightType.SetCurSel( iSelect );
		}

		// --------------------------- 빛 Range 변경.. --------------------------- //
		_uint iNumPadArr[10] = { DIK_NUMPAD0, DIK_NUMPAD1, DIK_NUMPAD2, DIK_NUMPAD3, DIK_NUMPAD4, DIK_NUMPAD5, DIK_NUMPAD6, DIK_NUMPAD7, DIK_NUMPAD8, DIK_NUMPAD9 };
		for ( _uint i = 0; i < 10; ++i )
		{
			if ( pInput_Device->Get_DownKeyState( iNumPadArr[i] ) )
			{
				if ( m_fTimeAcc >= 2.f )
					m_fLightRange = 0.f;

				m_fLightRange *= 10.f;
				m_fLightRange += i;
				m_fTimeAcc = 0.f;

				break;
			}
		}
		if ( pInput_Device->Get_DownKeyState( DIK_BACKSPACE ) )
		{
			m_fLightRange = 0.f;
			m_fTimeAcc = 0.f;
		}

		// --------------------------- Y 좌표 변경 키 체크..--------------------------- //
		_long dwWheelMove = pInput_Device->Get_DIMouseMove( CInput_Device::DIMM_Z );
		if ( dwWheelMove != 0 )
		{
			Set_BtnCheck( Btn_YPosPickingPos, FALSE );
			m_fCurY += dwWheelMove / 1000.f * m_fYSpeed;
		}
	}

	// --------------------------- 마우스가 클라이언트 ( 다이렉트 창 ) 안에 있든 없든 검사하는 키들.. --------------------------- //
	if ( CInput_Device::GetInstance()->Get_DIKeyState( DIK_LCONTROL ) )
	{
		// --------------------------- 라이트 그룹 삭제 키 체크.. --------------------------- //
		if ( CInput_Device::GetInstance()->Get_DownKeyState( DIK_DELETE ) )
		{
			if ( m_CurSelectLightGroupItem.first != nullptr )
			{
				CLight_Manager::GetInstance()->Release_Light( m_CurSelectLightGroupItem.first );
				m_pLightMgr->Delete_Light( m_CurSelectLightGroupItem.first );
				m_TreeCtrl_LightGroup.DeleteItem( m_CurSelectLightGroupItem.second );

				_bool bIsDeleteStaticGroup = false;

				if ( 0 == lstrcmp( m_CurSelectLightGroupItem.first, L"Static" ) )
					bIsDeleteStaticGroup = true;

				_uint iCount = m_vecLightGroupItem.size();
				for ( _uint i = 0; i < iCount; ++i )
				{
					if ( 0 == lstrcmp( m_vecLightGroupItem[i].first, m_CurSelectLightGroupItem.first ) )
					{
						m_vecLightGroupItem.erase( m_vecLightGroupItem.begin() + i );
						break;
					}
				}

				m_CurSelectLightGroupItem.first = nullptr;
				m_CurSelectLightGroupItem.second = NULL;

				if ( bIsDeleteStaticGroup == true )
				{
					Add_LightGroup( L"Static" );
				}
			}
		}
	}

	// --------------------------- 라이트 그룹 On/Off 키 체크.. --------------------------- //
	if ( CInput_Device::GetInstance()->Get_DownKeyState( DIK_3 ) )
	{
		Set_BtnCheck( Btn_LightGroupOnOff, ((Get_BtnCheck( Btn_LightGroupOnOff ) == FALSE) ? TRUE : FALSE) );
	}

	// --------------------------- 라이트 그룹 이동 키 체크.. --------------------------- //
	if ( m_CurSelectItem.first != nullptr )
	{
		if ( CInput_Device::GetInstance()->Get_DownKeyState( DIK_LBRACKET ) )
		{
			_tchar szName[128] = L"";
			if ( SUCCEEDED( m_pLightMgr->Move_LightGroup( szName, m_CurSelectItem.first, true ) ) )
			{
				_uint iCount = 0;

				_tchar szTemp[128] = L"";
				lstrcpy( szTemp, m_CurSelectItem.first );

				iCount = m_vecLightItem.size();
				for ( _uint i = 0; i < iCount; i++ )
				{
					if ( 0 == lstrcmp( m_vecLightItem[i].first, m_CurSelectItem.first ) )
					{
						m_TreeCtrl_LightGroup.DeleteItem( m_vecLightItem[i].second );
						m_vecLightItem.erase( m_vecLightItem.begin() + i );
						m_CurSelectItem.second = NULL;
						m_CurSelectItem.first = nullptr;
						break;
					}
				}

				iCount = m_vecLightGroupItem.size();
				for ( _uint i = 0; i < iCount; ++i )
				{
					if ( 0 == lstrcmp( m_vecLightGroupItem[i].first, szName ) )
					{
						InsertLightToTreeItem( szTemp, szName );
						break;
					}
				}
			}
		}
		else if ( CInput_Device::GetInstance()->Get_DownKeyState( DIK_RBRACKET ) )
		{
			_tchar szName[128] = L"";
			if ( SUCCEEDED( m_pLightMgr->Move_LightGroup( szName, m_CurSelectItem.first, false ) ) )
			{
				_uint iCount = 0;

				_tchar szTemp[128] = L"";
				lstrcpy( szTemp, m_CurSelectItem.first );

				iCount = m_vecLightItem.size();
				for ( _uint i = 0; i < iCount; i++ )
				{
					if ( 0 == lstrcmp( m_vecLightItem[i].first, m_CurSelectItem.first ) )
					{
						m_TreeCtrl_LightGroup.DeleteItem( m_vecLightItem[i].second );
						m_vecLightItem.erase( m_vecLightItem.begin() + i );
						m_CurSelectItem.second = NULL;
						m_CurSelectItem.first = nullptr;
						break;
					}
				}

				iCount = m_vecLightGroupItem.size();
				for ( _uint i = 0; i < iCount; ++i )
				{
					if ( 0 == lstrcmp( m_vecLightGroupItem[i].first, szName ) )
					{
						InsertLightToTreeItem( szTemp, szName );
						break;
					}
				}
			}
		}
	}
}

void CTool_Light::Update_LightData( const _float & _fTimeDelta )
{
	_vec3 vPos;
	if ( true == Compute_PickingPos( &vPos ) )
	{
		if ( false == m_bIsSelectLight )
		{
			m_StrArr[Str_LightPosX].Format( L"%f", vPos.x );
			m_StrArr[Str_LightPosZ].Format( L"%f", vPos.z );

			if ( m_BtnArr[Btn_YPosPickingPos].GetCheck() != 0 )
			{
				m_StrArr[Str_LightPosY].Format( L"%f", vPos.y );
				m_fCurY = vPos.y;
			}
			else
				m_StrArr[Str_LightPosY].Format( L"%f", m_fCurY );

			UpdateData( FALSE );
		}
	}
}

void CTool_Light::Update_StrData( void )
{
	UpdateData( TRUE );

	m_StrArr[Str_YPosPlus].Format( L"%f", m_fYSpeed );
	m_StrArr[Str_LightRange].Format( L"%f", m_fLightRange );

	UpdateData( FALSE );
}

void CTool_Light::Update_SliderData( void )
{
	UpdateData( TRUE );

	// ----- Modify Light Direction.. ----- //
	_vec3 vDir( _ttof( m_StrArr[Str_LightDirX] ), _ttof( m_StrArr[Str_LightDirY] ), _ttof( m_StrArr[Str_LightDirZ] ) );

	m_SliderArr[Slider_LightDirX].SetPos( (_int)(vDir.x * m_fDirMul) );
	m_SliderArr[Slider_LightDirY].SetPos( (_int)(vDir.y * m_fDirMul) );
	m_SliderArr[Slider_LightDirZ].SetPos( (_int)(vDir.z * m_fDirMul) );


	// ----- Modify Light Spotlight Min Max Angle.. ----- //
	_float fMinAngle = _ttof( m_StrArr[Str_LightMinAngle] );
	_float fMaxAngle = _ttof( m_StrArr[Str_LightMaxAngle] );

	m_SliderArr[Slider_LightMinAngle].SetPos( (_int)(fMinAngle * m_fAngleMul) );
	m_SliderArr[Slider_LightMaxAngle].SetPos( (_int)(fMaxAngle * m_fAngleMul) );


	// ----- Modify Light Color.. ----- //
	_float fColor[3] = { 0.f };

	fColor[0] = _ttof( m_StrArr[Str_LightDiffuseA] );
	fColor[1] = _ttof( m_StrArr[Str_LightAmbientA] );
	fColor[2] = _ttof( m_StrArr[Str_LightSpecularA] );

	m_SliderArr[Slider_LightDiffuseA].SetPos( (_int)(fColor[0] * m_fColorMul) );
	m_SliderArr[Slider_LightAmbientA].SetPos( (_int)(fColor[1] * m_fColorMul) );
	m_SliderArr[Slider_LightSpecularA].SetPos( (_int)(fColor[2] * m_fColorMul) );

	UpdateData( TRUE );
}

void CTool_Light::Update_SelectLight( void )
{
	if ( true == m_bIsSelectLight )
	{
		D3DLIGHT9 LightInfo;

		Compute_LightInfo( LightInfo );

		_vec3 vLightPower( _ttof( m_StrArr[Str_LightDiffuseA] ), _ttof( m_StrArr[Str_LightAmbientA] ), _ttof( m_StrArr[Str_LightSpecularA] ) );

		LightInfo.Position = m_vPickingLightPos;

		CLight_Manager::GetInstance()->Modify_Light( m_szPickingLightGroup, m_iPickingLightIndex, &LightInfo, vLightPower );
	}
}

void CTool_Light::InsertLightToTreeItem( const _tchar* _pLightName, const _tchar * _pGroupTag )
{
	for ( auto& LightGroupItem : m_vecLightGroupItem )
	{
		if ( 0 == lstrcmp( LightGroupItem.first, _pGroupTag ) )
		{
			_tchar* pName = new _tchar[128];
			lstrcpy( pName, _pLightName );
			HTREEITEM hItem = m_TreeCtrl_LightGroup.InsertItem( pName, LightGroupItem.second, TVI_LAST );
			m_vecLightItem.push_back( make_pair( pName, hItem ) );
			return;
		}
	}

	Add_LightGroup( _pGroupTag );
}

void CTool_Light::CheckPickingLight( void )
{
	_vec3 vPickingPos;
	_tchar szGroupName[256] = L"";
	_uint iIndex = 0;

	_vec3 vRayPos;
	_vec3 vRayDir;

	if ( FAILED( ComputeRay( &vRayPos, &vRayDir ) ) )
		return;

	CLight_Manager* pLight_Manager = CLight_Manager::GetInstance();

	D3DLIGHT9 LightInfo;
	if ( true == pLight_Manager->CheckPicking( &LightInfo, szGroupName, &iIndex, vRayPos, vRayDir ) )
	{
		if ( true == m_bIsSelectLight )
		{
			pLight_Manager->Set_LightDetailDraw( m_szPickingLightGroup, m_iPickingLightIndex, false );
		}

		m_vPickingLightPos = LightInfo.Position;

		Setting_MFCBox( LightInfo );

		pLight_Manager->Set_LightDetailDraw( szGroupName, iIndex, true );

		lstrcpy( m_szPickingLightGroup, szGroupName );
		m_iPickingLightIndex = iIndex;
		m_bIsSelectLight = true;
	}
	else
	{
		if ( true == m_bIsSelectLight )
		{
			pLight_Manager->Set_LightDetailDraw( m_szPickingLightGroup, m_iPickingLightIndex, false );

			lstrcpy( m_szPickingLightGroup, L"" );
			m_iPickingLightIndex = 0;
			m_bIsSelectLight = false;
		}
	}
}

HRESULT CTool_Light::ComputeRay( _vec3* _pOutRayPos, _vec3* _pOutRayDir )
{
	POINT ptMouse = ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->m_ptMouse;

	// Viewport	-	>	Projection..
	D3DVIEWPORT9 ViewPort;
	if ( FAILED( m_pGraphicDev->GetViewport( &ViewPort ) ) )
		return E_FAIL;

	_vec3 vRayDir( 0.f, 0.f, 0.f );
	vRayDir.x = (ptMouse.x / (ViewPort.Width * 0.5f)) - 1.f;
	vRayDir.y = -(ptMouse.y / (ViewPort.Height * 0.5f)) + 1.f;
	vRayDir.z = 0.f;


	// Projection	->	View..
	_matrix mat_Projection;
	if ( FAILED( m_pGraphicDev->GetTransform( D3DTS_PROJECTION, &mat_Projection ) ) ) 
		return E_FAIL;

	_matrix matInv_Projection;
	D3DXMatrixInverse( &matInv_Projection, nullptr, &mat_Projection );

	D3DXVec3TransformCoord( &vRayDir, &vRayDir, &matInv_Projection );


	// View		->		World..
	_matrix mat_View;
	if ( FAILED( m_pGraphicDev->GetTransform( D3DTS_VIEW, &mat_View ) ) ) 
		return E_FAIL;

	_matrix matInv_View;
	D3DXMatrixInverse( &matInv_View, nullptr, &mat_View );

	_vec3 vRayPos( 0.f, 0.f, 0.f );
	vRayDir = vRayDir - vRayPos;
	D3DXVec3TransformCoord( &vRayPos, &vRayPos, &matInv_View );
	D3DXVec3TransformNormal( &vRayDir, &vRayDir, &matInv_View );

	*_pOutRayPos = vRayPos;

	*_pOutRayDir = vRayDir;

	return NOERROR;
}

void CTool_Light::Delete_Light( void )
{
	if ( true == m_bIsSelectLight )
	{
		// 삭제 시킬 Light 의 이름 받아 놓기..
		_tchar szName[128] = L"";
		if ( FAILED( m_pLightMgr->Get_LightName( szName, m_szPickingLightGroup, m_iPickingLightIndex ) ) )
			return;

		// Light 들을 보관하고 있는 클래스들에게 삭제시키라고 함수 호출..
		CLight_Manager::GetInstance()->Release_Light( m_szPickingLightGroup, m_iPickingLightIndex );
		m_pLightMgr->Delete_Light( m_szPickingLightGroup, m_iPickingLightIndex );

		// Light 를 보관하고 있는 변수들 갱신..
		_uint iCount = m_vecLightItem.size();
		for ( size_t i = 0; i < iCount; ++i )
		{
			if ( 0 == lstrcmp( m_vecLightItem[i].first, szName ) )
			{
				m_TreeCtrl_LightGroup.DeleteItem( m_vecLightItem[i].second );
				Engine::Safe_Delete_Array( m_vecLightItem[i].first );
				m_vecLightItem.erase( m_vecLightItem.begin() + i );
				break;
			}
		}

		if ( 0 == lstrcmp( m_CurSelectItem.first, szName ) )
		{
			m_CurSelectItem.first = nullptr;
			m_CurSelectItem.second = NULL;
		}

		// 관련 변수 초기화..
		lstrcpy( m_szPickingLightGroup, L"" );
		m_iPickingLightIndex = 0;

		m_bIsSelectLight = false;
	}
}

void CTool_Light::Save_Data( void )
{
	// 일단 백업해두기..
	m_pLightMgr->Save_File( L"./BackUpFile/SaveFileBackUp.dat" );

	CFileDialog		Dlg(FALSE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	TCHAR szCurrDirectory[MAX_PATH] = L"";		// 따로 보관할 문자열

	GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.

	lstrcpy(szCurrDirectory, szDirectory);

	PathRemoveFileSpec(szDirectory);

	Dlg.GetOFN().lpstrInitialDir = szDirectory;

	if (Dlg.DoModal() == IDOK)
	{
		if ( FAILED( m_pLightMgr->Save_File( Dlg.GetPathName() ) ) )
		{
			m_bIsSaveOrLoadState = false;
			return;
		}
		else
			AfxMessageBox(L"Save Succesed!!");

		m_bIsSaveOrLoadState = false;
	}
}

void CTool_Light::Load_Data( void )
{
	CFileDialog		Dlg( TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE );

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	TCHAR szCurrDirectory[MAX_PATH] = L"";		// 따로 보관할 문자열

	GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.

	lstrcpy(szCurrDirectory, szDirectory);

	PathRemoveFileSpec(szDirectory);

	Dlg.GetOFN().lpstrInitialDir = szDirectory;

	if (Dlg.DoModal() == IDOK)
	{
		if ( FAILED( m_pLightMgr->Load_File( Dlg.GetPathName() ) ) )
		{
			m_bIsSaveOrLoadState = false;
			return;
		}
		else
			AfxMessageBox( L"Load Succesed!!" );

		m_bIsSaveOrLoadState = false;
	}
}

void CTool_Light::Load_ObjectData( const MAPKIND& _eMapKind )
{
	HANDLE hFile = NULL;

	switch ( _eMapKind )
	{
		case MapKind_Stage1:
		{
			// Stage1
			hFile = CreateFile(L"../Bin/Data/Stage0.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
									   , FILE_ATTRIBUTE_NORMAL, NULL);
		}
		break;
		case MapKind_Stage2:
		{
			// Stage2
			hFile = CreateFile(L"../Bin/Data/Stage2_1.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
									   , FILE_ATTRIBUTE_NORMAL, NULL);
		}
		break;
		case MapKind_Stage3:
		{
			// Stage3
			hFile = CreateFile(L"../Bin/Data/Stage3_0.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
									   , FILE_ATTRIBUTE_NORMAL, NULL);
		}
		break;
		case MapKind_BossStage:
		{
			// BossStage
			hFile = CreateFile(L"../Bin/Data/StageBoss_0.txt", GENERIC_READ, NULL, NULL, OPEN_EXISTING
									   , FILE_ATTRIBUTE_NORMAL, NULL);
		}
		break;
		default:
			AfxMessageBox( L"Strange Map Kind" );
			return;
	}

	Engine::CComponent*		pComponent = nullptr;
	Engine::CGameObject*	pGameObject = nullptr;

	if ( INVALID_HANDLE_VALUE == hFile ||
		 NULL == hFile )
	{
		AfxMessageBox( L"Load Failed!!" );
		return;
	}

	DWORD dwByte = 0;

	_ulong dwNumObject = 0;

	ReadFile(hFile, &dwNumObject, sizeof(_ulong), &dwByte, nullptr);

	MAPOBJECT* pObjectData = new MAPOBJECT;

	///////////////////////////////////////////////////////////////////////
	// Add By.KMH - 렌더러에 계속 저장안하게 씬 불러올 때만 저장..
	///////////////////////////////////////////////////////////////////////
	CRenderer::STATICRENDERGROUP	StaticRenderGroup;
	_ulong							dwTotalCount = dwNumObject;
	StaticRenderGroup.iGroupCount = dwTotalCount;
	StaticRenderGroup.pGroup = new CRenderer::STATICRENDEROBJECT[dwTotalCount];

	for (_uint i = 0; i < dwNumObject; ++i)
	{
		//ZeroMemory(pObjectData, sizeof(MAPOBJECT));

		ReadFile(hFile, pObjectData, sizeof(MAPOBJECT), &dwByte, nullptr);

		CComponent* pComponent_Find = CComponent_Manager::GetInstance()->Find_Component( SCENE_TOOL, pObjectData->ComName );
		if (nullptr == pComponent_Find)
		{
			// For.Mesh_Tree
			pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, pObjectData->FullPath, pObjectData->FileName);
			if (nullptr == pComponent)
				return;
			if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, CString_Manager::GetInstance()->GetStr(pObjectData->ComName), pComponent)))
				return;
		}

		pGameObject = CBaseObject_UseTool::Create(m_pGraphicDev, *pObjectData, true, nullptr);
		if (nullptr == pGameObject)
			return;

		if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_TOOL, L"Layer_BaseObject", pGameObject)))
			return;


		// Add By.KMH - 생성된 게임 오브젝트 구조체에 저장..
		StaticRenderGroup.pGroup[i].pGameObject = pGameObject;
		StaticRenderGroup.pGroup[i].fSort = 0.f;
		StaticRenderGroup.pGroup[i].fCullSize = ((CBaseObject_UseTool*)pGameObject)->Get_CullSize();
	}

	ReadFile(hFile, &dwNumObject, sizeof(_uint), &dwByte, nullptr);

	//MAPOBJECT* pObjectData = new MAPOBJECT;
	while (true)
	{
		ZeroMemory(pObjectData, sizeof(MAPOBJECT));

		_uint dwComObject = 0;

		ReadFile(hFile, &dwComObject, sizeof(_uint), &dwByte, nullptr);

		_tchar tComKey[128] = L"";

		ReadFile(hFile, &tComKey, sizeof(_tchar) * 128, &dwByte, nullptr);

		_uint iSize = 0;

		if (dwNumObject != iSize)
		{
			_uint iFirst = 0;

			// Add By.KMH - 두 번째 루프를 돌기 전 더 큰 공간을 생성하고 거기에 옮기기..
			_ulong dwTemp = dwTotalCount;
			dwTotalCount += dwComObject;
			CRenderer::STATICRENDERGROUP	Tmp;
			Tmp.pGroup = new CRenderer::STATICRENDEROBJECT[dwTotalCount];
			Tmp.iGroupCount = dwTotalCount;

			for ( _uint i = 0; i < dwTemp; ++i )
			{
				Tmp.pGroup[i].pGameObject = StaticRenderGroup.pGroup[i].pGameObject;
				Tmp.pGroup[i].fSort = StaticRenderGroup.pGroup[i].fSort;
				Tmp.pGroup[i].fCullSize = StaticRenderGroup.pGroup[i].fCullSize;
			}

			Engine::Safe_Delete_Array( StaticRenderGroup.pGroup );
			StaticRenderGroup = Tmp;

			for (_uint i  = 0; i < dwComObject; ++i)
			{
				_matrix matWorld;
				ReadFile(hFile, matWorld, sizeof(_matrix), &dwByte, nullptr);

				ZeroMemory(pObjectData, sizeof(MAPOBJECT));
				ReadFile(hFile, pObjectData, sizeof(MAPOBJECT), &dwByte, nullptr);

				CComponent* pComponent_Find = CComponent_Manager::GetInstance()->Find_Component(SCENE_TOOL, pObjectData->ComName);
				if (nullptr == pComponent_Find)
				{
					// For.Mesh_Tree
					pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, pObjectData->FullPath, pObjectData->FileName);
					if (nullptr == pComponent)
						return;
					if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_TOOL, CString_Manager::GetInstance()->GetStr(pObjectData->ComName), pComponent)))
						return;
				}

				pGameObject = CBaseObject_UseTool::Create(m_pGraphicDev, *pObjectData, false, &matWorld);

				if (nullptr == pGameObject)
					return;

				if (FAILED(CObject_Manager::GetInstance()->Ready_GameObject(SCENE_TOOL, L"Layer_BaseObject", pGameObject)))
					return;

				// Add By.KMH - 생성된 게임 오브젝트 구조체에 저장..
				StaticRenderGroup.pGroup[dwTemp + i].pGameObject = pGameObject;
				StaticRenderGroup.pGroup[dwTemp + i].fSort = 0.f;
				StaticRenderGroup.pGroup[dwTemp + i].fCullSize = ((CBaseObject_UseTool*)pGameObject)->Get_CullSize();

				++iFirst;
			}
		}

		if (0 == dwByte)
		{
			//Engine::Safe_Delete(pObjectData);
			break;
		}

		++iSize;
	}

	Engine::Safe_Delete(pObjectData);
	CloseHandle(hFile);

	// Add By.KMH - 렌더러에 최종적으로 저장..
	CRenderer* pRenderer = dynamic_cast<CRenderer*>(CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" ));
	if ( nullptr == pRenderer )
	{
		Engine::Safe_Delete_Array( StaticRenderGroup.pGroup );
		return;
	}
	if ( FAILED( pRenderer->Add_StaticRenderGroup( CRenderer::STATICRENDER_NONALPHA, StaticRenderGroup ) ) )
	{
		Engine::Safe_Delete_Array( StaticRenderGroup.pGroup );
		return;
	}

	m_iObjectCount = StaticRenderGroup.iGroupCount;
	m_ppObjects = new CBaseObject_UseTool*[StaticRenderGroup.iGroupCount];
	for ( _uint i = 0; i < StaticRenderGroup.iGroupCount; ++i )
	{
		m_ppObjects[i] = (CBaseObject_UseTool*)StaticRenderGroup.pGroup[i].pGameObject;
	}

	Engine::Safe_Release( pRenderer );
}

void CTool_Light::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_TREE1, m_TreeCtrl_LightGroup );

	DDX_Text( pDX, IDC_EDIT_LIGHTGROUPNAME, m_StrArr[Str_LightGroupName] );

	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIRX, m_StrArr[Str_LightDirX] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIRY, m_StrArr[Str_LightDirY] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIRZ, m_StrArr[Str_LightDirZ] );

	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTPOSX, m_StrArr[Str_LightPosX] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTPOSY, m_StrArr[Str_LightPosY] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTPOSZ, m_StrArr[Str_LightPosZ] );

	DDX_Text( pDX, IDC_EDIT_SHADER_SPOTLIGHT_MINANGLE, m_StrArr[Str_LightMinAngle] );
	DDX_Text( pDX, IDC_EDIT_SHADER_SPOTLIGHT_MAXANGLE, m_StrArr[Str_LightMaxAngle] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTRANGE, m_StrArr[Str_LightRange] );

	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIFFUSER, m_StrArr[Str_LightDiffuseR] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIFFUSEG, m_StrArr[Str_LightDiffuseG] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIFFUSEB, m_StrArr[Str_LightDiffuseB] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTDIFFUSEA, m_StrArr[Str_LightDiffuseA] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTAMBIENTR, m_StrArr[Str_LightAmbientR] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTAMBIENTG, m_StrArr[Str_LightAmbientG] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTAMBIENTB, m_StrArr[Str_LightAmbientB] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTAMBIENTA, m_StrArr[Str_LightAmbientA] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTSPECULARR, m_StrArr[Str_LightSpecularR] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTSPECULARG, m_StrArr[Str_LightSpecularG] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTSPECULARB, m_StrArr[Str_LightSpecularB] );
	DDX_Text( pDX, IDC_EDIT_SHADER_LIGHTSPECULARA, m_StrArr[Str_LightSpecularA] );

	DDX_Text( pDX, IDC_EDIT_YPOSPLUS, m_StrArr[Str_YPosPlus] );

	DDX_Radio( pDX, IDC_RADIO_LIGHTMODE1, m_Radio_LightMode );

	DDX_Control( pDX, IDC_COMBO_SHADER_LIGHTTYPE, m_Combo_LightType );

	DDX_Control( pDX, IDC_CHECK_PICKINGYPOSTYPE, m_BtnArr[Btn_YPosPickingPos] );
	DDX_Control( pDX, IDC_CHECK_SHOWLIGHTDEBUG, m_BtnArr[Btn_LightDebug] );
	DDX_Control( pDX, IDC_CHECK_LIGHTPICKING, m_BtnArr[Btn_LightPicking] );
	DDX_Control( pDX, IDC_CHECK_CAMRUNMODE, m_BtnArr[Btn_CamRunMode] );
	DDX_Control( pDX, IDC_CHECK_LIGHTONOFF, m_BtnArr[Btn_LightOnOff] );
	DDX_Control( pDX, IDC_CHECK_LIGHTGROUPONOFF, m_BtnArr[Btn_LightGroupOnOff] );
	DDX_Control( pDX, IDC_CHECK_DIFFUSESPECULARSAMEPOWER, m_BtnArr[Btn_DiffuseSpecularSamePower] );
	DDX_Control( pDX, IDC_CHECK_FLOATINGPOWER, m_BtnArr[Btn_FloatingPower] );

	DDX_Control( pDX, IDC_SLIDER_LIGHTDIRX, m_SliderArr[Slider_LightDirX] );
	DDX_Control( pDX, IDC_SLIDER_LIGHTDIRY, m_SliderArr[Slider_LightDirY] );
	DDX_Control( pDX, IDC_SLIDER_LIGHTDIRZ, m_SliderArr[Slider_LightDirZ] );
	DDX_Control( pDX, IDC_SLIDER_SPOTLIGHT_MINANGLE, m_SliderArr[Slider_LightMinAngle] );
	DDX_Control( pDX, IDC_SLIDER_SPOTLIGHT_MAXANGLE, m_SliderArr[Slider_LightMaxAngle] );
	DDX_Control( pDX, IDC_SLIDER_LIGHTDIFFUSEA, m_SliderArr[Slider_LightDiffuseA] );
	DDX_Control( pDX, IDC_SLIDER_LIGHTAMBIENTA, m_SliderArr[Slider_LightAmbientA] );
	DDX_Control( pDX, IDC_SLIDER_LIGHTSPECULARA, m_SliderArr[Slider_LightSpecularA] );
	DDX_Control( pDX, IDC_COMBO_MAPKIND, m_ComboBox_Map );
}


BEGIN_MESSAGE_MAP(CTool_Light, CPropertyPage)
	ON_NOTIFY( TVN_SELCHANGED, IDC_TREE1, &CTool_Light::OnTvnSelchangedTree1 )
	ON_BN_CLICKED( IDC_BUTTON_MAKELIGHTGROUP, &CTool_Light::OnBnClickedButtonMakelightgroup )
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_LIGHTMODE1, IDC_RADIO_LIGHTMODE2, &CTool_Light::OnChangeRadioData )
	ON_WM_HSCROLL()
	ON_BN_CLICKED( IDC_BUTTON_SHOW_COLORDIALOG, &CTool_Light::OnBnClickedButtonShowColordialog )
	ON_BN_CLICKED( IDC_BUTTON_SHOW_AMBIENTCOLORDIALOG2, &CTool_Light::OnBnClickedButtonShowAmbientcolordialog2 )
	ON_BN_CLICKED( IDC_BUTTON_SHOW_SPECULARCOLORDIALOG3, &CTool_Light::OnBnClickedButtonShowSpecularcolordialog3 )
	ON_BN_CLICKED(IDC_MAP_RENDER_REFRESH, &CTool_Light::OnBnClickedMapRenderRefresh)
	ON_BN_CLICKED( IDC_BUTTON_CREATE_MAP, &CTool_Light::OnBnClickedButtonCreateMap )
END_MESSAGE_MAP()


// CTool_Light 메시지 처리기입니다.


BOOL CTool_Light::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 사용할 컴포넌트 생성..
	m_pPickingCom = dynamic_cast<CPicking*>(CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CPicking" ));
	if ( nullptr == m_pPickingCom )
	{
		AfxMessageBox( L"m_pPickingCom is nullptr" );
		return FALSE;
	}

	m_pRendererCom = dynamic_cast<CRenderer*>(CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" ));
	if ( nullptr == m_pRendererCom )
	{
		AfxMessageBox( L"m_pRendererCom is nullptr" );
		return FALSE;
	}

	m_pTerrainBufferCom = ((CTerrain_ToolUse*)CObject_Manager::GetInstance()->Get_FindObject( SCENE_TOOL, L"Layer_Terrain", 0 ))->Get_TerrainBuffer();
	if(nullptr == m_pTerrainBufferCom )
	{
		AfxMessageBox( L"m_pTerrainBufferCom is nullptr" );
		return FALSE;
	}

	// 필요한 오브젝트들 가져오기..
	m_pCamera = dynamic_cast<CCamera_UseTool*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
	if ( nullptr == m_pCamera )
	{
		AfxMessageBox( L"m_pCamera is nullptr" );
		return FALSE;
	}


	// ============ GraphicDevice 가져오기.. ============  //
	m_pGraphicDev = ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->Get_GraphicDev();
	if ( nullptr == m_pGraphicDev )
	{
		AfxMessageBox( L"m_pGraphicDev is nullptr" );
		return FALSE;
	}


	// ============ LightMgr 클래스 생성.. ============ //
	m_pLightMgr = CLightMgr_UseTool::Create( m_pGraphicDev );
	if ( nullptr == m_pLightMgr )
	{
		AfxMessageBox( L"FAILED to m_pLightMgr Create" );
		return FALSE;
	}


	// 트리 컨트롤의 Root Item 설정..
	m_RootItem = m_TreeCtrl_LightGroup.InsertItem( L"LightData", TVI_ROOT, TVI_LAST );


	// Default LightGroup 생성..
	Add_LightGroup( L"Static" );


	// ============ Combo Box 초기화.. ============ //
	m_Combo_LightType.AddString( L"Point Light" );
	m_Combo_LightType.AddString( L"Spot Light" );
	m_Combo_LightType.AddString( L"Directional Light" );

	m_Combo_LightType.SetCurSel( 0 );


	m_ComboBox_Map.AddString( L"Stage1" );
	m_ComboBox_Map.AddString( L"Stage2" );
	m_ComboBox_Map.AddString( L"Stage3" );
	m_ComboBox_Map.AddString( L"Boss Stage" );

	m_ComboBox_Map.SetCurSel( 0 );


	// ============ Edit Box 초기화.. ============ //
	m_StrArr[Str_LightDirX] = m_StrArr[Str_LightDirY] = m_StrArr[Str_LightDirZ] = L"1";

	m_StrArr[Str_LightPosX] = m_StrArr[Str_LightPosY] = m_StrArr[Str_LightPosZ] = L"0";

	m_StrArr[Str_LightDiffuseR] = m_StrArr[Str_LightDiffuseG] = m_StrArr[Str_LightDiffuseB] = m_StrArr[Str_LightDiffuseA] = L"1";
	m_StrArr[Str_LightAmbientR] = m_StrArr[Str_LightAmbientG] = m_StrArr[Str_LightAmbientB] = L"0.3";
	m_StrArr[Str_LightAmbientA] = L"1";
	m_StrArr[Str_LightSpecularR] = m_StrArr[Str_LightSpecularG] = m_StrArr[Str_LightSpecularB] = m_StrArr[Str_LightSpecularA] = L"1";

	m_StrArr[Str_LightMinAngle] = m_StrArr[Str_LightMaxAngle] = m_StrArr[Str_LightRange] = L"0";

	m_StrArr[Str_LightGroupName] = L"";


	// ============ Slider 초기화.. ============ //
	for ( _uint i = Slider_LightDirX; i <= Slider_LightDirZ; ++i )
	{
		m_SliderArr[i].SetRange( (_int)(-1 * m_fDirMul), (_int)(1 * m_fDirMul) );
		m_SliderArr[i].SetRangeMin( (_int)(-1 * m_fDirMul) );
		m_SliderArr[i].SetRangeMax( (_int)(1 * m_fDirMul) );
	}

	for ( _uint i = Slider_LightMinAngle; i <= Slider_LightMaxAngle; ++i )
	{
		m_SliderArr[i].SetRange( 0, (_int)(360 * m_fAngleMul) );
		m_SliderArr[i].SetRangeMin( 0 );
		m_SliderArr[i].SetRangeMax( (_int)(360 * m_fAngleMul) );
	}

	for ( _uint i = Slider_LightDiffuseA; i <= Slider_LightSpecularA; ++i )
	{
		m_SliderArr[i].SetRange( (_int)(0.f * m_fColorMul), (_int)(10.f * m_fColorMul) );
		m_SliderArr[i].SetRangeMin( (_int)(0.f * m_fColorMul) );
		m_SliderArr[i].SetRangeMax( (_int)(10.f * m_fColorMul) );
	}


	// Check Box 초기화..
	Set_BtnCheck( Btn_YPosPickingPos, TRUE );
	Set_BtnCheck( Btn_LightDebug, TRUE );
	Set_BtnCheck( Btn_LightPicking, TRUE );
	Set_BtnCheck( Btn_CamRunMode, FALSE );
	Set_BtnCheck( Btn_LightOnOff, FALSE );
	Set_BtnCheck( Btn_DiffuseSpecularSamePower, TRUE );
	Set_BtnCheck( Btn_FloatingPower, TRUE );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CTool_Light::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	bIsActive = false;
	return CPropertyPage::OnKillActive();
}


BOOL CTool_Light::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	bIsActive = true;
	return CPropertyPage::OnSetActive();
}


void CTool_Light::OnTvnSelchangedTree1( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	_float fTime = m_fActiveTimeAcc;

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	CString str = m_TreeCtrl_LightGroup.GetItemText( hTreeItem );

	m_CurSelectItem.first = m_CurSelectLightGroupItem.first = nullptr;
	m_CurSelectItem.second = m_CurSelectLightGroupItem.second = NULL;

	_uint iCount = m_vecLightItem.size();
	for ( _uint i = 0; i < iCount; ++i )
	{
		if ( 0 == lstrcmp( m_vecLightItem[i].first, str ) )
		{
			m_CurSelectItem.second = m_vecLightItem[i].second;
			m_CurSelectItem.first = m_vecLightItem[i].first;
		}
	}

	iCount = m_vecLightGroupItem.size();
	Set_BtnEnable( Btn_LightGroupOnOff, FALSE );
	for ( _uint i = 0; i < iCount; i++ )
	{
		if ( 0 == lstrcmp( m_vecLightGroupItem[i].first, str ) )
		{
			m_CurSelectLightGroupItem.first = m_vecLightGroupItem[i].first;
			m_CurSelectLightGroupItem.second = m_vecLightGroupItem[i].second;

			Set_BtnEnable( Btn_LightGroupOnOff, TRUE );
			break;
		}
	}
}

void CTool_Light::OnBnClickedButtonMakelightgroup()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CTool_Light::OnChangeRadioData( _uint value )
{
	UpdateData( TRUE );

	for ( _uint i = 0; i < LightMode_End; ++i )
	{
		if ( i == m_Radio_LightMode )
			m_pLightMgr->Set_LightMode( LIGHTMODE( i ) );
	}

}


void CTool_Light::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( pScrollBar )
	{
		_vec3 vDir( 
			  (_float)m_SliderArr[Slider_LightDirX].GetPos() / m_fDirMul
			, (_float)m_SliderArr[Slider_LightDirY].GetPos() / m_fDirMul
			, (_float)m_SliderArr[Slider_LightDirZ].GetPos() / m_fDirMul 
		);

		_float fMinAngle = (_float)m_SliderArr[Slider_LightMinAngle].GetPos() / m_fAngleMul;
		_float fMaxAngle = (_float)m_SliderArr[Slider_LightMaxAngle].GetPos() / m_fAngleMul;

		_float fTempDiffusePower = _ttof( m_StrArr[Str_LightDiffuseA] );
		_float fTempAmbientPower = _ttof( m_StrArr[Str_LightAmbientA] );
		_float fTempSpecularPower = _ttof( m_StrArr[Str_LightSpecularA] );

		_float fDiffusePower = (_float)m_SliderArr[Slider_LightDiffuseA].GetPos() / m_fColorMul;

		_float fAmbientPower = (_float)m_SliderArr[Slider_LightAmbientA].GetPos() / m_fColorMul;

		_float fSpecularPower = (_float)m_SliderArr[Slider_LightSpecularA].GetPos() / m_fColorMul;

		if ( Get_BtnCheck( Btn_DiffuseSpecularSamePower ) == TRUE )
		{
			fSpecularPower = fDiffusePower;
		}

		if ( Get_BtnCheck( Btn_FloatingPower ) == FALSE )
		{
			fDiffusePower = (_int)fDiffusePower;
			fAmbientPower = (_int)fAmbientPower;
			fSpecularPower = (_int)fSpecularPower;
		}

		m_StrArr[Str_LightDirX].Format( L"%f", vDir.x );
		m_StrArr[Str_LightDirY].Format( L"%f", vDir.y );
		m_StrArr[Str_LightDirZ].Format( L"%f", vDir.z );

		m_StrArr[Str_LightMinAngle].Format( L"%f", fMinAngle );
		m_StrArr[Str_LightMaxAngle].Format( L"%f", fMaxAngle );

		m_StrArr[Str_LightDiffuseA].Format( L"%f", fDiffusePower );

		m_StrArr[Str_LightAmbientA].Format( L"%f", fAmbientPower );

		m_StrArr[Str_LightSpecularA].Format( L"%f", fSpecularPower );

	}
	else
	{
		// CScrollView를 상속받은 뷰의 경우 프래임의 스크롤롤 동작시 pScrollBar이 NULL된다.
	}

	UpdateData( FALSE );

	CPropertyPage::OnHScroll( nSBCode, nPos, pScrollBar );
}


void CTool_Light::OnBnClickedButtonShowColordialog()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_ColorDialog.m_cc.Flags |= CC_FULLOPEN;
	//m_ColorDlg.m_cc.rgbResult = RGB(m_iDiffuse_R_Edit, m_iDiffuse_G_Edit, m_iDiffuse_B_Edit);

	m_bIsSelectColor = true;

	if (m_ColorDialog.DoModal() == IDOK)
	{
		COLORREF color = m_ColorDialog.GetColor();

		_ulong dwRValue = GetRValue( color );
		_ulong dwGValue = GetGValue( color );
		_ulong dwBValue = GetBValue( color );

		m_StrArr[Str_LightDiffuseR].Format( L"%f", ((_float)dwRValue) / 255.f );
		m_StrArr[Str_LightDiffuseG].Format( L"%f", ((_float)dwGValue) / 255.f );
		m_StrArr[Str_LightDiffuseB].Format( L"%f", ((_float)dwBValue) / 255.f );

		m_bIsSelectColor = false;
	}
	else
		m_bIsSelectColor = false;

	UpdateData(FALSE);
}


void CTool_Light::OnBnClickedButtonShowAmbientcolordialog2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_ColorDialog.m_cc.Flags |= CC_FULLOPEN;
	//m_ColorDlg.m_cc.rgbResult = RGB(m_iDiffuse_R_Edit, m_iDiffuse_G_Edit, m_iDiffuse_B_Edit);

	m_bIsSelectColor = true;

	if (m_ColorDialog.DoModal() == IDOK)
	{
		COLORREF color = m_ColorDialog.GetColor();

		_ulong dwRValue = GetRValue( color );
		_ulong dwGValue = GetGValue( color );
		_ulong dwBValue = GetBValue( color );

		m_StrArr[Str_LightAmbientR].Format( L"%f", ((_float)dwRValue) / 255.f );
		m_StrArr[Str_LightAmbientG].Format( L"%f", ((_float)dwGValue) / 255.f );
		m_StrArr[Str_LightAmbientB].Format( L"%f", ((_float)dwBValue) / 255.f );

		m_bIsSelectColor = false;
	}
	else
		m_bIsSelectColor = false;

	UpdateData(FALSE);
}


void CTool_Light::OnBnClickedButtonShowSpecularcolordialog3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	m_ColorDialog.m_cc.Flags |= CC_FULLOPEN;
	//m_ColorDlg.m_cc.rgbResult = RGB(m_iDiffuse_R_Edit, m_iDiffuse_G_Edit, m_iDiffuse_B_Edit);

	m_bIsSelectColor = true;

	if (m_ColorDialog.DoModal() == IDOK)
	{
		COLORREF color = m_ColorDialog.GetColor();

		_ulong dwRValue = GetRValue( color );
		_ulong dwGValue = GetGValue( color );
		_ulong dwBValue = GetBValue( color );

		m_StrArr[Str_LightSpecularR].Format( L"%f", ((_float)dwRValue) / 255.f );
		m_StrArr[Str_LightSpecularG].Format( L"%f", ((_float)dwGValue) / 255.f );
		m_StrArr[Str_LightSpecularB].Format( L"%f", ((_float)dwBValue) / 255.f );

		m_bIsSelectColor = false;
	}
	else
		m_bIsSelectColor = false;

	UpdateData(FALSE);
}


void CTool_Light::OnBnClickedMapRenderRefresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CObject_Manager::GetInstance()->Update_StaticObject();
}


void CTool_Light::OnBnClickedButtonCreateMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pRendererCom->Release_StaticRenderGroup();

	_int iIndex = m_ComboBox_Map.GetCurSel();

	if ( -1 == iIndex )
		return;

	Modify_StageType( MAPKIND( iIndex ) );

	((CMainFrame*)AfxGetMainWnd())->Get_EditBox()->Get_EditSheet()->Get_ShaderTool()->Modify_StageType( m_eMapKind );
}
