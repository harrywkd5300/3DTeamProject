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
		COLLIDER_NOCOL		// �浹 ���� ���� ����..
		,COLLIDER_COL		// �浹 ���� ����..
		,COLLIDER_NOACTIVE	// Ȱ��ȭ�� ���� ���� ���� ( �浹 ó���� �ƿ� �������� ���� )..
		,COLLIDER_END
	};
public:
	typedef struct tagColliderData	// �浹�� ���� ����ü..
	{
		//////////////// �ݸ��� Original ������ ( �浹 �� ��� ���� ).. ////////////////
		_vec3		vOriginalPoint[8] = { _vec3( 0.f, 0.f, 0.f ) };

		//////////////// 2 �� �̻��� �ݸ��� Ÿ���� �����.. ////////////////
		_vec3		vCenter = _vec3( 0.f, 0.f, 0.f );				// Collider �� �߽� ��ǥ..

		//////////////// OBB ���� �����.. ////////////////
		_vec3		vPoint[8] = { _vec3( 0.f,0.f,0.f ) };			// �ݸ����� �� 8��( ���� ��ǥ )..
		_vec3		vAlign_Axis[3] = { _vec3( 0.f,0.f,0.f ) };		// ȸ������ ���� ���¿����� 3 ���� �� ( ���� ���� �� )..
		_vec3		vProj_Axis[3] = { _vec3( 0.f,0.f,0.f ) };		// ȸ���� ���¿����� 3 ���� �� ( ���� ���� Rotation ���� �� )..

																	//////////////// AABB ���� �����.. ////////////////
		_vec3		vMinWorld_NotRot;								// AABB �浹���� ����� Min Positoin..
		_vec3		vMaxWorld_NotRot;								// AABB �浹���� ����� Max Positoin..

																	//////////////// Sphere ���� �����.. ////////////////
		_float		fRadius = 0.f;									// ���� ������( ������(���� ū)�� ������ ���� ��, �� �浹�� ��� ��� )..
	} COLLIDERDATA;

	typedef struct tagResizeData
	{
		_vec3				vOriginalMin = _vec3( 0.f, 0.f, 0.f );	// Local ���� Min ��ǥ..
		_vec3				vOriginalMax = _vec3( 0.f, 0.f, 0.f );	// Local ���� Max ��ǥ..
		_vec3				vRotation = _vec3( 0.f, 0.f, 0.f );		// ȸ�� ����( Degree )..
		_vec3				vOffset = _vec3( 0.f, 0.f, 0.f );		// ���������������� �󸶸�ŭ ������ �Ÿ��� ������ ���ΰ�( ��������� �� ���� )..
		_float				fOriginalRadius = 0.f;					// ���� ������( �������� ������ ���� ���� ������ )..
		CTransform*			pTransformCom = nullptr;				// �θ� Ʈ������( ������ )..
		_matrix*			pParent = nullptr;						// �θ� ��� ( ex)�� )..
	} RESIZEDATA;

	typedef struct tagInfoData	// �ݸ������� ����� �����͵�..
	{
		_matrix					matLocal;					// Collider ���� ���� ���..
		_matrix					matWorld;					// ���� ��� ( OBB ����� ���꿡�� ���� ��� )..
		_matrix					matWorld_NotRot;			// ���� ��� ( AABB ���� �� ȸ�� ������ ����� ���� ��� )..
		TYPE					eType = TYPE_END;			// �浹 Ÿ��..
		COLLIDER_STATE			eState = COLLIDER_END;		// �ݸ��� ����..
		Engine::CGameObject*	pGameObject = nullptr;		// ���� ���� ������Ʈ..
		Engine::CTransform*		pObjectTransform = nullptr;	// ���� ���� ������Ʈ�� Ʈ������..
		const _tchar*			pGroupTag = nullptr;		// �ݸ����� ���� �ִ� �׷��� �±�..
		_bool					bCollision = false;			// ���� �����ӿ� �浹�ߴ°�??( �Ź� false �� �ʱ�ȭ�� )..

		tagInfoData()
		{
			D3DXMatrixIdentity( &matLocal ); D3DXMatrixIdentity( &matWorld ); D3DXMatrixIdentity( &matWorld_NotRot );
		}

	} INFODATA;

	typedef struct tagDebugingData	// ������� ���� �뵵�� �����͵�..
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
	// �浹 �˻縦 ���� �Լ���..
	_bool Collision_AABB(CCollider* pTargetCollider);
	_bool Collision_OBB(CCollider* pTargetCollider);
	_bool Collision_Sphere(CCollider* pTargetCollider, COLLISIONDATA& _CollisionData);
public:
	void ReSizing( TYPE eType, const _vec3 * pMin, const _vec3 * pMax, const CTransform* pTransformCom );
	void ReSizing( const TYPE& eType, const RESIZEDATA& ResizeData );
private:
	list<OPTIONDATA>		m_OptionList;			// �ݸ����� �߰��� �ɼ� ����..
	STATICOPTIONDATA		m_StaticOptionArr[StaticOption_End];	// �ݸ����ʿ��� ������ �⺻���� �ɼǵ�..
private:
	COLLIDERDATA			m_ColliderData;			// �浹�� ���� �����͵�..
	INFODATA				m_InfoData;				// �ʼ������� ������ �־�� �� �����͵�..
	RESIZEDATA				m_ResizeData;			// Collider ������ ���� ������ �־�� �� �����͵�..
private:
	// ������ ���ҽ���..
	DEBUGDATA				m_DebugData;
	static _bool			m_bIsShowCollider;
	static _bool			m_bIsKeyDown;
private:
	// �ʱ�ȭ�� ���� �Լ���..
	HRESULT Ready_Buffer( void );
	HRESULT Ready_DebugResource( void );
	HRESULT Ready_OriginalData( void );
private:
	// �����͸� ���Ž�Ű�� ���� �Լ���..
	void Update_ColliderData( void );
	void Modify_WorldMatrix( void );
	void Modify_ColliderData( void );
	void Update_InfoData( void );
private:
	// �з����� ���� �浹�� �Ͼ�� �����ؾ� �� �͵� ����..
	_bool ActCollisionWork( Engine::Collision& _Collision );
	_bool ActPushProcess( Engine::Collision& _Collision );
	_bool PushObject( Engine::Collision& _Collision, const _ulong& _dwMyPushOption, const _ulong& _dwOtherPushOption );
private:
	friend CCollider_Manager;
	///////////////// CCollider_Manager Ŭ���������� ȣ���� �� �ִ� �Լ���.. /////////////////
	// �浹 �� �浹 �Ǿ��ٴ� �޽����� �����ֱ� ���� �Լ�..
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