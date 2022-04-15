// Jangwon
#pragma once

#include "Engine_Defines.h"
#include "gameObject.h"
#include "AI/GOAP_AI_Manager.h"
#include "Controller.h"

BEGIN(Engine)

typedef struct tag_CharacterInfo
{
	enum TYPE{ CHAR_PLAYER ,CHAR_MONSTER, CHAR_END}; //�Ʊ�, ����
	enum CHARACTERSIZEKIND { CharacterSize_Small, CharacterSize_Mid, CharacterSize_Big, CharacterSize_End };

	_int					m_iCurHp = 100;		// ����ü��
	_int					m_iMaxHp = 100;		// �ִ� ü��
	_float					m_fHpAcc = 1.f;		// ����ü��/�ִ�ü��

	_float					m_fCurCp = 30.f;	// ���� ī���� By.PSH
	_float					m_fMaxCp = 30.f;	// �ִ� ī����
	_float					m_fCspCp = 10.f;	// ī���� ���ҷ�
	_float					m_fIcmCp = 3.f;		// ī���� �ʴ� ������
	_float					m_fCpAcc = 1.f;		// ����ī����/�ִ�ī����

	_float					m_fCurEp = 30;		// ���� ��� By.PSH
	_float					m_fMaxEp = 30;		// �ִ� ���
	_float					m_fEpAcc = 1.f;		// ������/�ִ���

	_int					m_iDefence = 0;		// ����


	_float					m_fAngle = 0.f;  // ���� Y��ȸ���� // By.PSH

	_bool					m_bHit = false;	// ��Ʈ�Ǿ�����
	_vec3					m_MoveDelta = _vec3(0.f, 0.f, 0.f);	// �̵�������
	_float					m_fSpeed = 0.f; //�̵��ӵ� ��
	tag_CharacterInfo::TYPE	m_eCharType = CHAR_MONSTER; // ĳ���� Ÿ��
	CHARACTERSIZEKIND		m_eCharSizeType = CharacterSize_End;	// ĳ���� ������ Ÿ��( ����, ����, ���� )..

	_vec3					m_vecAniMoveDelta = _vec3(0.f, 0.f, 0.f); // (��Ʈ���)�����Ǻ�ȭ��
	_vec3					m_vecAniAngleDelta = _vec3(0.f, 0.f, 0.f); // (��Ʈ���)�ޱۺ�ȭ�� 

	_float					m_fAngleSpeed = 5.f;				// �ޱ� ���ǵ�
	_float					m_fDestAngle = 0.f;					// �̵��� �ޱ�
	_float					m_fAngleGap_CameraToPlayer = 0.f;	// ī�޶�-�÷��̾� �躤�� �ޱ� ��

	_float					m_fIdleDelay = 8.f;			// ��ȭ ���� �Ѿ�� ���� �ð�

														//CCharacter*				m_pNearEnemy = nullptr;		// ��ó ��
	CCharacter*				m_pEnemy_Attacked = nullptr;	// ���� ������ ��

	_uint					m_iMoveColliderGroupCount = 0;	// �浹�� �ݸ��� �׷� ��ȣ..

	_float					m_fRange_FOV	= 75.f;	// ĳ������ �þ� ��..

	_float					m_fRange_Close	= 2.5f;
	_float					m_fRange_Near	= 10.f;
	_float					m_fRange_Mid	= 20.f;
	_float					m_fRange_Far	= 30.f;
	_float					m_fRange_TooFar = 40.f;
	_float					m_fRange_FoundEnemy = 20.f;

	//--------------------Monster State----------------------------------

	_int					m_iShield_Point = 0;
	_float					m_fEvade_Point = 0.f;

	_bool					m_bShield = false;
	_bool					m_bEvade = false;

	//------------------------------------------------------


	// --------- AI ��� --------
	_tchar					m_StartAniName[32];			// ĳ���� �ִϸ��̼� ���� ��Ʈ��

	CWorldState				m_WorldState;				// ĳ���� ���� ������Ʈ

	vector<CAction>			m_vecAction_Act;			// ĳ���� �ൿ ���� �׼� ����
	CAction					m_CurAction_Act;			// ĳ���� ���� �ൿ ���� �׼�

	vector<CAction>			m_vecAction_Ani;			// ĳ���� �ִϸ��̼� ���� �׼� ����
	CAction					m_CurAction_Ani;			// ĳ���� ���� �ִϸ��̼� ���� �׼�

	int						m_StartAniNum = 0;			// ĳ���� �ִϸ��̼� ���� �ѹ�
	int						m_EndAniNum = 0;			// ĳ���� �ֹ̳��̼� ���� �ѹ�

														//tag_CharacterInfo()
														//{};
														//tag_CharacterInfo(const tag_CharacterInfo& rhs)
														//{
														//	m_iCurHp = rhs.m_iCurHp;
														//	m_iMaxHp = rhs.m_iMaxHp;
														//	m_fHpAcc = rhs.m_fHpAcc;

														//	m_iCurCp = rhs.m_iCurCp;
														//	m_iMaxCp = rhs.m_iMaxCp;
														//	m_fCpAcc = rhs.m_fCpAcc;

														//	m_iCurEp = rhs.m_iCurEp;
														//	m_iMaxEp = rhs.m_iMaxEp;
														//	m_fEpAcc = rhs.m_fEpAcc;

														//	m_fAngle = rhs.m_fAngle;

														//	m_bHit = rhs.m_bHit;
														//	m_MoveDelta = rhs.m_MoveDelta;
														//	m_fSpeed = rhs.m_fSpeed;
														//	m_eCharType = rhs.m_eCharType;

														//	m_vecAniMoveDelta = rhs.m_vecAniMoveDelta;
														//	m_vecAniAngleDelta = rhs.m_vecAniAngleDelta;

														//	m_fAngleSpeed = rhs.m_fAngleSpeed;
														//	m_fDestAngle = rhs.m_fDestAngle;
														//	m_fAngleGap_CameraToPlayer = rhs.m_fAngleGap_CameraToPlayer;

														//	m_fIdleDelay = rhs.m_fIdleDelay;

														//	m_pEnemy_Attacked = rhs.m_pEnemy_Attacked;

														//	m_iMoveColliderGroupCount = rhs.m_iMoveColliderGroupCount;

														//	m_fRange_Close = rhs.m_fRange_Close;
														//	m_fRange_Near = rhs.m_fRange_Near;
														//	m_fRange_Mid = rhs.m_fRange_Mid;
														//	m_fRange_Far = rhs.m_fRange_Far;
														//	m_fRange_TooFar = rhs.m_fRange_TooFar;
														//	m_fRange_FoundEnemy = rhs.m_fRange_FoundEnemy;


														//	m_WorldState = rhs.m_WorldState;

														//	m_vecAction_Act = rhs.m_vecAction_Act;
														//	m_CurAction_Act = rhs.m_CurAction_Act;

														//	m_vecAction_Ani = rhs.m_vecAction_Ani;
														//	m_CurAction_Ani = rhs.m_CurAction_Ani;

														//	m_StartAniNum = rhs.m_StartAniNum;
														//	m_EndAniNum = rhs.m_EndAniNum;
														//};
														// --------- AI ��� --------
}CHARINFO;


