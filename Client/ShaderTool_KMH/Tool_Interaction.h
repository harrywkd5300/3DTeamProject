#pragma once
#include "Engine_Defines.h"
#include "Tool_Defines.h"
#include "afxwin.h"


// CTool_Interaction 대화 상자입니다.
class CInteraction_Character_UseTool;
class CCamera_UseTool;
class CTool_Interaction : public CPropertyPage
{
	DECLARE_DYNAMIC(CTool_Interaction)
public:
	enum STRINGKIND
	{
		Str_PL_Ani_Period,
		Str_PL_Ani_Pos,
		Str_PL_Ani_Index,
		Str_PL_Ani_CurIndex,
		Str_PL_Ani_Speed,
		Str_PL_Ani_CurSpeed,
		Str_Mon_Ani_Period,
		Str_Mon_Ani_Pos,
		Str_Mon_Ani_Index,
		Str_Mon_Ani_CurIndex,
		Str_Mon_Ani_Speed,
		Str_Mon_Ani_CurSpeed,
		Str_PL_Event_AniPos,
		Str_PL_Event_Option,
		Str_PL_Event_DetailData,
		Str_PL_Event_CurOption,

		Str_Mon_Start_AniTime,
		Str_BetweenAngle_Pl_To_Mon,
		Str_BetweenDist_Pl_To_Mon,

		Str_Mon_Start_AniTime_Decide,
		Str_BetweenAngle_Pl_To_Mon_Decide,
		Str_BetweenDist_Pl_To_Mon_Decide,

		Str_PL_PosX,
		Str_PL_PosY,
		Str_PL_PosZ,
		Str_PL_AngleY,
		Str_Mon_PosX,
		Str_Mon_PosY,
		Str_Mon_PosZ,
		Str_Mon_AngleY,

		Str_PL_PosX_Decide,
		Str_PL_PosY_Decide,
		Str_PL_PosZ_Decide,
		Str_PL_AngleY_Decide,
		Str_Mon_PosX_Decide,
		Str_Mon_PosY_Decide,
		Str_Mon_PosZ_Decide,
		Str_Mon_AngleY_Decide,

		Str_End
	};

	enum CHECKBOXKIND
	{
		CheckBox_AngleChangePlayer,
		CheckBox_AngleChangeMonster,
		CheckBox_PosChangePlayer,
		CheckBox_PosChangeMonster,
		CheckBox_End
	};

	enum COMBOBOXKIND
	{
		ComboBox_PlayerType,
		ComboBox_MonsterType,
		ComboBox_End
	};

	enum LISTBOXKIND
	{
		ListBox_Interaction,
		ListBox_Option,
		ListBox_DetailData,
		ListBox_End
	};
public:
	CTool_Interaction();
	virtual ~CTool_Interaction();
public:
	_bool bIsActive = false;
public:
	void Set_ChangeAngleChar( const _bool& _bIsPlayer = true );
	void Set_ChangePosChar( const _bool& _bIsPlayer = true );
	_bool Get_IsCheckBoxCheck( const CHECKBOXKIND& _eKind );
public:
	void Update_Tool( const _float& _fTimeDelta );
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
private:
	CString		m_StrArr[Str_End];
	CButton		m_CheckBoxArr[CheckBox_End];
	CComboBox	m_ComBoBoxArr[ComboBox_End];
	CListBox	m_ListBoxArr[ListBox_End];
private:
	vector<INTERACTIONDATA>		m_vecInteractionData;
	INTERACTIONDATA				m_CurInteractionData;
	INTERACTIONOPTIONDATAINFO	m_CurInteractionOptionData;
	CCamera_UseTool*			m_pCamera = nullptr;
private:
	_int			m_iSelectInteractionIndex = -1;
	_int			m_iSelectOptionIndex = -1;
	_int			m_iSelectDetailDataIndex = -1;
	_bool			m_bIsSaveOrLoadState = false;
private:
	CInteraction_Character_UseTool*	m_pCharacter1 = nullptr;
	CInteraction_Character_UseTool*	m_pCharacter2 = nullptr;
private:
	HRESULT Init_String( void );
	HRESULT Init_CheckBox( void );
	HRESULT Init_ComboBox( void );
private:
	CHARACTERKIND Compute_CharacterKind( const _tchar* _pName );
private:
	HRESULT Save_InteractionData( const _tchar* _pFilePath );
	HRESULT Load_InteractionData( const _tchar* _pFilePath );
	HRESULT Setting_MFCToolBox( const INTERACTIONDATA& _InteractionData );

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_INTERACTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCreateplayer();
	afx_msg void OnBnClickedButtonCreatemonster();
	afx_msg void OnBnClickedButtonStartAnimation();
	afx_msg void OnBnClickedButtonPlChangeAniindex();
	afx_msg void OnBnClickedButtonPlChangeAnispeed();
	afx_msg void OnBnClickedButtonMonChangeAniindex();
	afx_msg void OnBnClickedButtonMonChangeAnispeed();
	afx_msg void OnBnClickedEditPlAddEventAnipos();
	afx_msg void OnBnClickedEditPlModifyEventOption();
	afx_msg void OnBnClickedEditPlAddEventDetaildata();
	afx_msg void OnBnClickedButtonModifyMonStartAnitime();
	afx_msg void OnBnClickedButtonModifyBetweenanglePlToMon();
	afx_msg void OnBnClickedButtonModifyBetweendistPlToMon();
	afx_msg void OnBnClickedButtonResetMonStartAnitime();
	afx_msg void OnBnClickedCheckPlChangeAngle();
	afx_msg void OnBnClickedCheckMonChangeAngle();
	afx_msg void OnBnClickedCheckPlChangePos();
	afx_msg void OnBnClickedCheckMonChangePos();
	afx_msg void OnBnClickedButtonStopAnimation();
	afx_msg void OnBnClickedButtonStartInteraction();
	afx_msg void OnBnClickedButtonPlChangePos();
	afx_msg void OnBnClickedButtonMonChangePos();
	afx_msg void OnBnClickedButtonMonChangeAngle();
	afx_msg void OnBnClickedButtonPlChangeAngle();
	afx_msg void OnBnClickedButtonAddInteraction();
	afx_msg void OnBnClickedButtonSaveInteraction();
	afx_msg void OnBnClickedButtonLoadInteraction();
	afx_msg void OnLbnSelchangeListOption();
	afx_msg void OnLbnSelchangeListDetaildata();
	afx_msg void OnLbnSelchangeListInteraction();
	afx_msg void OnBnClickedButtonDeleteInteraction();
	afx_msg void OnBnClickedButtonSelectcancelInteraction();
	afx_msg void OnBnClickedButtonPlayerCameraOnoff();
};
