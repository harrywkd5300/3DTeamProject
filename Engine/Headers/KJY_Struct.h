#pragma once

#define	SHADER_EFFECT_DEFAULT		0
#define	SHADER_EFFECT_CUTBLACK		1
#define	SHADER_EFFECT_BUFFERNORMAL	2
#define	SHADER_EFFECT_MASK			3
#define	SHADER_EFFECT_MESH			4
#define	SHADER_EFFECT_PARTICLE		5
#define	SHADER_EFFECT_DISTORTION	6

#define	PARTICLE_BUFFER_SIZE 2048
#define	PARTICLE_BATCH_SIZE 512

using namespace std;

namespace Engine
{
	// MFC 파싱용 //////////////////////////////////////////////////
	typedef struct tagImagePath
	{
		wstring		wstrObjKey;		//이미지의 종류(ex.Player, Monster...)
		wstring		wstrStateKey;	//이미지의 상태(Att, Idle, Run...)
		wstring		wstrFileName;
		wstring		wstrPath;		//이미지의 경로
		int			iCount;			//이미지 장수

		tagImagePath(void)
			:wstrObjKey(L"")
			, wstrStateKey(L"")
			, wstrFileName(L"")
			, wstrPath(L"")
			, iCount(0)
		{}

	}EFFECT_PATH;

	// 파티클 정점 구조체 /////////////////////////////////////////
	typedef struct Vertex_Particle
	{
		enum {
			FVF = D3DFVF_XYZ | D3DFVF_PSIZE | D3DFVF_DIFFUSE | D3DFVF_TEX1
		};

		D3DXVECTOR3		vPosition;
		float			fSize;
		D3DCOLOR		vColor;
		D3DXVECTOR2		vTexUV;

	}VTXPTC;

