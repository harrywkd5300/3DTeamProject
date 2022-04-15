// ../Codes/Tool_Shader.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "Tool_Shader.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "ShaderToolMainView.h"
#include "EditBox.h"
#include "EditSheet.h"
#include "Tool_Light.h"

#include "Component_Manager.h"
#include "Light_Manager.h"
#include "LightMgr_UseTool.h"
#include "Object_manager.h"


// CTool_Shader 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTool_Shader, CPropertyPage)

CTool_Shader::CTool_Shader()
	: CPropertyPage( IDD_TOOL_SHADER )
	, m_pGraphicDev( nullptr )
{

}

CTool_Shader::~CTool_Shader()
{
	Engine::Safe_Delete_Array( m_pAllEffectArr );

	_uint iTargetCount = m_pRendererCom->Get_TargetCount();
	for ( size_t i = 0; i < iTargetCount; ++i )
	{
		Engine::Safe_Delete_Array( m_pAllTagetNameArr[i] );
		Engine::Safe_Delete_Array( m_pAllTagetOriginalNameArr[i] );
	}
	Engine::Safe_Delete_Array( m_pAllTagetNameArr );
	Engine::Safe_Delete_Array( m_pAllTagetOriginalNameArr );
}

void CTool_Shader::Update_Tool( const _float & _fTimeDelta )
{
	if ( nullptr == m_pRendererCom )
	{
		m_pRendererCom = (CRenderer*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_TOOL, L"CRenderer" );

#ifdef ADD_GETTER_TARGETANDSHADER
		_uint iEffectCount = m_pRendererCom->Get_EffectCount();
		m_pAllEffectArr = new LPD3DXEFFECT[iEffectCount];
		m_vecShaderData.resize( iEffectCount );
		for ( size_t i = 0; i < iEffectCount; ++i )
		{
			m_pRendererCom->Get_Effect( &m_pAllEffectArr[i], iEffectCount );

			string str;

			switch ( i )
			{
				case Engine::CRenderer::Shader_Deferred:
					str = etos( Deferred );
					break;

				case Engine::CRenderer::Shader_Filter:
					str = etos( Filter );
					break;

				case Engine::CRenderer::Shader_FXAA:
					str = etos( FXAA );
					break;

				case Engine::CRenderer::Shader_HDR:
					str = etos( HDR );
					break;

				case Engine::CRenderer::Shader_Light:
					str = etos( Light );
					break;

				case Engine::CRenderer::Shader_PostProcess:
					str = etos( PostProcess );
					break;

				case Engine::CRenderer::Shader_Shadow:
					str = etos( Shadow );
					break;
			}

			_tchar szTemp[128] = L"";
			const char* pStr = str.c_str();
			int nLen = MultiByteToWideChar( CP_ACP, 0, pStr, strlen( pStr ), NULL, NULL );
			MultiByteToWideChar( CP_ACP, 0, pStr, strlen( pStr ), szTemp, nLen );

			m_ListBox_Shader.AddString( szTemp );
		}

		m_ListBox_RenderTarget.AddString( L"None" );
		_uint iTargetCount = m_pRendererCom->Get_TargetCount();
		m_pAllTagetNameArr = new _tchar*[iTargetCount];
		m_pAllTagetOriginalNameArr = new _tchar*[iTargetCount];
		for ( size_t i = 0; i < iTargetCount; ++i )
		{
			_tchar szTargetName[128] = L"";
			m_pAllTagetNameArr[i] = new _tchar[128];
			m_pAllTagetOriginalNameArr[i] = new _tchar[128];
			m_pRendererCom->Get_TargetName( szTargetName, (_uint)i );
			lstrcpy( m_pAllTagetOriginalNameArr[i], szTargetName );

			_uint jLength = lstrlen( szTargetName );
			_bool bFind = false;
			for ( size_t j = 0; j < jLength; ++j )
			{
				if ( szTargetName[j] == L'_' )
				{
					memcpy( m_pAllTagetNameArr[i], szTargetName + j + 1, sizeof( _tchar ) * (jLength - j - 1) );
					m_pAllTagetNameArr[i][jLength - j - 1] = L'\0';
					bFind = true;
					break;
				}
			}
			if ( false == bFind )
				lstrcpy( m_pAllTagetNameArr[i], szTargetName );

			m_ListBox_RenderTarget.AddString( m_pAllTagetNameArr[i] );
		}
#endif
	}

	UpdateData( TRUE );

	Update_CheckBox( _fTimeDelta );
	Update_EditBox( _fTimeDelta );
	Update_Slider( _fTimeDelta );
	Update_SelectShaderValue( _fTimeDelta );

	Update_CurShaderVar( _fTimeDelta );
}

