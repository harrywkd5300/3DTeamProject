//// Sprite
//pGameObject = CEffect_Sprite::Create(m_pGraphicDev);
//if (nullptr == pGameObject)
//return E_FAIL;
//
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Effect", pGameObject)))
//return E_FAIL;
//
////// Scroll

//for (size_t i = 0; i < 5; ++i)
//{
//	pGameObject = CEffect_Buffer::Create(m_pGraphicDev);
//	if (nullptr == pGameObject)
//		return E_FAIL;
//
//	if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Effect", pGameObject)))
//		return E_FAIL;
//}

//pGameObject = CEffect_Distort::Create(m_pGraphicDev);
//if (nullptr == pGameObject)
//return E_FAIL;
//
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Effect", pGameObject)))
//return E_FAIL;

////Mesh
//pGameObject = CEffect_Mesh::Create(m_pGraphicDev);
//if (nullptr == pGameObject)
//return E_FAIL;
//
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Effect", pGameObject)))
//return E_FAIL;
//
////Particle
//pGameObject = CParticle::Create(m_pGraphicDev);
//if (nullptr == pGameObject)
//return E_FAIL;
//
//if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Layer_Effect", pGameObject)))
//return E_FAIL;

// ---------------------------------------------------------------------------------------------- //
// ÀÌÆåÆ® ÆÄ½Ìµ¥ÀÌÅÍ
pGameObject = CEffectOBJ::Create(m_pGraphicDev);
if (nullptr == pGameObject)
return E_FAIL;

HANDLE	hFile_Effect = CreateFile(L"../Bin/Data/EffectData/Effect_Test.dat", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

if (INVALID_HANDLE_VALUE == hFile_Effect)
{
	MSG_BOX("Effect System Load Failed!!");
	return E_FAIL;
}

_ulong		dwByteKJY = 0;
_ulong		dwNumComponents = 0;
_uint		iLength, iListCnt = 0;
_tchar*		pEffectName = nullptr;

EFFECT_INFO		tInfo = EFFECT_INFO();
//ZeroMemory(&tInfo, sizeof(EFFECT_INFO));

ReadFile(hFile_Effect, &iLength, sizeof(_uint), &dwByteKJY, nullptr);
pEffectName = CString_Manager::GetInstance()->GetTchar(iLength);
ReadFile(hFile_Effect, pEffectName, sizeof(TCHAR) * iLength, &dwByteKJY, nullptr);

dynamic_cast<CEffectOBJ*>(pGameObject)->Set_EffectTag(pEffectName);

ReadFile(hFile_Effect, &iListCnt, sizeof(_uint), &dwByteKJY, nullptr);

for (_uint i = 0; i < iListCnt; ++i)
{
	ReadFile(hFile_Effect, &tInfo, sizeof(EFFECT_INFO), &dwByteKJY, nullptr);
	if (0 == dwByteKJY)
		break;

	Engine::CMyEffect*	pEffectCom = nullptr;

	if (tInfo.m_ePassType == EFFECT_INFO::PASS_MASKING
		|| tInfo.m_ePassType == EFFECT_INFO::PASS_BUFFER
		|| tInfo.m_ePassType == EFFECT_INFO::PASS_CUTBLACK_SPRITE)
	{
		pEffectCom = CMyEffect_Buff::Create(m_pGraphicDev, tInfo);
		dynamic_cast<CEffectOBJ*>(pGameObject)->Add_Effect(pEffectCom);
	}
	else if (tInfo.m_ePassType == EFFECT_INFO::PASS_PARTICLE)
	{
		pEffectCom = CMyEffect_Particle::Create(m_pGraphicDev, tInfo);
		dynamic_cast<CEffectOBJ*>(pGameObject)->Add_Effect(pEffectCom);
	}
}

if (FAILED(m_pObject_Manager->Ready_GameObject(SCENE_STAGE, L"Effect", pGameObject)))
return E_FAIL;