#pragma once


#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CCamera;
class CComponent;

class DLL_EXPORT CCameraLayer : public CBase
{
private:
	explicit CCameraLayer();
	virtual ~CCameraLayer() = default;
public:
	Engine::CComponent* Get_Component(const _uint& iIndex, const _tchar* pComTag);
	_uint Get_NumCamera(void) {
		return _uint(m_CameraList.size());
	}
public:
	HRESULT Add_Camera(CCamera* pCamera);
	CCamera*	Find_Camera(const _uint& iLayerIdx);
public:
	HRESULT Ready_Layer(void);
	_int Update_Layer(const _float& fTimeDelta);
	_int LastUpdate_Layer(const _float& fTimeDelta);
	void Render_Layer(void);
	CCamera* Find_ObjectByKey(const _uint& dwKey);
	HRESULT Delete_ObjectByKey(const _uint& pKey);
private:
	list<CCamera*>				m_CameraList;
	typedef list<CCamera*>		CAMERALIST;
public:
	static CCameraLayer* Create(void);
#ifndef _FINAL
public:
	void Debug_Render();
#endif

private:
	virtual _ulong Free(void) final;
};

END