void CTool_Shader::Modify_StageType( const MAPKIND & _eMapKind )
{
	switch ( _eMapKind )
	{
		case MapKind_Stage1:		// Stage 1..
			Load_ShaderVarData( L"./EffectVarData_Stage1.dat" );
			break;
		case MapKind_Stage2:		// Stage 2..
			Load_ShaderVarData( L"./EffectVarData_Stage2.dat" );
			break;
		case MapKind_Stage3:		// Stage 3..
			Load_ShaderVarData( L"./EffectVarData_Stage3.dat" );
			break;
		case MapKind_BossStage:		// Boss Stage..
			Load_ShaderVarData( L"./EffectVarData_BossStage.dat" );
			break;
		default:
			AfxMessageBox( L"Strange MapKind" );
			return;
	}

	m_eMapKind = _eMapKind;
}

void CTool_Shader::Update_CheckBox( const _float & _fTimeDelta )
{
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_Light, m_BtnArr[Btn_Light].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_HDR, m_BtnArr[Btn_HDR].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_EdgeDetecting, m_BtnArr[Btn_EdgeDetecting].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_Shadow, m_BtnArr[Btn_Shadow].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_MotionBlur, m_BtnArr[Btn_MotionBlur].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_SSAO, m_BtnArr[Btn_SSAO].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_DOF, m_BtnArr[Btn_DOF].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_Fog, m_BtnArr[Btn_Fog].GetCheck() != 0 );
	m_pRendererCom->Set_ShaderOptionOnOff( Engine::CRenderer::ShaderOption_LightShaft, m_BtnArr[Btn_LightShaft].GetCheck() != 0 );
}

void CTool_Shader::Update_EditBox( const _float& _fTimeDelta )
{
	
}

void CTool_Shader::Update_Slider( const _float& _fTimeDelta )
{
	// ---------------Setting Select Var Value..--------------- //
	_float fSelectVarValue = _ttof( m_StrArr[Str_SelectVarValue] );
	m_SliderArr[Slider_SelectVarValue].SetPos( (_int)(fSelectVarValue * 100000.f) );

	_float fMin = _ttof( m_StrArr[Str_SelectVarMin] );
	_float fMax = _ttof( m_StrArr[Str_SelectVarMax] );
	_int iMin = (_int)(fMin * 100000.f);
	_int iMax = (_int)(fMax * 100000.f);
	m_SliderArr[Slider_SelectVarValue].SetRange( iMin, iMax );
	m_SliderArr[Slider_SelectVarValue].SetRangeMin( iMin );
	m_SliderArr[Slider_SelectVarValue].SetRangeMax( iMax );
}

void CTool_Shader::Update_CurShaderVar( const _float & _fTimeDelta )
{
	if ( -1 == m_iCurShader ||
		 m_iCurShader >= (_uint)m_vecShaderData.size() ||
		 -1 == m_iCurShaderVar ||
		 m_iCurShaderVar >= (_uint)m_vecShaderData[m_iCurShader].iEffectVarCount )
		return;

	Update_ShaderVarToolBox( m_vecShaderData[m_iCurShader].pEffectVar[m_iCurShaderVar] );
}

