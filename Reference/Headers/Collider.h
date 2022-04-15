#pragma once

#include "VIBuffer.h"
#include "Transform.h"

BEGIN( Engine )

class CCollider_Manager;
class CGameObject;
class DLL_EXPORT CCollider : public CVIBuffer
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
private:
	enum COLLIDER_STATE
	{
		COLLIDER_NOCOL		// 충돌 되지 않은 상태..
		,COLLIDER_COL		// 충돌 중인 상태..
		,COLLIDER_NOACTIVE	// 활성화가 되지 않은 상태 ( 충돌 처리를 아예 실행하지 않음 )..
		,COLLIDER_END
	};
public:
	typedef struct tagColliderData	// 충돌을 위한 구조체..
	{
		//////////////// 콜리더 Original 데이터 ( 충돌 시 사용 안함 ).. ////////////////
		_vec3		vOriginalPoint[8] = { _vec3( 0.f, 0.f, 0.f ) };

		//////////////// 2 개 이상의 콜리더 타입이 사용함.. ////////////////
		_vec3		vCenter = _vec3( 0.f, 0.f, 0.f );				// Collider 의 중심 좌표..

		//////////////// OBB 에서 사용함.. ////////////////
		_vec3		vPoint[8] = { _vec3( 0.f,0.f,0.f ) };			// 콜리더의 점 8개( 로컬 좌표 )..
		_vec3		vAlign_Axis[3] = { _vec3( 0.f,0.f,0.f ) };		// 회전하지 않은 상태에서의 3 개의 축 ( 월드 기준 축 )..
		_vec3		vProj_Axis[3] = { _vec3( 0.f,0.f,0.f ) };		// 회전한 상태에서의 3 개의 축 ( 나의 월드 Rotation 기준 축 )..

																	//////////////// AABB 에서 사용함.. ////////////////
		_vec3		vMinWorld_NotRot;								// AABB 충돌에서 사용할 Min Positoin..
		_vec3		vMaxWorld_NotRot;								// AABB 충돌에서 사용할 Max Positoin..

																	//////////////// Sphere 에서 사용함.. ////////////////
		_float		fRadius = 0.f;									// 원의 반지름( 스케일(가장 큰)의 영향을 받은 것, 원 충돌일 경우 사용 )..
	} COLLIDERDATA;

	typedef struct tagResizeData
	{
		_vec3				vOriginalMin = _vec3( 0.f, 0.f, 0.f );	// Local 상의 Min 좌표..
		_vec3				vOriginalMax = _vec3( 0.f, 0.f, 0.f );	// Local 상의 Max 좌표..
		_vec3				vRotation = _vec3( 0.f, 0.f, 0.f );		// 회전 각도( Degree )..
		_vec3				vOffset = _vec3( 0.f, 0.f, 0.f );		// 시작지점에서부터 얼마만큼 떨어진 거리에 생성할 것인가( 최종행렬의 축 기준 )..
		_float				fOriginalRadius = 0.f;					// 원의 반지름( 스케일의 영향을 받지 않은 상태의 )..
		CTransform*			pTransformCom = nullptr;				// 부모 트랜스폼( 장착할 )..
		_matrix*			pParent = nullptr;						// 부모 행렬 ( ex)뼈 )..
	} RESIZEDATA;

	typedef struct tagInfoData	// 콜리더에서 사용할 데이터들..
	{
		_matrix					matLocal;					// Collider 만의 로컬 행렬..
		_matrix					matWorld;					// 최종 행렬 ( OBB 등등의 연산에서 쓰일 행렬 )..
		_matrix					matWorld_NotRot;			// 최종 행렬 ( AABB 에서 쓸 회전 정보가 담기지 않은 행렬 )..
		TYPE					eType = TYPE_END;			// 충돌 타입..
		COLLIDER_STATE			eState = COLLIDER_END;		// 콜리더 상태..
		Engine::CGameObject*	pGameObject = nullptr;		// 주인 게임 오브젝트..
		Engine::CTransform*		pObjectTransform = nullptr;	// 주인 게임 오브젝트의 트랜스폼..
		const _tchar*			pGroupTag = nullptr;		// 콜리더가 속해 있는 그룹의 태그..
		_bool					bCollision = false;			// 현재 프레임에 충돌했는가??( 매번 false 로 초기화됨 )..

		tagInfoData()
		{
			D3DXMatrixIdentity( &matLocal ); D3DXMatrixIdentity( &matWorld ); D3DXMatrixIdentity( &matWorld_NotRot );
		}

	} INFODATA;

	typedef struct tagDebugingData	// 디버깅을 위한 용도의 데이터들..
	{
		LPDIRECT3DTEXTURE9		pTexture[COLLIDER_END] = { nullptr };
		LPD3DXMESH				pSphereMesh = nullptr;
		_matrix					matSphereWorld;
	} DEBUGDATA;
