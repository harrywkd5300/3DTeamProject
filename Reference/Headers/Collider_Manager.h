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
		std::function<void( const Engine::Collision& )>	pEventFunction = nullptr; // �浹 �� ȣ���� �Լ� ( ������ ȣ�� �� �� )..
	} COLLIDERDATA;

	typedef struct tagColliderGroup
	{
		COLLIDERDATA*	pColliderData = nullptr;		// �ݸ�����( ���� �迭�� ���·� ���� )..
		_uint			iCollisionDataCount = 0;		// �ݸ������� ����..
		const _tchar*	pGroupName = nullptr;			// �׷� �̸�..
		_bool			bIsActive = true;				// Ȱ��ȭ ��ų�� ����..
		list<Engine::CCollider*>	CollisionList;		// �浹�� �ݸ���(�ٸ� �׷��� �ݸ���)���� ��� ����Ʈ..
		list<Engine::CCollider*>	CurCollisionList;	// �浹�� �ݸ���(�ٸ� �׷��� �ݸ���)���� ��� ����Ʈ..

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
		list<COLLIDERGROUP*>	ColliderGroupList;		// �ݸ��� �׷���� ������ ����Ʈ..
		list<tagColliderLayer*>	IgnoreLayerList;		// ������ �ݸ��� ���̾���� ������ ����Ʈ..
		const _tchar*			pLayerName = nullptr;	// ���̾� �̸�..
		_bool					bIsActive = true;		// Ȱ��ȭ ��ų�� ����..
		_ulong					dwPriorityKey = 0;		// ���̾� ������ Ű..

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
	list<COLLIDERGROUP>		m_ColliderGroupList[GROUP_END];		// ��� �ݸ��� �׷���� ��� �ִ� ����Ʈ..
	list<COLLIDERLAYER>		m_ColliderLayerList[GROUP_END];		// ��� �ݸ��� ���̾���� ��� �ִ� ����Ʈ..
public:
	//////////////////////////////////////////////////////////////////
	// Collider �� Collider Group �� ����ϴ� ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	HRESULT Add_Collider( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, CCollider* _pCollider, Engine::CGameObject* _pGameObject = nullptr,
						  std::function<void( const Engine::Collision& )> _pEventFunction = nullptr );

	//////////////////////////////////////////////////////////////////
	// Collider Group �� Collider Layer �� ����ϴ� ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	HRESULT Add_ColliderGroup( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, const _tchar* _pFindGroupTag );
public:
	//////////////////////////////////////////////////////////////////
	// Collider Group �� �浹 �˻� Ȱ��ȭ��Ű�� ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	HRESULT Activate_ColliderGroup( const GROUPKIND& _eGroupKind, const _tchar* _pLayerTag, const _tchar* _pFindGroupTag );
public:
	//////////////////////////////////////////////////////////////////
	// Collider �� Collider Group ���� ���� �ϴ� ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	_bool Delete_Collider( const _tchar* _pLayerTag, Engine::CCollider* _pCollider );

	//////////////////////////////////////////////////////////////////
	// Collider Group �� Collider Group List ���� �ϴ� ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	_bool Delete_ColliderInGroup( const GROUPKIND& _eGroupKind, const _tchar* _pFindGroupTag );

	//////////////////////////////////////////////////////////////////
	// Collider Layer �� ���� �ϴ� ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	_bool Delete_ColliderLayer( const GROUPKIND& _eGroupKind, const _tchar* _pFindLayerTag );

	//////////////////////////////////////////////////////////////////
	// Collider Layer �� Collider Layer List ���� ����(�׳� erase �� �ϴ�) ����� �Լ�..
	//////////////////////////////////////////////////////////////////
	_bool Exclude_ColliderGroupInLayer( const GROUPKIND& _eGroupKind, const _tchar* _pFindGroupTag, const _tchar* _pFindLayerTag );
public:
	//////////////////////////////////////////////////////////////////
	// ������ Collider Layer ����..
	//////////////////////////////////////////////////////////////////
	HRESULT Ignore_ColliderLayer( const _tchar* _pLayerTag, const _tchar* _pIgnoreLayerTag );
public:
	HRESULT Ready_Collider_Manager( void );
	void Update_Collider_Manager( void );
	void Release_DynamicLayer( void );
private:
	// �浹 ���� �Լ���..
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