// Tool_Interaction.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ShaderTool_KMH.h"
#include "Tool_Interaction.h"
#include "afxdialogex.h"

#include "ShaderToolMainView.h"
#include "MainFrm.h"

#include "Object_Manager.h"
#include "Camera_Manager.h"

#include "Interaction_Character_UseTool.h"
#include "Camera_UseTool.h"


// CTool_Interaction 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTool_Interaction, CPropertyPage)

CTool_Interaction::CTool_Interaction()
	: CPropertyPage(IDD_TOOL_INTERACTION)
{

}

CTool_Interaction::~CTool_Interaction()
{
}

void CTool_Interaction::Set_ChangeAngleChar( const _bool & _bIsPlayer )
{
	m_CheckBoxArr[CheckBox_AngleChangePlayer].SetCheck( ((_bIsPlayer == true) ? TRUE : FALSE) );
	m_CheckBoxArr[CheckBox_AngleChangeMonster].SetCheck( ((_bIsPlayer == false) ? TRUE : FALSE) );

	UpdateData( FALSE );
}

void CTool_Interaction::Set_ChangePosChar( const _bool & _bIsPlayer )
{
	m_CheckBoxArr[CheckBox_PosChangePlayer].SetCheck( ((_bIsPlayer == true) ? TRUE : FALSE) );
	m_CheckBoxArr[CheckBox_PosChangeMonster].SetCheck( ((_bIsPlayer == false) ? TRUE : FALSE) );

	UpdateData( FALSE );
}

_bool CTool_Interaction::Get_IsCheckBoxCheck( const CHECKBOXKIND & _eKind )
{
	return m_CheckBoxArr[_eKind].GetCheck() != 0;
}

void CTool_Interaction::Update_Tool( const _float & _fTimeDelta )
{
	if ( m_iSelectInteractionIndex != -1 )
	{
		if ( m_vecInteractionData.size() > m_iSelectInteractionIndex )
			m_vecInteractionData[m_iSelectInteractionIndex] = m_CurInteractionData;
	}

	if ( nullptr != m_pCharacter1 )
	{
		UpdateData( TRUE );

		_double dPeriod = m_pCharacter1->Get_AniPeriod();
		_double dPosition = m_pCharacter1->Get_AniPosition();
		m_StrArr[Str_PL_Ani_Period].Format( L"%f", dPeriod );
		m_StrArr[Str_PL_Ani_Pos].Format( L"%f", dPosition );

		UpdateData( FALSE );
	}

	if ( nullptr != m_pCharacter2 )
	{
		UpdateData( TRUE );

		_double dPeriod = m_pCharacter2->Get_AniPeriod();
		_double dPosition = m_pCharacter2->Get_AniPosition();
		m_StrArr[Str_Mon_Ani_Period].Format( L"%f", dPeriod );
		m_StrArr[Str_Mon_Ani_Pos].Format( L"%f", dPosition );

		UpdateData( FALSE );
	}
}

HRESULT CTool_Interaction::Init_String( void )
{
	for ( _uint i = 0; i < Str_End; ++i )
	{
		m_StrArr[i] = "0";
	}

	UpdateData( FALSE );
		
	return NOERROR;
}

HRESULT CTool_Interaction::Init_CheckBox( void )
{
	Set_ChangeAngleChar( true );
	Set_ChangePosChar( false );

	return NOERROR;
}

HRESULT CTool_Interaction::Init_ComboBox( void )
{
	_tchar szName[][256] =
	{
		L"Donatello", L"Leonard", L"Michelangelo", L"Raphael", L"KnifeThug", L"SlimThug", L"BigThug", L"Foot_Soldier"
	};
	
	_uint iSize = sizeof( szName ) / sizeof( _tchar[256] );

	for ( _uint i = 0; i < iSize; ++i )
	{
		m_ComBoBoxArr[ComboBox_PlayerType].AddString( szName[i] );	
		m_ComBoBoxArr[ComboBox_MonsterType].AddString( szName[i] );	
	}

	UpdateData( FALSE );

	return NOERROR;
}

CHARACTERKIND CTool_Interaction::Compute_CharacterKind( const _tchar * _pName )
{
	CHARACTERKIND eKind;

	if ( 0 == lstrcmp( _pName, L"Donatello" ) )
		eKind = Character_Donatello;
	else if ( 0 == lstrcmp( _pName, L"Leonard" ) )
		eKind = Character_Leronard;
	else if ( 0 == lstrcmp( _pName, L"Michelangelo" ) )
		eKind = Character_Michelangelo;
	else if ( 0 == lstrcmp( _pName, L"Raphael" ) )
		eKind = Character_Raphael;
	else if ( 0 == lstrcmp( _pName, L"KnifeThug" ) )
		eKind = Character_KinfeThug;
	else if ( 0 == lstrcmp( _pName, L"SlimThug" ) )
		eKind = Character_SlimThug;
	else if ( 0 == lstrcmp( _pName, L"BigThug" ) )
		eKind = Character_BigThug;
	else if ( 0 == lstrcmp( _pName, L"Foot_Soldier" ) )
		eKind = Character_FootSoldier;
	else
		eKind = Character_End;


	return eKind;
}

