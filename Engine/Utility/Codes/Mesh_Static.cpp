#include "..\Headers\Mesh_Static.h"
#include "atlstr.h"

USING(Engine)

CMesh_Static::CMesh_Static(LPDIRECT3DDEVICE9 pGraphicDev)
	: CMesh(pGraphicDev)
	, m_pAdjancey(nullptr)
	, m_pSubsets(nullptr)
{
#ifndef _FINAL
	Tag(L"CMesh_Static", std::bind(&CMesh_Static::Debug_Render, this));
#endif

}

CMesh_Static::CMesh_Static(const CMesh_Static & rhs)
	: CMesh(rhs)
	, m_pAdjancey(rhs.m_pAdjancey)
	, m_pSubsets(rhs.m_pSubsets)
	, m_dwNumSubsets(rhs.m_dwNumSubsets)
	, m_pMesh(rhs.m_pMesh)
	, m_pSubsetInfo(rhs.m_pSubsetInfo)	
	, m_ppTextures(new TEXTUREDATA[m_dwNumSubsets])
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)

{
	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		m_ppTextures[i] = rhs.m_ppTextures[i];

		for (size_t j = 0; j < TEXTUREDATA::TEX_END; ++j)
		{
			if(nullptr != m_ppTextures[i].pTexture[j])
				m_ppTextures[i].pTexture[j]->AddRef();
		}		
	}

	m_pMesh->AddRef();

	LPDIRECT3DTEXTURE9*				m_ppTextures = nullptr;
#ifndef _FINAL
	Tag(L"CMesh_Static", std::bind(&CMesh_Static::Debug_Render, this));
#endif

}

HRESULT CMesh_Static::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName)
{
	if (nullptr == m_pGraphicDev)
		goto FAILED_STATIC_MESH;

	_tchar			szFullPath[128] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	//LPD3DXMESH tmpMesh = nullptr;

	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphicDev
		, &m_pAdjancey, &m_pSubsets, nullptr, &m_dwNumSubsets, &m_pMesh)))
	{
		goto FAILED_STATIC_MESH;
		//return E_FAIL;
	}

	m_pSubsetInfo = new D3DXMATERIAL_DERIVED[m_dwNumSubsets];
	m_ppTextures = new TEXTUREDATA[m_dwNumSubsets];
	ZeroMemory(m_ppTextures, sizeof(TEXTUREDATA) * m_dwNumSubsets);

	D3DVERTEXELEMENT9			Vertex_Element[MAX_FVF_DECL_SIZE];
	m_pMesh->GetDeclaration(Vertex_Element);


	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		D3DXMATERIAL*	pSubset = (D3DXMATERIAL*)m_pSubsets->GetBufferPointer();

		if (pSubset->pTextureFilename == nullptr)
		{
			break;
		}
		m_pSubsetInfo[i].MatD3D = pSubset[i].MatD3D;
		m_pSubsetInfo[i].pTextureFilename = pSubset[i].pTextureFilename;

		_tchar			szFileName[128] = L"";


		lstrcpy(szFullPath, pFilePath);

		MultiByteToWideChar(CP_ACP, 0, m_pSubsetInfo[i].pTextureFilename, int(strlen(m_pSubsetInfo[i].pTextureFilename))
			, szFileName, 128);
		lstrcat(szFullPath, szFileName);
		lstrcat(szFullPath, L".mat");

		HANDLE		hFile = CreateFile(szFullPath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return false;

		_ulong			dwByte = 0;
		_ulong			dwNumCell = 0;
		_vec3			vPoint[3];
		_ulong			iIndex = 0;

		char			szTextureFileName[256] = "";
		_tchar			tzTextureFileName[256] = L"";
		ReadFile(hFile, &szTextureFileName, sizeof(char) * 256, &dwByte, nullptr);
		CString TextureFileName = szTextureFileName;
		int iTexNameIndex = 0;
		int CheckFindIndex = 0;
		// ------------- Diffuse Texture ------------- 
		CheckFindIndex = TextureFileName.Find(L"Diffuse=", iTexNameIndex);
		if (CheckFindIndex == -1)
		{
			//goto FAILED_STATIC_MESH;
		}
		else
		{
			iTexNameIndex = CheckFindIndex + 8;
			lstrcpy(tzTextureFileName, TextureFileName.Mid(iTexNameIndex, TextureFileName.Find(L"\n", iTexNameIndex) - iTexNameIndex));
			lstrcat(tzTextureFileName, L".tga");
			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, tzTextureFileName);
			D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE]);
		}

		//MultiByteToWideChar(CP_ACP, 0, m_pSubsetInfo[i].pTextureFilename, strlen(m_pSubsetInfo[i].pTextureFilename)
		//	, tzTextureFileName, 128);
		m_pSubsetInfo[i].isAlpha = false;

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
			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, tzTextureFileName);
			D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTextures[i].pTexture[TEXTUREDATA::TEX_NORMAL]);
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
			lstrcpy(szFullPath, pFilePath);
			lstrcat(szFullPath, tzTextureFileName);
			D3DXCreateTextureFromFile(m_pGraphicDev, szFullPath, &m_ppTextures[i].pTexture[TEXTUREDATA::TEX_SPECULAR]);
		}

		CloseHandle(hFile);

	}

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

	m_pMesh->LockVertexBuffer(0, &pVertex);

	_ulong dwNumVertices = m_pMesh->GetNumVertices();
	_uint iStride = D3DXGetDeclVertexSize(Vertex_Element, 0);

	D3DXComputeBoundingBox((_vec3*)((_byte*)pVertex + wOffset), dwNumVertices, iStride, &m_vMin, &m_vMax);

	m_pMesh->UnlockVertexBuffer();

	return NOERROR;

