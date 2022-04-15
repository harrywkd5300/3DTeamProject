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
	//피킹된 경우 피킹된 월드스페이스 포지션 벡터가 vOutPos에 채워지고 bool값 반환
	bool Picking_ToBuffer(LPDIRECT3DDEVICE9	pGraphicDev,HWND hWnd, _vec3* vOutPos, Engine::CVIBuffer * pTargetBuffer, const _matrix* pWorldMatrix = nullptr);
	//피킹된 경우 피킹된 월드스페이스 포지션 벡터가 vOutPos에, 거리값이 pfDist에 채워지고 bool 반환
	bool Picking_ToMesh(LPDIRECT3DDEVICE9 pGraphicDev, HWND hWnd, _vec3* vOutPos, _float* pfDist, LPD3DXMESH* ppMesh, const _matrix* pWorldMatrix);
	//피킹된 경우 bool 반환
	bool Chekc_Pick_ToMesh(LPDIRECT3DDEVICE9 pGraphicDev,HWND hWnd, LPD3DXMESH* ppMesh, const _matrix * pWorldMatrix);
private:
	virtual _ulong Free();
};

END