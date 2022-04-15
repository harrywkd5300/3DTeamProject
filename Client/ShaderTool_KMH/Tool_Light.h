#pragma once
#include "Tool_Defines.h"
#include "Engine_Defines.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "MyColor.h"

// CTool_Light ��ȭ �����Դϴ�.
namespace Engine
{
	class CPicking;
	class CVIBuffer;
	class CRenderer;
}

class CLightMgr_UseTool;
class CCamera_UseTool;
class CBaseObject_UseTool;
class CTool_Light : public CPropertyPage
{
	DECLARE_DYNAMIC(CTool_Light)

public:
	CTool_Light();
	virtual ~CTool_Light();
public:
	enum 
	{
		Str_LightDirX, Str_LightDirY, Str_LightDirZ

		, Str_LightPosX, Str_LightPosY, Str_LightPosZ

		, Str_LightMinAngle, Str_LightMaxAngle, Str_LightRange

		, Str_LightDiffuseR, Str_LightDiffuseG, Str_LightDiffuseB, Str_LightDiffuseA
		, Str_LightAmbientR, Str_LightAmbientG, Str_LightAmbientB, Str_LightAmbientA
		, Str_LightSpecularR, Str_LightSpecularG, Str_LightSpecularB, Str_LightSpecularA

		, Str_LightGroupName

		, Str_YPosPlus

		, Str_End
	};

	enum BUTTONKIND 
	{
		Btn_YPosPickingPos,
		Btn_LightDebug,
		Btn_LightPicking,
		Btn_CamRunMode,
		Btn_LightOnOff,
		Btn_LightGroupOnOff,
		Btn_DiffuseSpecularSamePower,
		Btn_FloatingPower,
		Btn_End
	};

	enum 
	{
		Slider_LightDirX, Slider_LightDirY, Slider_LightDirZ

		,Slider_LightMinAngle, Slider_LightMaxAngle

		,Slider_LightDiffuseA, Slider_LightAmbientA, Slider_LightSpecularA

		,Slider_End
	};
public:
	_bool bIsActive = false;
	_float	m_fActiveTimeAcc = 0.f;
public:
	void Update_Tool( const _float& _fTimeDelta );
	_bool Compute_PickingPos( _vec3* _pOut );
	void Set_BtnCheck( const BUTTONKIND& _eBtnKind, const BOOL& _bCheck );
	void Set_BtnEnable( const BUTTONKIND& _eBtnKind, const BOOL& _bCheck );
	BOOL Get_BtnCheck( const BUTTONKIND& _eBtnKind );
public:
	void Modify_StageType( const MAPKIND& _eMapKind );
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	Engine::CPicking*	m_pPickingCom = nullptr;
	Engine::CRenderer*	m_pRendererCom = nullptr;
	Engine::CVIBuffer*	m_pTerrainBufferCom = nullptr;
	CCamera_UseTool*	m_pCamera = nullptr;
private:
	// Light Group ���� ��Ʈ��..
	CTreeCtrl							m_TreeCtrl_LightGroup;
	vector<pair<_tchar*, HTREEITEM>>	m_vecLightGroupItem;
	HTREEITEM							m_RootItem;

	// Light Group ������..
	CLightMgr_UseTool*			m_pLightMgr = nullptr;
	_uint						m_iDefaultLightGroupCount = 0;
	vector<_tchar*>				m_vecLightGroupName;
	CBaseObject_UseTool**		m_ppObjects = nullptr;
	_uint						m_iObjectCount = 0;
private:
	// Light ���� ��Ʈ��..
	CComboBox		m_Combo_LightType;
	_int			m_Radio_LightMode;

	// Map ���� ������..
	CComboBox		m_ComboBox_Map;
	MAPKIND			m_eMapKind;

	// Light ���� ������..
	_float			m_fDirMul = 10000.f;
	_float			m_fAngleMul = 10000.f;
	_float			m_fColorMul = 1000.f;
	_vec3			m_vPickingLightPos;
private:
	// �÷� ���̾�α�..
	CMyColor		m_ColorDialog;
	_bool			m_bIsSelectColor = false;
private:
	CString m_StrArr[Str_End];
	CButton m_BtnArr[Btn_End];
	CSliderCtrl m_SliderArr[Slider_End];
private:
	// ����� ����..
	_float	m_fYSpeed = 1.f;
	_float	m_fCurY = 0.f;
	_tchar	m_szPickingLightGroup[256] = L"";
	_uint	m_iPickingLightIndex = 0;
	// Light Range ����..
	_float	m_fLightRange = 0.f;
	_float	m_fTimeAcc = 0.f;
	// Tree Control ����..
	_float	m_fTreeCtrlClickTime = 0.f;	// Tree Ctrl �������� Ŭ���� �ð�
	vector<pair<_tchar*, HTREEITEM>>	m_vecLightItem;
	_uint	m_iLightCount = 0;
	pair<_tchar*, HTREEITEM>			m_CurSelectItem = { nullptr, NULL };
	pair<_tchar*, HTREEITEM>			m_CurSelectLightGroupItem = { nullptr, NULL };
private:
	_int m_iAddCountAcc = LIGHTSAVEINTERVAL;	// 5 �� ���� �����ϸ� �ڵ� �����ϰԲ� �ϱ� ����..
	_int m_iAutoSaveCountAcc = 0;				// �ڵ� ������ ��� �Ǿ����� ( ���� �ٸ� ���Ͽ� �����ϱ� ���� )..
private:
	_bool	m_bIsSelectLight = false;	// ���� ����Ʈ�� �����ߴ°�??..
public:
	// ���� �ε� ����..
	_bool	m_bIsSaveOrLoadState = false;
private:
	void Add_LightGroup( void );
	void Add_LightGroup( const _tchar* _pName );
public:
	void Add_Light( void );
	void Compute_LightInfo( D3DLIGHT9& _LightInfo );
	void Setting_MFCBox( D3DLIGHT9& _LightInfo );
private:
	// Ű �Է� üũ..
	void KeyCheck( const _float& _fTimeDelta );
private:
	void Update_LightData( const _float& _fTimeDelta );
	void Update_StrData( void );
	void Update_SliderData( void );
	void Update_SelectLight( void );
private:
	void InsertLightToTreeItem( const _tchar* _pLightName, const _tchar* _pGroupTag );
private:
	void CheckPickingLight( void ); // ����Ʈ ��ŷ On �� ���콺 Ŭ�� �� ȣ��..
	HRESULT ComputeRay( _vec3* _pOutRayPos, _vec3* _pOutRayDir );
	void Delete_Light( void );
	void Save_Data( void );
	void Load_Data( void );
	void Load_ObjectData( const MAPKIND& _eMapKind );
private:
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_LIGHT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnTvnSelchangedTree1( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnBnClickedButtonMakelightgroup();
	afx_msg void OnChangeRadioData( _uint value );
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnBnClickedButtonShowColordialog();
	afx_msg void OnBnClickedButtonShowAmbientcolordialog2();
	afx_msg void OnBnClickedButtonShowSpecularcolordialog3();
	afx_msg void OnBnClickedMapRenderRefresh();
	afx_msg void OnBnClickedButtonCreateMap();
};