HRESULT CTool_Interaction::Save_InteractionData( const _tchar * _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return E_FAIL;

	unsigned int iCount = (unsigned int)m_vecInteractionData.size();
	_ulong dwByte = 0;

	// 상호 작용 개수 저장..
	WriteFile( hFile, &iCount, sizeof( unsigned int ), &dwByte, nullptr );

	for ( _uint i = 0; i < iCount; ++i )
	{
		if ( FAILED( m_vecInteractionData[i].Save( hFile, dwByte ) ) )
		{
			CloseHandle( hFile );
			return E_FAIL;
		}
	}

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CTool_Interaction::Load_InteractionData( const _tchar * _pFilePath )
{
	HANDLE hFile = CreateFile( _pFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if ( INVALID_HANDLE_VALUE == hFile )
		return E_FAIL;

	unsigned int iCount = 0;
	_ulong dwByte = 0;

	// 상호 작용 개수 저장..
	ReadFile( hFile, &iCount, sizeof( unsigned int ), &dwByte, nullptr );

	INTERACTIONDATA InteractionData;

	for ( _uint i = 0; i < iCount; ++i )
	{
		if ( FAILED( InteractionData.Load( hFile, dwByte ) ) )
			return E_FAIL;

		m_vecInteractionData.push_back( InteractionData );

		_tchar szName[256] = L"";
		swprintf_s( szName, L"Interaction%d", m_ListBoxArr[ListBox_Interaction].GetCount() );

		m_ListBoxArr[ListBox_Interaction].AddString( szName );
	}

	CloseHandle( hFile );

	return NOERROR;
}

HRESULT CTool_Interaction::Setting_MFCToolBox( const INTERACTIONDATA& _InteractionData )
{
	m_CurInteractionData = _InteractionData;

	m_StrArr[Str_BetweenAngle_Pl_To_Mon_Decide].Format( L"%f", m_CurInteractionData.fAngle );

	m_StrArr[Str_BetweenDist_Pl_To_Mon_Decide].Format( L"%f", m_CurInteractionData.fDist );

	m_StrArr[Str_PL_Ani_CurIndex].Format( L"%d", m_CurInteractionData.iAniIndex );

	m_StrArr[Str_Mon_Ani_CurIndex].Format( L"%d", m_CurInteractionData.iOthersAniIndex );

	m_ListBoxArr[ListBox_Option].ResetContent();

	for ( _uint i = 0; i < m_CurInteractionData.iOptionCount; ++i )
	{
		_tchar szName[128] = L"";
		swprintf_s( szName, L"옵션%d", m_ListBoxArr[ListBox_Option].GetCount() );

		m_ListBoxArr[ListBox_Option].AddString( szName );
	}

	UpdateData( FALSE );

	return NOERROR;
}

void CTool_Interaction::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange( pDX );
	DDX_Text( pDX, IDC_EDIT_PL_ANI_PERIOD, m_StrArr[Str_PL_Ani_Period] );
	DDX_Text( pDX, IDC_EDIT_PL_ANI_POSITION, m_StrArr[Str_PL_Ani_Pos] );
	DDX_Text( pDX, IDC_EDIT_PL_ANI_INDEX, m_StrArr[Str_PL_Ani_Index] );
	DDX_Text( pDX, IDC_EDIT_PL_ANI_CURINDEX, m_StrArr[Str_PL_Ani_CurIndex] );
	DDX_Text( pDX, IDC_EDIT_PL_ANI_SPEED, m_StrArr[Str_PL_Ani_Speed] );
	DDX_Text( pDX, IDC_EDIT_PL_ANI_CURSPEED, m_StrArr[Str_PL_Ani_CurSpeed] );
	DDX_Text( pDX, IDC_EDIT_MON_ANI_PERIOD, m_StrArr[Str_Mon_Ani_Period] );
	DDX_Text( pDX, IDC_EDIT_MON_ANI_POS, m_StrArr[Str_Mon_Ani_Pos] );
	DDX_Text( pDX, IDC_EDIT_MON_ANI_INDEX, m_StrArr[Str_Mon_Ani_Index] );
	DDX_Text( pDX, IDC_EDIT_MON_ANI_CURINDEX, m_StrArr[Str_Mon_Ani_CurIndex] );
	DDX_Text( pDX, IDC_EDIT_MON_ANI_SPEED, m_StrArr[Str_Mon_Ani_Speed] );
	DDX_Text( pDX, IDC_EDIT_MON_ANI_CURSPEED, m_StrArr[Str_Mon_Ani_CurSpeed] );
	DDX_Text( pDX, IDC_EDIT_PL_EVENT_ANIPOS, m_StrArr[Str_PL_Event_AniPos] );
	DDX_Text( pDX, IDC_EDIT_PL_EVENT_OPTION, m_StrArr[Str_PL_Event_Option] );
	DDX_Text( pDX, IDC_EDIT_PL_EVENT_DETAILDATA, m_StrArr[Str_PL_Event_DetailData] );
	DDX_Text( pDX, IDC_EDIT_PL_EVENT_CUROPTION, m_StrArr[Str_PL_Event_CurOption] );
	DDX_Text( pDX, IDC_EDIT_MON_START_ANITIME, m_StrArr[Str_Mon_Start_AniTime] );
	DDX_Text( pDX, IDC_EDIT_BETWEENANGLE_PL_TO_MON, m_StrArr[Str_BetweenAngle_Pl_To_Mon] );
	DDX_Text( pDX, IDC_EDIT_BETWEENDIST_PL_TO_MON, m_StrArr[Str_BetweenDist_Pl_To_Mon] );
	DDX_Text( pDX, IDC_EDIT_MON_START_ANITIME2, m_StrArr[Str_Mon_Start_AniTime_Decide] );
	DDX_Text( pDX, IDC_EDIT_BETWEENANGLE_PL_TO_MON2, m_StrArr[Str_BetweenAngle_Pl_To_Mon_Decide] );
	DDX_Text( pDX, IDC_EDIT_BETWEENDIST_PL_TO_MON2, m_StrArr[Str_BetweenDist_Pl_To_Mon_Decide] );
	DDX_Text( pDX, IDC_EDIT_PL_POSX, m_StrArr[Str_PL_PosX] );
	DDX_Text( pDX, IDC_EDIT_PL_POSY, m_StrArr[Str_PL_PosY] );
	DDX_Text( pDX, IDC_EDIT_PL_POSZ, m_StrArr[Str_PL_PosZ] );
	DDX_Text( pDX, IDC_EDIT_PL_ANGLEY, m_StrArr[Str_PL_AngleY] );
	DDX_Text( pDX, IDC_EDIT_MON_POSX, m_StrArr[Str_Mon_PosX] );
	DDX_Text( pDX, IDC_EDIT_MON_POSY, m_StrArr[Str_Mon_PosY] );
	DDX_Text( pDX, IDC_EDIT_MON_POSZ, m_StrArr[Str_Mon_PosZ] );
	DDX_Text( pDX, IDC_EDIT_MON_ANGLEY, m_StrArr[Str_Mon_AngleY] );
	DDX_Text( pDX, IDC_EDIT_PL_POSX2, m_StrArr[Str_PL_PosX_Decide] );
	DDX_Text( pDX, IDC_EDIT_PL_POSY2, m_StrArr[Str_PL_PosY_Decide] );
	DDX_Text( pDX, IDC_EDIT_PL_POSZ2, m_StrArr[Str_PL_PosZ_Decide] );
	DDX_Text( pDX, IDC_EDIT_PL_ANGLEY2, m_StrArr[Str_PL_AngleY_Decide] );
	DDX_Text( pDX, IDC_EDIT_MON_POSX2, m_StrArr[Str_Mon_PosX_Decide] );
	DDX_Text( pDX, IDC_EDIT_MON_POSY2, m_StrArr[Str_Mon_PosY_Decide] );
	DDX_Text( pDX, IDC_EDIT_MON_POSZ2, m_StrArr[Str_Mon_PosZ_Decide] );
	DDX_Text( pDX, IDC_EDIT_MON_ANGLEY2, m_StrArr[Str_Mon_AngleY_Decide] );

	DDX_Control( pDX, IDC_CHECK_PL_CHANGE_ANGLE, m_CheckBoxArr[CheckBox_AngleChangePlayer] );
	DDX_Control( pDX, IDC_CHECK_MON_CHANGE_ANGLE, m_CheckBoxArr[CheckBox_AngleChangeMonster] );
	DDX_Control( pDX, IDC_CHECK_PL_CHANGE_POS, m_CheckBoxArr[CheckBox_PosChangePlayer] );
	DDX_Control( pDX, IDC_CHECK_MON_CHANGE_POS, m_CheckBoxArr[CheckBox_PosChangeMonster] );

	DDX_Control( pDX, IDC_COMBO_PLAYERTYPE, m_ComBoBoxArr[ComboBox_PlayerType] );
	DDX_Control( pDX, IDC_COMBO_MONSTERTYPE, m_ComBoBoxArr[ComboBox_MonsterType] );

	DDX_Control( pDX, IDC_LIST_OPTION, m_ListBoxArr[ListBox_Option] );
	DDX_Control( pDX, IDC_LIST_DETAILDATA, m_ListBoxArr[ListBox_DetailData] );
	DDX_Control( pDX, IDC_LIST_INTERACTION, m_ListBoxArr[ListBox_Interaction] );
}


BEGIN_MESSAGE_MAP(CTool_Interaction, CPropertyPage)
	ON_BN_CLICKED( IDC_BUTTON_CREATEPLAYER, &CTool_Interaction::OnBnClickedButtonCreateplayer )
	ON_BN_CLICKED( IDC_BUTTON_CREATEMONSTER, &CTool_Interaction::OnBnClickedButtonCreatemonster )
	ON_BN_CLICKED( IDC_BUTTON_START_ANIMATION, &CTool_Interaction::OnBnClickedButtonStartAnimation )
	ON_BN_CLICKED( IDC_BUTTON_PL_CHANGE_ANIINDEX, &CTool_Interaction::OnBnClickedButtonPlChangeAniindex )
	ON_BN_CLICKED( IDC_BUTTON_PL_CHANGE_ANISPEED, &CTool_Interaction::OnBnClickedButtonPlChangeAnispeed )
	ON_BN_CLICKED( IDC_BUTTON_MON_CHANGE_ANIINDEX, &CTool_Interaction::OnBnClickedButtonMonChangeAniindex )
	ON_BN_CLICKED( IDC_BUTTON_MON_CHANGE_ANISPEED, &CTool_Interaction::OnBnClickedButtonMonChangeAnispeed )
	ON_BN_CLICKED( IDC_EDIT_PL_ADD_EVENT_ANIPOS, &CTool_Interaction::OnBnClickedEditPlAddEventAnipos )
	ON_BN_CLICKED( IDC_EDIT_PL_MODIFY_EVENT_OPTION, &CTool_Interaction::OnBnClickedEditPlModifyEventOption )
	ON_BN_CLICKED( IDC_EDIT_PL_ADD_EVENT_DETAILDATA, &CTool_Interaction::OnBnClickedEditPlAddEventDetaildata )
	ON_BN_CLICKED( IDC_BUTTON_MODIFY_MON_START_ANITIME, &CTool_Interaction::OnBnClickedButtonModifyMonStartAnitime )
	ON_BN_CLICKED( IDC_BUTTON_MODIFY_BETWEENANGLE_PL_TO_MON, &CTool_Interaction::OnBnClickedButtonModifyBetweenanglePlToMon )
	ON_BN_CLICKED( IDC_BUTTON_MODIFY_BETWEENDIST_PL_TO_MON, &CTool_Interaction::OnBnClickedButtonModifyBetweendistPlToMon )
	ON_BN_CLICKED( IDC_BUTTON_RESET_MON_START_ANITIME, &CTool_Interaction::OnBnClickedButtonResetMonStartAnitime )
	ON_BN_CLICKED( IDC_CHECK_PL_CHANGE_ANGLE, &CTool_Interaction::OnBnClickedCheckPlChangeAngle )
	ON_BN_CLICKED( IDC_CHECK_MON_CHANGE_ANGLE, &CTool_Interaction::OnBnClickedCheckMonChangeAngle )
	ON_BN_CLICKED( IDC_CHECK_PL_CHANGE_POS, &CTool_Interaction::OnBnClickedCheckPlChangePos )
	ON_BN_CLICKED( IDC_CHECK_MON_CHANGE_POS, &CTool_Interaction::OnBnClickedCheckMonChangePos )
	ON_BN_CLICKED( IDC_BUTTON_STOP_ANIMATION, &CTool_Interaction::OnBnClickedButtonStopAnimation )
	ON_BN_CLICKED( IDC_BUTTON_START_INTERACTION, &CTool_Interaction::OnBnClickedButtonStartInteraction )
	ON_BN_CLICKED( IDC_BUTTON_PL_CHANGE_POS, &CTool_Interaction::OnBnClickedButtonPlChangePos )
	ON_BN_CLICKED( IDC_BUTTON_MON_CHANGE_POS, &CTool_Interaction::OnBnClickedButtonMonChangePos )
	ON_BN_CLICKED( IDC_BUTTON_MON_CHANGE_ANGLE, &CTool_Interaction::OnBnClickedButtonMonChangeAngle )
	ON_BN_CLICKED( IDC_BUTTON_PL_CHANGE_ANGLE, &CTool_Interaction::OnBnClickedButtonPlChangeAngle )
	ON_BN_CLICKED( IDC_BUTTON_ADD_INTERACTION, &CTool_Interaction::OnBnClickedButtonAddInteraction )
	ON_BN_CLICKED( IDC_BUTTON_SAVE_INTERACTION, &CTool_Interaction::OnBnClickedButtonSaveInteraction )
	ON_BN_CLICKED( IDC_BUTTON_LOAD_INTERACTION, &CTool_Interaction::OnBnClickedButtonLoadInteraction )
	ON_LBN_SELCHANGE( IDC_LIST_OPTION, &CTool_Interaction::OnLbnSelchangeListOption )
	ON_LBN_SELCHANGE( IDC_LIST_DETAILDATA, &CTool_Interaction::OnLbnSelchangeListDetaildata )
	ON_LBN_SELCHANGE( IDC_LIST_INTERACTION, &CTool_Interaction::OnLbnSelchangeListInteraction )
	ON_BN_CLICKED( IDC_BUTTON_DELETE_INTERACTION, &CTool_Interaction::OnBnClickedButtonDeleteInteraction )
	ON_BN_CLICKED( IDC_BUTTON_SELECTCANCEL_INTERACTION, &CTool_Interaction::OnBnClickedButtonSelectcancelInteraction )
	ON_BN_CLICKED( IDC_BUTTON_PLAYER_CAMERA_ONOFF, &CTool_Interaction::OnBnClickedButtonPlayerCameraOnoff )
END_MESSAGE_MAP()


// CTool_Interaction 메시지 처리기입니다.


BOOL CTool_Interaction::OnKillActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	bIsActive = false;
	return CPropertyPage::OnKillActive();
}


