//Jangwon
#pragma once

#include "Engine_Defines.h"
#include "base.h"
#include "Text_3D.h"

BEGIN(Engine)
//class C3D_Text;
class CShader;
//class CManagement;
//typedef std::function<void(void)> RenderFunc;

class DLL_EXPORT CDebug :public CBase
{
	DECLARE_SINGLETON(CDebug)
public:
	static const _ubyte Key[256];
	static const char DisplayKey[256];

	enum { 
		ConsoleDebug_MaxSize  = 10,	// �� ȭ�鿡 �׸� �� �ִ� �ֵܼ���� �ؽ�Ʈ ���μ�
		Debug_MaxSize = 1000		// �� ȭ�鿡 �׸� �� �ִ� ����� �ؽ�Ʈ ���μ�
	};

	enum class DEBUG_MODE
	{
		FreeMode,
		SelectMode
	};

	typedef struct tag_DebugTextInfo
	{
		char text[256];
		FLOAT xOffset;
		FLOAT yOffset; 
		DWORD dwFontColor;
	}DEBUGTEXTINFO;
private:
	explicit CDebug();
	virtual ~CDebug() = default;

protected:
	bool				m_bInit = false;
	C3D_Text*			m_pD3dFont = nullptr;
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;

	DEBUGTEXTINFO	m_DebugArr[Debug_MaxSize];

	list<char*> m_ConsoleDebugList;
	char	m_DebugStr[256];
	_tchar	m_DebugWStr[256];

	int m_iEnterLineSize = 20;				// �ؽ�Ʈ ���� ����

	int m_iConsoleDebugList_TotalCount = 0;	// ���� �׷����� �ֵܼ���� �ؽ�Ʈ ���μ�
	int m_iConsoleDebugList_CurSize = 0;	// ���� �׷����� �ֵܼ���� �ؽ�Ʈ ���μ�
	int m_iDebugArr_Size = 0;				// ���� �׷����� ����� �ؽ�Ʈ ���μ�
	int m_iStartDebugArr_Size = 0;			// ���� �׷����� ���� �ؽ�Ʈ ����

	float m_fXOffset_Console = 1200.f;		// �ܼ� ����� ������
	float m_fYOffset_Console = 200.f;		// �ܼ� ����� ������

	float m_fXOffset = 400.f;				// ����� ������
	float m_fYOffset = 200.f;				// ����� ������

	float m_fLineInterval = 400.f;			// �� ����

	float m_fAIRunTime = 0.f;				// AI ����ð�
	float m_fColliderRunTime = 0.f;			// �ݶ��̴� ����ð�
	float m_fEffectRunTime = 0.f;			// ����Ʈ ����ð�

	
	bool m_bDebugOn = false; // ����� ��
	DEBUG_MODE m_eMode = DEBUG_MODE::FreeMode;
	bool m_bManagerSelect = false; // �Ŵ��� ���� ���
	bool m_bObjManager = false; // ������Ʈ �Ŵ��� ���� ���
	bool m_bComManager = false; // ������Ʈ �Ŵ��� ���� ���
	
	
	Func	m_curDebug = nullptr;				// ���� Ȯ���� ��ü�� ����� �Լ�
	CBase*	m_pCurClass = nullptr;				// ���� ����� ���� ��ü

	
	//CBase::DEBUGNODE* m_curDebugNode = nullptr;	// ���� ����� ���� ���
	Func m_pRenderFunc = nullptr;
	float m_fGameUpdateTime = 1.f;
	float m_fSystemTimeDelta = 0.f;
	float m_fGameTimeDelta = 0.f;

	CShader*					m_pShader = nullptr;
	
	D3DVIEWPORT9				m_ViewPort;
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;

	LPDIRECT3DTEXTURE9			m_pTargetTexture = nullptr;
	LPDIRECT3DSURFACE9			m_pTargetSurface = nullptr;
	LPDIRECT3DSURFACE9			m_pOriginalSurface = nullptr;
	_matrix						m_matView;
	_matrix						m_matProj;
	bool						m_bNeedClearBuffer = true;
	bool						m_bGameUpdate = true;
	_ulong						m_iTotalFrame = 0;


public:
	C3D_Text* Get_Font() { return m_pD3dFont; }
	void Update_GameSpeed();
	void Update_DrawOffest();
	void BeginUpdate();
	void EndUpdate();
	void BeginScene();
	void EndScene();
	void Set_RenderFunc(Func pRenderFunc, CBase* classPtr);
	void Set_CurDebug(Func pDebugFunc, CBase* classPtr);
	void Release_CurDebug();

