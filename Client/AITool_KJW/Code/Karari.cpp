#include "stdafx.h"
#include "..\Headers\Karari.h"
#include "Clinet_Component.h"
#include "Object_Manager.h"
#include "Component_Manager.h"

#include "Soldier_Sword.h"

#include "Navigation.h"
#include "Graphic_Device.h"
#include "Collider_Manager.h"
#include "Camera_Player.h"

CKarari::CKarari(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMonster(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CKarari", std::bind(&CKarari::Debug_Render, this));
#endif

}


HRESULT CKarari::Ready_GameObject(void)
{
	Engine::CComponent*			pComponent = nullptr;

	pComponent = m_pMeshCom = (Engine::CMesh_Dynamic*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CMesh_Dynamic_Karari");
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::TYPE_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", pComponent));
	m_pMeshCom->AddRef();
	m_pMeshCom->Set_RootMotionFrame("Origin");
	lstrcpy(m_tInfo.m_StartAniName, L"ANI_KARARI_TYPE_START");
	m_tInfo.m_StartAniNum = ANI_KARARI_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_KARARI_TYPE_END;

	if (FAILED(CMonster::Ready_Monster_Component()))
		return E_FAIL;


#ifdef CHARACTER_COLLIDERMODIFY
	Engine::CCollider* pCollider = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pCollider)
		return E_FAIL;
	Ready_LoadCollider(L"../Bin/Data/ColliderData/Karari_Collider.Colldat", pCollider);
	Engine::Safe_Release(pCollider);

	/////////////////////////////// �ݸ��� �׷� ���� �Ŵ����� ���.. ///////////////////////////////
	// �ݸ����� ����� ���� ���� ���� �ݸ����� �÷��̾�, ���� ���� ���� ���̾ ����ؾߵǿ�..
	// �浹 or ��Ÿ ����� �ݸ����� �����Ǵ� ��ü �� �ϳ� ��..

	if ( FAILED( Ready_CollidersToColliderManager( CCollider_Manager::GROUP_STATIC, this ) ) )
		return E_FAIL;

	_tchar szLayerName[256] = L"";
	swprintf_s(szLayerName, L"Layer_Karari_Hit%d", GetKey());
	const _tchar* pLayerName = CString_Manager::GetInstance()->GetStr(szLayerName);

	if ( false == Coiiders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, pLayerName, KarariColGroup_Hit) )
		return E_FAIL;
	if ( false == Coiiders_Add_ColliderLayer( CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", Karari_RWeapon_ColGroup_Attack) )
		return E_FAIL;
	if (false == Coiiders_Add_ColliderLayer(CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", Karari_LWeapon_ColGroup_Attack))
		return E_FAIL;
	if (false == Coiiders_Add_ColliderLayer(CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", Karari_RFoot_ColGroup_Attack))
		return E_FAIL;
	if (false == Coiiders_Add_ColliderLayer(CCollider_Manager::GROUP_STATIC, L"Layer_Monster_Attack", Karari_LFoot_ColGroup_Attack))
		return E_FAIL;

	// -------------- Hit �� �ݸ����� Attack �� �ݸ����� ������ �ʿ� ���� ������ �������� ���ϰ�.. -------------- //
	m_pCollider_Manager->Ignore_ColliderLayer( L"Layer_Monster_Attack", pLayerName );
	m_tInfo.m_iMoveColliderGroupCount = KarariColGroup_Hit;	// Hit �� �ݸ����� ĳ������ ���꿡 ���� ���ŵ� �� �ֵ��� �� ��° �׷����� ����..

	/////////////////////////////// �ݸ��� �׷� ���� �Ŵ����� ��� ��.. ///////////////////////////////


#else
	// For.Collider
	pComponent = m_pColliderCom = (Engine::CCollider*)CComponent_Manager::GetInstance()->Clone_Component(SCENE_STAGE, L"CCollider");
	if (nullptr == pComponent)
		return E_FAIL;

	CCollider::RESIZEDATA Data;

	//Data.fOriginalRadius = 70.f;
	Data.pTransformCom = m_pTransformCom;
	Data.pParent = m_pMeshCom->Find_FrameMatrixPointer("spine01");
	Data.vOriginalMin = _vec3(-70.f, -70.f, -70.f);
	Data.vOriginalMax = _vec3(70.f, 70.f, 70.f);


	m_pColliderCom->ReSizing(CCollider::TYPE_SPHERE, Data);
	m_mapComponent[Engine::CComponent::TYPE_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Collider", pComponent));
	m_pColliderCom->AddRef();


	// Add By.KMH
	m_pColliderCom->Modify_StaticOption(CCollider::STATICOPTIONDATA(PushOption_BePushedOut), StaticOption_Push);
	CCollider_Manager::GetInstance()->Add_Collider(Engine::CCollider_Manager::GROUP_STATIC, L"Group_KnifeThug", m_pColliderCom, this);
	CCollider_Manager::GetInstance()->Add_ColliderGroup(Engine::CCollider_Manager::GROUP_STATIC, L"Layer_KnifeThug", L"Group_KnifeThug");
#endif

	if (FAILED(Equip_Item(CSoldier_Sword::Create(m_pGraphicDev), m_pMeshCom->Find_FrameMatrixPointer("SaberStaff_Root"))))
		return E_FAIL;

	if (FAILED(Ready_AI()))
		return E_FAIL;

	

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_POSITION, &_vec3(130.2f, 0.f, 120.0f));
	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(0.01f, 0.01f, 0.01f));

	m_pMeshCom->Set_AnimationSet(0);
	m_pMeshCom->Play_Animation(0.f);






	m_eMonsterType = MonsterType_MidBoss;

	// Info ������ �ʱⰪ ����..
	m_tInfo.m_fSpeed = 2.f;

	// ĳ������ Range �� ����..
	m_tInfo.m_fRange_Close = 2.f;
	m_tInfo.m_fRange_Near = 4.f;
	m_tInfo.m_fRange_Mid = 4.5f;
	m_tInfo.m_fRange_Far = 20.f;
	m_tInfo.m_fRange_TooFar = 40.f;
	m_tInfo.m_fRange_FoundEnemy = 20.f;

	m_tInfo.m_iMaxHp = m_tInfo.m_iCurHp = 100;


	// ���� ������ ����..
	m_tInfo.m_iShield_Point = 40;
	m_tInfo.m_bShield = true;

	m_tInfo.m_fEvade_Point = 1.f;
	m_tInfo.m_bEvade = false;


	//���� �ƽ���
	m_iShieldMax = 40;

	//�ʴ� ������ �� ��ġ
	m_iAdd_Shield_Point = 5;	//8�ʴ� ���
	m_fAdd_Evade_Point = 1.f;			// ���߿� �ٲ��� �׽�Ʈ��     1�ʸ��� ȸ���ϰ� �س���


	_uint iCount = CRandom::GetInstance()->GetValue(0, 6);
	m_iSelect_Atk_Count = iCount;


	return NOERROR;
}

_int CKarari::Update_GameObject(const _float & fTimeDelta)
{
	int returnValue = CMonster::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_NONALPHA, this);
	}

	
	return returnValue;
}

_int CKarari::LastUpdate_GameObject(const _float & fTimeDelta)
{
	CMonster::LastUpdate_GameObject(fTimeDelta);

	if (true == m_bIsDead)
		return 1;



	for (_uint i = 0; i < m_ColliderGroups.pGroup[Karari_RWeapon_ColGroup_Attack].iColliderCount; ++i)
	{
		OPTIONDATA OptionData;
		OptionData.dwOption = SIGNAL_HIT_ENEMY;
		OptionData.DetailDataList.push_back(0);
		m_ColliderGroups.pGroup[Karari_RWeapon_ColGroup_Attack].ppCollider[i]->Add_Option(OptionData);
	}


	return 0;
}

void CKarari::Render_GameObject(void)
{
	if (nullptr == m_pMeshCom)
		return;

	if (nullptr == m_pGraphicDev || nullptr == m_pShaderCom)
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
#else
	m_pColliderCom->Render_Buffer();
#endif
#endif 

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();


	SetUp_OnConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	m_pMeshCom->Render_Mesh(pEffect, "g_BaseTexture", "g_NormalTexture", "g_SpecularTexture");

	pEffect->EndPass();

	pEffect->End();

	Safe_Release(pEffect);

	m_pNaviCom->Render_Navigation();
	
}

HRESULT CKarari::Ready_AI(void)
{
	m_tInfo.m_StartAniNum = ANI_KARARI_TYPE_START;
	m_tInfo.m_EndAniNum = ANI_KARARI_TYPE_END;
	// %%%%%%%%%%%%%%%%%%%%%%%%%% ĳ���� ���� ������Ʈ ���� %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ---- ACT ----
	m_tInfo.m_WorldState.setVariable(ACT_TYPE_START, ACT_TYPE_END, false);
	// ---- ANI_SOLDIER ----
	m_tInfo.m_WorldState.setVariable(ANI_KARARI_TYPE_START, ANI_KARARI_TYPE_END, false);
	// ---- Signal ----
	m_tInfo.m_WorldState.setVariable(SIGNAL_TYPE_START, SIGNAL_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(CH_STATE_TYPE_START, CH_STATE_TYPE_END, false);
	m_tInfo.m_WorldState.setVariable(TARGET_TYPE_START, TARGET_TYPE_END, false);

	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_ANIMATION, true);
	m_tInfo.m_WorldState.setVariable(ACT_IDLE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_PEACE, true);
	m_tInfo.m_WorldState.setVariable(CH_STATE_FOUND_ENEMY, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);


	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ñ׳� ó�� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- �ִϸ��̼� ���� ��ȯ -----
	CAction ACT_SIGNAL_CHANGE_ANIMATION_FORCED("SIGNAL_CHANGE_ANIMATION_FORCED", 1);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setPrecondition(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_SIGNAL_CHANGE_ANIMATION_FORCED.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_SIGNAL_CHANGE_ANIMATION_FORCED);
	//// ----- �ִϸ��̼� ���� ��ȯ -----
	//// ----- ���� ���� -----
	//CAction ACT_WALK_EXIT
	//// ----- ���� ���� -----
	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ñ׳� ó�� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ൿ ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%

	//�⺻ Idle
	CAction AI_ACT_Idle("AI_ACT_Idle", 300);
	AI_ACT_Idle.setPrecondition(CH_STATE_FOUND_ENEMY, false);

	AI_ACT_Idle.setEffect(CH_STATE_PEACE, true);
	AI_ACT_Idle.setEffect(ACT_IDLE, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_Idle);

	// ���� �ָ� ���� ��..
	{
		CAction AI_ACT_GoTo_Target("AI_ACT_GoTo_Enemy", 2000);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_FOUND_ENEMY, true);
		AI_ACT_GoTo_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_MID_TARGET, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_NEAR_TARGET, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_CLOSE_TARGET, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_GoTo_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_GoTo_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_GoTo_Target.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_GoTo_Target.setEffect(SIGNAL_CHANGE_DIR_FRONT, true);
		AI_ACT_GoTo_Target.setEffect(ACT_RUN, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_GoTo_Target);
	}

	// ���� �� �ɵ��� ���� ���̶��..
	{
		CAction AI_ACT_MoveAround_Target("AI_ACT_MoveAround_Target", 2000);
		AI_ACT_MoveAround_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_NEAR_TARGET, true);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_ATTACK, false);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_MoveAround_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_MoveAround_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_MoveAround_Target.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_MoveAround_Target.setEffect(ACT_WALK, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_MoveAround_Target);
	}

	// ���� �����϶�� ��ȣ�� �޾��� �� ( ������ ���� ���� �ۿ� ���� ��, ������ �� )..
	{
		CAction AI_ACT_MoveTo_Target("AI_ACT_MoveTo_Target", 1999);
		AI_ACT_MoveTo_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_CLOSE_TARGET, false);
		AI_ACT_MoveTo_Target.setPrecondition(SIGNAL_ATTACK_TARGET, true);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_ATTACK, false);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_MoveTo_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_MoveTo_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_MoveTo_Target.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_MoveTo_Target.setEffect(SIGNAL_CHANGE_DIR_FRONT, true);
		AI_ACT_MoveTo_Target.setEffect(ACT_WALK, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_MoveTo_Target);
	}

	// ���� �����϶�� ��ȣ�� �޾��� �� ( ���� ���� �ȿ� ���� ��, ���� �Ѵ� )..
	{
		CAction AI_ACT_Attack_Target("AI_ACT_Attack_Target", 2000);
		AI_ACT_Attack_Target.setPrecondition(TARGET_NONE, false);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_CLOSE_TARGET, true);
		AI_ACT_Attack_Target.setPrecondition(SIGNAL_ATTACK_TARGET, true);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_ATTACK, false);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_HIT, false);
		AI_ACT_Attack_Target.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_Attack_Target.setEffect(SIGNAL_SET_ANGLE_TARGET, false);
		AI_ACT_Attack_Target.setEffect(SIGNAL_CHANGE_ANGLE, false);
		AI_ACT_Attack_Target.setEffect(ACT_ATTACK, true);
		AI_ACT_Attack_Target.setEffect(SIGNAL_ATTACK_TARGET, false);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_Attack_Target);
	}




	//����
	CAction AI_ACT_HIT("AI_ACT_HIT", 199);
	AI_ACT_HIT.setPrecondition(SIGNAL_HIT_ENEMY, true);
	
	AI_ACT_HIT.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	AI_ACT_HIT.setEffect(SIGNAL_CHANGE_ANGLE, true);
	AI_ACT_HIT.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	AI_ACT_HIT.setEffect(ACT_HIT, true);
	m_tInfo.m_vecAction_Act.push_back(AI_ACT_HIT);


	// �׾���..
	{
		CAction AI_ACT_Die("AI_ACT_Die", 100);
		AI_ACT_Die.setPrecondition(SIGNAL_DIE, true);
		AI_ACT_Die.setPrecondition(CH_STATE_DIE, false);

		AI_ACT_Die.setEffect(SIGNAL_DIE, false);
		AI_ACT_Die.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		AI_ACT_Die.setEffect(SIGNAL_CHANGE_ANGLE, true);
		AI_ACT_Die.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		AI_ACT_Die.setEffect(ACT_DEATH, true);
		m_tInfo.m_vecAction_Act.push_back(AI_ACT_Die);
	}

	


	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ൿ ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%


	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ִϸ��̼� ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%
	// ----- �⺻ ��� ���� -----
	CAction ACT_ANI_KARARI_Idle("ANI_KARARI_Idle", 200);
	ACT_ANI_KARARI_Idle.setPrecondition(ACT_IDLE, true);
	ACT_ANI_KARARI_Idle.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Idle.setEffect(CH_STATE_IDLE, true);
	ACT_ANI_KARARI_Idle.setEffect(ANI_KARARI_Idle, true);
	ACT_ANI_KARARI_Idle.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_KARARI_Idle.setEffect(SIGNAL_CHANGE_ANGLE, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Idle);



	// ----- �ȱ�( Front ).. -----
	{
		CAction ACT_ANI_KARARI_Walk_F("ANI_KARARI_Walk_F", 90);
		ACT_ANI_KARARI_Walk_F.setPrecondition(ACT_WALK, true);
		ACT_ANI_KARARI_Walk_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_KARARI_Walk_F.setPrecondition(MOVE_DIR_FRONT, true);

		ACT_ANI_KARARI_Walk_F.setEffect(ANI_KARARI_Walk_F, true);
		ACT_ANI_KARARI_Walk_F.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_KARARI_Walk_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_KARARI_Walk_F.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Walk_F);
	}

	// ----- �ȱ�( Back ).. -----
	{
		CAction ACT_ANI_KARARI_Walk_B("ANI_KARARI_Walk_B", 90);
		ACT_ANI_KARARI_Walk_B.setPrecondition(ACT_WALK, true);
		ACT_ANI_KARARI_Walk_B.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_KARARI_Walk_B.setPrecondition(MOVE_DIR_BACK, true);

		ACT_ANI_KARARI_Walk_B.setEffect(ANI_KARARI_Walk_B, true);
		ACT_ANI_KARARI_Walk_B.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_KARARI_Walk_B.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_KARARI_Walk_B.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Walk_B);
	}

	// ----- �ȱ�( Left ).. -----
	{
		CAction ACT_ANI_KARARI_Walk_L("ANI_KARARI_Walk_L", 90);
		ACT_ANI_KARARI_Walk_L.setPrecondition(ACT_WALK, true);
		ACT_ANI_KARARI_Walk_L.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_KARARI_Walk_L.setPrecondition(MOVE_DIR_LEFT, true);

		ACT_ANI_KARARI_Walk_L.setEffect(ANI_KARARI_Walk_L, true);
		ACT_ANI_KARARI_Walk_L.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_KARARI_Walk_L.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_KARARI_Walk_L.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Walk_L);
	}

	// ----- �ȱ�( Right ).. -----
	{
		CAction ACT_ANI_KARARI_Walk_R("ANI_KARARI_Walk_R", 90);
		ACT_ANI_KARARI_Walk_R.setPrecondition(ACT_WALK, true);
		ACT_ANI_KARARI_Walk_R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_KARARI_Walk_R.setPrecondition(MOVE_DIR_RIGHT, true);

		ACT_ANI_KARARI_Walk_R.setEffect(ANI_KARARI_Walk_R, true);
		ACT_ANI_KARARI_Walk_R.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
		ACT_ANI_KARARI_Walk_R.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_KARARI_Walk_R.setEffect(CH_STATE_WALK, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Walk_R);
	}






	// ----- �޸��� -----
	CAction ACT_ANI_KARARI_Run_F_OV("ANI_KARARI_Run_F_OV", 90);
	ACT_ANI_KARARI_Run_F_OV.setPrecondition(ACT_RUN, true);
	ACT_ANI_KARARI_Run_F_OV.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Run_F_OV.setEffect(ANI_KARARI_Run_F_OV, true);
	ACT_ANI_KARARI_Run_F_OV.setEffect(SIGNAL_CHANGE_ANIMATION_FORCED, true);
	ACT_ANI_KARARI_Run_F_OV.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Run_F_OV.setEffect(CH_STATE_RUN, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Run_F_OV);



	// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	//�⺻ �޺� ���� 3����
	// ----- �޺� ����1 -----
	CAction ACT_ANI_KARARI_Melee_01("ANI_KARARI_Melee_01", 80);
	ACT_ANI_KARARI_Melee_01.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_KARARI_Melee_01.setPrecondition(SIGNAL_KARARI_DEFAULT_COMBO_ATK, true);
	ACT_ANI_KARARI_Melee_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Melee_01.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Melee_01.setEffect(SIGNAL_KARARI_DEFAULT_COMBO_ATK, false);
	ACT_ANI_KARARI_Melee_01.setEffect(ANI_KARARI_Melee_01, true);
	ACT_ANI_KARARI_Melee_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Melee_01);

	// ----- �޺� ����2 -----
	CAction ACT_ANI_KARARI_Melee_02("ANI_KARARI_Melee_02", 79);
	ACT_ANI_KARARI_Melee_02.setPrecondition(ANI_KARARI_Melee_01, true);
	ACT_ANI_KARARI_Melee_02.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Melee_02.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Melee_02.setEffect(SIGNAL_KARARI_DEFAULT_COMBO_ATK, false);
	ACT_ANI_KARARI_Melee_02.setEffect(ANI_KARARI_Melee_02, true);
	ACT_ANI_KARARI_Melee_02.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Melee_02);

	// ----- �޺� ����3 -----
	CAction ACT_ANI_KARARI_Melee_03("ANI_KARARI_Melee_03", 78);
	ACT_ANI_KARARI_Melee_03.setPrecondition(ANI_KARARI_Melee_02, true);
	ACT_ANI_KARARI_Melee_03.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Melee_03.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Melee_03.setEffect(SIGNAL_KARARI_DEFAULT_COMBO_ATK, false);
	ACT_ANI_KARARI_Melee_03.setEffect(ANI_KARARI_Melee_03, true);
	ACT_ANI_KARARI_Melee_03.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Melee_03);



	//�ι�° �޺� ���� 2����
	// ----- �޺� ����1 -----
	CAction ACT_ANI_KARARI_Melee_01_b("ANI_KARARI_Melee_01_b", 80);
	ACT_ANI_KARARI_Melee_01_b.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_KARARI_Melee_01_b.setPrecondition(SIGNAL_KARARI_COMBO_ATK, true);
	ACT_ANI_KARARI_Melee_01_b.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Melee_01_b.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Melee_01_b.setEffect(SIGNAL_KARARI_COMBO_ATK, false);
	ACT_ANI_KARARI_Melee_01_b.setEffect(ANI_KARARI_Melee_01_b, true);
	ACT_ANI_KARARI_Melee_01_b.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Melee_01_b);

	// ----- �޺� ����2 -----
	CAction ACT_ANI_KARARI_Melee_02_b("ANI_KARARI_Melee_02_b", 79);
	ACT_ANI_KARARI_Melee_02_b.setPrecondition(ANI_KARARI_Melee_01_b, true);
	ACT_ANI_KARARI_Melee_02_b.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Melee_02_b.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Melee_02_b.setEffect(SIGNAL_KARARI_COMBO_ATK, false);
	ACT_ANI_KARARI_Melee_02_b.setEffect(ANI_KARARI_Melee_02_b, true);
	ACT_ANI_KARARI_Melee_02_b.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Melee_02_b);



	//  ������   �������
	CAction ACT_ANI_KARARI_Melee_BackStab_x("ANI_KARARI_Melee_BackStab_x", 80);
	ACT_ANI_KARARI_Melee_BackStab_x.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_KARARI_Melee_BackStab_x.setPrecondition(SIGNAL_STRONG_ATK, true);
	ACT_ANI_KARARI_Melee_BackStab_x.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
	
	ACT_ANI_KARARI_Melee_BackStab_x.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Melee_BackStab_x.setEffect(SIGNAL_STRONG_ATK, false);
	ACT_ANI_KARARI_Melee_BackStab_x.setEffect(ANI_KARARI_Melee_BackStab_x, true);
	ACT_ANI_KARARI_Melee_BackStab_x.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Melee_BackStab_x);


	// ȸ���� ������
	CAction ACT_ANI_KARARI_Kick_Turn_L_180("ANI_KARARI_Kick_Turn_L_180", 80);
	ACT_ANI_KARARI_Kick_Turn_L_180.setPrecondition(ACT_ATTACK, true);
	ACT_ANI_KARARI_Kick_Turn_L_180.setPrecondition(SIGNAL_KARARI_FOOT_ATK, true);
	ACT_ANI_KARARI_Kick_Turn_L_180.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Kick_Turn_L_180.setEffect(CH_STATE_ATTACK, true);
	ACT_ANI_KARARI_Kick_Turn_L_180.setEffect(SIGNAL_KARARI_FOOT_ATK, false);
	ACT_ANI_KARARI_Kick_Turn_L_180.setEffect(ANI_KARARI_Kick_Turn_L_180, true);
	ACT_ANI_KARARI_Kick_Turn_L_180.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Kick_Turn_L_180);



	//// ~~~~~~~~~~~~~~~~~~~~~~ ACT_ATTACK ~~~~~~~~~~~~~~~~~~~~~~

	//// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DEFENSE ~~~~~~~~~~~~~~~~~~~~~~

	// ----- �⺻ ���1 -----
	CAction ACT_ANI_KARARI_Block_F_R2R("ANI_KARARI_Block_F_R2R", 80);
	ACT_ANI_KARARI_Block_F_R2R.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_KARARI_Block_F_R2R.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Block_F_R2R.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_KARARI_Block_F_R2R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Block_F_R2R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Block_F_R2R.setEffect(ANI_KARARI_Block_F_R2R, true);
	ACT_ANI_KARARI_Block_F_R2R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Block_F_R2R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Block_F_R2R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Block_F_R2R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Block_F_R2R);

	// ----- �⺻ ���2 -----
	CAction ACT_ANI_KARARI_Block_L_R2R("ANI_KARARI_Block_L_R2R", 79);
	ACT_ANI_KARARI_Block_L_R2R.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_KARARI_Block_L_R2R.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Block_L_R2R.setPrecondition(ANI_SOLDIER_Block_F_R2R, true);
	ACT_ANI_KARARI_Block_L_R2R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Block_L_R2R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Block_L_R2R.setEffect(ANI_KARARI_Block_L_R2R, true);
	ACT_ANI_KARARI_Block_L_R2R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Block_L_R2R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Block_L_R2R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Block_L_R2R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Block_L_R2R);

	// ----- �⺻ ���3 -----
	CAction ACT_ANI_KARARI_Block_R_R2R("ANI_KARARI_Block_R_R2R", 78);
	ACT_ANI_KARARI_Block_R_R2R.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_KARARI_Block_R_R2R.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Block_R_R2R.setPrecondition(ANI_SOLDIER_Block_L_R2R, true);
	ACT_ANI_KARARI_Block_R_R2R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Block_R_R2R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Block_R_R2R.setEffect(ANI_KARARI_Block_R_R2R, true);
	ACT_ANI_KARARI_Block_R_R2R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Block_R_R2R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Block_R_R2R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Block_R_R2R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Block_R_R2R);

	// ----- �⺻ ��� ���� -----
	CAction ACT_ANI_KARARI_Block_F_R2R_01("ANI_KARARI_Block_F_R2R_01", 77);
	ACT_ANI_KARARI_Block_F_R2R_01.setPrecondition(SIGNAL_SHIELD_ON, true);
	ACT_ANI_KARARI_Block_F_R2R_01.setPrecondition(SIGNAL_SHIELD_BREAK_ON, true);
	ACT_ANI_KARARI_Block_F_R2R_01.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Block_F_R2R_01.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(SIGNAL_SHIELD_BREAK_ON, false);
	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(ANI_KARARI_Block_F_R2R_01, true);
	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Block_F_R2R_01.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Block_F_R2R_01);




	// ----- ȸ�� F -----        
	CAction ACT_ANI_KARARI_Evade_Vault("ANI_KARARI_Evade_Vault", 76);
	ACT_ANI_KARARI_Evade_Vault.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_KARARI_Evade_Vault.setPrecondition(SIGNAL_EVADE_FRONT, true);
	ACT_ANI_KARARI_Evade_Vault.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Evade_Vault.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_KARARI_Evade_Vault.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Evade_Vault.setEffect(SIGNAL_EVADE_FRONT, false);
	ACT_ANI_KARARI_Evade_Vault.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Evade_Vault.setEffect(ANI_KARARI_Evade_Vault, true);
	ACT_ANI_KARARI_Evade_Vault.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Evade_Vault.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Evade_Vault.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Evade_Vault.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Evade_Vault);



	// ----- ȸ�� B -----        
	CAction ACT_ANI_KARARI_Evade_B("ANI_KARARI_Evade_B", 76);
	ACT_ANI_KARARI_Evade_B.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_KARARI_Evade_B.setPrecondition(SIGNAL_EVADE_BACK, true);
	ACT_ANI_KARARI_Evade_B.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Evade_B.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_KARARI_Evade_B.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Evade_B.setEffect(SIGNAL_EVADE_BACK, false);
	ACT_ANI_KARARI_Evade_B.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Evade_B.setEffect(ANI_KARARI_Evade_B, true);
	ACT_ANI_KARARI_Evade_B.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Evade_B.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Evade_B.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Evade_B.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Evade_B);



	// ----- ȸ�� Default Combo ȸ�� 1 -----        
	CAction ACT_ANI_KARARI_Evade_L("ANI_KARARI_Evade_L", 76);
	ACT_ANI_KARARI_Evade_L.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_KARARI_Evade_L.setPrecondition(SIGNAL_EVADE_DEFAULT, true);
	ACT_ANI_KARARI_Evade_L.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Evade_L.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_KARARI_Evade_L.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Evade_L.setEffect(SIGNAL_EVADE_DEFAULT, false);
	ACT_ANI_KARARI_Evade_L.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Evade_L.setEffect(ANI_KARARI_Evade_L, true);
	ACT_ANI_KARARI_Evade_L.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Evade_L.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Evade_L.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Evade_L.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Evade_L);



	// ----- ȸ�� Default Combo ȸ�� 2 -----        
	CAction ACT_ANI_KARARI_Evade_R("ANI_KARARI_Evade_R", 75);
	ACT_ANI_KARARI_Evade_R.setPrecondition(ANI_KARARI_Evade_L, true);
	ACT_ANI_KARARI_Evade_R.setPrecondition(SIGNAL_EVADE_ON, true);
	ACT_ANI_KARARI_Evade_R.setPrecondition(SIGNAL_EVADE_DEFAULT, true);
	ACT_ANI_KARARI_Evade_R.setPrecondition(ACT_HIT, true);
	ACT_ANI_KARARI_Evade_R.setPrecondition(SIGNAL_HIT_ENEMY, true);
	ACT_ANI_KARARI_Evade_R.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

	ACT_ANI_KARARI_Evade_R.setEffect(SIGNAL_EVADE_DEFAULT, false);
	ACT_ANI_KARARI_Evade_R.setEffect(SIGNAL_HIT_ENEMY, false);
	ACT_ANI_KARARI_Evade_R.setEffect(ANI_KARARI_Evade_R, true);
	ACT_ANI_KARARI_Evade_R.setEffect(SIGNAL_CHANGE_ANIMATION, false);
	ACT_ANI_KARARI_Evade_R.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
	ACT_ANI_KARARI_Evade_R.setEffect(SIGNAL_CHANGE_ANGLE, true);
	ACT_ANI_KARARI_Evade_R.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
	m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Evade_R);



	//// ~~~~~~~~~~~~~~~~~~~~~~ ACT_DEFENSE ~~~~~~~~~~~~~~~~~~~~~~

	//// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	{
		// ----- �⺻ ��Ʈ1 -----
		CAction ACT_ANI_KARARI_Dmg_F("ANI_KARARI_Dmg_F", 81);
		ACT_ANI_KARARI_Dmg_F.setPrecondition(SIGNAL_EVADE_ON, false);
		ACT_ANI_KARARI_Dmg_F.setPrecondition(SIGNAL_SHIELD_ON, false);
		ACT_ANI_KARARI_Dmg_F.setPrecondition(ACT_HIT, true);
		ACT_ANI_KARARI_Dmg_F.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

		ACT_ANI_KARARI_Dmg_F.setEffect(SIGNAL_HIT_ENEMY, false);
		ACT_ANI_KARARI_Dmg_F.setEffect(ANI_KARARI_Dmg_F, true);
		ACT_ANI_KARARI_Dmg_F.setEffect(SIGNAL_CHANGE_ANIMATION, false);
		ACT_ANI_KARARI_Dmg_F.setEffect(SIGNAL_SET_ANGLE_TARGET, true);
		ACT_ANI_KARARI_Dmg_F.setEffect(SIGNAL_CHANGE_ANGLE, true);
		ACT_ANI_KARARI_Dmg_F.setEffect(SIGNAL_CHANGE_TARGET_NEAR_ENEMY, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Dmg_F);
	}

	//// ~~~~~~~~~~~~~~~~~~~~~~ ACT_HIT ~~~~~~~~~~~~~~~~~~~~~~

	////------------------------Die--------------------------

	// ----- �ڷ� ���ư�( �¾Ƽ� ).. -----
	{
		CAction ACT_ANI_KARARI_Kndown_Gut_x("ANI_KARARI_Kndown_Gut_x", 80);
		ACT_ANI_KARARI_Kndown_Gut_x.setPrecondition(ACT_DEATH, true);
		ACT_ANI_KARARI_Kndown_Gut_x.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);

		ACT_ANI_KARARI_Kndown_Gut_x.setEffect(ANI_KARARI_Kndown_Gut_x, true);
		ACT_ANI_KARARI_Kndown_Gut_x.setEffect(SIGNAL_CHANGE_ANIMATION, false);
		ACT_ANI_KARARI_Kndown_Gut_x.setEffect(CH_STATE_DIE, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Kndown_Gut_x);
	}

	// ----- �ڷ� ����1.. -----
	{
		CAction ACT_ANI_KARARI_Kndown_Loop_b("ANI_KARARI_Kndown_Loop_b", 79);
		ACT_ANI_KARARI_Kndown_Loop_b.setPrecondition(ACT_DEATH, true);
		ACT_ANI_KARARI_Kndown_Loop_b.setPrecondition(SIGNAL_CHANGE_ANIMATION, true);
		ACT_ANI_KARARI_Kndown_Loop_b.setPrecondition(ANI_SOLDIER_Kndown_Gut_x, true);

		ACT_ANI_KARARI_Kndown_Loop_b.setEffect(ANI_KARARI_Kndown_Loop_b, true);
		ACT_ANI_KARARI_Kndown_Loop_b.setEffect(SIGNAL_CHANGE_ANIMATION, false);
		ACT_ANI_KARARI_Kndown_Loop_b.setEffect(CH_STATE_DIE, true);
		m_tInfo.m_vecAction_Ani.push_back(ACT_ANI_KARARI_Kndown_Loop_b);
	}

	//------------------------Die--------------------------




	// %%%%%%%%%%%%%%%%%%%%%%%%%% �ִϸ��̼� ���� �׼� %%%%%%%%%%%%%%%%%%%%%%%%%%

	return NOERROR;

}

