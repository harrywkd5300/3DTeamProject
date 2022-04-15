#include "..\Headers\Resources.h"

USING(Engine)

CResources::CResources(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CResources", std::bind(&CResources::Debug_Render, this));
#endif

}

CResources::CResources(const CResources & rhs)
	: CComponent(rhs)
{
#ifndef _FINAL
	Tag(L"CResources", std::bind(&CResources::Debug_Render, this));
#endif

}

CResources::~CResources()
{
}
#ifndef _FINAL
void CResources::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CResources");

}
#endif
_ulong CResources::Free(void)
{
	return CComponent::Free();
}
