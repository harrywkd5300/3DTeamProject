#include "stdafx.h"
#include "..\Headers\SlimThug.h"

#include "Object_Manager.h"
#include "Component_Manager.h"
#include "Navigation.h"
#include "Graphic_Device.h"
#include "Collider_Manager.h"
#include "Random.h"

#include "Clinet_Component.h"
#include "Camera_Player.h"
#include "Monster_Default_Weapon.h"

CSlimThug::CSlimThug( LPDIRECT3DDEVICE9 pGraphicDev )
	: CMonster( pGraphicDev )
{
#ifndef _FINAL
	Tag( L"CSlimThug", std::bind( &CSlimThug::Debug_Render, this ) );
#endif

}


HRESULT CSlimThug::Ready_GameObject( void )
{
	if ( FAILED( this->Ready_Component() ) )
		return E_FAIL;

	// �������� ���⸦ �� �� �ְԲ� �Ѵ�..
	_uint iWeaponNum = CRandom::GetInstance()->GetValue( 2 );
	if ( FAILED( Equip_Item( CMonster_Default_Weapon::Create( m_pGraphicDev, (iWeaponNum == 0) ? CMonster_Default_Weapon::Bat : CMonster_Default_Weapon::Pipe ), m_pMeshCom->Find_FrameMatrixPointer( "SaberStaff_Root" ) ) ) )
		return E_FAIL;

	if ( FAILED( Ready_AI() ) )
		return E_FAIL;

	if ( FAILED( Ready_DefaultSetting() ) )
		return E_FAIL;

	return NOERROR;
}

_int CSlimThug::Update_GameObject( const _float & fTimeDelta )
{
	if ( true == m_bIsDead )
		return 1;

	int returnValue = CMonster::Update_GameObject( fTimeDelta );

	if ( nullptr != m_pRendererCom )
	{
		m_pRendererCom->Add_RenderGroup( Engine::CRenderer::RENDER_NONALPHA, this );
		m_pRendererCom->Add_ShadowGroup( this, *m_pTransformCom->Get_Infomation( Engine::CTransform::INFO_POSITION ), 3.f );
	}

	return returnValue;
}

_int CSlimThug::LastUpdate_GameObject( const _float & fTimeDelta )
{
	CMonster::LastUpdate_GameObject( fTimeDelta );

	//CComponent* pTransform_Sword = m_pRSword->Get_Component(L"Com_Transform");
	//if (nullptr == pTransform_Sword)
	//	return -1;

	//((CTransform*)pTransform_Sword)->Set_ParentMatrix(&(*m_pRHandMatrix * m_pTransformCom->Get_WorldMatrix()));

	//m_pRSword->LastUpdate_GameObject(fTimeDelta);

	return _int();
}

void CSlimThug::Render_GameObject( void )
{
	if ( nullptr == m_pMeshCom )
		return;

	if ( nullptr == m_pGraphicDev || nullptr == m_pShaderCom )
		return;

#ifndef _FINAL
#ifdef CHARACTER_COLLIDERMODIFY	// Add By.KMH - ��� �ݶ��̴��� ���� �ϴ� �ڵ� �߰�..
	for ( _uint i = 0; i < m_ColliderGroups.iGroupCount; ++i )
	{
		for ( _uint j = 0; j < m_ColliderGroups.pGroup[i].iColliderCount; ++j )
		{
			m_ColliderGroups.pGroup[i].ppCollider[j]->Render_Buffer();
		}
	}

	Render_DebugSphere();
#else
	m_pColliderCom->Render_Buffer();
#endif
#endif 

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if ( nullptr == pEffect )
		return;
	pEffect->AddRef();


	SetUp_OnConstantTable( pEffect );

	pEffect->Begin( nullptr, 0 );

	pEffect->BeginPass( 0 );

	m_pMeshCom->Render_Mesh( pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture" );

	pEffect->EndPass();

	pEffect->End();

	Safe_Release( pEffect );

	m_pNaviCom->Render_Navigation();

}

void CSlimThug::SignalUpdate(CCharacterGroup::CHARDATA* ptChar)
{
	_uint iCount = CRandom::GetInstance()->GetValue(0, 6);

	ptChar->fTime_Atk = 8.f + (float)iCount;
}

void CSlimThug::Move_SignalUpdate(CCharacterGroup::CHARDATA * ptChar)
{
	if (true == m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_TARGET))
	{
		_uint iCount = 0;

		while (true)
		{
			iCount = CRandom::GetInstance()->GetValue(0, 4);

			if (0 != iCount)
				break;
		}

		m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_FRONT + iCount, true);
	}
	else if (false == m_tInfo.m_WorldState.getVariable(CH_STATE_CLOSE_TARGET))
	{
		if (false == m_tInfo.m_WorldState.getVariable(CH_STATE_NEAR_TARGET))
		{
			_uint iCount = 0;

			while (true)
			{
				iCount = CRandom::GetInstance()->GetValue(0, 4);

				if (1 != iCount)
					break;
			}

			m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_FRONT + iCount, true);
		}
		else
		{
			_uint iCount = CRandom::GetInstance()->GetValue(0, 4);

			m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_FRONT + iCount, true);
		}

	}

	ptChar->fTime = 0.8f;
}