void CTool_Shader::Update_SelectShaderValue( const _float & _fTimeDelta )
{
	if ( -1 == m_iCurShader ||
		 m_iCurShader >= (_uint)m_vecShaderData.size() ||
		 -1 == m_iCurShaderVar ||
		 m_iCurShaderVar >= (_uint)m_vecShaderData[m_iCurShader].iEffectVarCount ||
		 0 == m_iCurSelectShaderValueNum )
		return;

	_float fFloat = _ttof( m_StrArr[Str_Float1 + (m_iCurSelectShaderValueNum - 1)] );
	m_vecShaderData[m_iCurShader].pEffectVar[m_iCurShaderVar].mData[m_iCurSelectShaderValueNum - 1] = fFloat;
}

void CTool_Shader::Show_ShaderData( const _uint& _iIndex )
{
	if ( _iIndex >= m_vecShaderData.size() )
		return;

	m_ListBox_ShaderVar.ResetContent();
	m_ListBox_ShaderVar.AddString( L"SelectNone" );

	_uint iCount = m_vecShaderData[_iIndex].iEffectVarCount;

	for ( _uint i = 0; i < iCount; ++i )
	{
		_tchar szTemp[128] = L"";
		int nLen = MultiByteToWideChar( CP_ACP, 0, m_vecShaderData[_iIndex].pEffectVar[i].szName, strlen( m_vecShaderData[_iIndex].pEffectVar[i].szName ), NULL, NULL );
		MultiByteToWideChar( CP_ACP, 0, m_vecShaderData[_iIndex].pEffectVar[i].szName, strlen( m_vecShaderData[_iIndex].pEffectVar[i].szName ), szTemp, nLen );

		m_ListBox_ShaderVar.AddString( szTemp );
	}

	m_iCurShader = _iIndex;
}

void CTool_Shader::Show_ShaderVarData( const _uint& _iIndex )
{
	if ( -1 == m_iCurShader ||
		 m_iCurShader >= m_vecShaderData.size() )
		return;
	if ( _iIndex >= m_vecShaderData[m_iCurShader].iEffectVarCount )
		return;

	CRenderer::EFFECTVARTYPE& ShaderVar = m_vecShaderData[m_iCurShader].pEffectVar[_iIndex];
	m_Combo_ShaderVar.SetCurSel( ShaderVar.eType );

	switch ( ShaderVar.eType )
	{
		case Engine::CRenderer::ShaderVar_Float:
		{
			_float var = ShaderVar.mData.m[0][0];
			m_StrArr[Str_Float1].Format( L"%f", var );
			m_StrArr[Str_Float2].Format( L"%d", 0 );
			m_StrArr[Str_Float3].Format( L"%d", 0 );
			m_StrArr[Str_Float4].Format( L"%d", 0 );
		}
		break;
		case Engine::CRenderer::ShaderVar_Int:
		{
			_int var = (_int)ShaderVar.mData.m[0][0];
			m_StrArr[Str_Float1].Format( L"%d", var );
			m_StrArr[Str_Float2].Format( L"%d", 0 );
			m_StrArr[Str_Float3].Format( L"%d", 0 );
			m_StrArr[Str_Float4].Format( L"%d", 0 );
		}
		break;
		case Engine::CRenderer::ShaderVar_Bool:
		{
			_bool var = (_bool)(ShaderVar.mData.m[0][0] == 0);
			m_StrArr[Str_Float1].Format( L"%d", var );
			m_StrArr[Str_Float2].Format( L"%d", 0 );
			m_StrArr[Str_Float3].Format( L"%d", 0 );
			m_StrArr[Str_Float4].Format( L"%d", 0 );
		}
		break;
		case Engine::CRenderer::ShaderVar_Vector:
		{
			_vec4 var = *(_vec4*)&ShaderVar.mData.m[0];
			m_StrArr[Str_Float1].Format( L"%f", var.x );
			m_StrArr[Str_Float2].Format( L"%f", var.y );
			m_StrArr[Str_Float3].Format( L"%f", var.z );
			m_StrArr[Str_Float4].Format( L"%f", var.w );
		}
		break;
		case Engine::CRenderer::ShaderVar_Matrix:
		{

		}
		break;
		case Engine::CRenderer::ShaderVar_Texture:
		{

		}
		break;
		default:
			AfxMessageBox( L"Sahder Variable Type is Strange" );
			return;
	}

	_tchar szTemp[128] = L"";
	int nLen = MultiByteToWideChar( CP_ACP, 0, ShaderVar.szName, strlen( ShaderVar.szName ), NULL, NULL );
	MultiByteToWideChar( CP_ACP, 0, ShaderVar.szName, strlen( ShaderVar.szName ), szTemp, nLen );
	m_StrArr[Str_VarName] = szTemp;

	m_iCurShaderVar = _iIndex;

	UpdateData( FALSE );
}

