#pragma once

// 객체들을 모아서관리하낟
// 클라이언트에서 지정한 갯수만큽ㅁ의 배열 컨테이너를 가진다.

#include "Engine_Defines.h"
#include "Base.h"
#include "GameObject.h"
#include "Trigger.h"
//#include "Character.h"

BEGIN(Engine)

class CLayer;
class CGameObject;
class CComponent;
class DLL_EXPORT CObject_Manager : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	enum OBJTYPE {};
private:
	explicit CObject_Manager();
	virtual ~CObject_Manager(); 
public:
	Engine::CComponent* Get_Component(_uint iSceneIdx, const _tchar* pLayerTag, const _uint& iIndex, const _tchar* pComTag);
	_uint Get_NumObject(const _uint& iSceneIdx, const _tchar* pLayerTag);
public:
	HRESULT Reserve_ObjectContainer(const _uint&  iMaxSize);
	HRESULT Release_Object(const _uint& iSceneIdx);


public:
	CGameObject*	Get_FindObject(const _uint& iArrayIdx, const _tchar* pLayerTag, const _uint & iLayerIdx);
	CGameObject*	Get_FindObject_Near_Position(const _uint& iArrayIdx, const _tchar* pLayerTag, CGameObject* pObj); // 가장 가까운 오브젝트 반환
	CGameObject*	Get_FindObject_Near_Position(const _uint& iArrayIdx, const _tchar* pLayerTag, CGameObject* pObj, // 내 시야 범위 내의 적 중 가장 가까운 오브젝트 반환
												  const _float& _fOffsetDegree, const _float& _fSightAngleDegree, const _float& _fSightRange);

public:
	HRESULT Ready_GameObject(const _uint& iArrayIdx, const _tchar* pLayerTag, CGameObject* pGameObject, OBJ_TYPE eType = OBJ_TYPE::OBJ_TYPE_DYNAMIC);
	//HRESULT Ready_StaticGameObject(const _uint& iArrayIdx, const _tchar* pLayerTag, CGameObject* pGameObject);
public:
	HRESULT Ready_Object_Manager(void);
	_int Update_Object_Manager(const _float& fTimeDelta);
	_int LastUpdate_Object_Manager(const _float& fTimeDelta);
	
	HRESULT Update_StaticObject();


	HRESULT			Delete_ObjectByKey(const _uint& dwKey);
	CGameObject*	Find_ObjectByKey(const _uint& dwKey);
	
	void			Add_DeleteObject(CGameObject* pDeleteObject);
	_ulong			Release_DeleteObjectList();
private:
	list<CGameObject*>						m_DeleteObjectList;

	//map<const _tchar*, CLayer*>*			m_StaticMapLayer = nullptr;
	
	map<const _tchar*, CLayer*>*			m_pMapLayer[OBJ_TYPE_END];
	typedef map<const _tchar*, CLayer*>		MAPLAYER;
private:
	_uint									m_iContainerSize = 0;
private:
	CLayer* Find_Layer(const _uint& iArrayIdx, const _tchar* pLayerTag, int eType = OBJ_TYPE_DYNAMIC);
#ifndef _FINAL
public:
	CLayer* Find_Layer_ForMapTool(const _uint& iArrayIdx, const _tchar* pLayerTag, int eType = OBJ_TYPE_DYNAMIC);
	void Debug_Render();
#endif
private:
	virtual _ulong Free(void) final;
};

END