class CRenderer;
class CTransform;
class CPicking;
class CMaterial;
class CVIBuffer;
class CMesh_Dynamic;
class CCollider;
class CNavigation;
class CShader;
//class CController;
class CEquipment;
class CCamera;
class CCollider_Manager;

#ifdef CHARACTER_COLLIDERMODIFY
typedef struct tagColliderGroup
{
	const _tchar*			pGroupName = nullptr;	// �׷� �̸� ( ���� �Ⱦ���, Ȥ�� ���� )..
	const _tchar*			pLayerName = nullptr;	// �׷��� ���� �ִ� ���̾� �̸�( or ���� ������ ���̾� �̸� )..
	Engine::CCollider**		ppCollider = nullptr;	// ���� ���� �ݸ����� ���� �迭�� ���·� ���..
	_uint					iColliderCount = 0;		// �ݸ��� ����..

	tagColliderGroup( const _tchar* _pGroupName = nullptr, Engine::CCollider* _pCollider = nullptr )
		: pGroupName( _pGroupName )
		, pLayerName( nullptr )
	{
		if ( nullptr != _pCollider )
			Add_Collider( _pCollider );
	}

	void Add_Collider( Engine::CCollider* _pCollider = nullptr )
	{
		// �ݸ����� ���� ������ �ø��� ��� �ִ� �ݸ����� ��Ƴ���..
		Engine::CCollider**	ppTemp = new Engine::CCollider*[iColliderCount + 1];

		for ( _uint i = 0; i < iColliderCount; ++i )
		{
			ppTemp[i] = ppCollider[i];
		}

		ppTemp[iColliderCount] = _pCollider;

		// ���� �ִ� ������ �����ϰ� ��� ���� �ݸ����� �޾ƿ�..
		Engine::Safe_Delete_Array( ppCollider );

		ppCollider = ppTemp;

		++iColliderCount;
	}

	void Release( void );

} COLLIDERGROUP;