void CTool_Shader::Update_ShaderVarToolBox( CRenderer::EFFECTVARTYPE& _ShaderVar )
{
	_int iShaderTypeIndex = m_Combo_ShaderVar.GetCurSel();
	if ( -1 == iShaderTypeIndex )
		return;

	_ShaderVar.eType = CRenderer::ShaderVariableType( iShaderTypeIndex );

	switch ( _ShaderVar.eType )
	{
		case Engine::CRenderer::ShaderVar_Float:
			_ShaderVar.mData.m[0][0] = _ttof( m_StrArr[Str_Float1] );
			break;
		case Engine::CRenderer::ShaderVar_Int:
			_ShaderVar.mData.m[0][0] = (_float)_ttoi( m_StrArr[Str_Float1] );
			break;
		case Engine::CRenderer::ShaderVar_Bool:
			_ShaderVar.mData.m[0][0] = (_float)_ttoi( m_StrArr[Str_Float1] );
			break;
		case Engine::CRenderer::ShaderVar_Vector:
			_ShaderVar.mData.m[0][0] = _ttof( m_StrArr[Str_Float1] );
			_ShaderVar.mData.m[0][1] = _ttof( m_StrArr[Str_Float2] );
			_ShaderVar.mData.m[0][2] = _ttof( m_StrArr[Str_Float3] );
			_ShaderVar.mData.m[0][3] = _ttof( m_StrArr[Str_Float4] );
			break;
			//case Engine::CRenderer::_ShaderVar_Matrix:
			//	break;
			//case Engine::CRenderer::_ShaderVar_Texture:
			//	break;
		default:
			AfxMessageBox( L"Sahder Variable Type is Strange" );
			return;
	}

	int len = WideCharToMultiByte( CP_ACP, 0, m_StrArr[Str_VarName], -1, NULL, 0, NULL, NULL );
	WideCharToMultiByte( CP_ACP, 0, m_StrArr[Str_VarName], -1, _ShaderVar.szName, len, NULL, NULL );
}

void CTool_Shader::Save_ShaderVar( const _tchar * _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox( L"Failed to Save ShaderVarData" );
		return;
	}

	_uint iCount = (_uint)m_vecShaderData.size();
	_ulong dwByte = 0;
	
	WriteFile( hFile, &iCount, sizeof( _uint ), &dwByte, nullptr );
	for ( _uint i = 0; i < iCount; ++i )
	{
		WriteFile( hFile, &m_vecShaderData[i].iEffectVarCount, sizeof( _uint ), &dwByte, nullptr );
		for ( _uint j = 0; j < m_vecShaderData[i].iEffectVarCount; ++j )
		{
			WriteFile( hFile, &m_vecShaderData[i].pEffectVar[j], sizeof( CRenderer::EFFECTVARTYPE ), &dwByte, nullptr );
		}
	}

	CloseHandle( hFile );
	AfxMessageBox(L"Save Succed!!");
}

