// For.Mesh_Bat
pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/Bat/", L"Bat.X");
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Bat", pComponent)))
return E_FAIL;


// For.Mesh_SpikeBat
pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/BatSpike/", L"BatSpike.X");
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_BatSpike", pComponent)))
return E_FAIL;


// For.Mesh_Pipe
pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/Pipe/", L"Pipe.X");
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Pipe", pComponent)))
return E_FAIL;


// For.Mesh_PipeWRench
pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/PipeWRench/", L"PipeWRench.X");
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_PipeWRench", pComponent)))
return E_FAIL;


// For.Mesh_Monster_Sword
pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Monster_Weapon/Sword/", L"Sword.X");
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Monster_Sword", pComponent)))
return E_FAIL;


// For.Mesh_Bomb_Mask
pComponent = Engine::CMesh_Static::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/StaticMesh/Bomb_Mask/", L"Bomb_Mask.X");
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STATIC, L"CMesh_Static_Monster_Bomb_Mask", pComponent)))
return E_FAIL;







// For.Component_Mesh_BigThug
pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/BigThug/", L"BigThug.X", _vec3(270.f, 180.f, 0.f));
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Dynamic_BigThug", pComponent)))
return E_FAIL;

// For.Component_Mesh_Solider
pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/Foot_Soldier/", L"Foot_Soldier.X", _vec3(270.f, 180.f, 0.f));
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Dynamic_Foot_Soldier", pComponent)))
return E_FAIL;


// For.Component_Mesh_Solider_Bomb
pComponent = Engine::CMesh_Dynamic::Create(m_pGraphicDev, L"../Bin/Resources/Mesh/DynamicMesh/Monster/Foot_Soldier_Bomb/", L"Foot_Soldier_Bomb.X", _vec3(270.f, 180.f, 0.f));
if (nullptr == pComponent)
return E_FAIL;
if (FAILED(Engine::CComponent_Manager::GetInstance()->Ready_Component(SCENE_STAGE, L"CMesh_Dynamic_Foot_Soldier_Bomb", pComponent)))
return E_FAIL;