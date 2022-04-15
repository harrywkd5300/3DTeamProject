#include "..\Headers\Debug_Manager.h"

#include <atlstr.h>
#include "Debug_Manager.h"
#include "Graphic_Device.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "Effect_Manager.h"



IMPLEMENT_SINGLETON(CDebug)

//const _ubyte CDebug::Key[256] = {
//	DIK_1, DIK_2, DIK_3, DIK_4, DIK_5, DIK_6, DIK_7, DIK_8, DIK_9,
//	DIK_A, DIK_B, DIK_C, DIK_D, DIK_E, DIK_F, DIK_G, DIK_H, DIK_I, DIK_J, DIK_K, DIK_L, DIK_M,
//	DIK_N, DIK_O, DIK_P, DIK_Q, DIK_R, DIK_S, DIK_T, DIK_U, DIK_V, DIK_W, DIK_X, DIK_Y, DIK_Z 
//};
const _ubyte CDebug::Key[256] = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

const char CDebug::DisplayKey[256] = {
	'1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

CDebug::CDebug()
{

}

void CDebug::Update_GameSpeed()
{
#ifndef _FINAL
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
	{
		if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
		{
			CDebug::GetInstance()->Add_UpdateTime(0.00001f);
		}
		else if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
		{
			CDebug::GetInstance()->Min_UpdateTime(0.00001f);
		}
		else if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000)
		{
			CDebug::GetInstance()->Set_UpdateTime(1.f);
		}
		else if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000)
		{
			CDebug::GetInstance()->Set_UpdateTime(0.f);
		}
		else if (GetAsyncKeyState(VK_NUMPAD7) & 0x0001)
		{
			CDebug::GetInstance()->Min_UpdateTime(0.1f);
		}
		else if (GetAsyncKeyState(VK_NUMPAD9) & 0x0001)
		{
			CDebug::GetInstance()->Add_UpdateTime(0.1f);
		}
	}
#endif
}

void CDebug::Update_DrawOffest()
{
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000)
	{
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			m_fXOffset += 1.0f;
			m_fXOffset_Console += 1.0f;
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			m_fXOffset -= 1.0f;
			m_fXOffset_Console -= 1.0f;
		}
		else if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			m_fYOffset -= 1.0f;
			m_fYOffset_Console -= 1.0f;
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			m_fYOffset += 1.0f;
			m_fYOffset_Console += 1.0f;
		}
	}
}


void CDebug::BeginUpdate()
{
	m_fAIRunTime = 0.f;
	m_fColliderRunTime = 0.f;
	m_fEffectRunTime = 0.f;

	m_iDebugArr_Size = 0;
}

void CDebug::EndUpdate()
{
}

void CDebug::BeginScene()
{
#ifndef _FINAL
	++m_iTotalFrame;

	if (m_bInit == false || m_bDebugOn == false)
		return;

	m_pGraphicDev->GetRenderTarget(0, &m_pOriginalSurface);
	m_pGraphicDev->SetRenderTarget(0, m_pTargetSurface);
	m_pGraphicDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);
	m_pGraphicDev->SetRenderTarget(0, m_pOriginalSurface);
	Engine::Safe_Release(m_pOriginalSurface);

	m_pGraphicDev->GetRenderTarget(0, &m_pOriginalSurface);
	m_pGraphicDev->SetRenderTarget(0, m_pTargetSurface);

	m_bNeedClearBuffer = false;
#endif
}
void CDebug::EndScene()
{
#ifndef _FINAL
	if (m_bInit == false || m_bDebugOn == false)
		return;

	m_pGraphicDev->SetRenderTarget(0, m_pOriginalSurface);
	Engine::Safe_Release(m_pOriginalSurface);
#endif
}
void CDebug::Set_RenderFunc(Func pRenderFunc, CBase * classPtr)
{
}
void CDebug::Set_CurDebug(Func pDebugFunc, CBase* classPtr)
{
#ifndef _FINAL
	if (pDebugFunc != nullptr
		&& classPtr != nullptr)
	{
		Release_CurDebug();
		m_curDebug = pDebugFunc;
		m_pCurClass = classPtr;
		if (classPtr != this)
			classPtr->AddRef();
	}
#endif
}