void CKarari::SignalUpdate(CCharacterGroup::CHARDATA* ptChar)
{
	/*++m_iSelect_Atk_Count;

	if (7 <= m_iSelect_Atk_Count)
		m_iSelect_Atk_Count = 1;

	if (2 >= m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Defalut_Atk;
	else if (3 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Combo_Atk;
	else if (4 == m_iSelect_Atk_Count || 5 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Defalut_Atk;
	else if (6 == m_iSelect_Atk_Count)
		m_iAtk_Count = Soldier_Jump_Atk;

	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_DEFAULT_ATK, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_COMBO_ATK, false);
	m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_JUMP_ATK, false);


	if (Soldier_Defalut_Atk == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_DEFAULT_ATK, true);
	else if (Soldier_Combo_Atk == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_COMBO_ATK, true);
	else if (Soldier_Jump_Atk == m_iAtk_Count)
		m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_JUMP_ATK, true);

	_uint iCount = CRandom::GetInstance()->GetValue(0, 6);

	ptChar->fTime_Atk = 8.f + (float)iCount;*/
}

void CKarari::Move_SignalUpdate(CCharacterGroup::CHARDATA* ptChar)
{
	/*if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_JUMP_ATK))
	{
		if (true == m_tInfo.m_WorldState.getVariable(SIGNAL_SOLDIER_DEFAULT_ATK))
		{
			m_tInfo.m_WorldState.setVariable(SIGNAL_SOLDIER_JUMP_ATK, false);
		}

		m_tInfo.m_WorldState.setVariable(SIGNAL_CHANGE_DIR_BACK, true);

		ptChar->fTime = 0.5f;
	}
	else
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


		ptChar->fTime = 1.f;
	}*/

}