typedef struct tagColliderGroupData
{
	COLLIDERGROUP*			pGroup = nullptr;
	_uint					iGroupCount = 0;		// �ݸ��� ����..

	tagColliderGroupData()
		: pGroup( nullptr ), iGroupCount( 0 )
	{}

	void Add_ColliderGroup( const COLLIDERGROUP& _ColliderGroup )
	{
		// �ݸ��� �׷��� ���� ������ �ø��� ��� �ִ� �ݸ��� �׷��� ��Ƴ���..
		COLLIDERGROUP* pTemp = new COLLIDERGROUP[iGroupCount + 1];

		for ( _uint i = 0; i < iGroupCount; ++i )
		{
			pTemp[i] = pGroup[i];
		}

		pTemp[iGroupCount] = _ColliderGroup;

		// ���� �ִ� ������ �����ϰ� ��� ���� �ݸ����� �޾ƿ�..
		Engine::Safe_Delete_Array( pGroup );

		pGroup = pTemp;

		++iGroupCount;
	}

	void Release()
	{
		for ( _uint i = 0; i < iGroupCount; ++i )
		{
			pGroup[i].Release();
		}

		Engine::Safe_Delete_Array( pGroup );

		iGroupCount = 0;
	}

} COLLIDERGROUPDATA;
#endif