HRESULT CSlimThug::Ready_AI( void )
{
	m_tInfo.m_StartAniNum = ANI_SLIMTHUG_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_SLIMTHUG_TYPE_END;
	// %%%%%%%%%%%%%%%%%%%%%%%%%% ĳ���� ���� ������Ʈ ���� %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable( ACT_TYPE_START, ACT_TYPE_END, false );
	// ---- ANI_SLIMTHUG ----
	m_tInfo.m_WorldState.setVariable( ANI_SLIMTHUG_TYPE_START, ANI_SLIMTHUG_TYPE_END, false );
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable( SIGNAL_TYPE_START, SIGNAL_TYPE_END, false );
	m_tInfo.m_WorldState.setVariable( CH_STATE_TYPE_START, CH_STATE_TYPE_END, false );
	m_tInfo.m_WorldState.setVariable( TARGET_TYPE_START, TARGET_TYPE_END, false );

	m_tInfo.m_WorldState.setVariable( SIGNAL_CHANGE_ANIMATION, true );
	m_tInfo.m_WorldState.setVariable( ACT_IDLE, true );
	m_tInfo.m_WorldState.setVariable( CH_STATE_PEACE, true );
	m_tInfo.m_WorldState.setVariable( CH_STATE_FOUND_ENEMY, false );
	m_tInfo.m_WorldState.setVariable( TARGET_NEAR_ENEMY, true );


	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ñ׳� ó�� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- �ִϸ��̼� ���� ��ȯ -----
	CAction ACT_SIGNAL_CHANGE_ANIMATION_FORCED( "SIGNAL_CHANGE_ANIMATION_FORCED", 1 );
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition( SIGNAL_CHANGE_ANIMATION_FORCED, true );

	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect( SIGNAL_CHANGE_ANIMATION, false );
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, false );
	m_tInfo.m_vecAction_Ani.push_back( ACT_SIGNAL_CHANGE_ANIMATION_FORCED );
	//// ----- �ִϸ��̼� ���� ��ȯ -----
	//// ----- ���� ���� -----
	//CAction ACT_WALK_EXIT
	//// ----- ���� ���� -----
	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ñ׳� ó�� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ൿ ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%

	// %%%%%%%%%%%%%%%%%%%%%%%%%% ���� ���°� �ƴ� ��( ��ȭ ������ �� ).. %%%%%%%%%%%%%%%%%%%%%%%%%% //

	// �⺻ ����..
	{
		CAction AI_ACT_Idle( "AI_ACT_Idle", 300 );
		AI_ACT_Idle.setPrecondition( TARGET_NONE, true );
		AI_ACT_Idle.setPrecondition( CH_STATE_ATTACK, false );
		AI_ACT_Idle.setPrecondition( CH_STATE_HIT, false );
		AI_ACT_Idle.setPrecondition( CH_STATE_DIE, false );

		AI_ACT_Idle.setEffect( CH_STATE_PEACE, true );
		AI_ACT_Idle.setEffect( ACT_IDLE, true );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_Idle );
	}

	// ���� �ָ� ���� ��..
	{
		CAction AI_ACT_GoTo_Target( "AI_ACT_GoTo_Enemy", 2000 );
		AI_ACT_GoTo_Target.setPrecondition( TARGET_NONE, false );
		AI_ACT_GoTo_Target.setPrecondition( CH_STATE_MID_TARGET, false );
		AI_ACT_GoTo_Target.setPrecondition( CH_STATE_NEAR_TARGET, false );
		AI_ACT_GoTo_Target.setPrecondition( CH_STATE_CLOSE_TARGET, false );
		AI_ACT_GoTo_Target.setPrecondition( CH_STATE_HIT, false );
		AI_ACT_GoTo_Target.setPrecondition( CH_STATE_DIE, false );

		AI_ACT_GoTo_Target.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		AI_ACT_GoTo_Target.setEffect( SIGNAL_CHANGE_ANGLE, true );
		AI_ACT_GoTo_Target.setEffect( SIGNAL_CHANGE_DIR_FRONT, true );
		AI_ACT_GoTo_Target.setEffect( ACT_RUN, true );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_GoTo_Target );
	}

	// ���� �� �ɵ��� ���� ���̶��..
	{
		CAction AI_ACT_MoveAround_Target( "AI_ACT_MoveAround_Target", 2000 );
		AI_ACT_MoveAround_Target.setPrecondition( TARGET_NONE, false );
		AI_ACT_MoveAround_Target.setPrecondition( CH_STATE_NEAR_TARGET, true );
		AI_ACT_MoveAround_Target.setPrecondition( CH_STATE_ATTACK, false );
		AI_ACT_MoveAround_Target.setPrecondition( CH_STATE_HIT, false );
		AI_ACT_MoveAround_Target.setPrecondition( CH_STATE_DIE, false );

		AI_ACT_MoveAround_Target.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		AI_ACT_MoveAround_Target.setEffect( SIGNAL_CHANGE_ANGLE, true );
		AI_ACT_MoveAround_Target.setEffect( ACT_WALK, true );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_MoveAround_Target );
	}

	// ���� �����϶�� ��ȣ�� �޾��� �� ( ������ ���� ���� �ۿ� ���� ��, ������ �� )..
	{
		CAction AI_ACT_MoveTo_Target( "AI_ACT_MoveTo_Target", 1999 );
		AI_ACT_MoveTo_Target.setPrecondition( TARGET_NONE, false );
		AI_ACT_MoveTo_Target.setPrecondition( CH_STATE_CLOSE_TARGET, false );
		AI_ACT_MoveTo_Target.setPrecondition( SIGNAL_ATTACK_TARGET, true );
		AI_ACT_MoveTo_Target.setPrecondition( CH_STATE_ATTACK, false );
		AI_ACT_MoveTo_Target.setPrecondition( CH_STATE_HIT, false );
		AI_ACT_MoveTo_Target.setPrecondition( CH_STATE_DIE, false );

		AI_ACT_MoveTo_Target.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		AI_ACT_MoveTo_Target.setEffect( SIGNAL_CHANGE_ANGLE, true );
		AI_ACT_MoveTo_Target.setEffect( SIGNAL_CHANGE_DIR_FRONT, true );
		AI_ACT_MoveTo_Target.setEffect( ACT_WALK, true );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_MoveTo_Target );
	}

	// ���� �����϶�� ��ȣ�� �޾��� �� ( ���� ���� �ȿ� ���� ��, ���� �Ѵ� )..
	{
		CAction AI_ACT_Attack_Target( "AI_ACT_Attack_Target", 2000 );
		AI_ACT_Attack_Target.setPrecondition( TARGET_NONE, false );
		AI_ACT_Attack_Target.setPrecondition( CH_STATE_CLOSE_TARGET, true );
		AI_ACT_Attack_Target.setPrecondition( SIGNAL_ATTACK_TARGET, true );
		AI_ACT_Attack_Target.setPrecondition( CH_STATE_ATTACK, false );
		AI_ACT_Attack_Target.setPrecondition( CH_STATE_HIT, false );
		AI_ACT_Attack_Target.setPrecondition( CH_STATE_DIE, false );

		AI_ACT_Attack_Target.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		AI_ACT_Attack_Target.setEffect( SIGNAL_CHANGE_ANGLE, true );
		AI_ACT_Attack_Target.setEffect( ACT_ATTACK, true );
		AI_ACT_Attack_Target.setEffect( SIGNAL_ATTACK_TARGET, false );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_Attack_Target );
	}

	// ������ �¾Ҵ�..
	{
		CAction AI_ACT_Hit_Target( "AI_ACT_Attack_Target", 500 );
		AI_ACT_Hit_Target.setPrecondition( SIGNAL_HIT_ENEMY, true );
		AI_ACT_Hit_Target.setPrecondition( CH_STATE_HIT, false );
		AI_ACT_Hit_Target.setPrecondition( CH_STATE_DIE, false );

		AI_ACT_Hit_Target.setEffect( SIGNAL_HIT_ENEMY, false );
		AI_ACT_Hit_Target.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		AI_ACT_Hit_Target.setEffect( SIGNAL_CHANGE_ANGLE, true );
		AI_ACT_Hit_Target.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		AI_ACT_Hit_Target.setEffect( ACT_HIT, true );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_Hit_Target );
	}

	// ��ȣ�ۿ��� �ϰڴ�( ī����, ������ � ���� )..
	{
		CAction AI_ACT_INTERACTION( "AI_ACT_INTERACTION", 499 );
		AI_ACT_INTERACTION.setPrecondition( SIGNAL_ACT_INTERACTION, true );

		AI_ACT_INTERACTION.setEffect( SIGNAL_ACT_INTERACTION, false );
		AI_ACT_INTERACTION.setEffect( ACT_INTERACTION, true );
		AI_ACT_INTERACTION.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		m_tInfo.m_vecAction_Act.push_back( AI_ACT_INTERACTION );
	}

	// �׾���..
	{
		//CAction AI_ACT_Die( "AI_ACT_Die", 100 );
		//AI_ACT_Die.setPrecondition( SIGNAL_DIE, true );
		//AI_ACT_Die.setPrecondition( CH_STATE_DIE, false );
		//
		//AI_ACT_Die.setEffect( SIGNAL_DIE, false );
		//AI_ACT_Die.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		//AI_ACT_Die.setEffect( SIGNAL_CHANGE_ANGLE, true );
		//AI_ACT_Die.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		//AI_ACT_Die.setEffect( ACT_DEATH, true );
		//m_tInfo.m_vecAction_Act.push_back( AI_ACT_Die );
	}

	//// ���� ã��..
	//{
	//	CAction AI_ACT_Found_Target( "AI_ACT_Found_Target", 2000 );
	//	AI_ACT_Found_Target.setPrecondition( TARGET_NONE, true );
	//
	//	AI_ACT_Found_Target.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_Found_Target.setEffect( ACT_IDLE, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_Found_Target );
	//}
	//
	//
	//// ��Ʋ ���¿��� Ÿ���� ã��??
	//{
	//	CAction AI_ACT_Found_Target_Battle( "AI_ACT_Found_Target_Battle", 1900 );
	//	AI_ACT_Found_Target_Battle.setPrecondition( TARGET_NONE, true );
	//	AI_ACT_Found_Target_Battle.setPrecondition( CH_STATE_BATTLE, true );
	//
	//	AI_ACT_Found_Target_Battle.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_Found_Target_Battle.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_Found_Target_Battle.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_Found_Target_Battle.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
	//	AI_ACT_Found_Target_Battle.setEffect( ACT_IDLE, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_Found_Target_Battle );
	//}
	//
	//
	//// Ÿ���� ���� �޷���..
	//{
	//	CAction AI_ACT_RUN_TO_ENEMY( "AI_ACT_RUN_TO_ENEMY", 200 );
	//	AI_ACT_RUN_TO_ENEMY.setPrecondition( TARGET_EXTERNAL, true );
	//	//AI_ACT_RUN_TO_ENEMY.setPrecondition( TARGET_NONE, false );
	//	//AI_ACT_RUN_TO_ENEMY.setPrecondition( CH_STATE_FOUND_ENEMY, true );
	//	AI_ACT_RUN_TO_ENEMY.setPrecondition( CH_STATE_MID_TARGET, false );
	//
	//	//AI_ACT_RUN_TO_ENEMY.setEffect(CH_STATE_PEACE, true);
	//	AI_ACT_RUN_TO_ENEMY.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_RUN_TO_ENEMY.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_RUN_TO_ENEMY.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_RUN_TO_ENEMY.setEffect( SIGNAL_CHANGE_DIR_FRONT, true );
	//	AI_ACT_RUN_TO_ENEMY.setEffect( ACT_RUN, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_RUN_TO_ENEMY );
	//}
	//
	//
	//// ���� �ൿ..
	//{
	//	CAction AI_ACT_BLOCK_ATTACK( "AI_ACT_BLOCK_ATTACK", 200 );
	//	AI_ACT_BLOCK_ATTACK.setPrecondition( SIGNAL_HIT_ENEMY, true );
	//
	//	//AI_ACT_RUN_TO_ENEMY.setEffect(CH_STATE_PEACE, true);
	//	AI_ACT_BLOCK_ATTACK.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_BLOCK_ATTACK.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_BLOCK_ATTACK.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_BLOCK_ATTACK.setEffect( ACT_HIT, true );
	//	AI_ACT_BLOCK_ATTACK.setEffect( SIGNAL_HIT_ENEMY, false );
	//
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_BLOCK_ATTACK );
	//}
	//
	//
	//// �� ������ �ɵ�..
	//{
	//	CAction AI_ACT_AROUND_TARGET( "AI_ACT_AROUND_TARGET", 200 );
	//	AI_ACT_AROUND_TARGET.setPrecondition( CH_STATE_FOUND_ENEMY, true );
	//	AI_ACT_AROUND_TARGET.setPrecondition( CH_STATE_MID_TARGET, true );
	//	//AI_ACT_AROUND_TARGET.setPrecondition( SIGNAL_ATTACK_TARGET, true );
	//
	//	AI_ACT_AROUND_TARGET.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_AROUND_TARGET.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_AROUND_TARGET.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_AROUND_TARGET.setEffect( SIGNAL_CHANGE_DIR_RIGHT, true );
	//	AI_ACT_AROUND_TARGET.setEffect( ACT_RUN, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_AROUND_TARGET );
	//}
	//
	//
	//// �� ����..
	//{
	//	CAction AI_ACT_ATTACK_TO_ENEMY( "AI_ACT_ATTACK_TO_ENEMY", 200 );
	//	AI_ACT_ATTACK_TO_ENEMY.setPrecondition( SIGNAL_ATTACK_TARGET, true );
	//	AI_ACT_ATTACK_TO_ENEMY.setPrecondition( CH_STATE_CLOSE_TARGET, true );
	//	AI_ACT_ATTACK_TO_ENEMY.setPrecondition( ACT_ATTACK, false );
	//
	//	AI_ACT_ATTACK_TO_ENEMY.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_ATTACK_TO_ENEMY.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_ATTACK_TO_ENEMY.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_ATTACK_TO_ENEMY.setEffect( ACT_ATTACK, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_ATTACK_TO_ENEMY );
	//}
	//
	//
	//// ���� ���� �ٰ���( ���� �ϱ� ���ؼ� )..
	//{
	//	CAction AI_ACT_GO_TO_ENEMY( "AI_ACT_GO_TO_ENEMY", 200 );
	//	AI_ACT_GO_TO_ENEMY.setPrecondition( SIGNAL_ATTACK_TARGET, true );
	//	AI_ACT_GO_TO_ENEMY.setPrecondition( CH_STATE_CLOSE_TARGET, false );
	//
	//	AI_ACT_GO_TO_ENEMY.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
	//	AI_ACT_GO_TO_ENEMY.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_GO_TO_ENEMY.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_GO_TO_ENEMY.setEffect( ACT_RUN, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_GO_TO_ENEMY );
	//}
	//
	//
	//// ���ݿ��� �ٽ� Idle ��..
	//{
	//	CAction AI_ACT_ATTACK_TO_IDLE( "AI_ACT_ATTACK_TO_IDLE", 200 );
	//	AI_ACT_ATTACK_TO_IDLE.setPrecondition( SIGNAL_ATTACK_TARGET, true );
	//	AI_ACT_ATTACK_TO_IDLE.setPrecondition( ACT_ATTACK, true );
	//	
	//	AI_ACT_ATTACK_TO_IDLE.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, false );
	//	AI_ACT_ATTACK_TO_IDLE.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
	//	AI_ACT_ATTACK_TO_IDLE.setEffect( SIGNAL_CHANGE_ANGLE, true );
	//	AI_ACT_ATTACK_TO_IDLE.setEffect( SIGNAL_ATTACK_TARGET, false );
	//	AI_ACT_ATTACK_TO_IDLE.setEffect( ACT_ATTACK, false );
	//	AI_ACT_ATTACK_TO_IDLE.setEffect( ACT_IDLE, true );
	//	m_tInfo.m_vecAction_Act.push_back( AI_ACT_ATTACK_TO_IDLE );
	//}

	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ൿ ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%
	
	
	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ִϸ��̼� ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%
	
	// ----- �⺻ ��� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_BigT_Idle_b( "ACT_ANI_SLIMTHUG_BigT_Idle_b", 200 );
		ACT_ANI_SLIMTHUG_BigT_Idle_b.setPrecondition( ACT_IDLE, true );
		ACT_ANI_SLIMTHUG_BigT_Idle_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );

		ACT_ANI_SLIMTHUG_BigT_Idle_b.setEffect( CH_STATE_IDLE, true );
		ACT_ANI_SLIMTHUG_BigT_Idle_b.setEffect( ANI_SLIMTHUG_BigT_Idle_b, true );
		ACT_ANI_SLIMTHUG_BigT_Idle_b.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_SLIMTHUG_BigT_Idle_b.setEffect( SIGNAL_CHANGE_ANGLE, false );
		ACT_ANI_SLIMTHUG_BigT_Idle_b.setEffect( CH_STATE_IDLE, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_BigT_Idle_b );
	}

	// ----- �ȱ�( Front ).. -----
	{
		CAction ACT_ANI_SLIMTHUG_BigT_Walk_F_b( "ACT_ANI_SLIMTHUG_BigT_Walk_F_b", 90 );
		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setPrecondition( ACT_WALK, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setPrecondition( MOVE_DIR_FRONT, true );

		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setEffect( ANI_SLIMTHUG_BigT_Walk_F_b, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setEffect( SIGNAL_CHANGE_ANGLE, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_F_b.setEffect( CH_STATE_WALK, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_BigT_Walk_F_b );
	}

	// ----- �ȱ�( Back ).. -----
	{
		CAction ACT_ANI_SLIMTHUG_BigT_Walk_B_b( "ACT_ANI_SLIMTHUG_BigT_Walk_B_b", 90 );
		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setPrecondition( ACT_WALK, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setPrecondition( MOVE_DIR_BACK, true );

		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setEffect( ANI_SLIMTHUG_BigT_Walk_B_b, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setEffect( SIGNAL_CHANGE_ANGLE, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_B_b.setEffect( CH_STATE_WALK, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_BigT_Walk_B_b );
	}

	// ----- �ȱ�( Left ).. -----
	{
		CAction ACT_ANI_SLIMTHUG_BigT_Walk_L_b( "ACT_ANI_SLIMTHUG_BigT_Walk_L_b", 90 );
		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setPrecondition( ACT_WALK, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setPrecondition( MOVE_DIR_LEFT, true );

		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setEffect( ANI_SLIMTHUG_BigT_Walk_L_b, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setEffect( SIGNAL_CHANGE_ANGLE, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_L_b.setEffect( CH_STATE_WALK, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_BigT_Walk_L_b );
	}

	// ----- �ȱ�( Right ).. -----
	{
		CAction ACT_ANI_SLIMTHUG_BigT_Walk_R_b( "ACT_ANI_SLIMTHUG_BigT_Walk_R_b", 90 );
		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setPrecondition( ACT_WALK, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setPrecondition( MOVE_DIR_RIGHT, true );

		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setEffect( ANI_SLIMTHUG_BigT_Walk_R_b, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setEffect( SIGNAL_CHANGE_ANGLE, true );
		ACT_ANI_SLIMTHUG_BigT_Walk_R_b.setEffect( CH_STATE_WALK, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_BigT_Walk_R_b );
	}

	// ----- �޸��� -----
	{
		CAction ACT_ANI_SLIMTHUG_Run_F( "ANI_SLIMTHUG_Run_F", 90 );
		ACT_ANI_SLIMTHUG_Run_F.setPrecondition( ACT_RUN, true );
		ACT_ANI_SLIMTHUG_Run_F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		//ACT_ANI_SLIMTHUG_Run_F.setPrecondition( SIGNAL_CHANGE_DIR_FRONT, true );

		ACT_ANI_SLIMTHUG_Run_F.setEffect( ANI_SLIMTHUG_Run_F, true );
		ACT_ANI_SLIMTHUG_Run_F.setEffect( SIGNAL_CHANGE_ANIMATION_FORCED, true );
		ACT_ANI_SLIMTHUG_Run_F.setEffect( SIGNAL_CHANGE_ANGLE, true );
		ACT_ANI_SLIMTHUG_Run_F.setEffect( CH_STATE_RUN, true );
		//ACT_ANI_SLIMTHUG_Run_F.setEffect( SIGNAL_CHANGE_DIR_FRONT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Run_F );
	}

	 

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ----- �⺻ ����1 -----
	{
		CAction ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x( "ANI_SLIMTHUG_BigT_Melee_01_b_x", 80 );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setPrecondition( ACT_ATTACK, true );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );

		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setEffect( ANI_SLIMTHUG_BigT_Melee_01_b_x, true );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setEffect( SIGNAL_SET_ANGLE_TARGET, true );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setEffect( SIGNAL_CHANGE_ANGLE, true );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setEffect( SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true );
		ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x.setEffect( CH_STATE_ATTACK, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_BigT_Melee_01_b_x );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DEFENSE ~~~~~~~~~~~~~~~~~~~~~~

	// ----- �⺻ ���1 -----
	{
		CAction ACT_ANI_SLIMTHUG_Block_F_R2R( "ANI_SLIMTHUG_Block_F_R2R", 80 );
		ACT_ANI_SLIMTHUG_Block_F_R2R.setPrecondition( SIGNAL_SHIELD_ON, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );

		ACT_ANI_SLIMTHUG_Block_F_R2R.setEffect( ANI_SLIMTHUG_Block_F_R2R, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Block_F_R2R );
	}

	// ----- �⺻ ���2 -----
	{
		CAction ACT_ANI_SLIMTHUG_Block_L_R2R( "ANI_SLIMTHUG_Block_L_R2R", 79 );
		ACT_ANI_SLIMTHUG_Block_L_R2R.setPrecondition( SIGNAL_SHIELD_ON, true );
		ACT_ANI_SLIMTHUG_Block_L_R2R.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Block_L_R2R.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Block_L_R2R.setPrecondition( ANI_KNIFETHUG_Block_F_R2R, true );

		ACT_ANI_SLIMTHUG_Block_L_R2R.setEffect( ANI_SLIMTHUG_Block_L_R2R, true );
		ACT_ANI_SLIMTHUG_Block_L_R2R.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Block_L_R2R );
	}

	// ----- �⺻ ���3 -----
	{
		CAction ACT_ANI_SLIMTHUG_Block_R_R2R_b( "ANI_SLIMTHUG_Block_R_R2R_b", 78 );
		ACT_ANI_SLIMTHUG_Block_R_R2R_b.setPrecondition( SIGNAL_SHIELD_ON, true );
		ACT_ANI_SLIMTHUG_Block_R_R2R_b.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Block_R_R2R_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Block_R_R2R_b.setPrecondition( ANI_BIGTHUG_Block_L_R2R, true );

		ACT_ANI_SLIMTHUG_Block_R_R2R_b.setEffect( ANI_SLIMTHUG_Block_R_R2R_b, true );
		ACT_ANI_SLIMTHUG_Block_R_R2R_b.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Block_R_R2R_b );
	}

	// ----- �⺻ ��� ���� -----
	{
		CAction ACT_ANI_SLIMTHUG_Block_F_R2R_01( "ANI_SLIMTHUG_Block_F_R2R_01", 77 );
		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setPrecondition( SIGNAL_SHIELD_ON, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setPrecondition( SIGNAL_SHIELD_BREAK_ON, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );

		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setEffect( SIGNAL_SHIELD_BREAK_ON, false );
		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setEffect( ANI_SLIMTHUG_Block_F_R2R_01, true );
		ACT_ANI_SLIMTHUG_Block_F_R2R_01.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Block_F_R2R_01 );
	}


	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DEFENSE ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	// ----- �⺻ ��Ʈ1 -----
	{
		CAction ACT_ANI_SLIMTHUG_Dmg_F_R01( "ANI_SLIMTHUG_Dmg_F_R01", 80 );
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setPrecondition( SIGNAL_SHIELD_ON, false );
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setPrecondition( ANI_SLIMTHUG_Dmg_F_R01, true );
		
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setEffect( ANI_SLIMTHUG_Dmg_F_R01, true );
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Dmg_F_R01.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Dmg_F_R01 );
	}

	// ----- �⺻ ��Ʈ2 -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Don_B( "ANI_SLIMTHUG_Countered_Don_B", 80 );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( SIGNAL_SHIELD_ON, false );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( ANI_SLIMTHUG_Countered_Don_B, true );

		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( ANI_SLIMTHUG_Countered_Don_B, true );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Don_B );
	}

	// ----- �⺻ ��Ʈ3 -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Don_F2F( "ANI_SLIMTHUG_Countered_Don_F2F", 80 );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( SIGNAL_SHIELD_ON, false );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( ACT_HIT, true );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( ANI_SLIMTHUG_Countered_Don_F2F, true );

		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( ANI_SLIMTHUG_Countered_Don_F2F, true );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( CH_STATE_HIT, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Don_F2F );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_INTERACTION ~~~~~~~~~~~~~~~~~~~~~~

	// ----- Donatello �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Don_F2F( "ANI_SLIMTHUG_Countered_Don_F2F", 80 );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_DONFRONT1, true );

		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( ANI_SLIMTHUG_Countered_Don_F2F, true );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( SIGNAL_INTERACTION_COUNTER_DONFRONT1, false );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Don_F2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Don_F2F );
	}

	// ----- Donatello �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Don_B( "ANI_SLIMTHUG_Countered_Don_B", 80 );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setPrecondition( SIGNAL_INTERACTION_COUNTER_DONBACK1, true );

		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( ANI_SLIMTHUG_Countered_Don_B, true );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( SIGNAL_INTERACTION_COUNTER_DONBACK1, false );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Don_B.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Don_B );
	}

	// ----- Leonard �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Leo_F2B( "ANI_SLIMTHUG_Countered_Leo_F2B", 80 );
		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setPrecondition( SIGNAL_INTERACTION_COUNTER_LEOFRONT1, true );

		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setEffect( ANI_SLIMTHUG_Countered_Leo_F2B, true );
		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setEffect( SIGNAL_INTERACTION_COUNTER_LEOFRONT1, false );
		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Leo_F2B.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Leo_F2B );
	}

	// ----- Leonard �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Leo_B2F( "ANI_SLIMTHUG_Countered_Leo_B2F", 80 );
		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_LEOBACK1, true );

		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setEffect( ANI_SLIMTHUG_Countered_Leo_B2F, true );
		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setEffect( SIGNAL_INTERACTION_COUNTER_LEOBACK1, false );
		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Leo_B2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Leo_B2F );
	}

	// ----- Michelangelo �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Mike_F2F( "ANI_SLIMTHUG_Countered_Mike_F2F", 80 );
		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setPrecondition( SIGNAL_INTERACTION_COUNTER_MIKEFRONT1, true );

		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setEffect( ANI_SLIMTHUG_Countered_Mike_F2F, true );
		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setEffect( SIGNAL_INTERACTION_COUNTER_MIKEFRONT1, false );
		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Mike_F2F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Mike_F2F );
	}

	// ----- Michelangelo �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Mike_B( "ANI_SLIMTHUG_Countered_Mike_B", 80 );
		ACT_ANI_SLIMTHUG_Countered_Mike_B.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Mike_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Mike_B.setPrecondition( SIGNAL_INTERACTION_COUNTER_MIKEBACK1, true );

		ACT_ANI_SLIMTHUG_Countered_Mike_B.setEffect( ANI_SLIMTHUG_Countered_Mike_B, true );
		ACT_ANI_SLIMTHUG_Countered_Mike_B.setEffect( SIGNAL_INTERACTION_COUNTER_MIKEBACK1, false );
		ACT_ANI_SLIMTHUG_Countered_Mike_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Mike_B.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Mike_B );
	}

	// ----- Raphael �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Raph_Push_F( "ANI_SLIMTHUG_Countered_Raph_Push_F", 80 );
		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setPrecondition( SIGNAL_INTERACTION_COUNTER_RAPHFRONT1, true );

		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setEffect( ANI_SLIMTHUG_Countered_Raph_Push_F, true );
		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setEffect( SIGNAL_INTERACTION_COUNTER_RAPHFRONT1, false );
		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Raph_Push_F.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Raph_Push_F );
	}

	// ----- Raphael �� �� ī���� ����.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Countered_Push_Raph_B( "ANI_SLIMTHUG_Countered_Push_Raph_B", 80 );
		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setPrecondition( ACT_INTERACTION, true );
		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setPrecondition( SIGNAL_INTERACTION_COUNTER_RAPHBACK1, true );

		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setEffect( ANI_SLIMTHUG_Countered_Push_Raph_B, true );
		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setEffect( SIGNAL_INTERACTION_COUNTER_RAPHBACK1, false );
		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Countered_Push_Raph_B.setEffect( CH_STATE_INTERACTION, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Countered_Push_Raph_B );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_INTERACTION ~~~~~~~~~~~~~~~~~~~~~~

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DIE ~~~~~~~~~~~~~~~~~~~~~~

	// ----- �ڷ� ���ư�( �¾Ƽ� ).. -----
	{
		CAction ACT_ANI_SLIMTHUG_Kndown_Gut_x( "ANI_SLIMTHUG_Kndown_Gut_x", 80 );
		ACT_ANI_SLIMTHUG_Kndown_Gut_x.setPrecondition( ACT_DEATH, true );
		ACT_ANI_SLIMTHUG_Kndown_Gut_x.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );

		ACT_ANI_SLIMTHUG_Kndown_Gut_x.setEffect( ANI_SLIMTHUG_Kndown_Gut_x, true );
		ACT_ANI_SLIMTHUG_Kndown_Gut_x.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Kndown_Gut_x.setEffect( CH_STATE_DIE, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Kndown_Gut_x );
	}

	// ----- �ڷ� ����1.. -----
	{
		CAction ACT_ANI_SLIMTHUG_Kndown_Loop_b( "ANI_SLIMTHUG_Kndown_Loop_b", 79 );
		ACT_ANI_SLIMTHUG_Kndown_Loop_b.setPrecondition( ACT_DEATH, true );
		ACT_ANI_SLIMTHUG_Kndown_Loop_b.setPrecondition( SIGNAL_CHANGE_ANIMATION, true );
		ACT_ANI_SLIMTHUG_Kndown_Loop_b.setPrecondition( ANI_SLIMTHUG_Kndown_Gut_x, true );

		ACT_ANI_SLIMTHUG_Kndown_Loop_b.setEffect( ANI_SLIMTHUG_Kndown_Loop_b, true );
		ACT_ANI_SLIMTHUG_Kndown_Loop_b.setEffect( SIGNAL_CHANGE_ANIMATION, false );
		ACT_ANI_SLIMTHUG_Kndown_Loop_b.setEffect( CH_STATE_DIE, true );
		m_tInfo.m_vecAction_Ani.push_back( ACT_ANI_SLIMTHUG_Kndown_Loop_b );
	}

	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DIE ~~~~~~~~~~~~~~~~~~~~~~

	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ִϸ��̼� ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%

	return NOERROR;

}