BOOL CTool_Interaction::OnSetActive()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	bIsActive = true;
	return CPropertyPage::OnSetActive();
}


BOOL CTool_Interaction::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// 전에 있던 오브젝트를 싹 다 지워버린다..
	//if ( FAILED( CObject_Manager::GetInstance()->Release_Object( SCENE_TOOL ) ) )
	//{
	//	AfxMessageBox( L"Failed to Clear Object_Manager" );
	//	return FALSE;
	//}


	// 카메라의 위치를 원점으로 이동시킨다..
	// 필요한 오브젝트들 가져오기..
	m_pCamera = dynamic_cast<CCamera_UseTool*>(CCamera_Manager::GetInstance()->Get_CurrentCamera());
	if ( nullptr == m_pCamera )
	{
		AfxMessageBox( L"m_pCamera is nullptr" );
		return FALSE;
	}
	m_pCamera->Set_Position( _vec3( 0.f, 15.f, -5.f ) );

	// 그래픽 디바이스를 가져온다..
	m_pGraphicDev = ((CMainFrame*)AfxGetMainWnd())->Get_MainView()->Get_GraphicDev();
	if ( nullptr == m_pGraphicDev )
	{
		AfxMessageBox( L"m_pGraphic is nullptr" );
		return FALSE;
	}


	// MFC 도구 상자 초기화..
	if ( FAILED( Init_String() ) )
	{
		AfxMessageBox( L"Init_String FAILED" );
		return FALSE;
	}
	if ( FAILED( Init_CheckBox() ) )
	{
		AfxMessageBox( L"Init_CheckBox FAILED" );
		return FALSE;
	}
	if ( FAILED( Init_ComboBox() ) )
	{
		AfxMessageBox( L"Init_ComboBox FAILED" );
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CTool_Interaction::OnBnClickedButtonCreateplayer()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ComBoBoxArr[ComboBox_PlayerType].GetCurSel();
	if ( -1 == iIndex )
		return;

	// 플레이어의 타입을 가져옴..
	CString strType;
	m_ComBoBoxArr[ComboBox_PlayerType].GetLBText( iIndex, strType );

	if ( nullptr != m_pCharacter1 )
		m_pCharacter1->Set_IsDead( true );

	CHARACTERKIND eKind = Compute_CharacterKind( strType );

	m_pCharacter1 = CInteraction_Character_UseTool::Create( m_pGraphicDev, eKind );
	if ( nullptr == m_pCharacter1 )
	{
		AfxMessageBox( L"Character Create Failed!!" );
		return;
	}

	if ( FAILED( CObject_Manager::GetInstance()->Ready_GameObject( SCENE_TOOL, L"Layer_Character", m_pCharacter1 ) ) )
		AfxMessageBox( L"Failed Add Object to Object_Manager" );
}


