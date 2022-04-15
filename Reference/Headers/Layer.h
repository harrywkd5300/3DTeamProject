#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGameObject;
class CComponent;
class DLL_EXPORT CLayer : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() = default; 
public:
	Engine::CComponent* Get_Component(const _uint& iIndex, const _tchar* pComTag);
	_uint Get_NumObject(void) {
		return _uint(m_ObjectList.size());}
public:
	HRESULT Add_GameObject(CGameObject* pGameObject);
	CGameObject* Find_GameObject(const _uint& iLayerIdx);
	CGameObject* FindObject_Near_Position(CGameObject* pObj); // 가장 가까운 오브젝트 반환
	CGameObject* FindObject_Near_Position(CGameObject* pObj, const _float& _fOffsetDegree, const _float& _fSightAngleDegree, const _float& _fSightRange); // 가장 가까운 오브젝트 반환
public:
	HRESULT Ready_Layer(void);
	_int Update_Layer(const _float& fTimeDelta);
	_int LastUpdate_Layer(const _float& fTimeDelta);
	void Render_Layer(void);
	CGameObject* Find_ObjectByKey(const _uint& dwKey);
	
	HRESULT Delete_ObjectByKey(const _uint& pKey);
private:
	list<CGameObject*>				m_ObjectList;
	typedef list<CGameObject*>		OBJECTLIST;
public:
	static CLayer* Create(void);
#ifndef _FINAL
public:
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;
};

END