class DLL_EXPORT CCharacter abstract
	: public CGameObject
{
protected:
	explicit CCharacter(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCharacter() = default;

	/*FUNCTION*/
	//public:
	//CHAR_TYPE Get_CharType() { return m_tInfo.m_eCharType; }
public:
	virtual HRESULT Ready_GameObject() PURE;
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual _int	LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void	Render_GameObject() PURE;
	virtual void	Render_Shadow( LPD3DXEFFECT pEffect, const char* pConstantWorldMatrixName ) override;
	virtual void	Reset_WorldState();
public:
	// �ִϸ��̼� �̺�Ʈ�� �߻����� �� ȣ���� �Լ�..
	void Act_AnimationEvent( const ANI_STATUS& _AniStatus, const _uint& _iEventIndex );
public:
	// ��ȣ �ۿ� ���� �� ȣ��Ǵ� �Լ�( ��ȣ �ۿ��� �ϴ� ����� ��ȣ �ۿ��� ���ϴ� ����� �Լ��� ȣ������ )..
	void OnInteraction( const INTERACTIONINFO& _InteractionInfo, const _uint& _iInteractionIndex );
	_bool IsCanInteraction( void );
protected:
	HRESULT Ready_Character_Component();
	HRESULT Ready_LoadCollider( const _tchar* _pFilePath, Engine::CCollider* _pCollder );
	HRESULT Ready_CollidersToColliderManager( const _int& _iGroupType, Engine::CGameObject * _pGameObject );
	_bool	Colliders_Add_ColliderLayer( const _int& _iGroupType, const _tchar* _pLayerTag, const _uint& _iGroupIndex );
	_bool	Colliders_Exclude_ColliderLayer( const _int& _iGroupType, const _uint& _iGroupIndex );
protected:
	// �ݶ��̴��� Ű��� �ִϸ��̼� �̺�Ʈ�� �߻����� �� ȣ���ϴ� �Լ�( ���� �Լ��� ����� �ڽĿ��� ������ )..
	virtual HRESULT Event_ColliderActOn( const ANI_STATUS & _AniStatus, const _uint & _iEventIndex );
	HRESULT Event_ColliderActOff( const ANI_STATUS & _AniStatus, const _uint & _iEventIndex );
protected:
	void LookTargetLookDir( CCharacter* _pCharacter, const _float& _fOffset );
protected:
	void Acc_CurrentHp( const _uint& _iAccHp );
	void Acc_CurrentCp( const _float& _fAccCp );
	/*MEMBER*/
protected:
	CCollider_Manager*		m_pCollider_Manager = nullptr;	// Add By.KMH
protected:
	//CTransform*				m_pTransformCom = nullptr;
	CMesh_Dynamic*			m_pMeshCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
#ifdef CHARACTER_COLLIDERMODIFY
	COLLIDERGROUPDATA		m_ColliderGroups;
#else
	CCollider*				m_pColliderCom = nullptr;
#endif
	CNavigation*			m_pNaviCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CController*			m_pController = nullptr; // �����̷��� ��Ʈ�ѷ� ��� �ʼ�
	CCamera*				m_pCamera = nullptr;

															//vector<CAction>			m_vecAction;			// ĳ���� �׼� ����
															//CWorldState				m_WorldState;			// ĳ���� ���� ������Ʈ
															//int						m_StartAnimNum = 0;		// ĳ���� �ִϸ��̼� ���� �ѹ�
															//int						m_EndAniNum = 0;		// ĳ���� �ֹ̳��̼� ���� �ѹ�
	_tchar*					m_TrailEffectTag = nullptr;
	CHARINFO				m_tInfo;		// ĳ���� ����
public:
	//FORCEINLINE int	Get_StartAniNum() { return m_StartAnimNum; } // ĳ���� �׼� ���� ��ȯ
	//FORCEINLINE int	Get_EndAniNum() { return m_EndAniNum; } // ĳ���� �׼� ���� ��ȯ
	//FORCEINLINE const vector<CAction>*	Get_ActionVector() const { return &m_vecAction; } // ĳ���� �׼� ���� ��ȯ
	//FORCEINLINE vector<CAction>*		Get_ActionVector_Point() { return &m_vecAction; } // (��������_����Ʈ) ĳ���� �׼� ���� ��ȯ
	//FORCEINLINE vector<CAction>&		Get_ActionVector_Reference() { return m_vecAction; } // (��������_���۷���) ĳ���� �׼� ���� ��ȯ
	//FORCEINLINE const CWorldState&		Get_WorldState() const { return m_WorldState; } // ĳ���� ���彺����Ʈ ��ȯ
	//FORCEINLINE CWorldState&			Get_WorldState_Reference() { return m_WorldState; } // (��������_���۷���) ĳ���� ���彺����Ʈ ��ȯ

	FORCEINLINE const CHARINFO&			Get_Info() const { return m_tInfo; }		// ĳ���� ���� ��ȯ
	FORCEINLINE CController*			Get_Controller() { return m_pController; }
	//FORCEINLINE CTransform*				Get_Transform() { return m_pTransformCom; }
	FORCEINLINE CMesh_Dynamic*			Get_Mesh() { return m_pMeshCom; }
	FORCEINLINE CCamera*				Get_Camera() { return m_pCamera; }
	FORCEINLINE CHARINFO&				Get_Info_ForMapTool() { return m_tInfo; }		// ��� ����

public:
	void Set_Controller(CController* pController);// ��� ����
	HRESULT Set_Camera(CCamera* pCamera); // ��� ������ ��.
										  // --------- ��� ------------
private:
	vector<CEquipment*>	m_pEquipVector; //�Ϻη� private �س��� -JW
	int m_iItemCount = 0;
public:
	HRESULT Equip_Item(CEquipment* pItem,_matrix* pBoneMatrix);
	void	Hide_Item(bool bHide,int iIndex = -1); // true - ���� / false - ����
	CEquipment*	Get_Equipment(int iIndex = 0) { 
		if (m_pEquipVector.size() > iIndex)
			return m_pEquipVector[iIndex];
		else
			return nullptr;
	}
	FORCEINLINE int	Get_EquipmentSize() { return m_iItemCount; }
	// --------- ��� ------------


	// �ൿ �Լ�(�ʿ� �Լ��� ���� Ŭ�������� �����ε�)
public:
	virtual EStatus Move_Forward(float fScale = 1.f);
	virtual EStatus Move_Right(float fScale = 1.f);
	//virtual EStatus Move_Jump(float fScale = 1.f) PURE;
	//virtual EStatus Turn(float fScale = 1.f) PURE;

	//virtual EStatus	Change_Player();
	//virtual EStatus	Tag_Leo();
	//virtual EStatus	Tag_Mike();
	//virtual EStatus	Tag_Raph();
	//virtual EStatus	Tag_Don();


	//
	//virtual EStatus Mode_On_Stealth();
	//virtual EStatus Mode_Off_Stealth();
	//virtual EStatus Attack_Forward_Dash();

private:
	void  Move_Navigation();

	// Test  �����Ұ�
protected:
	_bool	m_bNavi = false;

public:
	friend void CController::Get_TargetInfo(CCharacter* pTarget);

#ifndef _FINAL
public:
	void	Debug_Render(void);
#endif
protected:
	virtual _ulong	Free() override;
};

END