void CTool_Interaction::OnBnClickedButtonCreatemonster()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ComBoBoxArr[ComboBox_MonsterType].GetCurSel();
	if ( -1 == iIndex )
		return;

	// 플레이어의 타입을 가져옴..
	CString strType;
	m_ComBoBoxArr[ComboBox_MonsterType].GetLBText( iIndex, strType );

	if ( nullptr != m_pCharacter2 )
		m_pCharacter2->Set_IsDead( true );

	CHARACTERKIND eKind = Compute_CharacterKind( strType );

	m_pCharacter2 = CInteraction_Character_UseTool::Create( m_pGraphicDev, eKind );
	if ( nullptr == m_pCharacter2 )
	{
		AfxMessageBox( L"Character Create Failed!!" );
		return;
	}

	if ( FAILED( CObject_Manager::GetInstance()->Ready_GameObject( SCENE_TOOL, L"Layer_Character", m_pCharacter2 ) ) )
		AfxMessageBox( L"Failed Add Object to Object_Manager" );
}


void CTool_Interaction::OnBnClickedButtonStartAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( nullptr != m_pCharacter1 )
		m_pCharacter1->Set_IsStartAni( true );
	if ( nullptr != m_pCharacter2 )
		m_pCharacter2->Set_IsStartAni( true );
}