void CDebug::Release_CurDebug()
{
	if(m_pCurClass !=this)
		Safe_Release(m_pCurClass);
	m_pCurClass = nullptr;
	m_curDebug = nullptr;
}

void CDebug::Set_GraphicDevice(LPDIRECT3DDEVICE9 pGraphicDev)
{
	if (m_pGraphicDev != nullptr)
		Safe_Release(m_pGraphicDev);

	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	m_pD3dFont = C3D_Text::Create(m_pGraphicDev, L"Arial", 15);

	if (nullptr == m_pD3dFont)
	{
		m_bInit = false;
		return;
	}

	m_pGraphicDev->GetViewport(&m_ViewPort);

	if (FAILED(D3DXCreateTexture(m_pGraphicDev, m_ViewPort.Width, m_ViewPort.Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTargetTexture)))
		return ;
	m_pTargetTexture->GetSurfaceLevel(0, &m_pTargetSurface);
	Engine::Safe_Release(m_pTargetTexture);

	if (FAILED(m_pGraphicDev->CreateVertexBuffer(sizeof(VTXTEX_VIEWPORT) * 4, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return;

	VTXTEX_VIEWPORT*	pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec4(0.0f - 0.5f, 0.0f - 0.5f, 0.f, 1.f);
	pVertex[0].vTexUV = _vec2(0.f, 0.f);

	pVertex[1].vPosition = _vec4(0.0f + m_ViewPort.Width - 0.5f, 0.0f - 0.5f, 0.f, 1.f);
	pVertex[1].vTexUV = _vec2(1.f, 0.f);

	pVertex[2].vPosition = _vec4(0.0f + m_ViewPort.Width - 0.5f, 0.0f + m_ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[2].vTexUV = _vec2(1.f, 1.f);

	pVertex[3].vPosition = _vec4(0.0f - 0.5f, 0.0f + m_ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertex[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	if (FAILED(m_pGraphicDev->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return;

	INDEX16*			pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	m_pShader = CShader::Create(m_pGraphicDev, L"../../Reference/Headers/Shader_Debug.fx");
	if (nullptr == m_pShader)
		return;

#ifndef _FINAL
	if (FAILED(Load_WorldState(L"../../Reference/Headers/WorldState_Enum.h")))
	{
		//AfxMessageBox(L"Load Failed");
		return;
	}
#endif
	m_bInit = true;

}
#ifndef _FINAL
HRESULT CDebug::Load_WorldState(_tchar * pFileFullPath)
{
	ifstream File(pFileFullPath);

	if (File.fail())
		return E_FAIL;

	string Buffer;

	size_t iIndex = 0;
	int iCount = 0;
	int iOffsetCount = 0;
	bool isCount = true;
	m_WorldStateNameList.reserve(ANIMATION_START);
	while (getline(File, Buffer))
	{
		if ((iIndex = Buffer.find(',')) != std::string::npos)
		{
			CString WorldState(Buffer.substr(0, iIndex).c_str());
			WorldState.Trim();

			// 클래스별 애니메이션 시작위치를 찾으면
			//if (WorldState.Find('=') != -1)
			if(iCount == ANIMATION_START)
			{
				break;
				//for (int i = iOffsetCount; i < CHARCTER_CLASS_MAX_SIZE; ++i)
				//{
				//	m_AniOffesetArr[i] = iCount;
				//}
				//++iOffsetCount;
			}
			m_WorldStateNameList.push_back(CString_Manager::GetInstance()->GetStr(WorldState.GetBuffer()));
			++iCount;
		}
		else
			continue;
	}
	//cout << "입력한 문장: " << line << endl;

	File.close();

	return NOERROR;

}
#endif
void CDebug::Add_ConsoleDebugText(const _tchar * str)
{
#ifndef _FINAL
	if (nullptr == str 
		|| m_bInit == false
		|| m_bDebugOn == false)
		return;

	if (m_iConsoleDebugList_CurSize > ConsoleDebug_MaxSize)
	{
		auto LastStr = m_ConsoleDebugList.begin();
		Safe_Delete_Array((*LastStr));
		--m_iConsoleDebugList_CurSize;
		m_ConsoleDebugList.erase(LastStr);
	}

	char count[16];
	sprintf_s(count, "%d >>", m_iConsoleDebugList_TotalCount);
	int lenCount = strlen(count) + 1;

	int len = lstrlen(str)+1;
	WideCharToMultiByte(CP_ACP, 0, str, len, m_DebugStr, len, NULL, NULL);

	char* tmpStr = new char[len + lenCount];

	strcpy_s(tmpStr, lenCount, count);
	strcat_s(tmpStr, len + lenCount, m_DebugStr);

	++m_iConsoleDebugList_CurSize;
	++m_iConsoleDebugList_TotalCount;
	m_ConsoleDebugList.push_back(tmpStr);
#endif
}

void CDebug::Add_ConsoleDebugText(const char * str)
{
#ifndef _FINAL
	if (nullptr == str)
		return;

	Add_ConsoleDebugText("%d", str);
#endif
}

void CDebug::Add_ConsoleDebugText(int iStr)
{
#ifndef _FINAL
	Add_ConsoleDebugText("%d", iStr);
#endif
}

void CDebug::Add_ConsoleDebugText(float fStr)
{
#ifndef _FINAL
	Add_ConsoleDebugText("%f", fStr);
#endif
}

void CDebug::Add_ConsoleDebugText(bool bStr)
{
#ifndef _FINAL
	if (bStr)
		Add_ConsoleDebugText("true");
	else
		Add_ConsoleDebugText("false");
#endif
}
void CDebug::Add_ConsoleDebugText(D3DXVECTOR3 vecStr)
{
#ifndef _FINAL
	Add_ConsoleDebugText("%f / %f / %f", vecStr.x, vecStr.y, vecStr.z);
#endif
}

void CDebug::Add_ConsoleDebugText(D3DXQUATERNION quatStr)
{
#ifndef _FINAL
	Add_ConsoleDebugText("%f / %f / %f / %f", quatStr.x, quatStr.y, quatStr.z, quatStr.w);
#endif
}

void CDebug::Add_ConsoleDebugText(char * format, ...)
{
#ifndef _FINAL
	if (m_bInit == false || m_bDebugOn == false)
		return;

	if (m_iConsoleDebugList_CurSize > ConsoleDebug_MaxSize)
	{
		auto LastStr = m_ConsoleDebugList.begin();
		Safe_Delete_Array((*LastStr));
		--m_iConsoleDebugList_CurSize;
		m_ConsoleDebugList.erase(LastStr);
	}

	va_list ap;
	va_start(ap, format);
	vsprintf_s(m_DebugStr, 256, format, ap);
	va_end(ap);
	puts(m_DebugStr);

	char count[16];
	sprintf_s(count, "%d >>", m_iConsoleDebugList_TotalCount);
	int lenCount = strlen(count) + 1;
	
	int len = int(strlen(m_DebugStr)) + 1;
	char* tmpStr = new char[len + lenCount];
	strcpy_s(tmpStr, lenCount, count);
	strcat_s(tmpStr, len + lenCount, m_DebugStr);

	++m_iConsoleDebugList_CurSize;
	++m_iConsoleDebugList_TotalCount;
	m_ConsoleDebugList.push_back(tmpStr);
#endif
}

void CDebug::Add_DebugText(FLOAT xOffset, FLOAT yOffset, DWORD dwFontColor, const char * strTextFormat, ...)
{
#ifndef _FINAL
	if (m_bInit == false || m_bDebugOn == false)
		return;

	if (m_iDebugArr_Size >= Debug_MaxSize)
		m_iDebugArr_Size = Debug_MaxSize -1;

	m_DebugArr[m_iDebugArr_Size].xOffset = xOffset;
	m_DebugArr[m_iDebugArr_Size].yOffset = yOffset;
	m_DebugArr[m_iDebugArr_Size].dwFontColor = dwFontColor;
	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_DebugArr[m_iDebugArr_Size].text, 256, strTextFormat, ap);
	va_end(ap);
	puts(m_DebugArr[m_iDebugArr_Size].text);

	++m_iDebugArr_Size;
#endif
}

void CDebug::Add_DebugText(const char * strTextFormat, ...)
{
#ifndef _FINAL
	if (m_bInit == false || m_bDebugOn == false)
		return;

	if (m_iDebugArr_Size >= Debug_MaxSize)
		m_iDebugArr_Size = Debug_MaxSize - 1;
	
	m_DebugArr[m_iDebugArr_Size].yOffset = m_fYOffset + m_iDebugArr_Size * m_iEnterLineSize;
	//m_DebugArr[m_iDebugArr_Size].xOffset = m_fXOffset + (int((m_DebugArr[m_iDebugArr_Size].yOffset)) / 1080) * m_fLineInterval;
	m_DebugArr[m_iDebugArr_Size].xOffset = m_fXOffset;
	m_DebugArr[m_iDebugArr_Size].dwFontColor = D3DXCOLOR(1.f, 0.f, 1.f, 1.f);

	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_DebugArr[m_iDebugArr_Size].text, 256, strTextFormat, ap);
	va_end(ap);
	puts(m_DebugArr[m_iDebugArr_Size].text);

	++m_iDebugArr_Size;
#endif
}


void CDebug::Add_DebugText(int var2, bool var, const char * varStr)
{
	if (var)
	{
		sprintf_s(m_DebugStr, "%s : true", varStr);
		Add_DebugText(m_DebugStr);
	}
	else
	{
		sprintf_s(m_DebugStr, "%s : false", varStr);
		Add_DebugText(m_DebugStr);
	}
}

void CDebug::Add_DebugText(const _tchar * strTextFormat, ...)
{
#ifndef _FINAL
	if (m_bInit == false || m_bDebugOn == false)
		return;

	if (m_iDebugArr_Size >= Debug_MaxSize)
		m_iDebugArr_Size = Debug_MaxSize - 1;

	m_DebugArr[m_iDebugArr_Size].yOffset = m_fYOffset + m_iDebugArr_Size * m_iEnterLineSize;
	//m_DebugArr[m_iDebugArr_Size].xOffset = m_fXOffset + (int((m_DebugArr[m_iDebugArr_Size].yOffset)) / 1080) * m_fLineInterval;
	m_DebugArr[m_iDebugArr_Size].xOffset = m_fXOffset;
	m_DebugArr[m_iDebugArr_Size].dwFontColor = D3DXCOLOR(1.f, 0.f, 1.f, 1.f);

	va_list        marker;
	va_start(marker, strTextFormat);
	_vstprintf(m_DebugWStr, strTextFormat, marker);
	va_end(marker);

	OutputDebugString(m_DebugWStr);

	int len = lstrlen(m_DebugWStr) + 1;
	WideCharToMultiByte(CP_ACP, 0, m_DebugWStr, len, m_DebugArr[m_iDebugArr_Size].text, len, NULL, NULL);

	++m_iDebugArr_Size;
#endif
}

void CDebug::Render_DebugText_Forced(FLOAT xOffset, FLOAT yOffset, DWORD dwFontColor, const char * strTextFormat, ...)
{
#ifndef _FINAL
	va_list ap;
	va_start(ap, strTextFormat);
	vsprintf_s(m_DebugStr, 256, strTextFormat, ap);
	va_end(ap);
	puts(m_DebugStr);

	m_pD3dFont->DrawTextW(xOffset, yOffset, dwFontColor, m_DebugStr);
#endif
}


void CDebug::Help_Debug()
{
#ifndef _FINAL

	Add_DebugText(">>>  Prev Info (BackSpace) / Up (Home) / Down (End)  <<<");
	Add_DebugText(">>>  High Class(PageUp) / Low Class(PageDown)  <<<");

	if (m_pCurClass != nullptr)
	{
		//if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_PGDN))
		if (GetAsyncKeyState(VK_NEXT) & 0x0001)
		{
			m_iStartDebugArr_Size = 0;
			Set_CurDebug(m_pCurClass->Set_NextFunc(), m_pCurClass);
		}
		//if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_PGUP))
		if (GetAsyncKeyState(VK_PRIOR) & 0x0001)
		{
			m_iStartDebugArr_Size = 0;
			Set_CurDebug(m_pCurClass->Set_PrevFunc(), m_pCurClass);
		}
		if (GetAsyncKeyState(VK_HOME) & 0x0001)
		{
			m_iStartDebugArr_Size -= 1;
			if(m_iStartDebugArr_Size < 0)
				m_iStartDebugArr_Size = 0;
		}
		if (GetAsyncKeyState(VK_END) & 0x0001)
		{
			m_iStartDebugArr_Size += 1;
			//if (m_iStartDebugArr_Size >= m_iDebugArr_Size)
				//m_iStartDebugArr_Size = m_iDebugArr_Size;
			
		}
	}
#endif
}

void CDebug::Update_DebugList()
{
	//if(m_pCurClass==this)
	//	if(m_eMode!=DEBUG_MODE::FreeMode)
	//		m_iDebugArr_Size = 0;
	//m_fAIRunTime = 0.f;
	//m_fColliderRunTime = 0.f;
	//m_fEffectRunTime = 0.f;

	m_bNeedClearBuffer = true;
	
#ifndef _FINAL
	Update_DrawOffest();

	if (m_pCurClass != nullptr)
	{
		if (m_pCurClass->GetRefCnt() == 0 && m_pCurClass != this)
		{
			Release_CurDebug();
			//Safe_Release(m_pCurClass);
			//m_pCurClass = nullptr;
			//m_curDebug = nullptr;
		}
	}

	if (m_bDebugOn)
	{
		//if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_F1))
		if(GetAsyncKeyState(VK_F1)&0x0001)
		{
			//모드 초기화
			m_iStartDebugArr_Size = 0;
			m_curDebug = nullptr;
			m_bManagerSelect = false;
			m_bDebugOn = false;
			m_eMode = DEBUG_MODE::FreeMode;
		}
		else if (m_bManagerSelect)
		{

			//Add_DebugText("1) Object_Manager");
			//Add_DebugText("2) Component_Manager");

			//if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_1))
			//{
			//	m_curDebug = std::bind(&CObject_Manager::Debug_Render, CObject_Manager::GetInstance());
			//	//m_bObjManager = true;
			//	//m_bManagerSelect = false;
			//}
			//else if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_2))
			//{
			//	m_curDebug = std::bind(&CComponent_Manager::Debug_Render, CComponent_Manager::GetInstance());
			//	//m_bComManager = true;
			//	//m_bManagerSelect = false;
			//}
			//else if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_3))
			//{
			//	m_curDebug = nullptr;
			//	m_eMode = DEBUG_MODE::FreeMode;
			//}
		}
		//else if
	}
	else
	{
		if (GetAsyncKeyState(VK_F1) & 0x0001)
		{
			m_iStartDebugArr_Size = 0;
			m_bDebugOn = true;
			m_bManagerSelect = true;
			m_eMode = DEBUG_MODE::SelectMode;
			Set_CurDebug(std::bind(&CDebug::Debug_Render, CDebug::GetInstance()),this);
			//m_pCurClass = this;
			//m_curDebug = std::bind(&CDebug::Debug_Render, CDebug::GetInstance());
			//m_curDebug = std::bind(&CComponent_Manager::Debug_Render, CComponent_Manager::GetInstance());
		}
	}
#else
	//if (m_bDebugOn)
	//{
	//	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_F1))
	//	{
	//		//모드 초기화
	//		m_curDebug = nullptr;
	//		m_bManagerSelect = false;
	//		m_bDebugOn = false;
	//		m_eMode = DEBUG_MODE::FreeMode;
	//	}
	//}
	//else
	//{
	//	if (CInput_Device::GetInstance()->Get_DownKeyState(DIK_F1))
	//	{
	//		m_bDebugOn = true;
	//		m_bManagerSelect = true;
	//		m_eMode = DEBUG_MODE::SelectMode;
	//	}
	//}
#endif
}

void CDebug::Render_DebugList()
{
#ifndef _FINAL
	if (m_bInit == false || m_bDebugOn == false)
		return;

	if (m_bNeedClearBuffer)
	{
		//m_iDebugArr_Size = 0;
		m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DXCOLOR(0.f, 0.f, 1.f, 1.f), 1.f, 0);
		m_pGraphicDev->BeginScene();
	}

	LPD3DXEFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->SetTexture("g_DebugTexture", m_pTargetTexture);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pGraphicDev->SetStreamSource(0, m_pVB, 0, sizeof(VTXTEX_VIEWPORT));
	m_pGraphicDev->SetFVF(VTXTEX_VIEWPORT::FVF);
	m_pGraphicDev->SetIndices(m_pIB);
	m_pGraphicDev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();

	if (m_eMode == DEBUG_MODE::SelectMode)
	{
		if (m_pCurClass != nullptr)
		{
			//if (false == m_bNeedClearBuffer)
			m_iDebugArr_Size = 0;
			Help_Debug();
			m_curDebug();
		}
	}
	//else if (m_eMode == DEBUG_MODE::FreeMode)
	//{
	//	if (false == m_bNeedClearBuffer)
	//		m_iDebugArr_Size = 0;
	//}


	float fEnterLen = 20.f;
	int iCount = 0;
	
	//콘솔 디버그 출력
	//Render_DebugText_Forced(m_fXOffset_Console, m_fYOffset_Console - 60.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  Total Console Size : %d<<<", m_ConsoleDebugList.size());
	//Render_DebugText_Forced(m_fXOffset_Console, m_fYOffset_Console - 40.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  Total Size : %d<<<", m_iDebugArr_Size);
	m_pD3dFont->DrawTextW(m_fXOffset_Console, m_fYOffset_Console - 20.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), "--Console Debug--");
	for (auto pStr : m_ConsoleDebugList)
	{	
		m_pD3dFont->DrawTextW(m_fXOffset_Console, fEnterLen * iCount + m_fYOffset_Console, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), pStr);
		++iCount;
		//m_pD3dFont->DebugText(m_fXOffset_Console, fEnterLen * iCount + m_fYOffset_Console, D3DXCOLOR(1.f, 1.f, 1.f, 1.f), pStr);
	}

	//디버그 출력
	if (m_fGameTimeDelta > SMALL_NUMBER)
	{
		Render_DebugText_Forced(m_fXOffset, m_fYOffset - 200.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  GameFps : %f<<<", float(1.0 / double(m_fGameTimeDelta)));
	}
	if (m_fSystemTimeDelta > SMALL_NUMBER)
	{
		Render_DebugText_Forced(m_fXOffset, m_fYOffset - 180.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  SystemFps : %f<<<", float(1.0 / double(m_fSystemTimeDelta)));
	}
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 160.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  Total Frame : %d<<<", m_iTotalFrame);
	m_pD3dFont->DrawTextW(m_fXOffset, m_fYOffset - 140.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), "--------------- Debug ---------------");
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 120.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  GameSpeedAcc : %f<<<", m_fGameUpdateTime);
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 100.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  fTimeDelta(System)  : %f<<<", m_fSystemTimeDelta);
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 80.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  fTimeDelta(Game)  : %f<<<", m_fGameTimeDelta);
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 60.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  AI Plan Time  : %f(ms)<<<", m_fAIRunTime);
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 40.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  Collider Time  : %f(ms)<<<", m_fColliderRunTime);
	Render_DebugText_Forced(m_fXOffset, m_fYOffset - 20.f, D3DXCOLOR(1.f, 0.f, 1.f, 1.f), ">>>  Effect Time  : %f(ms)<<<", m_fEffectRunTime);
	
	iCount = 0;
	if (m_iStartDebugArr_Size < 0)
		m_iStartDebugArr_Size = 0;

	if (m_iDebugArr_Size >= Debug_MaxSize)
		m_iDebugArr_Size = Debug_MaxSize;

	for (int i = m_iStartDebugArr_Size; i < m_iDebugArr_Size; ++i)
	{
		m_pD3dFont->DrawTextW(m_DebugArr[i].xOffset, m_DebugArr[i].yOffset - m_iStartDebugArr_Size*m_iEnterLineSize, m_DebugArr[i].dwFontColor, m_DebugArr[i].text);
		++iCount;
		//m_pD3dFont->DebugText(m_DebugArr[i].xOffset , m_DebugArr[i].yOffset, m_DebugArr[i].dwFontColor, m_DebugArr[i].text);
	}
	
	if (m_bNeedClearBuffer)
	{
		m_pGraphicDev->EndScene();
		m_pGraphicDev->Present(nullptr, nullptr, nullptr, nullptr);
	}
#endif

}
#ifndef _FINAL
void CDebug::Debug_Render()
{

	Add_DebugText("%c) Object_Manager", CDebug::DisplayKey[0]);
	Add_DebugText("%c) Component_Manager", CDebug::DisplayKey[1]);
	Add_DebugText("%c) Free", CDebug::DisplayKey[2]);
	Add_DebugText("%c) Camera_Manager", CDebug::DisplayKey[3]);
	Add_DebugText("%c) Effect_Manager", CDebug::DisplayKey[4]);


	//if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[0]))
	if (GetAsyncKeyState(CDebug::Key[0]) & 0x0001)
	{
		Set_CurDebug(std::bind(&CObject_Manager::Debug_Render, CObject_Manager::GetInstance()), CObject_Manager::GetInstance());
		//m_curDebug = std::bind(&CObject_Manager::Debug_Render, CObject_Manager::GetInstance());
	}
	//else if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[1]))
	else if (GetAsyncKeyState(CDebug::Key[1]) & 0x0001)
	{
		Set_CurDebug(std::bind(&CComponent_Manager::Debug_Render, CComponent_Manager::GetInstance()), CComponent_Manager::GetInstance());
		//m_curDebug = std::bind(&CComponent_Manager::Debug_Render, CComponent_Manager::GetInstance());
	}
	//else if (CInput_Device::GetInstance()->Get_DownKeyState(CDebug::Key[2]))
	else if (GetAsyncKeyState(CDebug::Key[2]) & 0x0001)
	{
		//m_curDebug = nullptr;
		Release_CurDebug();
		m_eMode = DEBUG_MODE::FreeMode;
	}
	else if (GetAsyncKeyState(CDebug::Key[3]) & 0x0001)
	{
		Set_CurDebug(std::bind(&CCamera_Manager::Debug_Render, CCamera_Manager::GetInstance()), CCamera_Manager::GetInstance());
		//m_curDebug = std::bind(&CComponent_Manager::Debug_Render, CComponent_Manager::GetInstance());
	}
	else if (GetAsyncKeyState(CDebug::Key[4]) & 0x0001)
	{
		Set_CurDebug(std::bind(&CEffect_Manager::Debug_Render, CEffect_Manager::GetInstance()), CEffect_Manager::GetInstance());
		//m_curDebug = std::bind(&CComponent_Manager::Debug_Render, CComponent_Manager::GetInstance());
	}
}
#endif

_ulong CDebug::Free()
{
	_ulong dwRefCount = 0;
	Safe_Release(m_pGraphicDev);

	if (dwRefCount += Safe_Release(m_pD3dFont))
	{
		MSG_BOX("D3dFont Release Failed");
		return dwRefCount;
	}

	for (auto pTchar : m_ConsoleDebugList)
	{
		Safe_Delete_Array(pTchar);
	}
	m_ConsoleDebugList.clear();
	m_iConsoleDebugList_CurSize = 0;

	if(m_pCurClass!=this)
		Safe_Release(m_pCurClass);

	
	m_pCurClass = nullptr;
	m_curDebug = nullptr;

	Safe_Release(m_pShader);
	Safe_Release(m_pTargetTexture);
	Safe_Release(m_pTargetSurface);
	Safe_Release(m_pOriginalSurface);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	return dwRefCount;
}
