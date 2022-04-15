#pragma once

#include "Defines.h"
#include "Character.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CPicking;
	class CMaterial;
	class CVIBuffer;
	class CMesh_Dynamic;
	class CCollider;
	class CNavigation;
	class CShader;
	//class CCharacter;
}


class CCamera_Player;

class CPlayer final : public CCharacter
{
	enum LeoColliderGroupKind
	{
		LeoColGroup_LWeapon,
		LeoColGroup_RWeapon,
		LeoColGroup_Hit,
		LeoColGroup_RFoot,
		LeoColGroup_LFoot,
		LeoColGroup_End
	};
	enum DonColliderGroupKind
	{
		DonColGroup_Hit,
		DonColGroup_RWeapon,
		DonColGroup_RFoot,
		DonColGroup_LFoot,
		DonColGroup_End
	};
	enum RaphColliderGroupKind
	{
		RaphColGroup_Hit,
		RaphColGroup_RWeapon,
		RaphColGroup_LWeapon,
		RaphColGroup_RFoot,
		RaphColGroup_LFoot, 
		RaphColGroup_RKnee,
		RaphColGroup_LKnee,
		RaphColGroup_End
	};
	enum MikeColliderGroupKind
	{
		MikeColGroup_Hit,
		MikeColGroup_RWeapon,
		MikeColGroup_LWeapon,
		MikeColGroup_RKnee,
		MikeColGroup_LKnee,
		MikeColGroup_End
	};

	enum HITANIINDEXNUM
	{
		HitIndex_Front,		// 앞에서 맞았을 때 나오는 애니메이션 인덱스..
		HitIndex_Right,		// 오른쪽에서 맞았을 때 나오는 애니메이션 인덱스..
		HitIndex_Back,		// 뒤에서 맞았을 때 나오는 애니메이션 인덱스..
		HitIndex_Left,		// 왼쪽에서 맞았을 때 나오는 애니메이션 인덱스..
		HitIndex_End
	};

	enum INTERACTIONKIND
	{
		Interaction_DonStart = -1,

		Interaction_DonFrontCounter1,
		Interaction_DonBackCounter1,
		Interaction_DonStrongAtk1,

		Interaction_DonEnd,

		Interaction_LeoStart = -1,

		Interaction_LeoFrontCounter1,
		Interaction_LeoBackCounter1,
		Interaction_LeoStrongAtk1,

		Interaction_LeoEnd,

		Interaction_MikeStart = -1,

		Interaction_MikeFrontCounter1,
		Interaction_MikeBackCounter1,
		Interaction_MikeStrongAtk1,

		Interaction_MikeEnd,

		Interaction_RaphStart = -1,

		Interaction_RaphFrontCounter1,
		Interaction_RaphBackCounter1,
		Interaction_RaphStrongAtk1,

		Interaction_RaphEnd,
	};
private:
	explicit CPlayer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Ready_GameObject() final;
	virtual _int Update_GameObject(const _float& fTimeDelta) final;
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta) final;
	virtual void Render_GameObject(void) final;
	// 충돌 시 호출되는 함수들..
	virtual void OnCollision( const Engine::Collision& _Collision ) override;
private:
	PLAYER_TYPE			m_eType = PLAYER_TYPE::PLAYER_None;
	_uint				m_iHitAniArr[HitIndex_End] = { 0 };	// 앞, 오른쪽, 뒤, 왼쪽 순으로 넣어줘야 함..
	INTERACTIONINFO		m_InteractionInfo;
private:
	HRESULT Ready_Component(void);
	HRESULT Ready_Donatello(void);
	HRESULT Ready_Leonardo(void);
	HRESULT Ready_Michelangelo(void);
	HRESULT Ready_Raphael(void);
	HRESULT Ready_Camera(void);
	HRESULT Ready_Controller(void);
	HRESULT Ready_LoadInteractionData( const _tchar* _pFilePath );
	HRESULT SetUp_OnConstantTable(LPD3DXEFFECT pEffect);
private:
	_uint Compute_HitAniIndex( const Engine::Collision& _Collision );
	_float ComputeAngle_HitTargetTransform( const Engine::Collision& _Collision );
	_float ComputeAngle_HitTargetCollider( const Engine::Collision& _Collision );
public:
	static CPlayer* Create(LPDIRECT3DDEVICE9 pGraphicDev, PLAYER_TYPE eType);

public:
	PLAYER_TYPE Get_Current_Type() { return m_eType; }
	//virtual EStatus Move_Forward(float fScale = 1.f);
	//virtual EStatus Move_Right(float fScale = 1.f);
#ifndef _FINAL
public:
	void Debug_Render(void);
#endif
protected:
	virtual _ulong Free(void) final;

};