void CTool_Interaction::OnBnClickedButtonStopAnimation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( nullptr != m_pCharacter1 )
		m_pCharacter1->Set_IsStartAni( false );
	if ( nullptr != m_pCharacter2 )
		m_pCharacter2->Set_IsStartAni( false );
}


void CTool_Interaction::OnBnClickedButtonStartInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_int iAni1Index = _ttoi( m_StrArr[Str_PL_Ani_CurIndex] );
	_int iAni2Index = _ttoi( m_StrArr[Str_Mon_Ani_CurIndex] );

	if ( nullptr != m_pCharacter1 )
	{
		m_pCharacter1->Set_CurAniIndex( iAni1Index );
		m_pCharacter1->Set_IsStartAni( true );
	}
	if ( nullptr != m_pCharacter2 )
	{
		m_pCharacter2->Set_CurAniIndex( iAni2Index );
		m_pCharacter2->Set_IsStartAni( true );
	}

	_float fOffsetAngle = _ttof( m_StrArr[Str_BetweenAngle_Pl_To_Mon_Decide] );
	_float fDist = _ttof( m_StrArr[Str_BetweenDist_Pl_To_Mon_Decide] );

	if ( Get_IsCheckBoxCheck( CheckBox_AngleChangePlayer ) )
	{
		if ( nullptr != m_pCharacter1 )
			m_pCharacter1->LookCharacter( m_pCharacter2, fOffsetAngle );
	}
	else
	{
		if ( nullptr != m_pCharacter2 )
			m_pCharacter2->LookCharacter( m_pCharacter1, fOffsetAngle );
	}

	if ( Get_IsCheckBoxCheck( CheckBox_PosChangePlayer ) )
	{
		if ( nullptr != m_pCharacter1 )
			m_pCharacter1->PositionUpdate( m_pCharacter2, fDist );
	}
	else
	{
		if ( nullptr != m_pCharacter2 )
			m_pCharacter2->PositionUpdate( m_pCharacter1, fDist );
	}
}