HRESULT CSlimThug::Ready_Component( void )
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CMesh_Dynamic_SlimThug" );
	if ( nullptr == pComponent )
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert( MAPCOMPONENT::value_type( L"Com_Mesh", pComponent ) );
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame( "Origin" );
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_SLIMTHUG_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_SLIMTHUG_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_SLIMTHUG_TYPE_END;

	if ( FAILED( CMonster::Ready_Monster_Component() ) )
		return E_FAIL;


#ifdef CHARACTER_COLLIDERMODIFY
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CCollider" );
	if ( nullptr == pCollider )
		return E_FAIL;
	Ready_LoadCollider( L"../Bin/Data/ColliderData/Slim_Collider.Colldat", pCollider );
	Engine::Safe_Release( pCollider );

	/////////////////////////////// �ݸ��� �׷� ���� �Ŵ����� ���.. ///////////////////////////////
	// �ݸ����� ����� ���� ���� ���� �ݸ����� �÷��̾�, ���� ���� ���� ���̾ ����ؾߵǿ�..
	// �浹 or ��Ÿ ����� �ݸ����� �����Ǵ� ��ü �� �ϳ� ��..

	if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
		return E_FAIL;

	_tchar szLayerName[256] = L"";
	swprintf_s( szLayerName, L"Layer_SlimThug_Hit%d", GetKey() );
	const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr( szLayerName );

	if ( false == Coiiders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, SlimThugColGroup_Hit ) )
		return E_FAIL;
	if ( false == Coiiders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", SlimThugColGroup_Attack ) )
		return E_FAIL;

	// -------------- Hit �� �ݸ����� Attack �� �ݸ����� ������ �ʿ� ���� ������ �������� ���ϰ�.. -------------- //
	m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Monster_Attack", pLayerName );
	m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Player_Attack", L"Layer_Monster_Attack" );
	m_tInfo.m_iMoveColliderGroupCount = SlimThugColGroup_Hit;	// Hit �� �ݸ����� ĳ������ ���꿡 ���� ���ŵ� �� �ֵ��� �� ��° �׷����� ����..

	/////////////////////////////// �ݸ��� �׷� ���� �Ŵ����� ��� ��.. ///////////////////////////////


#else
	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component( SCENE_STAGE, L"CCollider" );
	if ( nullptr == pComponent )
		return E_FAIL;

	CCollider::RESIZEDATA Data;

	//Data.fOriginalRadius = 70.f;
	Data.pTransformCom = m_pTransformCom;
	Data.pParent = m_pMeshCom->Find_FrameMatrixPointer( "spine01" );
	Data.vOriginalMin = _vec3( -70.f, -70.f, -70.f );
	Data.vOriginalMax = _vec3( 70.f, 70.f, 70.f );


	m_pColliderCom->ReSizing( CCollider::TYPE_SPHERE, Data );
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert( MAPCOMPONENT::value_type( L"Com_Collider", pComponent ) );
	m_pColliderCom->AddRef();


	// Add By.KMH
	m_pColliderCom->Modify_StaticOption( CCollider::STATICOPTIONDATA( PushOption_BePushedOut ), StaticOption_Push );
	CCollider_Manager::GetInstance()->Add_Collider( Engine::CCollider_Manager::GROUP_STATIC, L"Group_KnifeThug", m_pColliderCom, this );
	CCollider_Manager::GetInstance()->Add_ColliderGroup( Engine::CCollider_Manager::GROUP_STATIC, L"Layer_KnifeThug", L"Group_KnifeThug" );
#endif

	return NOERROR;
}

