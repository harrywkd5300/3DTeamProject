#pragma once
#include "afxwin.h"
#include "Tool_Defines.h"
#include "Engine_Defines.h"
#include "afxcmn.h"

#include "Renderer.h"

// CTool_Shader 대화 상자입니다.

#define etos(ENUM) std::string(#ENUM)

class CLightMgr_UseTool;
class CTool_Shader : public CPropertyPage
{
	DECLARE_DYNAMIC(CTool_Shader)

	enum
	{
		Str_VarName, Str_Float1, Str_Float2, Str_Float3, Str_Float4
		, Str_SelectVarNum, Str_SelectVarMin, Str_SelectVarMax, Str_SelectVarValue, Str_End
	};
	enum { Btn_Light, Btn_HDR, Btn_MotionBlur, Btn_DOF, Btn_SSAO, Btn_Shadow, Btn_EdgeDetecting, Btn_Fog, Btn_LightShaft, Btn_End };
	enum 
	{
		Slider_SelectVarValue, Slider_End
	};

public:
	CTool_Shader();
	virtual ~CTool_Shader();
public:
	_bool bIsActive = false;
public:
	void Update_Tool( const _float& _fTimeDelta );
	void Modify_StageType( const MAPKIND& _eMapKind );
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
private:
	CComboBox	m_Combo_ShaderVar;
	CString		m_StrArr[Str_End];
	CButton		m_BtnArr[Btn_End];
	CRenderer*	m_pRendererCom = nullptr;
	CSliderCtrl m_SliderArr[Slider_End];
	CComboBox	m_ComboBox_Map;
#ifdef ADD_GETTER_TARGETANDSHADER
private:
	_tchar**			m_pAllTagetNameArr = nullptr;
	_tchar**			m_pAllTagetOriginalNameArr = nullptr;
	LPD3DXEFFECT*		m_pAllEffectArr = nullptr;
	vector<CRenderer::EFFECTVARDATA> m_vecShaderData;
	_int				m_iCurShader = -1;
	_int				m_iCurShaderVar = -1;
	_int				m_iCurSelectShaderValueNum = 0;
	MAPKIND				m_eMapKind;
#endif
private:
	CListBox m_ListBox_RenderTarget;
	CListBox m_ListBox_Shader;
	CListBox m_ListBox_ShaderVar;
private:
	void Update_CheckBox( const _float& _fTimeDelta );
	void Update_EditBox( const _float& _fTimeDelta );
	void Update_Slider( const _float& _fTimeDelta );
	void Update_CurShaderVar( const _float& _fTimeDelta );
	void Update_SelectShaderValue( const _float& _fTimeDelta );
private:
	void Show_ShaderData( const _uint& _iIndex );
	void Show_ShaderVarData( const _uint& _iIndex );
private:
	void Update_ShaderVarToolBox( CRenderer::EFFECTVARTYPE& _ShaderVar );
private:
	void Save_ShaderVar( const _tchar* _pFilePath );
	void Load_ShaderVarData( const _tchar* _pFilePath );
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOOL_SHADER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeListShaderRendertarget();
	afx_msg void OnLbnSelchangeListShaderShaderlist();
	afx_msg void OnLbnSelchangeListShaderVariable();
	afx_msg void OnBnClickedButtonShaderAddvar();
	afx_msg void OnBnClickedButtonShaderSave();
	afx_msg void OnBnClickedButtonShaderLoad();
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnEnChangeEditShaderSelectvarnum();
	afx_msg void OnBnClickedButtonShaderDeletesShadervar();
	afx_msg void SetRadioStatus(UINT value);
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonModifyStageshaderdata();
};