void CTool_Interaction::OnBnClickedButtonPlChangeAniindex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	if ( nullptr == m_pCharacter1 )
		return;

	_int iAniIndex = _ttoi( m_StrArr[Str_PL_Ani_Index] );
	if ( 0 > iAniIndex )
		return;

	m_StrArr[Str_PL_Ani_CurIndex].Format( L"%d", iAniIndex );

	m_CurInteractionData.iAniIndex = iAniIndex;

	m_pCharacter1->Set_CurAniIndex( iAniIndex );

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedButtonPlChangeAnispeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	if ( nullptr == m_pCharacter1 )
		return;

	_float fAniSpeed = _ttof( m_StrArr[Str_PL_Ani_Speed] );
	if ( 0 > fAniSpeed )
		return;

	m_StrArr[Str_PL_Ani_CurSpeed].Format( L"%f", fAniSpeed );

	m_pCharacter1->Set_AniSpeed( fAniSpeed );

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedButtonMonChangeAniindex()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	if ( nullptr == m_pCharacter2 )
		return;

	_int iAniIndex = _ttoi( m_StrArr[Str_Mon_Ani_Index] );
	if ( 0 > iAniIndex )
		return;

	m_StrArr[Str_Mon_Ani_CurIndex].Format( L"%d", iAniIndex );

	m_CurInteractionData.iOthersAniIndex = iAniIndex;

	m_pCharacter2->Set_CurAniIndex( iAniIndex );

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedButtonMonChangeAnispeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	if ( nullptr == m_pCharacter2 )
		return;

	_float fAniSpeed = _ttof( m_StrArr[Str_Mon_Ani_Speed] );
	if ( 0 > fAniSpeed )
		return;

	m_StrArr[Str_Mon_Ani_CurSpeed].Format( L"%f", fAniSpeed );

	m_pCharacter2->Set_AniSpeed( fAniSpeed );

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedEditPlAddEventAnipos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	m_CurInteractionOptionData.fAniPos = _ttof( m_StrArr[Str_PL_Event_AniPos] );

	m_CurInteractionData.Add_Option( m_CurInteractionOptionData );

	_tchar szName[128] = L"";
	swprintf_s( szName, L"옵션%d", m_ListBoxArr[ListBox_Option].GetCount() );

	m_ListBoxArr[ListBox_Option].AddString( szName );

	m_ListBoxArr[ListBox_DetailData].ResetContent();

	m_CurInteractionOptionData.OptionData.DetailDataList.clear();
}