HRESULT CSlimThug::Ready_DefaultSetting( void )
{
	// Info ������ �ʱⰪ ����..
	m_tInfo.m_fSpeed = 2.f;

	// ĳ������ Range �� ����..
	m_tInfo.m_fRange_Close = 1.5f;
	m_tInfo.m_fRange_Near = 4.f;
	m_tInfo.m_fRange_Mid = 4.5f;
	m_tInfo.m_fRange_Far = 20.f;
	m_tInfo.m_fRange_TooFar = 40.f;
	m_tInfo.m_fRange_FoundEnemy = 20.f;

	m_tInfo.m_iMaxHp = m_tInfo.m_iCurHp = 80;


	// ���� ������ ����..
	m_tInfo.m_iShield_Point = 30;
	m_tInfo.m_bShield = true;

	m_tInfo.m_fEvade_Point = 0.f;
	m_tInfo.m_bEvade = false;


	//���� �ƽ���
	m_iShieldMax = 30;

	//�ʴ� ������ �� ��ġ
	m_iAdd_Shield_Point = 0;
	m_fAdd_Evade_Point = 0.f;


	// �÷��̾��� Ʈ������ �ʱⰪ ����..
	//m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_POSITION, &_vec3( 130.2f, 0.f, 120.0f ) );
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_SCALE, &_vec3( 0.01f, 0.01f, 0.01f ) );

	// �������� ��ġ�� ��������(�׽�Ʈ��)..
	_vec3 vPos( CRandom::GetInstance()->GetValueF( 100.f, 130.f ), 0.f, CRandom::GetInstance()->GetValueF( 90.f, 120.f ) );
	m_pTransformCom->Set_Infomation( Engine::CTransform::INFO_POSITION, &vPos );


	// �ִϸ��̼� �ʱⰪ ����..
	m_pMeshCom->Set_AnimationSet( 0 );
	m_pMeshCom->Play_Animation( 0.f );

	return NOERROR;
}


