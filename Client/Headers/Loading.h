#pragma once

#include "Defines.h"
#include "Engine_Defines.h"
#include "Base.h"

class CLoading : public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading() = default;
public:
	const _tchar* Get_LodingFont() {
		return m_szLoading;}
	_bool Get_Finish(void) {
		return m_isFinish;}
	LOADTYPE Get_LoadID(void) const {
		return m_eLoadID;}
	CRITICAL_SECTION* Get_Key(void) {
		return &m_Key;}
public:
	void Set_Finish(_bool isFinish) {
		m_isFinish = isFinish;}
public:
	HRESULT Ready_Loading(LOADTYPE eSceneID);
	HRESULT Loading_Shader(void);
	HRESULT Loading_Tex_UI(void);
	HRESULT Loading_ForEffect(void);
	HRESULT Loading_ForStageScene(void);
	HRESULT Loading_ForDonatello(void);
	HRESULT Loading_ForLeonardo(void);
	HRESULT Loading_ForMichelangelo(void);
	HRESULT Loading_ForRaphael(void);
	HRESULT Loading_ForStageScene_Monster(void);
	HRESULT Loading_ForBossScene(void);
	
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
private:
	HANDLE						m_hThread;
	LOADTYPE					m_eLoadID;
	CRITICAL_SECTION			m_Key;
	_bool						m_isFinish = false;
	_tchar						m_szLoading[128] = L"";

	
public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADTYPE eLoadID);
	virtual _ulong Free(void);

};