void CTool_Interaction::OnBnClickedEditPlModifyEventOption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_ulong dwOption = (_ulong)_ttol( m_StrArr[Str_PL_Event_Option] );

	m_StrArr[Str_PL_Event_CurOption].Format( L"%d", dwOption );

	m_CurInteractionOptionData.OptionData.dwOption = dwOption;

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedEditPlAddEventDetaildata()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다..
	UpdateData( TRUE );

	_ulong dwDetailData = (_ulong)_ttol( m_StrArr[Str_PL_Event_DetailData] );
	m_CurInteractionOptionData.OptionData.DetailDataList.push_back( dwDetailData );

	m_ListBoxArr[ListBox_DetailData].AddString( m_StrArr[Str_PL_Event_DetailData] );
}


void CTool_Interaction::OnBnClickedButtonModifyMonStartAnitime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_float fMonStartAniTime = _ttof( m_StrArr[Str_Mon_Start_AniTime] );

	m_StrArr[Str_Mon_Start_AniTime_Decide].Format( L"%f", fMonStartAniTime );

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedButtonModifyBetweenanglePlToMon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_float fAngle = _ttof( m_StrArr[Str_BetweenAngle_Pl_To_Mon] );

	m_StrArr[Str_BetweenAngle_Pl_To_Mon_Decide].Format( L"%f", fAngle );

	m_CurInteractionData.fAngle = fAngle;

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedButtonModifyBetweendistPlToMon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_float fDist = _ttof( m_StrArr[Str_BetweenDist_Pl_To_Mon] );

	m_StrArr[Str_BetweenDist_Pl_To_Mon_Decide].Format( L"%f", fDist );

	m_CurInteractionData.fDist = fDist;

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedButtonResetMonStartAnitime()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_float fMonStartAniTime = -1.f;

	m_StrArr[Str_Mon_Start_AniTime_Decide].Format( L"%f", fMonStartAniTime );

	UpdateData( FALSE );
}


void CTool_Interaction::OnBnClickedCheckPlChangeAngle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Set_ChangeAngleChar( true );
}


void CTool_Interaction::OnBnClickedCheckMonChangeAngle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Set_ChangeAngleChar( false );
}


void CTool_Interaction::OnBnClickedCheckPlChangePos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Set_ChangePosChar( true );
}


void CTool_Interaction::OnBnClickedCheckMonChangePos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	Set_ChangePosChar( false );
}


void CTool_Interaction::OnBnClickedButtonPlChangePos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_vec3 vPos( _ttof( m_StrArr[Str_PL_PosX] ), _ttof( m_StrArr[Str_PL_PosY] ), _ttof( m_StrArr[Str_PL_PosZ] ) );

	m_StrArr[Str_PL_PosX_Decide].Format( L"%f", vPos.x );
	m_StrArr[Str_PL_PosY_Decide].Format( L"%f", vPos.y );
	m_StrArr[Str_PL_PosZ_Decide].Format( L"%f", vPos.z );

	UpdateData( FALSE );

	if ( nullptr != m_pCharacter1 )
		m_pCharacter1->Set_StartPosition( vPos );
}


void CTool_Interaction::OnBnClickedButtonMonChangePos()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_vec3 vPos( _ttof( m_StrArr[Str_Mon_PosX] ), _ttof( m_StrArr[Str_Mon_PosY] ), _ttof( m_StrArr[Str_Mon_PosZ] ) );

	m_StrArr[Str_Mon_PosX_Decide].Format( L"%f", vPos.x );
	m_StrArr[Str_Mon_PosY_Decide].Format( L"%f", vPos.y );
	m_StrArr[Str_Mon_PosZ_Decide].Format( L"%f", vPos.z );

	UpdateData( FALSE );

	if ( nullptr != m_pCharacter2 )
		m_pCharacter2->Set_StartPosition( vPos );
}

void CTool_Interaction::OnBnClickedButtonPlChangeAngle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_vec3 vAngle( 0.f, _ttof( m_StrArr[Str_PL_AngleY] ), 0.f );

	m_StrArr[Str_PL_AngleY_Decide].Format( L"%f", vAngle.y );

	UpdateData( FALSE );

	if ( nullptr != m_pCharacter1 )
		m_pCharacter1->Set_StartAngle( vAngle );
}

void CTool_Interaction::OnBnClickedButtonMonChangeAngle()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData( TRUE );

	_vec3 vAngle( 0.f, _ttof( m_StrArr[Str_Mon_AngleY] ), 0.f );

	m_StrArr[Str_Mon_AngleY_Decide].Format( L"%f", vAngle.y );

	UpdateData( FALSE );

	if ( nullptr != m_pCharacter2 )
		m_pCharacter2->Set_StartAngle( vAngle );
}


