#pragma once

#include "Engine_Defines.h"
#include "Base.h"

#include "Triangle_Color.h"
#include "Rect_Color.h"
#include "Rect_Texture.h"
#include "Cube_Texture.h"
#include "Terrain_Buffer.h"
#include "Renderer.h"
#include "Texture.h"
#include "Transform.h"
#include "Material.h"
#include "Picking.h"
#include "RectUI_Texture.h"
#include "Mesh_Static.h"
#include "Mesh_Dynamic.h"
#include "Collider.h"
#include "Navigation.h"
#include "Shader.h"
#include "Frustum.h"
#include "MultiTexture.h"
#include "Controller.h"
#include "MyEffect.h" // By.KJY
#include "MyEffect_Buff.h" // By.KJY
#include "MyEffect_Particle.h" // By.KJY
//#include "PlayerController.h"


BEGIN(Engine)

// 용도 : 객체들을 렌더링 또는 사용함에 있어 필요한 부품객체들을 모아서 관리한다.


class DLL_EXPORT CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Ready_Component(const _uint& iArrayIdx, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Clone_Component(const _uint& iArrayIdx, const _tchar* pComponentTag);
public:
	//텍스쳐 파일을 로드
	//HRESULT Load_Texture_ByMatFile(const _uint& iArrayIdx, const _tchar* pComponentTag, CComponent* pComponent, LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName, TEXTUREDATA* pOut);
public:
	HRESULT Reserve_ComponentContainer(const _uint& iMaxSize, LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT Release_Component(const _uint& iSceneIdx);
public:
	// 사용시 주의 할 것. Safe_Release() 로 지우기 때문에 다른곳에서 참조하는 중이라면 완전히 삭제되지 않는다.
	// 단지, map 컨테이너에서는 확실히 사라진다.
	HRESULT Delete_Component(const _uint& iArrayIdx, const _tchar* pComponentTag);
	
private:
	map<const _tchar*, CComponent*>*			m_pMapComponents = nullptr;
	typedef map<const _tchar*, CComponent*>		MAPCOMPONENTS;
private:
	_uint				m_iContainerSize = 0;
	_uint				m_iComponentTotalSize = 0;
public:
	CComponent* Find_Component(const _uint& iArrayIdx, const _tchar* pComponentTag);

public:
#ifndef _FINAL
	void Debug_Render();
	int iStartCount = 0;
#endif
private:
	virtual _ulong Free(void) final;


};

END