HRESULT CSlimThug::SetUp_OnConstantTable( LPD3DXEFFECT pEffect )
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");
	//m_pTransformCom->SetUp_OnShader_FORCED( pEffect, "g_matWorld" );

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform( D3DTS_VIEW, &matView );
	m_pGraphicDev->GetTransform( D3DTS_PROJECTION, &matProj );

	pEffect->SetMatrix( "g_matView", &matView );
	pEffect->SetMatrix( "g_matProj", &matProj );

	Safe_Release( pEffect );

	return NOERROR;
}


CSlimThug * CSlimThug::Create( LPDIRECT3DDEVICE9 pGraphicDev )
{
	CSlimThug *	pInstance = new CSlimThug( pGraphicDev );

	if ( FAILED( pInstance->Ready_GameObject() ) )
	{
		MSG_BOX( "CSlimThug Created Failed" );
		Engine::Safe_Release( pInstance );
	}
	return pInstance;
}

Engine::EStatus CSlimThug::Move_Forward( float fScale /*= 1.f*/ )
{
	if ( nullptr == m_pTransformCom )
		return EStatus::Failure;

	m_pTransformCom->Go_Straight(
		m_tInfo.m_fSpeed * fScale,
		m_pTimer->Get_TimeDelta() );

	return EStatus::Success;
}

Engine::EStatus CSlimThug::Move_Right( float fScale /*= 1.f*/ )
{
	if ( nullptr == m_pTransformCom )
		return EStatus::Failure;

	m_pTransformCom->Go_Right(
		m_tInfo.m_fSpeed * fScale,
		m_pTimer->Get_TimeDelta() );

	return EStatus::Success;
}


#ifndef _FINAL
void CSlimThug::Debug_Render( void )
{
	CDebug::GetInstance()->Add_DebugText( "CSlimThug" );

}
#endif
_ulong CSlimThug::Free( void )
{
	_ulong		dwRefCnt = 0;

	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}