	// 파티클의 속성 //////////////////////////////////////////////
	typedef struct tagParticle_Attribute
	{
		D3DXVECTOR3		vPosition;
		D3DXVECTOR3		vVelocity;
		D3DXVECTOR3		vVelociy_Origin;
		D3DXVECTOR3		vAcceleration;
		float			fAge;
		float			fLifeTime;
		float			fSize;
		float			fSize_Origin;
		D3DXCOLOR		vColor;
		D3DXCOLOR		vColor_Fade;

		//D3DXMATRIX		matParent;

		bool			bIsAlive;

		tagParticle_Attribute()
			: vPosition(D3DXVECTOR3(0.f, 0.f, 0.f))
			, vVelocity(D3DXVECTOR3(0.f, 0.f, 0.f))
			, vAcceleration(D3DXVECTOR3(0.f, 0.f, 0.f))
			, fAge(0.f)
			, fLifeTime(0.f)
			, fSize(0.f)
			, fSize_Origin(0.f)
			, vColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
			, vColor_Fade(D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
		{
			//D3DXMatrixIdentity(&matParent);
		}

		// 파티클 복사 생성자
		tagParticle_Attribute(const tagParticle_Attribute& rhs)
			: vPosition(rhs.vPosition)
			, vVelocity(rhs.vVelocity)
			, vAcceleration(rhs.vAcceleration)
			, fAge(rhs.fAge)
			, fLifeTime(rhs.fLifeTime)
			, fSize(rhs.fSize)
			, vColor(rhs.vColor)
			, vColor_Fade(rhs.vColor_Fade)
			//, matParent(rhs.matParent)
		{ }

	}PTC_ATTRIBUTE;

	////////////////////////////////////////////////////////////////
	typedef struct tagParticle_Info
	{
		//PTC_ATTRIBUTE	tAttribute;

		D3DXVECTOR3		vOrigin;
		
		D3DXVECTOR3		vMin;
		D3DXVECTOR3		vMax;

		float	fRatePerSec;		// 초당 생성 개수
		float	fLoopTime;

		float	fCurRate;

		bool	bIsLooping;
		bool	bIsFirstUpdate;

		unsigned long	dwMaxCnt;

		DWORD		dwSize;			// 버텍스 버퍼가 보관할 수 있는 파티클의 수 - 2048
		DWORD		dwOffset;		// 다음 단계로의 오프셋(파티클 단위)
		DWORD		dwBatchSize;	// 하나의 단계에 정의된 파티클의 수

		D3DXMATRIX	matParent;

		tagParticle_Info()
			: vOrigin(D3DXVECTOR3(0.f, 0.f, 0.f))
			, vMin(D3DXVECTOR3(0.f, 0.f, 0.f))
			, vMax(D3DXVECTOR3(0.f, 0.f, 0.f))
			, fRatePerSec(0.f)
			, fLoopTime(0.f)
			, fCurRate(0.f)
			, bIsLooping(false)
			, bIsFirstUpdate(true)
			, dwMaxCnt(0)
			, dwSize(0)
			, dwOffset(0)
			, dwBatchSize(0)
		{
			D3DXMatrixIdentity(&matParent);
		}

	}PTC_INFO;

	typedef struct tagFX_Attribute
	{
		_vec3		vPosition;

		_vec3		vRotLocal;
		_vec3		vRotWorld;

		_vec3		vVelocity;
		_vec3		vVelociy_Origin;

		_float		fSpeed;

		_vec3		vAcceleration;

		_vec3		vAngVelLocal;
		_vec3		vAngVelWorld;

		_vec3		vScale;
		_vec3		vScale_Origin;

		D3DXCOLOR	vColor;
		D3DXCOLOR	vColor_Origin;

		_vec2		vUVSpeed[5];

		_float		fLifeTime;
		_float		fAge;

		// For.Sprite ------------//
		_vec2		vSlice; // 이펙트 구조체에 있지만 텍스쳐번수가 다름
		_vec2		vUVOffset;

		_float		fCurFrame;
		_float		fTotalFrame;
		_float		fSpriteSpd;
		// -----------------------//

		//D3DXMATRIX	matParent;

		// -----------------------//

		tagFX_Attribute()
			: vRotLocal(_vec3(0.f, 0.f, 0.f))
			, vRotWorld(_vec3(0.f, 0.f, 0.f))
			, vVelocity(_vec3(0.f, 0.f, 0.f))
			, vVelociy_Origin(_vec3(0.f, 0.f, 0.f))
			, fSpeed(0.f)
			, vAcceleration(_vec3(0.f, 0.f, 0.f))
			, vAngVelLocal(_vec3(0.f, 0.f, 0.f))
			, vAngVelWorld(_vec3(0.f, 0.f, 0.f))
			, vScale(_vec3(0.f, 0.f, 0.f))
			, vScale_Origin(_vec3(0.f, 0.f, 0.f))
			, vColor(D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
			, vColor_Origin(D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
			, fLifeTime(0.f)
			, vSlice(_vec2(0.f, 0.f))
			, vUVOffset(_vec2(0.f, 0.f))
			, fCurFrame(0.f)
			, fTotalFrame(0.f)
			, fSpriteSpd(0.f)
		{
			ZeroMemory(vUVSpeed, sizeof(_vec2) * 5);
			//D3DXMatrixIdentity(&matParent);
		}

	}FX_ATT;

	// 이펙트 속성값 /////////////////////////////////////////////
	typedef struct tagEffectInfo
	{
		enum SPAWN_TYPE { SPAWN_RATE_PER_SEC, SPAWN_BURST, SPWAN_END };
		enum BILLBOARD_TYPE { BB_NONE, BB_ALL, BB_Y, BB_END };
		enum PASS_TYPE { PASS_BUFFER, PASS_CUTBLACK_SPRITE, PASS_MASKING, PASS_MESH, PASS_PARTICLE, PASS_DISTORT, PASS_END };
		enum TRANS_TYPE { TYPE_POS_FIXED, TYPE_POS_UPDATED, TYPE_MATRIX_FIXED, TYPE_MATRIX_UPDATED, TYPE_POINTER, TYPE_END };
		enum PTC_SHAPE { SHAPE_NONE, SHAPE_SPHERE, SHAPE_CONE, SHAPE_END };
		enum TEX_TYPE { TEX_NONE, TEX_BASE, TEX_R, TEX_G, TEX_B, TEX_ALPHA, TEX_END };

		SPAWN_TYPE		m_eSpawnType;
		BILLBOARD_TYPE	m_eBillBoardType;
		PASS_TYPE		m_ePassType;
		TRANS_TYPE		m_eTransType;

		PTC_SHAPE		m_ePtc_Shape;
		TEX_TYPE		m_eTex_Type [5];

		_tchar		m_szEffectName[MAX_PATH];
		_tchar		m_szMeshPath[5][MAX_PATH];
		_tchar		m_szTexturePath[5][MAX_PATH];

		float m_fPos_OriginX;
		float m_fPos_OriginY;
		float m_fPos_OriginZ;

		float m_fRot_OriginX;
		float m_fRot_OriginY;
		float m_fRot_OriginZ;

		float m_fScale_OriginX;
		float m_fScale_OriginY;
		float m_fScale_OriginZ;

		// Time
		float m_fDuration;
		float m_fLifeTime_Min;
		float m_fLifeTime_MAX;

		// Random
		// ------------------ Offset
		float m_fOffset_MinX;
		float m_fOffset_MinY;
		float m_fOffset_MinZ;

		float m_fOffset_MaxX;
		float m_fOffset_MaxY;
		float m_fOffset_MaxZ;

		// ------------------ Scale
		float m_fScale_MinX;
		float m_fScale_MinY;
		float m_fScale_MinZ;

		float m_fScale_MaxX;
		float m_fScale_MaxY;
		float m_fScale_MaxZ;

		// ------------------ Rotation
		float m_fRotL_MinX;
		float m_fRotL_MinY;
		float m_fRotL_MinZ;

		float m_fRotL_MaxX;
		float m_fRotL_MaxY;
		float m_fRotL_MaxZ;

		float m_fRotW_MinX;
		float m_fRotW_MinY;
		float m_fRotW_MinZ;

		float m_fRotW_MaxX;
		float m_fRotW_MaxY;
		float m_fRotW_MaxZ;

		// ------------------ Velocity
		float m_fVel_MinX;
		float m_fVel_MinY;
		float m_fVel_MinZ;

		float m_fVel_MaxX;
		float m_fVel_MaxY;
		float m_fVel_MaxZ;

		// ------------------ Speed
		float m_fSpeed_Min;
		float m_fSpeed_Max;

		// ------------------ Force
		float m_fForce_X;
		float m_fForce_Y;
		float m_fForce_Z;

		float m_fForce_MaxX;
		float m_fForce_MaxY;
		float m_fForce_MaxZ;

		// ------------------ Angular Velocity
		float m_fAVel_L_MinX;
		float m_fAVel_L_MinY;
		float m_fAVel_L_MinZ;

		float m_fAVel_L_MaxX;
		float m_fAVel_L_MaxY;
		float m_fAVel_L_MaxZ;

		float m_fAVel_W_MinX;
		float m_fAVel_W_MinY;
		float m_fAVel_W_MinZ;

		float m_fAVel_W_MaxX;
		float m_fAVel_W_MaxY;
		float m_fAVel_W_MaxZ;

		// Radio - 필요한가?
		int m_iRadio_Fixed;
		int m_iRadio_Axis;

		// ------------------ Color
		float m_fCol_MinR;
		float m_fCol_MinG;
		float m_fCol_MinB;
		float m_fCol_MinA;

		float m_fCol_MaxR;
		float m_fCol_MaxG;
		float m_fCol_MaxB;
		float m_fCol_MaxA;

		float m_fBright;

		float m_fRate_Emis;
		float m_fCnt_Min;
		float m_fCnt_Max;

		bool  m_bIsLooping;

		// ------------------ For.Particle
		float m_fPtc_SizeMin;
		float m_fPtc_SizeMax;

		float m_fPtc_Radius;
		float m_fPtc_Angle;

		D3DXVECTOR3		m_vPtc_AreaMin;
		D3DXVECTOR3		m_vPtc_AreaMax;


		// ------------------ For.Resource
		unsigned int m_iMeshCnt;
		unsigned int m_iTexCnt;

		D3DXVECTOR2		m_vUVSpeed_Min[5];
		D3DXVECTOR2		m_vUVSpeed_Max[5];

		D3DXVECTOR2		m_vSliceUV[5];

		// ------------------ For.Curve
		enum CURVE_COLUMN { COLUMN_TIME, COLUMN_VALUE, COLUMN_END };

		int			m_iCurve_Alpha_Cnt;
		float		m_arrCurve_Alpha[CURVE_COLUMN::COLUMN_END][10];
		D3DXCOLOR	m_Color_Init;

		// Scalar Type -> Particle
		int		m_iCurve_Size_Cnt;
		float	m_arrCurve_Size[CURVE_COLUMN::COLUMN_END][10];
		float	m_fSize_Init;
		// -----------------------

		// Vector Type -> Buffer / Mesh
		enum COLUMN_AXIS { AXIS_X, AXIS_Y, AXIS_Z, AXIS_END };

		float	m_arrCurve_Size_Axis_Time[COLUMN_AXIS::AXIS_END][10];
		float	m_arrCurve_Size_Axis[COLUMN_AXIS::AXIS_END][10];

		int		m_arrCurve_Size_Cnt[COLUMN_AXIS::AXIS_END];
		// -----------------------

		int		m_iCurve_Spd_Cnt;
		float	m_arrCurve_Spd[CURVE_COLUMN::COLUMN_END][10];
		float	m_fSpd_Init;

		float	m_arrCurve_R[CURVE_COLUMN::COLUMN_END][10]; // ----For.Color Curve
		float	m_arrCurve_G[CURVE_COLUMN::COLUMN_END][10];
		float	m_arrCurve_B[CURVE_COLUMN::COLUMN_END][10];
		float	m_arrCurve_BRT[CURVE_COLUMN::COLUMN_END][10];

		int		m_iCurve_R_Cnt;
		int		m_iCurve_G_Cnt;
		int		m_iCurve_B_Cnt;
		int		m_iCurve_Brt_Cnt;

		float	m_fBright_Init;

		// ------------------- For.Buffer Option
		bool	m_bBuffSize_Scalar;

		// ------------------- 함수호출 줄이기 위한 bool
		bool	m_bUseForceRand;
		bool	m_bUseVelocity;
		bool	m_bUseAccel;
		bool	m_bUseAngVelL;
		bool	m_bUseAngVelW;
		
		tagEffectInfo()
			: m_fPos_OriginX(0.f)
			, m_fPos_OriginY(0.f)
			, m_fPos_OriginZ(0.f)
			, m_fRot_OriginX(0.f)
			, m_fRot_OriginY(0.f)
			, m_fRot_OriginZ(0.f)
			, m_fScale_OriginX(0.f)
			, m_fScale_OriginY(0.f)
			, m_fScale_OriginZ(0.f)
			, m_fDuration(0.f)
			, m_fLifeTime_Min(0.f)
			, m_fLifeTime_MAX(0.f)
			, m_fOffset_MinX(0.f)
			, m_fOffset_MinY(0.f)
			, m_fOffset_MinZ(0.f)
			, m_fOffset_MaxX(0.f)
			, m_fOffset_MaxY(0.f)
			, m_fOffset_MaxZ(0.f)
			, m_fScale_MinX(0.f)
			, m_fScale_MinY(0.f)
			, m_fScale_MinZ(0.f)
			, m_fScale_MaxX(0.f)
			, m_fScale_MaxY(0.f)
			, m_fScale_MaxZ(0.f)
			, m_fRotL_MinX(0.f)
			, m_fRotL_MinY(0.f)
			, m_fRotL_MinZ(0.f)
			, m_fRotL_MaxX(0.f)
			, m_fRotL_MaxY(0.f)
			, m_fRotL_MaxZ(0.f)
			, m_fRotW_MinX(0.f)
			, m_fRotW_MinY(0.f)
			, m_fRotW_MinZ(0.f)
			, m_fRotW_MaxX(0.f)
			, m_fRotW_MaxY(0.f)
			, m_fRotW_MaxZ(0.f)
			, m_fVel_MinX(0.f)
			, m_fVel_MinY(0.f)
			, m_fVel_MinZ(0.f)
			, m_fVel_MaxX(0.f)
			, m_fVel_MaxY(0.f)
			, m_fVel_MaxZ(0.f)
			, m_fSpeed_Min(0.f)
			, m_fSpeed_Max(0.f)
			, m_fForce_X(0.f)
			, m_fForce_Y(0.f)
			, m_fForce_Z(0.f)
			, m_fForce_MaxX(0.f)
			, m_fForce_MaxY(0.f)
			, m_fForce_MaxZ(0.f)
			, m_fAVel_L_MinX(0.f)
			, m_fAVel_L_MinY(0.f)
			, m_fAVel_L_MinZ(0.f)
			, m_fAVel_L_MaxX(0.f)
			, m_fAVel_L_MaxY(0.f)
			, m_fAVel_L_MaxZ(0.f)
			, m_fAVel_W_MinX(0.f)
			, m_fAVel_W_MinY(0.f)
			, m_fAVel_W_MinZ(0.f)
			, m_fAVel_W_MaxX(0.f)
			, m_fAVel_W_MaxY(0.f)
			, m_fAVel_W_MaxZ(0.f)
			, m_iRadio_Fixed(0)
			, m_iRadio_Axis(0)
			, m_fCol_MinR(0.f)
			, m_fCol_MinG(0.f)
			, m_fCol_MinB(0.f)
			, m_fCol_MinA(0.f)
			, m_fCol_MaxR(0.f)
			, m_fCol_MaxG(0.f)
			, m_fCol_MaxB(0.f)
			, m_fCol_MaxA(0.f)
			, m_fBright(0.f)
			, m_fRate_Emis(0.f)
			, m_fCnt_Min(0.f)
			, m_fCnt_Max(0.f)
			, m_bIsLooping(false)
			, m_fPtc_SizeMin(0.1f)
			, m_fPtc_SizeMax(0.1f)
			, m_fPtc_Radius(0.f)
			, m_fPtc_Angle(0.f)
			, m_vPtc_AreaMin(D3DXVECTOR3(0.f, 0.f, 0.f))
			, m_vPtc_AreaMax(D3DXVECTOR3(0.f, 0.f, 0.f))
			, m_iMeshCnt(0)
			, m_iTexCnt(0)
			, m_iCurve_Alpha_Cnt(1) // 기본 값
			, m_Color_Init(D3DXCOLOR(1.f, 1.f, 1.f, 1.f))
			, m_iCurve_Size_Cnt(1)
			, m_fSize_Init(0.f)
			, m_iCurve_Spd_Cnt(1)
			, m_fSpd_Init(0.f)
			, m_iCurve_R_Cnt(1)
			, m_iCurve_G_Cnt(1)
			, m_iCurve_B_Cnt(1)
			, m_iCurve_Brt_Cnt(1)
			, m_fBright_Init(1.f)
			, m_bBuffSize_Scalar(false)
			, m_bUseForceRand(false)
			, m_bUseVelocity(false)
			, m_bUseAccel(false)
			, m_bUseAngVelL(false)
			, m_bUseAngVelW(false)
		{
			ZeroMemory(m_szEffectName, sizeof(_tchar) * MAX_PATH);
			ZeroMemory(m_szMeshPath, sizeof(_tchar) * MAX_PATH);
			ZeroMemory(m_szTexturePath, sizeof(_tchar) * MAX_PATH * 5);

			ZeroMemory(m_vUVSpeed_Min, sizeof(D3DXVECTOR2) * 5);
			ZeroMemory(m_vUVSpeed_Max, sizeof(D3DXVECTOR2) * 5);
			ZeroMemory(m_vSliceUV, sizeof(D3DXVECTOR2) * 5);

			ZeroMemory(m_arrCurve_Alpha, sizeof(float) * 2 * 10);
			ZeroMemory(m_arrCurve_Size, sizeof(float) * 2 * 10);
			ZeroMemory(m_arrCurve_Spd, sizeof(float) * 2 * 10);

			ZeroMemory(m_arrCurve_R, sizeof(float) * 2 * 10);
			ZeroMemory(m_arrCurve_G, sizeof(float) * 2 * 10);
			ZeroMemory(m_arrCurve_B, sizeof(float) * 2 * 10);
			ZeroMemory(m_arrCurve_BRT, sizeof(float) * 2 * 10);

			ZeroMemory(m_arrCurve_Size_Axis_Time, sizeof(float) * 3 * 10);
			ZeroMemory(m_arrCurve_Size_Axis, sizeof(D3DXVECTOR3) * 3 * 10);
			
			for (int i = 0; i < 3; ++i)
				m_arrCurve_Size_Cnt[i] = 1;
		}

	}EFFECT_INFO;
}