void CTool_Shader::Load_ShaderVarData( const _tchar* _pFilePath )
{
	for ( _uint i = 0, iCount = m_vecShaderData.size(); i < iCount; ++i )
	{
		Engine::Safe_Delete_Array( m_vecShaderData[i].pEffectVar );
	}
	m_vecShaderData.clear();
	vector<CRenderer::EFFECTVARDATA>().swap( m_vecShaderData );

	HANDLE hFile = CreateFile( _pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile == INVALID_HANDLE_VALUE )
	{
		AfxMessageBox( L"Failed to Load ShaderVarData" );
		return;
	}

	_uint iCount = 0;
	_ulong dwByte = 0;

	ReadFile( hFile, &iCount, sizeof( _uint ), &dwByte, nullptr );

	m_vecShaderData.reserve( iCount );
	for ( _uint i = 0; i < iCount; ++i )
	{
		CRenderer::EFFECTVARDATA	EffectVarData;

		_uint jLength = 0;
		ReadFile( hFile, &jLength, sizeof( _uint ), &dwByte, nullptr );

		EffectVarData.iEffectVarCount = jLength;
		EffectVarData.pEffectVar = new Engine::CRenderer::EFFECTVARTYPE[jLength];
		for ( _uint j = 0; j < jLength; ++j )
		{
			ReadFile( hFile, &EffectVarData.pEffectVar[j], sizeof( Engine::CRenderer::EFFECTVARTYPE ), &dwByte, nullptr );
		}

		m_vecShaderData.push_back( EffectVarData );

		m_pRendererCom->Set_ShaderVarData( i, &m_vecShaderData[i] );
	}

	CloseHandle( hFile );

	AfxMessageBox(L"Load Succed!!");
}

void CTool_Shader::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_COMBO_SHADER_VAR_TYPE, m_Combo_ShaderVar );
	DDX_Text( pDX, IDC_EDIT_SHADER_VAR_NAME, m_StrArr[Str_VarName] );
	DDX_Text( pDX, IDC_EDIT_SHADER_VAR_FLOAT1, m_StrArr[Str_Float1] );
	DDX_Text( pDX, IDC_EDIT_SHADER_VAR_FLOAT2, m_StrArr[Str_Float2] );
	DDX_Text( pDX, IDC_EDIT_SHADER_VAR_FLOAT3, m_StrArr[Str_Float3] );
	DDX_Text( pDX, IDC_EDIT_SHADER_VAR_FLOAT4, m_StrArr[Str_Float4] );
	DDX_Text( pDX, IDC_EDIT_SHADER_SELECTVARNUM, m_StrArr[Str_SelectVarNum] );
	DDX_Text( pDX, IDC_EDIT_SHADER_SELECTVARMIN, m_StrArr[Str_SelectVarMin] );
	DDX_Text( pDX, IDC_EDIT_SHADER_SELECTVARMAX, m_StrArr[Str_SelectVarMax] );
	DDX_Text( pDX, IDC_EDIT_SHADER_SELECTVARUE, m_StrArr[Str_SelectVarValue] );

	DDX_Control( pDX, IDC_CHECK_SHADER_LIGHT, m_BtnArr[Btn_Light] );
	DDX_Control( pDX, IDC_CHECK_SHADER_HDR, m_BtnArr[Btn_HDR] );
	DDX_Control( pDX, IDC_CHECK_SHADER_MOTIONBLUR, m_BtnArr[Btn_MotionBlur] );
	DDX_Control( pDX, IDC_CHECK_SHADER_DOF, m_BtnArr[Btn_DOF] );
	DDX_Control( pDX, IDC_CHECK_SHADER_SSAO, m_BtnArr[Btn_SSAO] );
	DDX_Control( pDX, IDC_CHECK_SHADER_SHADOW, m_BtnArr[Btn_Shadow] );
	DDX_Control( pDX, IDC_CHECK_SHADER_EDGEDETECTION, m_BtnArr[Btn_EdgeDetecting] );
	DDX_Control( pDX, IDC_CHECK_SHADER_FOG, m_BtnArr[Btn_Fog] );
	DDX_Control( pDX, IDC_CHECK_SHADER_LIGHTSHAFT, m_BtnArr[Btn_LightShaft] );

	DDX_Control( pDX, IDC_SLIDER_SELECTNUMVALUE, m_SliderArr[Slider_SelectVarValue] );

	DDX_Control( pDX, IDC_LIST_SHADER_RENDERTARGET, m_ListBox_RenderTarget );
	DDX_Control( pDX, IDC_LIST_SHADER_SHADERLIST, m_ListBox_Shader );
	DDX_Control( pDX, IDC_LIST_SHADER_VARIABLE, m_ListBox_ShaderVar );
	DDX_Control( pDX, IDC_COMBO_STAGETYPE, m_ComboBox_Map );
}


