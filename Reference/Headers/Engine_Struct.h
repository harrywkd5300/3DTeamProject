#pragma once


namespace Engine
{

	enum class EStatus
	{
		Invalid,
		Success,
		Failure,
		Running,
		//Aborted,
	};
	
	//enum class EStatus :uint8_t
	//{
	//	Invalid,   
	//	Success,   
	//	Failure,   
	//	Running,  
	//	Aborted,  
	//};
	enum OBJ_TYPE { OBJ_TYPE_DYNAMIC, OBJ_TYPE_STATIC, OBJ_TYPE_END };

	enum eCOMPARE_TYPE
	{
		eCOMPARE_TYPE_LESS,
		eCOMPARE_TYPE_GREATER,
		eCOMPARE_TYPE_EQUAL,
		eCOMPARE_TYPE_NOT_EQUAL,
	};


	enum MATRIX_INFO {INFO_RIGHT, INFO_UP, INFO_LOOK};

	typedef struct tag_StructTexture
	{
		enum TEXTURETYPE { TEX_DIFFUSE, TEX_NORMAL, TEX_SPECULAR, TEX_END };

		LPDIRECT3DTEXTURE9	pTexture[TEX_END];

	}TEXTUREDATA;

	typedef struct MeshContainer_Derived : public D3DXMESHCONTAINER
	{
		TEXTUREDATA*				pMeshTexture;
		LPD3DXMESH					pOriginal_Mesh;
		D3DXVECTOR3					vMin, vMax;
		unsigned long				dwNumBones; //���� �޽ÿ� ������ ��ġ���ִ� ������ ����.
		D3DXMATRIX**				ppCombinedTransformationMatrices;
		D3DXMATRIX*					pOffsetMatrices;
		D3DXMATRIX*					pRenderingMatrices;
	}D3DXMESHCONTAINER_DERIVED;

	typedef struct Frame_Derived : public D3DXFRAME
	{
		// ������� TransformationMarix * �θ��� CombinedTransformafjdkalsjfkals
		D3DXMATRIX			matCombinedTransformationMatrix; 
		static Frame_Derived* Create(Frame_Derived* RootFrame)
		{
			if (RootFrame == nullptr)
				return nullptr;
			Frame_Derived* pNewFrame = new Frame_Derived();

			pNewFrame->matCombinedTransformationMatrix = RootFrame->matCombinedTransformationMatrix;
			pNewFrame->Name = RootFrame->Name;
			pNewFrame->pFrameFirstChild = Create((Frame_Derived*)RootFrame->pFrameFirstChild);
			pNewFrame->pFrameSibling = Create((Frame_Derived*)RootFrame->pFrameSibling);
			pNewFrame->pMeshContainer = RootFrame->pMeshContainer;
			pNewFrame->TransformationMatrix = RootFrame->TransformationMatrix;

			return pNewFrame;
		}
		static void Clone_Frame_Hierachy(Frame_Derived* SourRootFrame, Frame_Derived* DestRootFrame)
		{
			if (DestRootFrame == nullptr || SourRootFrame == nullptr)
				return;

			DestRootFrame->matCombinedTransformationMatrix = SourRootFrame->matCombinedTransformationMatrix;
			DestRootFrame->Name = SourRootFrame->Name;
			Clone_Frame_Hierachy((Frame_Derived*)SourRootFrame->pFrameFirstChild, (Frame_Derived*)DestRootFrame->pFrameFirstChild);
			Clone_Frame_Hierachy((Frame_Derived*)SourRootFrame->pFrameSibling, (Frame_Derived*)DestRootFrame->pFrameSibling);
			DestRootFrame->pMeshContainer = SourRootFrame->pMeshContainer;
			DestRootFrame->TransformationMatrix = SourRootFrame->TransformationMatrix;

			return;
		}
		static void DestroyFrame(Frame_Derived* RootFrame)
		{
			if (RootFrame->pFrameFirstChild != nullptr)
			{
				DestroyFrame((Frame_Derived*)RootFrame->pFrameFirstChild);
				RootFrame->pFrameFirstChild = nullptr;
			}
			if (RootFrame->pFrameSibling != nullptr)
			{
				DestroyFrame((Frame_Derived*)RootFrame->pFrameSibling);
				RootFrame->pFrameSibling = nullptr;
			}
			Safe_Delete(RootFrame);
		}
	}D3DXFRAME_DERIVED;
	

	typedef struct Material_Derived : public D3DXMATERIAL
	{
		bool isAlpha;
	}D3DXMATERIAL_DERIVED;

	// For.Vertex Struct
	typedef struct Vertex_Color
	{		
		D3DXVECTOR3		vPosition;	
		unsigned long	dwColor;
	}VTXCOL;

	typedef struct Vertex_Texture_ViewPort
	{
		enum {FVF = D3DFVF_XYZRHW | D3DFVF_TEX1};
		D3DXVECTOR4		vPosition; // D3DFVF_XYZRHW(��� ������������������ ��ģ ������ ���� ��ġ)
		D3DXVECTOR2		vTexUV; // D3DFVF_TEX1
	}VTXTEX_VIEWPORT;

	typedef struct Vertex_Texture
	{
		D3DXVECTOR3		vPosition; // D3DFVF_XYZ(3���� ���ý����̽� ���� ��ǥ)
		D3DXVECTOR2		vTexUV; // D3DFVF_TEX1

		enum { FVF = D3DFVF_XYZ | D3DFVF_TEX1 };
	}VTXTEX;

	typedef struct Vertex_CubeTexture
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vTexUV;
	}VTXCUBETEX;

	typedef struct Vertex_Texture_Normal
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		D3DXVECTOR2		vTexUV;
	}VTXTEXNORMAL;

	typedef struct Vertex_Texture_TBN
	{
		enum {
			FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEX2
			| D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEXCOORDSIZE3(2)
		};
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vNormal;
		D3DXVECTOR2		vTexUV;
		D3DXVECTOR3		vBinormal;
		D3DXVECTOR3		vTangent;
	}VTXTEXTBN;

	// For.Index Struct
	typedef struct Index_16
	{
		unsigned short _0, _1, _2;
	}INDEX16;

	typedef struct Index_32
	{
		unsigned long _0, _1, _2;
	}INDEX32;

	typedef struct BoneFrameInfo	// Update_CombinedTransformaitionMatrix() ���� ����Ѵ�
	{
		D3DXFRAME_DERIVED* pFrame = nullptr;
		const _matrix* pParentCombinedMatrix = nullptr;
		BoneFrameInfo()
			:pFrame(nullptr)
			, pParentCombinedMatrix(nullptr)
		{}
		BoneFrameInfo(BoneFrameInfo& rhs)
		{
			pFrame = rhs.pFrame;
			pParentCombinedMatrix = rhs.pParentCombinedMatrix;
		}
		BoneFrameInfo(D3DXFRAME_DERIVED* pFrame, const _matrix* pParentCombinedMatrix)
		{
			this->pFrame = pFrame;
			this->pParentCombinedMatrix = pParentCombinedMatrix;
		}
	}BONESTACK;


	// ĳ���� ������Ͽ� ���ǵǾ� �ִ�.
	typedef struct tag_CharacterInfo CHARINFO;

	
}