private:
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;
public: // Setter
	void Set_IsActive( const _bool& _bIsActive );
public: // Getter
	Engine::CGameObject* Get_GameObject( void );
	_vec3 Get_Min( void );
	_vec3 Get_Min_WorldNotRot( void );
	_vec3 Get_Max( void );
	_vec3 Get_Max_WorldNotRot( void );
	_matrix Get_WorldMatrix( void );
	_matrix Get_WorldMatrix_NotRot( void );
	_bool Get_ColliderData( COLLIDERDATA* _pOut );
	COLLIDER_STATE Get_State( void );
	_bool Get_OptionList( list<OPTIONDATA>& _OptionList );
	_bool Get_StaticOption( STATICOPTIONDATA& _Option, const STATICOPTIONKIND& _eOption );
	CTransform* Get_ParentTransform( void );
	CTransform* Get_ObjectTransform( void );
	_bool Get_Collision( void );
	const _tchar* Get_GroupName( void );
public:
	void Add_Option( const OPTIONDATA& _OptionData );
	void Modify_StaticOption( const STATICOPTIONDATA& _OptionData, const STATICOPTIONKIND& _eOption );
public:
	HRESULT Ready_Collider(void);
	virtual void Update_Component( const _float& fTimeDelta ) override;
	virtual void LastUpdate_Component( const _float& fTimeDelta ) override;
	void Render_Buffer(void);
public:
	// 충돌 검사를 위한 함수들..
	_bool Collision_AABB(CCollider* pTargetCollider);
	_bool Collision_OBB(CCollider* pTargetCollider);
	_bool Collision_Sphere(CCollider* pTargetCollider, COLLISIONDATA& _CollisionData);
public:
	void ReSizing( TYPE eType, const _vec3 * pMin, const _vec3 * pMax, const CTransform* pTransformCom );
	void ReSizing( const TYPE& eType, const RESIZEDATA& ResizeData );
private:
	list<OPTIONDATA>		m_OptionList;			// 콜리더에 추가된 옵션 값들..
	STATICOPTIONDATA		m_StaticOptionArr[StaticOption_End];	// 콜리더쪽에서 수행할 기본적인 옵션들..
private:
	COLLIDERDATA			m_ColliderData;			// 충돌을 위한 데이터들..
	INFODATA				m_InfoData;				// 필수적으로 가지고 있어야 할 데이터들..
	RESIZEDATA				m_ResizeData;			// Collider 갱신을 위해 가지고 있어야 할 데이터들..
private:
	// 디버깅용 리소스들..
	DEBUGDATA				m_DebugData;
	static _bool			m_bIsShowCollider;
	static _bool			m_bIsKeyDown;
private:
	// 초기화를 위한 함수들..
	HRESULT Ready_Buffer( void );
	HRESULT Ready_DebugResource( void );
	HRESULT Ready_OriginalData( void );
private:
	// 데이터를 갱신시키기 위한 함수들..
	void Update_ColliderData( void );
	void Modify_WorldMatrix( void );
	void Modify_ColliderData( void );
	void Update_InfoData( void );
private:
	// 밀려나기 등의 충돌이 일어나면 실행해야 할 것들 실행..
	_bool ActCollisionWork( Engine::Collision& _Collision );
	_bool ActPushProcess( Engine::Collision& _Collision );
	_bool PushObject( Engine::Collision& _Collision, const _ulong& _dwMyPushOption, const _ulong& _dwOtherPushOption );
private:
	friend CCollider_Manager;
	///////////////// CCollider_Manager 클래스에서만 호출할 수 있는 함수들.. /////////////////
	// 충돌 시 충돌 되었다는 메시지를 보내주기 위한 함수..
	void OnCollisionEvent( std::function<void( const Engine::Collision& )> _pEventFunction, const Collision& _Collision );
private:
	void Set_GameObject( Engine::CGameObject* _pGameObject );
	void Set_GroupName( const _tchar* _pGroupTag ) { m_InfoData.pGroupTag = _pGroupTag; }
public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone(void);
public:
#ifndef _FINAL
	void Debug_Render();
#endif

	virtual _ulong Free(void);
};

END