BEGIN_MESSAGE_MAP(CTool_Shader, CPropertyPage)
	ON_LBN_SELCHANGE( IDC_LIST_SHADER_RENDERTARGET, &CTool_Shader::OnLbnSelchangeListShaderRendertarget )
	ON_LBN_SELCHANGE( IDC_LIST_SHADER_SHADERLIST, &CTool_Shader::OnLbnSelchangeListShaderShaderlist )
	ON_LBN_SELCHANGE( IDC_LIST_SHADER_VARIABLE, &CTool_Shader::OnLbnSelchangeListShaderVariable )
	ON_BN_CLICKED( IDC_BUTTON_SHADER_ADDVAR, &CTool_Shader::OnBnClickedButtonShaderAddvar )
	ON_BN_CLICKED( IDC_BUTTON_SHADER_SAVE, &CTool_Shader::OnBnClickedButtonShaderSave )
	ON_BN_CLICKED( IDC_BUTTON_SHADER_LOAD, &CTool_Shader::OnBnClickedButtonShaderLoad )
	ON_WM_HSCROLL()
	ON_EN_CHANGE( IDC_EDIT_SHADER_SELECTVARNUM, &CTool_Shader::OnEnChangeEditShaderSelectvarnum )
	ON_BN_CLICKED( IDC_BUTTON_SHADER_DELETES_SHADERVAR, &CTool_Shader::OnBnClickedButtonShaderDeletesShadervar )
	ON_BN_CLICKED( IDC_BUTTON_MODIFY_STAGESHADERDATA, &CTool_Shader::OnBnClickedButtonModifyStageshaderdata )
END_MESSAGE_MAP()


// CTool_Shader 메시지 처리기입니다.


void CTool_Shader::OnLbnSelchangeListShaderRendertarget()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iCount = m_ListBox_RenderTarget.GetCurSel();
	if ( -1 == iCount )
		return;

	if ( 0 == iCount )
		m_pRendererCom->Set_RenderBackBufferKind( Engine::CRenderer::RenderBackBuffer_Default, nullptr );
	else
		m_pRendererCom->Set_RenderBackBufferKind( Engine::CRenderer::RenderBackBuffer_DebugRenderTarget, m_pAllTagetOriginalNameArr[iCount - 1] );
}


void CTool_Shader::OnLbnSelchangeListShaderShaderlist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iCount = m_ListBox_Shader.GetCurSel();
	if ( -1 == iCount )
		return;

	m_iCurSelectShaderValueNum = 0;
	m_StrArr[Str_SelectVarNum] = L"0";
	UpdateData( FALSE );

	m_iCurShaderVar = -1;
	m_ListBox_ShaderVar.SetCurSel( -1 );

	Show_ShaderData( iCount );
}


void CTool_Shader::OnLbnSelchangeListShaderVariable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iCount = m_ListBox_ShaderVar.GetCurSel();
	if ( -1 == iCount )
		return;

	m_iCurSelectShaderValueNum = 0;
	m_StrArr[Str_SelectVarNum] = L"0";
	UpdateData( FALSE );

	if ( 0 == iCount )
	{
		m_ListBox_ShaderVar.SetCurSel( -1 );
		m_iCurShaderVar = -1;
	}
	else
		Show_ShaderVarData( --iCount );
}


void CTool_Shader::OnBnClickedButtonShaderAddvar()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( -1 == m_iCurShader ||
		 m_vecShaderData.size() <= m_iCurShader )
		return;

	UpdateData( TRUE );

	CRenderer::EFFECTVARTYPE ShaderVar;

	Update_ShaderVarToolBox( ShaderVar );

	_uint iCount = m_vecShaderData[m_iCurShader].iEffectVarCount;

	CRenderer::EFFECTVARTYPE*	pEffectVar = new CRenderer::EFFECTVARTYPE[iCount + 1];
	for ( size_t i = 0; i < iCount; ++i )
	{
		pEffectVar[i] = m_vecShaderData[m_iCurShader].pEffectVar[i];
	}

	pEffectVar[iCount] = ShaderVar;
	++m_vecShaderData[m_iCurShader].iEffectVarCount;
	m_ListBox_ShaderVar.AddString( m_StrArr[Str_VarName] );

	m_vecShaderData[m_iCurShader].pEffectVar = pEffectVar;

	m_pRendererCom->Set_ShaderVarData( m_iCurShader, &m_vecShaderData[m_iCurShader] );
}


