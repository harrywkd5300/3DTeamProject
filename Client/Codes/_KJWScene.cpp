


//for (int i = 0; i < 10; ++i)
//{
	//pGameObject = CMonster::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Monster", pGameObject)))
	//	return E_FAIL;
//}

//// Ʈ���� �׽�Ʈ
//pGameObject = CTrigger::Create(m_pGraphicDev,
//	std::bind([] {CDebug::GetInstance()->Add_DebugText("%s", "test1"); })
//	, CTrigger::�浹���۵�); // -> �浹�� �Լ�ȣ��
//if (nullptr == pGameObject)
//return E_FAIL;
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Trigger", pGameObject)))
//return E_FAIL;

//
//pGameObject = CMous::Create(m_pGraphicDev);
//if (nullptr == pGameObject)
//	return E_FAIL;
//
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Monster", pGameObject)))
//	return E_FAIL;
//

//// SlimThug ���� ����..
//pGameObject = CSlimThug::Create(m_pGraphicDev);
//
//if (nullptr == pGameObject)
//return E_FAIL;
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Monster", pGameObject)))
//return E_FAIL;

//
//class �����Ŵ���;
//
//class ū����;
//
//class ����;
//
//

CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Stage2_Wind_01", _vec3(0.f, 0.f, 0.f));