#include "..\Headers\Hierarchy_Loader.h"
#include "atlstr.h"
#include "String_Manager.h"

USING(Engine)

CHierarchy_Loader::CHierarchy_Loader(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pPath)
	: m_pGraphicDev(pGraphicDev)
	, m_pPath(pPath)
{

}


CHierarchy_Loader::~CHierarchy_Loader()
{
}

STDMETHODIMP CHierarchy_Loader::CreateFrame(LPCSTR Name /*Input*/, LPD3DXFRAME * ppNewFrame /*Output*/)
{
	D3DXFRAME_DERIVED*		pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	if(nullptr != Name)
		Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->matCombinedTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);
		
	*ppNewFrame = pNewFrame;

	return NOERROR;	
}

STDMETHODIMP CHierarchy_Loader::CreateMeshContainer(LPCSTR Name
	, CONST D3DXMESHDATA * pMeshData
	, CONST D3DXMATERIAL * pMaterials
	, CONST D3DXEFFECTINSTANCE * pEffectInstances
	, DWORD NumMaterials
	, CONST DWORD * pAdjacency
	, LPD3DXSKININFO pSkinInfo	
	, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*			pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	// 이름을 채운다.
	if (nullptr != Name)
		Allocate_Name(&pNewMeshContainer->Name, Name);

	// 메시에 대한 정보를 채운다.
	if (nullptr == pMeshData)
		return E_FAIL;

	pNewMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	pNewMeshContainer->MeshData.pMesh = pMeshData->pMesh;
	pNewMeshContainer->MeshData.pMesh->AddRef();

	// 사용의 편리함을 위해 지역변수에 잠시 담아둔다.
	LPD3DXMESH			pMesh = pNewMeshContainer->MeshData.pMesh;
	pMesh->AddRef();

	_ulong dwNumFaces = pMesh->GetNumFaces();

	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	D3DVERTEXELEMENT9			Vertex_Element[MAX_FVF_DECL_SIZE];
	pMesh->GetDeclaration(Vertex_Element);

	//_ulong dwMeshFVF = pMesh->GetFVF();

	bool		bHaveNormal = false;
	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_NORMAL == Vertex_Element[i].Usage)
		{
			bHaveNormal = true;
			break;
		}
	}

	if (!bHaveNormal)
		D3DXComputeNormals(pMesh, pNewMeshContainer->pAdjacency);

	_ushort		wOffset = 0;
	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_POSITION == Vertex_Element[i].Usage)
		{
			wOffset = Vertex_Element[i].Offset;
			break;
		}
	}

	void*		pVertex = nullptr;

	pMesh->LockVertexBuffer(0, &pVertex);

	_ulong dwNumVertices = pMesh->GetNumVertices();

	_uint iStride = D3DXGetDeclVertexSize(Vertex_Element, 0);

	D3DXComputeBoundingBox((_vec3*)((_byte*)pVertex + wOffset), dwNumVertices, iStride, &pNewMeshContainer->vMin, &pNewMeshContainer->vMax);

	pMesh->UnlockVertexBuffer();

	pNewMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pNewMeshContainer->pMaterials = new D3DXMATERIAL_DERIVED[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * pNewMeshContainer->NumMaterials);

	pNewMeshContainer->pMeshTexture = new TEXTUREDATA[pNewMeshContainer->NumMaterials];
	ZeroMemory(pNewMeshContainer->pMeshTexture, sizeof(TEXTUREDATA) * pNewMeshContainer->NumMaterials);

	if (0 != NumMaterials)
	{

		for (size_t i = 0; i < pNewMeshContainer->NumMaterials; ++i)
		{
			pNewMeshContainer->pMaterials[i].MatD3D = pMaterials[i].MatD3D;
			pNewMeshContainer->pMaterials[i].pTextureFilename = CString_Manager::GetInstance()->GetStr(pMaterials[i].pTextureFilename);

			_tchar			szFullPath[128] = L"";
			_tchar			szFileName[128] = L"";


			lstrcpy(szFullPath, m_pPath);

			//((D3DXMATERIAL_DERIVED*)&pNewMeshContainer->pMaterials[i])->isAlpha = isAlpha(pNewMeshContainer->pMaterials[i].pTextureFilename, "A");
			((D3DXMATERIAL_DERIVED*)&pNewMeshContainer->pMaterials[i])->isAlpha = false;

			MultiByteToWideChar(CP_ACP, 0, pNewMeshContainer->pMaterials[i].pTextureFilename, _ulong(strlen(pNewMeshContainer->pMaterials[i].pTextureFilename))
				, szFileName, 128);

			lstrcat(szFullPath, szFileName);
			lstrcat(szFullPath, L".mat");

			HANDLE		hFile = CreateFile(szFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (0 == hFile)
				return false;

			_ulong			dwByte = 1;
			_ulong			dwNumCell = 0;
			_vec3			vPoint[3];
			_ulong			iIndex = 0;

			char			szTextureFileName[256] = "";
			_tchar			tzTextureFileName[128] = L"";
			int iCount = 0;
			while (dwByte != 0)
			{
				ReadFile(hFile, (char*)(szTextureFileName + iCount), sizeof(char), &dwByte, nullptr);
				++iCount;
			}

			CString TextureFileName = szTextureFileName;
			int iTexNameIndex = 0;
			int CheckFindIndex = 0;
			// ------------- Diffuse Texture ------------- 
			CheckFindIndex = TextureFileName.Find(L"Diffuse=", iTexNameIndex);
			if (CheckFindIndex == -1)
			{
				//return E_FAIL;
			}
			else
			{
				iTexNameIndex = CheckFindIndex + 8;
				lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
				lstrcat(tzTextureFileName, L".tga");
				lstrcpy(szFullPath, m_pPath);
				lstrcat(szFullPath, tzTextureFileName);
				D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_DIFFUSE]);
			}

			//MultiByteToWideChar(CP_ACP, 0, m_pSubsetInfo[i].pTextureFilename, strlen(m_pSubsetInfo[i].pTextureFilename)
			//	, tzTextureFileName, 128);
			((D3DXMATERIAL_DERIVED*)&pNewMeshContainer->pMaterials[i])->isAlpha = false;

			// ------------- Normal Texture ------------- 
			CheckFindIndex = TextureFileName.Find(L"Normal=", iTexNameIndex);
			if (CheckFindIndex == -1)
			{

			}
			else
			{
				iTexNameIndex = CheckFindIndex + 7;
				lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
				lstrcat(tzTextureFileName, L".tga");
				lstrcpy(szFullPath, m_pPath);
				lstrcat(szFullPath, tzTextureFileName);
				D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_NORMAL]);
			}
			// ------------- Specualr Texture ------------- 
			CheckFindIndex = TextureFileName.Find(L"Specular=", iTexNameIndex);
			if (CheckFindIndex == -1)
			{

			}
			else
			{
				iTexNameIndex = CheckFindIndex + 9;
				lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
				lstrcat(tzTextureFileName, L".tga");
				lstrcpy(szFullPath, m_pPath);
				lstrcat(szFullPath, tzTextureFileName);
				D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &pNewMeshContainer->pMeshTexture[i].pTexture[TEXTUREDATA::TEX_SPECULAR]);
			}

			CloseHandle(hFile);
		}
	}

	if (nullptr == pSkinInfo)
		return E_FAIL;

	pMesh->CloneMesh(pMesh->GetOptions(), Vertex_Element, m_pGraphicDev, &pNewMeshContainer->pOriginal_Mesh);
	//pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF(), m_pGraphicDev, &pNewMeshContainer->pOriginal_Mesh);	

	pNewMeshContainer->pSkinInfo = pSkinInfo;
	pNewMeshContainer->pSkinInfo->AddRef();

	pNewMeshContainer->pSkinInfo->SetDeclaration( Vertex_Element );

	pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones();

	pNewMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pNewMeshContainer->dwNumBones];
	pNewMeshContainer->pOffsetMatrices = new _matrix[pNewMeshContainer->dwNumBones];
	pNewMeshContainer->pRenderingMatrices = new _matrix[pNewMeshContainer->dwNumBones];

	for (_ulong i = 0; i < pNewMeshContainer->dwNumBones; i++)
	{
		pNewMeshContainer->pOffsetMatrices[i] = *(pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
	}

	Engine::Safe_Release(pMesh);

	*ppNewMeshContainer = pNewMeshContainer;

	return NOERROR;
}


