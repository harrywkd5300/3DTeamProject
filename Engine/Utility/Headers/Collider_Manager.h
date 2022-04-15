#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN( Engine )

class CCollider;
class CGameObject;
class DLL_EXPORT CCollider_Manager :
	public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)
public:
	enum GROUPKIND { GROUP_NONE = -1, GROUP_STATIC, GROUP_DYNAMIC, GROUP_END };
private:
	typedef struct tagColliderData
	{
		Engine::CCollider*		pCollider = nullptr;
		std::function<void( const Engine::Collision& )>	pEventFunction = nullptr; // 충돌 시 호출할 함수 ( 없으면 호출 안 함 )..
	} COLLIDERDATA;

	typedef struct tagColliderGroup
	{
		COLLIDERDATA*	pColliderData = nullptr;		// 콜리더들( 동적 배열의 형태로 담음 )..
		_uint			iCollisionDataCount = 0;		// 콜리더들의 개수..
		const _tchar*	pGroupName = nullptr;			// 그룹 이름..
		_bool			bIsActive = true;				// 활성화 시킬지 말지..
		list<Engine::CCollider*>	CollisionList;		// 충돌된 콜리더(다른 그룹의 콜리더)들이 담길 리스트..
		list<Engine::CCollider*>	CurCollisionList;	// 충돌된 콜리더(다른 그룹의 콜리더)들이 담길 리스트..

		tagColliderGroup()
		{}

		tagColliderGroup( const COLLIDERDATA& _ColliderData, const _tchar* _pGroupName )
			: pGroupName( _pGroupName )
		{
			AddCollider( _ColliderData );
		}

		void AddCollider( const COLLIDERDATA& _ColliderData )
		{
			COLLIDERDATA* pTemp = new COLLIDERDATA[iCollisionDataCount + 1];

			for ( _uint i = 0; i < iCollisionDataCount; ++i )
			{
				pTemp[i] = pColliderData[i];
			}

			pTemp[iCollisionDataCount] = _ColliderData;

			Engine::Safe_Delete_Array( pColliderData );
			pColliderData = pTemp;

			++iCollisionDataCount;
		}

		_bool IsInCollisionList( const _ulong& _dwColliderKey );

	} COLLIDERGROUP;

	typedef struct tagColliderLayer
	{
		list<COLLIDERGROUP*>	ColliderGroupList;		// 콜리더 그룹들을 저장할 리스트..
		list<tagColliderLayer*>	IgnoreLayerList;		// 무시할 콜리더 레이어들을 저장할 리스트..
		const _tchar*			pLayerName = nullptr;	// 레이어 이름..
		_bool					bIsActive = true;		// 활성화 시킬지 말지..
		_ulong					dwPriorityKey = 0;		// 레이어 고유의 키..

		tagColliderLayer()
		{}

		tagColliderLayer( COLLIDERGROUP* _pGroup, const _tchar* _pLayerName, const _ulong& _dwPriorityKey )
			: pLayerName( _pLayerName )
			, dwPriorityKey( _dwPriorityKey )
		{
			ColliderGroupList.push_back( _pGroup );
		}

		_bool IgnoreCheck( tagColliderLayer& _OtherLayer )
		{
			for ( auto& IgnoreLayer : IgnoreLayerList )
			{
				if ( IgnoreLayer->dwPriorityKey == _OtherLayer.dwPriorityKey )
					return true;
			}

			return false;
		}

	} COLLIDERLAYER;
private:
	explicit CCollider_Manager( void );
	virtual ~CCollider_Manager( void ) = default;
private:
	list<COLLIDERGROUP>		m_ColliderGroupList[GROUP_END];		// 모든 콜리더 그룹들이 담겨 있는 리스트..
	list<COLLIDERLAYER>		m_ColliderLayerList[GROUP_END];		// 모든 콜리더 레이어들이 담겨 있는 리스트..
public:
	//////////////////////////////////////////////////////////////////
	// Collider 를 Collider Group 에 등록하는 기능의 함수..
	//////////////////////////////////////////////////////////////////
	HRESULT Add_Collider( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, CCollider* _pCollider, Engine::CGameObject* _pGameObject = nullptr,
						  std::function<void( const Engine::Collision& )> _pEventFunction = nullptr );

	//////////////////////////////////////////////////////////////////
	// Collider Group 을 Collider Layer 에 등록하는 기능의 함수..
	//////////////////////////////////////////////////////////////////
	HRESULT Add_ColliderGroup( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, const _tchar* _pFindGroupTag );
public:
	//////////////////////////////////////////////////////////////////
	// Collider Group 의 충돌 검사 활성화시키는 기능의 함수..
	//////////////////////////////////////////////////////////////////
	HRESULT Activate_ColliderGroup( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, const _tchar* _pFindGroupTag );
public:
	//////////////////////////////////////////////////////////////////
	// Collider 를 Collider Group 에서 제거 하는 기능의 함수..
	//////////////////////////////////////////////////////////////////
	_bool Delete_Collider( const _tchar* _pLayerTag, Engine::CCollider* _pCollider );

	//////////////////////////////////////////////////////////////////
	// Collider Group 을 Collider Group List 제거 하는 기능의 함수..
	//////////////////////////////////////////////////////////////////
	_bool Delete_ColliderInGroup( const GROUPKIND& _eGroupKind, const _tchar* _pFindGroupTag );

	//////////////////////////////////////////////////////////////////
	// Collider Layer 를 제거 하는 기능의 함수..
	//////////////////////////////////////////////////////////////////
	_bool Delete_ColliderLayer( const GROUPKIND& _eGroupKind, const _tchar* _pFindLayerTag );

	//////////////////////////////////////////////////////////////////
	// Collider Layer 을 Collider Layer List 에서 빼는(그냥 erase 만 하는) 기능의 함수..
	//////////////////////////////////////////////////////////////////
	_bool Exclude_ColliderGroupInLayer( const GROUPKIND& _eGroupKind, const _tchar* _pFindGroupTag, const _tchar* _pFindLayerTag );
public:
	//////////////////////////////////////////////////////////////////
	// 무시할 Collider Layer 세팅..
	//////////////////////////////////////////////////////////////////
	HRESULT Ignore_ColliderLayer( const _tchar* _pLayerTag, const _tchar* _pIgnoreLayerTag );
public:
	HRESULT Ready_Collider_Manager( void );
	void Update_Collider_Manager( void );
	void Release_DynamicLayer( void );
private:
	// 충돌 관련 함수들..
	void CollisionGroupToGroup( COLLIDERGROUP* _pDstColliderGroup, COLLIDERGROUP* _pSrcColliderGroup );
	void OnCollisionEvent( COLLIDERDATA* _pDstColliderData, COLLIDERDATA* _pSrcColliderData, const COLLISIONTYPE& _eCollisionType,
						   const COLLISIONDATA& _CollisionData );
private:
	COLLIDERGROUP* Find_ColliderGroup( const _tchar* _pLayerTag, const GROUPKIND& _eGroupKind );
	COLLIDERLAYER* Find_ColliderLayer( const _tchar* _pLayerTag );
private:
	void Release_Group( const GROUPKIND& _eGroupKind );
private:
	virtual _ulong Free( void ) override;
};

END