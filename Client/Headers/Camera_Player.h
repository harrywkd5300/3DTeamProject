#pragma once


#include "Defines.h"
#include "Camera.h"


namespace Engine
{
	class CTransform;
	class CSubject_Manager;
}

class CCameraObserver;

class CCamera_Player final : public Engine::CCamera
{
public:
	typedef struct tagShakingInfo // By.PSH
	{
		_float fShakingMaxTime = 0.5f;
		_float fShakingTime = 0.f;
		_float fShakingHeight = 10000.f;
		_float fShakingHeightX = 9000.f;
		_float fShakingRange = 0.01f;

		_bool IsShaking = false;
	}SHAKINGINFO;
public:
	enum CAM_MODE{MODE_PLAYER,MODE_NOTPLAY,MODE_UI, MODE_DEBUG,MODE_END};

private:
	explicit CCamera_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCamera_Player() = default;

public:
	_vec3* Get_MoveLook() { return &m_vMoveLook; }
	_float Get_InverseY() { return m_fInverseY; }

public:
	HRESULT Set_Player(CTransform* pPlayerTransform);
	void Set_PlayerType(PLAYER_TYPE eType) { m_ePlayerType = eType; }
	//void Set_Shaking(_bool IsShaking) { m_IsShaking = IsShaking; }

	void Set_InverseY(_bool bInverseY) 	{ 
		if (bInverseY == true)
			m_fInverseY = 1.f;
		else
			m_fInverseY = -1.f;
	}
	void Set_Mouse_SensitivityX(_float fSensitivityX) { m_fInverseY = fSensitivityX; }
	void Set_Mouse_SensitivityY(_float fSensitivityY) { m_fInverseY = fSensitivityY; }
public:
	HRESULT Ready_Observer();
	virtual HRESULT Ready_GameObject(const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY, const _float& fAspect, const _float& fNear, const _float& fFar);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
private:
	void DebugCamera_InputState(const _float& fTimeDelta);
	void Check_InputState(const _float& fTimeDelta);
	void Fix_MousePointer(const _uint& iX, const _uint& iY);
private:
	void Check_State();

	void Update_Mode(const _float& fTimeDelta);
	void Shake_Mode(const _float& fTimeDelta);
	void Player_Mode(const _float& fTimeDelta);	
	void NotPlay_Mode(const _float& fTimeDelta);
	void UI_Mode(const _float& fTimeDelta);
	void Move_Camera(const _float& fTimeDelta);
	void Set_Distance(const _float& fTimeDelta);
private: //墨皋扼 可记 
	_float			m_fCamSpeed = 10.f;
	_float			m_fDistance = 1.2f;
	_float			m_fMaxDistance = 1.2f;
	_float			m_fMinDistance = 1.2f;

	_float			m_fNotPlayDistance = 5.f;
	
	_vec3			m_vRelative;
	_float			m_fRelative = 1.2f;

	_vec3			m_vMoveLook;

	_vec3			m_vEndEye;
	_vec3			m_vEndAt;	

	_vec3			m_vAtDir;
	_vec3			m_vDir; // 具概

private: //葛靛 可记
	//_float			m_fShakingTime = 0.f;
	//_float			m_fShakingMaxTime = 0.5f;

	//_float			m_fShakingRange = 0.01f;
	//_float			m_fShakingHeight = 10000.f;
	//_float			m_fShakingHeightX = 9000.f;

	//_bool			m_IsShaking = false;

	SHAKINGINFO		m_tShakingInfo;

	_bool			m_IsDebugMode = false;	
	_bool			m_IsMouseFix = false;

	CAM_MODE		m_eCurState = MODE_END;
	CAM_MODE		m_ePreState = MODE_PLAYER;

	PLAYER_TYPE		m_ePlayerType = PLAYER_TYPE::PLAYER_None;
private: // 付快胶 可记
	_vec3			m_vAngle;
	_vec3			m_vCurAngle;
	_float			m_fAngleSpeed = 0.f;

	_float			m_fInverseY = 1.f;
	_float			m_fMouse_SensitivityX = 5.f;
	_float			m_fMouse_SensitivityY = 5.f;


private:
	_float			m_fA = -13;
	_float			m_fB = -10;
	_float			m_fTimeA = 2;

	_float			m_fC = 1;


private:
	CTransform*		m_pPlayerTransform = nullptr;
	CSubject_Manager* m_pSubject_Manager = nullptr;
	CCameraObserver* m_pObserver = nullptr;

public:
	static CCamera_Player* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pEye, const _vec3* pAt, const _vec3* pUp
		, const _float& fFovY = D3DXToRadian(60.0f), const _float& fAspect = _float(BACKSIZEX) / _float(BACKSIZEY), const _float& fNear = 0.2f, const _float& fFar = 1000.f);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void);
};