	void Set_GraphicDevice(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
	HRESULT	Load_WorldState(_tchar* pFileFullPath);
	FORCEINLINE _tchar*	Get_WorldStateName(int iIndex)
	{
		//if (bLoadWorldStateName)
		if (iIndex < m_WorldStateNameList.size())
			return m_WorldStateNameList[iIndex];
		else
			return L"ANI_";
		//else
		//	return nullptr;
	}
	private:
		bool	bLoadWorldStateName = false;
		vector<_tchar*> m_WorldStateNameList;
	public:
#endif

	void Add_ConsoleDebugText(const _tchar* str);		// �ܼ� ���
	void Add_ConsoleDebugText(const char* str);			// �ܼ� ���
	void Add_ConsoleDebugText(int iStr);				// �ܼ� ���
	void Add_ConsoleDebugText(float fStr);				// �ܼ� ���
	void Add_ConsoleDebugText(bool bStr);				// �ܼ� ���
	void Add_ConsoleDebugText(D3DXVECTOR3 vecStr);		// �ܼ� ���
	void Add_ConsoleDebugText(D3DXQUATERNION quatStr);	// �ܼ� ���
	void Add_ConsoleDebugText(char* format, ...);		// �ܼ� ���

	void Add_DebugText(FLOAT xOffset, FLOAT yOffset, DWORD dwFontColor, const char * strTextFormat, ...);
	void Add_DebugText(const char * strTextFormat, ...);
	void Add_DebugText(int var, const char * varStr)
		{Add_DebugText("%s: %d", varStr, var);}
	void Add_DebugText(float var, const char * varStr)
		{Add_DebugText("%s: %f", varStr, var);}
	void Add_DebugText(int var2, bool var,  const char * varStr);
	void Add_DebugText(_vec3& var, const char * varStr)
	{Add_DebugText("%s: %f / %f / %f", varStr, var.x,var.y,var.z); }
	void Add_DebugText(_matrix& var, const char * varStr)
	{
#ifndef _FINAL
		Add_DebugText("%s[%d]: %f / %f / %f / %f", varStr, 0, var.m[0][0], var.m[0][1], var.m[0][2], var.m[0][3]);
		Add_DebugText("%s[%d]: %f / %f / %f / %f", varStr, 1, var.m[1][0], var.m[1][1], var.m[1][2], var.m[1][3]);
		Add_DebugText("%s[%d]: %f / %f / %f / %f", varStr, 2, var.m[2][0], var.m[2][1], var.m[2][2], var.m[2][3]);
		Add_DebugText("%s[%d]: %f / %f / %f / %f", varStr, 3, var.m[3][0], var.m[3][1], var.m[3][2], var.m[3][3]);
#endif
	}

	void Add_DebugText(const _tchar * strTextFormat, ...);

	void Render_DebugText_Forced(FLOAT xOffset, FLOAT yOffset, DWORD dwFontColor, const char * strTextFormat, ...);

	FORCEINLINE void Move_Render_OffsetX(float fXOffset) { m_fXOffset_Console += fXOffset; m_fXOffset += fXOffset;}
	FORCEINLINE void Move_Render_OffsetY(float fYOffset) { m_fYOffset_Console += fYOffset; m_fYOffset += fYOffset;}
	FORCEINLINE void Set_SystemTimeDelta(float fTimeDelta) { m_fSystemTimeDelta = fTimeDelta; }
	FORCEINLINE void Set_GameTimeDelta(float fTimeDelta) { m_fGameTimeDelta = fTimeDelta; }
	FORCEINLINE	float Get_GameUpdateTime() { return m_fGameUpdateTime; }
	FORCEINLINE void Set_UpdateTime(float fTimeAcc) {
		m_fGameUpdateTime = fTimeAcc;
		if (m_fGameUpdateTime <= SMALL_NUMBER)
			m_fGameUpdateTime = SMALL_NUMBER;
	}
	FORCEINLINE void Add_UpdateTime(float fTimeAcc) { m_fGameUpdateTime += fTimeAcc; }
	FORCEINLINE void Min_UpdateTime(float fTimeAcc) { 
		m_fGameUpdateTime -= fTimeAcc; 
		if (m_fGameUpdateTime <= SMALL_NUMBER) 
			m_fGameUpdateTime = SMALL_NUMBER; }
	FORCEINLINE bool Check_DebugMode() { return m_bDebugOn; }

	FORCEINLINE void Add_AIRunTime(float fTime) { m_fAIRunTime += fTime; }
	FORCEINLINE void Add_ColliderRunTime(float fTime) { m_fColliderRunTime += fTime; }
	FORCEINLINE void Add_EffectRunTime(float fTime) { m_fEffectRunTime += fTime; }

	void Help_Debug();
public:
	void Update_DebugList();
	void Render_DebugList();
#ifndef _FINAL
	void Debug_Render();
#endif
private:
	virtual _ulong Free();
};

END