void CTool_Shader::OnBnClickedButtonShaderSave()
{
	//// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	switch ( m_eMapKind )
	{
		case MapKind_Stage1:
			Save_ShaderVar( L"../Bin/Data/ShaderOptionData/EffectVarData_Stage1.dat" );
			Save_ShaderVar( L"./EffectVarData_Stage1.dat" );
			break;
		case MapKind_Stage2:
			Save_ShaderVar( L"../Bin/Data/ShaderOptionData/EffectVarData_Stage2.dat" );
			Save_ShaderVar( L"./EffectVarData_Stage2.dat" );
			break;
		case MapKind_Stage3:
			Save_ShaderVar( L"../Bin/Data/ShaderOptionData/EffectVarData_Stage3.dat" );
			Save_ShaderVar( L"./EffectVarData_Stage3.dat" );
			break;
		case MapKind_BossStage:
			Save_ShaderVar( L"../Bin/Data/ShaderOptionData/EffectVarData_BossStage.dat" );
			Save_ShaderVar( L"./EffectVarData_BossStage.dat" );
			break;
		default:
			return;
	}

	AfxMessageBox(L"Save Succesed!!");
}


void CTool_Shader::OnBnClickedButtonShaderLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//UpdateData(TRUE);
	//
	//CFileDialog		Dlg(TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE);
	//
	//TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.
	//
	//TCHAR szCurrDirectory[MAX_PATH] = L"";		// 따로 보관할 문자열
	//
	//GetCurrentDirectory(MAX_PATH, szDirectory);		//현재 프로젝트의 드라이브경로를 받아온다.
	//
	//lstrcpy(szCurrDirectory, szDirectory);
	//
	//PathRemoveFileSpec(szDirectory);
	//
	//Dlg.GetOFN().lpstrInitialDir = szDirectory;
	//
	//if (Dlg.DoModal() == IDOK)
	//	Load_ShaderVarData( Dlg.GetPathName() );
}

BOOL CTool_Shader::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pGraphicDev = ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->Get_GraphicDev();
	if ( nullptr == m_pGraphicDev )
		AfxMessageBox( L"m_pGraphicDev is nullptr" );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_StrArr[Str_Float1] = m_StrArr[Str_Float2] = m_StrArr[Str_Float3] = m_StrArr[Str_Float4] = L"0";

	for ( size_t i = 0; i < CRenderer::ShaderVar_End; ++i )
	{
		CRenderer::ShaderVariableType varType = (CRenderer::ShaderVariableType)i;
		
		string str;

		switch ( varType )
		{

			case Engine::CRenderer::ShaderVar_Float:
				str = etos( float );
				break;

			case Engine::CRenderer::ShaderVar_Int:
				str = etos( int );
				break;

			case Engine::CRenderer::ShaderVar_Bool:
				str = etos( bool );
				break;

			case Engine::CRenderer::ShaderVar_Vector:
				str = etos( vector );
				break;

			case Engine::CRenderer::ShaderVar_Matrix:
				str = etos( matrix );
				break;

			case Engine::CRenderer::ShaderVar_Texture:
				str = etos( texture );
				break;

			default:
				break;

		}

		_tchar szTemp[128] = L"";
		const char* pStr = str.c_str();
		int nLen = MultiByteToWideChar( CP_ACP, 0, pStr, strlen( pStr ), NULL, NULL );
		MultiByteToWideChar( CP_ACP, 0, pStr, strlen( pStr ), szTemp, nLen );

		m_Combo_ShaderVar.AddString( szTemp );
	}
	m_Combo_ShaderVar.SetCurSel( 0 );

	m_ComboBox_Map.AddString( L"Stage1" );
	m_ComboBox_Map.AddString( L"Stage2" );
	m_ComboBox_Map.AddString( L"Stage3" );
	m_ComboBox_Map.AddString( L"BossStage" );
	m_ComboBox_Map.SetCurSel( 0 );

	m_StrArr[Str_SelectVarNum] = m_StrArr[Str_SelectVarMin] = m_StrArr[Str_SelectVarMax] = m_StrArr[Str_SelectVarValue] = L"";

	m_ListBox_ShaderVar.AddString( L"SelectNone" );

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTool_Shader::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if ( pScrollBar )
	{
		// 어떤 슬라이더인지 검사
		_int iValue = m_SliderArr[Slider_SelectVarValue].GetPos();
		m_StrArr[Str_SelectVarValue].Format( L"%f", iValue / 100000.f );
		_float fValue = iValue / 100000.f;

		if ( !(-1 == m_iCurShader ||
				m_iCurShader >= (_uint)m_vecShaderData.size() ||
				-1 == m_iCurShaderVar ||
				m_iCurShaderVar >= (_uint)m_vecShaderData[m_iCurShader].iEffectVarCount ||
				0 == m_iCurSelectShaderValueNum) )
		{
			m_StrArr[Str_Float1 + m_iCurSelectShaderValueNum - 1].Format( L"%f", fValue );
		}
			
	}
	else
	{
		// CScrollView를 상속받은 뷰의 경우 프래임의 스크롤롤 동작시 pScrollBar이 NULL된다.
	}

	UpdateData( FALSE );

	CPropertyPage::OnHScroll( nSBCode, nPos, pScrollBar );
}