void CTool_Interaction::OnBnClickedButtonAddInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_vecInteractionData.push_back( m_CurInteractionData );

	_tchar szName[256] = L"";
	swprintf_s( szName, L"Interaction%d", m_ListBoxArr[ListBox_Interaction].GetCount() );

	m_ListBoxArr[ListBox_Interaction].AddString( szName );
}

void CTool_Interaction::OnBnClickedButtonDeleteInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ListBoxArr[ListBox_Interaction].GetCurSel();

	if ( -1 == iIndex )
		return;

	if ( iIndex >= m_vecInteractionData.size() )
		return;

	m_vecInteractionData[iIndex].Release();

	m_ListBoxArr[ListBox_Interaction].DeleteString( iIndex );
}

void CTool_Interaction::OnBnClickedButtonSaveInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog		Dlg( FALSE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE );

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	TCHAR szCurrDirectory[MAX_PATH] = L"";		// 따로 보관할 문자열

	GetCurrentDirectory( MAX_PATH, szDirectory );		//현재 프로젝트의 드라이브경로를 받아온다.
	lstrcpy( szCurrDirectory, szDirectory );

	PathRemoveFileSpec( szDirectory );

	//Dlg.GetOFN().lpstrInitialDir = szDirectory;
	Dlg.GetOFN().lpstrInitialDir = L"../Bin/Data/InteractionData/";

	m_bIsSaveOrLoadState = true;

	if ( Dlg.DoModal() == IDOK )
	{
		if ( FAILED( Save_InteractionData( Dlg.GetPathName() ) ) )
		{
			AfxMessageBox( L"Save Failed!!" );
			m_bIsSaveOrLoadState = false;
		}
		else
			AfxMessageBox( L"Save Succesed!!" );

		m_bIsSaveOrLoadState = false;
	}
}

void CTool_Interaction::OnBnClickedButtonLoadInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog		Dlg( TRUE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this, 0, FALSE );

	TCHAR szDirectory[MAX_PATH] = L"";		//드라이브경로를 받을 문자열 변수.

	TCHAR szCurrDirectory[MAX_PATH] = L"";		// 따로 보관할 문자열

	GetCurrentDirectory( MAX_PATH, szDirectory );		//현재 프로젝트의 드라이브경로를 받아온다.

	lstrcpy( szCurrDirectory, szDirectory );

	PathRemoveFileSpec( szDirectory );

	//Dlg.GetOFN().lpstrInitialDir = szDirectory;
	Dlg.GetOFN().lpstrInitialDir = L"../Bin/Data/InteractionData/";

	m_bIsSaveOrLoadState = true;

	if ( Dlg.DoModal() == IDOK )
	{
		for ( _uint i = 0; i < m_vecInteractionData.size(); ++i )
		{
			m_vecInteractionData[i].Release();
		}
		m_vecInteractionData.clear();
		m_ListBoxArr[ListBox_Interaction].ResetContent();

		if ( FAILED( Load_InteractionData( Dlg.GetPathName() ) ) )
		{
			m_bIsSaveOrLoadState = false;
			AfxMessageBox( L"Load Failed!!" );
		}
		else
			AfxMessageBox( L"Load Succesed!!" );

		m_bIsSaveOrLoadState = false;
	}
}


void CTool_Interaction::OnLbnSelchangeListOption()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ListBoxArr[ListBox_Option].GetCurSel();
	if ( -1 == iIndex )
		return;

	m_iSelectOptionIndex = iIndex;
}


void CTool_Interaction::OnLbnSelchangeListDetaildata()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ListBoxArr[ListBox_DetailData].GetCurSel();
	if ( -1 == iIndex )
		return;

	m_iSelectDetailDataIndex = iIndex;
}


void CTool_Interaction::OnLbnSelchangeListInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_int iIndex = m_ListBoxArr[ListBox_Interaction].GetCurSel();

	m_iSelectInteractionIndex = iIndex;

	if ( -1 == iIndex )
		return;

	Setting_MFCToolBox( m_vecInteractionData[iIndex] );
}


void CTool_Interaction::OnBnClickedButtonSelectcancelInteraction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ListBoxArr[ListBox_Interaction].SetCurSel( -1 );

	m_iSelectInteractionIndex = -1;
}


void CTool_Interaction::OnBnClickedButtonPlayerCameraOnoff()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( nullptr != m_pCamera )
	{
		if ( nullptr != m_pCharacter1 )
			m_pCamera->Set_LookTarget( m_pCharacter1 );
		else if ( nullptr != m_pCharacter2 )
			m_pCamera->Set_LookTarget( m_pCharacter2 );
		else
			return;

		m_pCamera->Set_PlayerCamMode( ((m_pCamera->Set_PlayerCamMode() == true) ? false : true) );
	}
}
