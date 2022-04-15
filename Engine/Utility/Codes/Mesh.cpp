#include "..\Headers\Mesh.h"

USING(Engine)

CMesh::CMesh(LPDIRECT3DDEVICE9 pGraphicDev)
	: CResources(pGraphicDev)
{
#ifndef _FINAL
	Tag(L"CMesh", std::bind(&CMesh::Debug_Render, this));
#endif

}

CMesh::CMesh(const CMesh & rhs)
	: CResources(rhs)
{
#ifndef _FINAL
	Tag(L"CMesh", std::bind(&CMesh::Debug_Render, this));
#endif

}

HRESULT CMesh::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName)
{
	return NOERROR;
}


_bool CMesh::isAlpha(const char * pFileName, const char* pKeyWord)
{
	size_t iLength = strlen(pFileName);

	for (size_t i = iLength - 1; i >= 0; --i)
	{
		if ('.' == pFileName[i])
		{
			if (*pKeyWord == pFileName[--i])
				return true;
			else
				break;
		}
	}

	return false;
}

//const _tchar * CMesh::Convert_KeyWordTextureName(const char * pFileName, const char * pKeyWord)
//{
//	char		szFileName[128] = "";
//	char		szExtention[128] = "";
//
//	size_t iLength = strlen(pFileName);
//
//	for (size_t i = iLength - 1; i >= 0; --i)
//	{
//		if ('.' == pFileName[i])
//		{
//			memcpy(szFileName, pFileName, sizeof(char) * (i - 1));
//
//			strcat_s(szFileName, pKeyWord);
//
//			memcpy(szExtention, pFileName + i, sizeof(char) * (iLength - i));
//
//			strcat_s(szFileName, szExtention);
//
//			break;
//		}
//	}
//
//	_tchar			szFileName_Uni[128] = L"";
//
//	MultiByteToWideChar(CP_ACP, 0, szFileName, int(strlen(szFileName))
//		, szFileName_Uni, 128);
//
//	return szFileName_Uni;
//}
#ifndef _FINAL
void CMesh::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CMesh");

}
#endif
_ulong CMesh::Free(void)
{
	return CResources::Free();
}