FAILED_STATIC_MESH:
	//char cTemp[256] ="";
	//int strLen = lstrlen(pFileName);
	//WideCharToMultiByte(CP_ACP, 0, pFileName, strLen, cTemp, 256, NULL, NULL);
	MessageBox(0, pFileName, TEXT("System Error"), MB_OK);
	return E_FAIL;
}

void CMesh_Static::Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_DiffuseTex_Name, const char* pConstant_NormalTex_Name, const char* pConstant_SpecularTex_Name)
{
	if(nullptr == m_pMesh
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pSubsetInfo)
		return;

	_uint		iPassIdx = 0;

	for(_ulong i = 0; i < m_dwNumSubsets; ++i)
	{
		//	m_pGraphicDev->SetMaterial(&m_pSubsetInfo[i].MatD3D);
		iPassIdx = 0;

		if ( nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE] )
			pEffect->SetTexture( pConstant_DiffuseTex_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE] );
		else
			continue;
		if ( nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_NORMAL] )
		{
			iPassIdx |= 0x1;
			pEffect->SetTexture( pConstant_NormalTex_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_NORMAL] );
		}
		if ( nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_SPECULAR] )
		{
			iPassIdx |= 0x2;
			pEffect->SetTexture( pConstant_SpecularTex_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_SPECULAR] );
		}

		if ( true == m_pSubsetInfo[i].isAlpha )
			iPassIdx |= 0x4;
			
		pEffect->CommitChanges();
		pEffect->BeginPass( iPassIdx );

		m_pMesh->DrawSubset( i );

		pEffect->EndPass();
	}

}

void CMesh_Static::Render_Mesh(LPD3DXEFFECT pEffect, const char * pConstant_DiffuseTex_Name, const char * pConstant_NormalTex_Name, const char * pConstant_SpecularTex_Name, _bool isAlpha)
{
	if (nullptr == m_pMesh
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pSubsetInfo)
		return;

	_uint		iPassIdx = 0;

	for (_ulong i = 0; i < m_dwNumSubsets; ++i)
	{
		//	m_pGraphicDev->SetMaterial(&m_pSubsetInfo[i].MatD3D);
		iPassIdx = 0;

		if (nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE])
			pEffect->SetTexture(pConstant_DiffuseTex_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE]);
		else
			continue;
		if (nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_NORMAL])
		{
			iPassIdx |= 0x1;
			pEffect->SetTexture(pConstant_NormalTex_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_NORMAL]);
		}
		if (nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_SPECULAR])
		{
			iPassIdx |= 0x2;
			pEffect->SetTexture(pConstant_SpecularTex_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_SPECULAR]);
		}

		if (true == isAlpha)
			iPassIdx |= 0x4;

		pEffect->CommitChanges();
		pEffect->BeginPass(iPassIdx);

		m_pMesh->DrawSubset(i);

		pEffect->EndPass();
	}
}

void CMesh_Static::Render_Mesh(LPD3DXEFFECT pEffect, const char* pConstant_Texture_Name)
{
	if (nullptr == m_pMesh
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pSubsetInfo)
		return;

	for (_ulong i = 0; i < m_dwNumSubsets; ++i)
	{
		//	m_pGraphicDev->SetMaterial(&m_pSubsetInfo[i].MatD3D);

		pEffect->SetTexture(pConstant_Texture_Name, m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE]);

		_uint		iPassIdx = 0;

		if (true == m_pSubsetInfo[i].isAlpha)
			iPassIdx = 1;
		else
			iPassIdx = 0;

		pEffect->BeginPass(iPassIdx);

		m_pMesh->DrawSubset(i);

		pEffect->EndPass();
	}

}