void CTool_Shader::OnEnChangeEditShaderSelectvarnum()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	if ( m_iCurSelectShaderValueNum <= 16 )
		m_iCurSelectShaderValueNum = _ttoi( m_StrArr[Str_SelectVarNum] );
}


void CTool_Shader::OnBnClickedButtonShaderDeletesShadervar()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iSelect = m_ListBox_ShaderVar.GetCurSel();
	if ( -1 == iSelect )
		return;
	--iSelect;

	if ( m_iCurShader == -1 ||
		 m_vecShaderData.size() <= m_iCurShader ||
		 0 == m_vecShaderData[m_iCurShader].iEffectVarCount )
		return;

	Engine::CRenderer::EFFECTVARTYPE*	pEffectVar = new Engine::CRenderer::EFFECTVARTYPE[m_vecShaderData[m_iCurShader].iEffectVarCount - 1];

	for ( _uint i = 0, iCount = 0; i < m_vecShaderData[m_iCurShader].iEffectVarCount; ++i )
	{
		if ( i == iSelect )
			continue;

		pEffectVar[i] = m_vecShaderData[m_iCurShader].pEffectVar[iCount++];
	}

	Engine::Safe_Delete_Array( m_vecShaderData[m_iCurShader].pEffectVar );
	--m_vecShaderData[m_iCurShader].iEffectVarCount;

	m_ListBox_ShaderVar.DeleteString( iSelect + 1 );

	m_vecShaderData[m_iCurShader].pEffectVar = pEffectVar;

	m_iCurShaderVar = -1;
	m_ListBox_ShaderVar.SetCurSel( -1 );

	m_pRendererCom->Set_ShaderVarData( m_iCurShader, &m_vecShaderData[m_iCurShader] );

}

//.cpp
void CTool_Shader::SetRadioStatus(UINT value)
{
	UpdateData( TRUE );

}


BOOL CTool_Shader::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	bIsActive = false;
	return CPropertyPage::OnKillActive();
}


BOOL CTool_Shader::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	bIsActive = true;
	return CPropertyPage::OnSetActive();
}


void CTool_Shader::OnBnClickedButtonModifyStageshaderdata()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ComboBox_Map.GetCurSel();
	if ( -1 == iIndex )
		return;

	Modify_StageType( MAPKIND( iIndex ) );

	if ( nullptr != ((CMainFrame*)AfxGetMainWnd())->Get_EditBox()->Get_EditSheet()->Get_LightTool() )
		((CMainFrame*)AfxGetMainWnd())->Get_EditBox()->Get_EditSheet()->Get_LightTool()->Modify_StageType( m_eMapKind );
}
