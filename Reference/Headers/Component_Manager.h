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

// �뵵 : ��ü���� ������ �Ǵ� ����Կ� �־� �ʿ��� ��ǰ��ü���� ��Ƽ� �����Ѵ�.


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
	//�ؽ��� ������ �ε�
	//HRESULT Load_Texture_ByMatFile(const _uint& iArrayIdx, const _tchar* pComponentTag, CComponent* pComponent, LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, const _tchar* pFileName, TEXTUREDATA* pOut);
public:
	HRESULT Reserve_ComponentContainer(const _uint& iMaxSize, LPDIRECT3DDEVICE9 pGraphicDev);
	HRESULT Release_Component(const _uint& iSceneIdx);
public:
	// ���� ���� �� ��. Safe_Release() �� ����� ������ �ٸ������� �����ϴ� ���̶�� ������ �������� �ʴ´�.
	// ����, map �����̳ʿ����� Ȯ���� �������.
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