void CMesh_Static::Render_Mesh(void)
{
	if (nullptr == m_pMesh
		|| nullptr == m_pGraphicDev
		|| nullptr == m_pSubsetInfo)
		return;

	for (_ulong i = 0; i < m_dwNumSubsets; ++i)
	{
		m_pMesh->DrawSubset(i);
	}	
}


CComponent * CMesh_Static::Clone(void)
{
	return new CMesh_Static(*this);
}

CMesh_Static * CMesh_Static::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, const _tchar * pFileName)
{
	CMesh_Static *	pInstance = new CMesh_Static(pGraphicDev);

	if (FAILED(pInstance->Ready_Mesh(pFilePath, pFileName)))
	{
		MessageBox(0, L"CMesh_Static Created Failed", nullptr, MB_OK);
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}
#ifndef _FINAL
void CMesh_Static::Debug_Render()
{
	CDebug::GetInstance()->Add_DebugText("CMesh_Static");
	CDebug::GetInstance()->Add_DebugText("m_dwNumSubsets: %d", (int)m_dwNumSubsets);
	CDebug::GetInstance()->Add_DebugText("-Mesh Declaration-", (int)m_dwNumSubsets);

	D3DVERTEXELEMENT9			Vertex_Element[MAX_FVF_DECL_SIZE];
	m_pMesh->GetDeclaration(Vertex_Element);
	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (D3DDECLUSAGE_POSITION == Vertex_Element[i].Usage)
		{
			CDebug::GetInstance()->Add_DebugText("D3DDECLUSAGE_POSITION");
		}
		if (D3DDECLUSAGE_TEXCOORD == Vertex_Element[i].Usage)
		{
			CDebug::GetInstance()->Add_DebugText("D3DDECLUSAGE_TEXCOORD");
		}
		if (D3DDECLUSAGE_NORMAL== Vertex_Element[i].Usage)
		{
			CDebug::GetInstance()->Add_DebugText("D3DDECLUSAGE_NORMAL");
		}
		if (D3DDECLUSAGE_BINORMAL == Vertex_Element[i].Usage)
		{
			CDebug::GetInstance()->Add_DebugText("D3DDECLUSAGE_BINORMAL");
		}
		if (D3DDECLUSAGE_TANGENT == Vertex_Element[i].Usage)
		{
			CDebug::GetInstance()->Add_DebugText("D3DDECLUSAGE_TANGENT");
		}
		if (D3DDECLUSAGE_COLOR == Vertex_Element[i].Usage)
		{
			CDebug::GetInstance()->Add_DebugText("D3DDECLUSAGE_COLOR");
		}
	}

	CDebug::GetInstance()->Add_DebugText("m_vMin: %f %f %f", m_vMin.x, m_vMin.y, m_vMin.z);
	CDebug::GetInstance()->Add_DebugText("m_vMax: %f %f %f", m_vMax.x, m_vMax.y, m_vMax.z);
	
	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		CDebug::GetInstance()->Add_DebugText("-- %s --", m_pSubsetInfo[i].pTextureFilename);
		if(m_pSubsetInfo[i].isAlpha)
			CDebug::GetInstance()->Add_DebugText("ALPHA");
		else
			CDebug::GetInstance()->Add_DebugText("NONE_ALPHA");

		if (nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_DIFFUSE])
			CDebug::GetInstance()->Add_DebugText("OK_DIFFUSE");
		else
			CDebug::GetInstance()->Add_DebugText("NO_DIFFUSE");

		if (nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_NORMAL])
			CDebug::GetInstance()->Add_DebugText("OK_NORMAL");
		else
			CDebug::GetInstance()->Add_DebugText("NO_NORMAL");

		if (nullptr != m_ppTextures[i].pTexture[TEXTUREDATA::TEX_SPECULAR])
			CDebug::GetInstance()->Add_DebugText("OK_SPECULAR");
		else
			CDebug::GetInstance()->Add_DebugText("NO_SPECULAR");
	}
}
#endif

_ulong CMesh_Static::Free(void)
{
	if (false == m_isClone)	
		Safe_Delete_Array(m_pSubsetInfo);

	
	for (size_t i = 0; i < m_dwNumSubsets; ++i)
	{
		for (size_t j = 0; j < TEXTUREDATA::TEX_END; ++j)
		{
			Safe_Release(m_ppTextures[i].pTexture[j]);			
		}		
	}

	Safe_Delete_Array(m_ppTextures);

	Safe_Release(m_pMesh);

	return CMesh::Free();
}