_bool CHierarchy_Loader::isAlpha(const char * pFileName, const char* pKeyWord)
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

//const _tchar * CHierarchy_Loader::Convert_KeyWordTextureName(const char * pFileName, const char * pKeyWord)
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


STDMETHODIMP CHierarchy_Loader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
	{
		DestroyMeshContainer(pFrameToFree->pMeshContainer);
	}

	if (nullptr != pFrameToFree->pFrameFirstChild)	
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	Safe_Delete(pFrameToFree);

	return NOERROR;
}

STDMETHODIMP CHierarchy_Loader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		for (size_t j = 0; j < TEXTUREDATA::TEX_END; ++j)
		{
			if(nullptr != pMeshContainer->pMeshTexture[i].pTexture[j])
				Safe_Release(pMeshContainer->pMeshTexture[i].pTexture[j]);
		}
	}

	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);
	Safe_Release(pMeshContainer->pSkinInfo);

	Safe_Delete_Array(pMeshContainer->pMeshTexture);
	Safe_Release(pMeshContainer->pOriginal_Mesh);
	Safe_Delete_Array(pMeshContainer->pOffsetMatrices);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrices);
	Safe_Delete_Array(pMeshContainer->ppCombinedTransformationMatrices);

	Safe_Delete(pMeshContainer);

	return NOERROR;
}

HRESULT CHierarchy_Loader::Allocate_Name(char** ppDestName, const char* const pName)
{
	size_t iLength = strlen(pName);

	*ppDestName = new char[iLength + 1];

	strcpy_s(*ppDestName, iLength+1, pName);

	return NOERROR;
}

CHierarchy_Loader * CHierarchy_Loader::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath)
{
	CHierarchy_Loader *	pInstance = new CHierarchy_Loader(pGraphicDev, pFilePath);

	return pInstance;
}