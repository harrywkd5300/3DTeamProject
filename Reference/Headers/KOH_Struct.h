#pragma once


namespace Engine
{
	enum OBJECT_STATE
	{
		BASE_OBJ,
		COL_OBJ,
		PARKURE_OBJ
	};

	typedef struct MapObjcet
	{
		TCHAR			ComName[128];
		TCHAR			FileName[128];
		TCHAR			InnateKey[128];
		TCHAR			FullPath[256];
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vScale;
		D3DXVECTOR3		vRotation;
		bool			bIsAlpha;
		bool			bStaticMesh;
		bool			bDrawShadow;

		OBJECT_STATE	Obj_State;
		D3DXMATRIX		LocalMatrix;
		// Add Option

	}MAPOBJECT;

	typedef struct NaviCell
	{
		unsigned int iAniIdx;
		bool		 bPakour;
		bool		 bMove;
	}NAVIOPTION;
	
}