HRESULT CKarari::SetUp_OnConstantTable(LPD3DXEFFECT pEffect)
{
	pEffect->AddRef();

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");
	//m_pTransformCom->SetUp_OnShader_FORCED(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);	

	Safe_Release(pEffect);

	return NOERROR;
}


CKarari * CKarari::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CKarari *	pInstance = new CKarari(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MSG_BOX("CKarari Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

Engine::EStatus CKarari::Move_Forward(float fScale /*= 1.f*/)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	m_pTransformCom->Go_Straight(
		m_tInfo.m_fSpeed * fScale,
		m_pTimer->Get_TimeDelta());

	return EStatus::Success;
}

Engine::EStatus CKarari::Move_Right(float fScale /*= 1.f*/)
{
	if (nullptr == m_pTransformCom)
		return EStatus::Failure;

	m_pTransformCom->Go_Right(
		m_tInfo.m_fSpeed * fScale,
		m_pTimer->Get_TimeDelta());

	return EStatus::Success;
}


#ifndef _FINAL
void CKarari::Debug_Render(void)
{
	CDebug::GetInstance()->Add_DebugText("CKarari");

}
#endif
_ulong CKarari::Free(void)
{
	_ulong		dwRefCnt = 0;

	dwRefCnt = CMonster::Free();

	return dwRefCnt;
}
