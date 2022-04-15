


//for (int i = 0; i < 10; ++i)
//{
	//pGameObject = CMonster::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;

	//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Monster", pGameObject)))
	//	return E_FAIL;
//}

//// 트리거 테스트
//pGameObject = CTrigger::Create(m_pGraphicDev,
//	std::bind([] {CDebug::GetInstance()->Add_DebugText("%s", "test1"); })
//	, CTrigger::충돌시작동); // -> 충돌중 함수호출
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

//// SlimThug 몬스터 생성..
//pGameObject = CSlimThug::Create(m_pGraphicDev);
//
//if (nullptr == pGameObject)
//return E_FAIL;
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Monster", pGameObject)))
//return E_FAIL;

//
//class 무리매니저;
//
//class 큰무리;
//
//class 무리;
//
//

CEffect_Manager::GetInstance()->Use_Effect(SCENE_STAGE, L"Effect_Stage2_Wind_01", _vec3(0.f, 0.f, 0.f));