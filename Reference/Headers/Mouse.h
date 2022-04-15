#pragma once

#include "Engine_Defines.h"
#include "base.h"

BEGIN(Engine)
class CVIBuffer;
class DLL_EXPORT CMouse :public CBase
{
	DECLARE_SINGLETON(CMouse)
public:
	enum class SPACE_TYPE{ViewPort, ProjectionSpace, ViewSpace, WorldSpace};
private:
	explicit CMouse();
	virtual ~CMouse() = default;

public:
	_vec3 Get_MousePos(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd, SPACE_TYPE eType);
	//��ŷ�� ��� ��ŷ�� ���彺���̽� ������ ���Ͱ� vOutPos�� ä������ bool�� ��ȯ
	bool Picking_ToBuffer(LPDIRECT3DDEVICE9	pGraphicDev,HWND hWnd, _vec3* vOutPos, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix = nullptr);
	//��ŷ�� ��� ��ŷ�� ���彺���̽� ������ ���Ͱ� vOutPos��, �Ÿ����� pfDist�� ä������ bool ��ȯ
	bool Picking_ToMesh(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd, _vec3* vOutPos, _float* pfDist, LPD3DXMESH* ppMesh, const _matrix* pWorldMatrix);
	//��ŷ�� ��� bool ��ȯ
	bool Chekc_Pick_ToMesh(LPDIRECT3DDEVICE9 pGraphicDev,HWND hWnd, LPD3DXMESH* ppMesh, const _matrix * pWorldMatrix);
private:
	virtual _ulong Free();
};

END