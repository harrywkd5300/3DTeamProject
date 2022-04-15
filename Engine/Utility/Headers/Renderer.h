#pragma once

#include "Engine_Defines.h"
#include "Component.h"

BEGIN(Engine)

#define BEGINMRT(RTName) m_pTarget_Manager->Begin_MRT(RTName)
#define BEGINRT(RTName) m_pTarget_Manager->Begin_Target(RTName)
#define ENDMRT(RTName) m_pTarget_Manager->End_MRT(RTName)
#define ENDRT(RTName) m_pTarget_Manager->End_Target(RTName)
#define SETRT(Effect, RTName, TexName) m_pTarget_Manager->SetUp_OnShader(Effect, RTName, TexName)

// 화면에 그려야할 객체들을 모아둔다.\
// 객체들을 그린다.
class CGameObject;
class CTarget_Manager;
class CShader;
class CLight_Manager;
class CCamera_Manager;
class CFrustum;
class CTransform;
class CGlareDef;
class CDecal_Manager;
class DLL_EXPORT CRenderer final : public CComponent
{
public:
	enum CASCADE_LEVEL { Cascade_Level1, Cascade_Level2, Cascade_End };
	enum RENDER { RENDER_PRIORITY, RENDER_NONALPHA, RENDER_ALPHA, RENDER_DISOTRT, RENDER_UI, RENDER_END };
	enum STATICRENDER { STATICRENDER_NONALPHA, STATICRENDER_END };
	// By.KMH
#ifdef KMH_MODIFY
	enum SHADERKIND
	{
		Shader_Deferred, Shader_Filter, Shader_FXAA, Shader_HDR, Shader_Light, Shader_PostProcess, Shader_Shadow, Shader_SSDecal, Shader_Effect, Shader_Mesh,Shader_End
	};
#ifdef DEBUGRENDERTARGET
	enum RENDERBACKBUFFERKIND { RenderBackBuffer_DebugRenderTarget, RenderBackBuffer_Default, RenderBackBuffer_End };
#endif
	enum SHADEROPTION
	{
		ShaderOption_None = -1, ShaderOption_Light, ShaderOption_HDR, ShaderOption_EdgeDetecting, ShaderOption_Shadow, ShaderOption_MotionBlur, ShaderOption_SSAO,
		ShaderOption_DOF, ShaderOption_FXAA, ShaderOption_LightShaft, ShaderOption_Fog, ShaderOption_RainAmbience, ShaderOption_End
	};

	enum ShaderVariableType 
	{
		ShaderVar_None = -1, ShaderVar_Float, ShaderVar_Int, ShaderVar_Bool, ShaderVar_Vector, ShaderVar_Matrix,
		ShaderVar_Texture, ShaderVar_End
	};

	typedef struct tagShaderVarType
	{
		char					szName[64] = "";
		ShaderVariableType		eType = ShaderVar_None;
		_matrix					mData;
		LPDIRECT3DTEXTURE9		pTexture = nullptr;

		tagShaderVarType()
		{
			mData.m[0][0] = mData.m[1][0] = mData.m[2][0] = mData.m[3][0] =
			mData.m[0][1] = mData.m[1][1] = mData.m[2][1] = mData.m[3][1] =
			mData.m[0][2] = mData.m[1][2] = mData.m[2][2] = mData.m[3][2] =
			mData.m[0][3] = mData.m[1][3] = mData.m[2][3] = mData.m[3][3] = 0.f;
		}

	} EFFECTVARTYPE;

	typedef struct tagShaderData
	{
		EFFECTVARTYPE*	pEffectVar = nullptr;
		_uint			iEffectVarCount = 0;
	} EFFECTVARDATA;

	typedef struct tagShaderVarData
	{
		EFFECTVARDATA*	pEffectVarData = nullptr;
		_uint			iEffectVarDataCount = 0;
	} SHADERVARDATA;

	typedef struct tagCameraData
	{
		_float fCamFar = 0.f;
		_vec4 vPosition = _vec4( 0.f, 0.f, 0.f, 0.f );
		_vec4 vDirection = _vec4( 0.f, 0.f, 0.f, 0.f );
		_matrix matInvView;
		_matrix matInvProj;
		_matrix matView;
		_matrix matProj;
		_matrix matPrevView;

		tagCameraData()
		{
			D3DXMatrixIdentity( &matInvView );
			D3DXMatrixIdentity( &matInvProj );
			D3DXMatrixIdentity( &matView );
			D3DXMatrixIdentity( &matProj );
			D3DXMatrixIdentity( &matPrevView );
		}

	} CAMERADATA;
#endif
public:
	typedef struct tagRenderObject // By.PSH
	{
		CGameObject* pGameObject;
		_float	fSort = 0.f;
	}REDNEROBJECT;

	typedef struct tagStaticRenderObject // By.KMH
	{
		CGameObject*	pGameObject = nullptr;
		CTransform*		pTransformCom = nullptr;
		_float			fSort = 0.f;
		_float			fCullSize = 0.f;
		_vec3			vPos = _vec3( 0.f, 0.f, 0.f );
	}STATICRENDEROBJECT;

	typedef struct tagStaticRenderGroup // By.KMH
	{
		STATICRENDEROBJECT*	pGroup = nullptr;
		_uint				iGroupCount = 0;
	}STATICRENDERGROUP;
private:
	std::function<_bool(const REDNEROBJECT& Dst, const REDNEROBJECT& Src)> m_SortRenderObject;
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRenderer() = default;
public: // Setter
	void Update_ViewMatrix(_matrix matView) { m_matView = matView; }
	void Update_ProjMatrix(_matrix matProj) { m_matProj = matProj; }
	HRESULT Add_RenderGroup(RENDER eRenderGroup, CGameObject* pGameObject,const _float& fSort = 0.f);
	HRESULT Add_StaticRenderObject( const STATICRENDER& eStaticRenderGroup, CGameObject* pGameObject, const _float& fSort = 0.f );
	HRESULT Add_StaticRenderGroup( const STATICRENDER& eStaticRenderGroup, STATICRENDERGROUP& StaticGroupData );
	HRESULT Add_ShadowGroup( CGameObject* pGameObject, _vec3 _vPos, const _float& _fRadius );
#ifdef KMH_MODIFY
	void Set_RenderBackBufferKind( const RENDERBACKBUFFERKIND& _eRenderTargetBackBuffer, const _tchar* _pRenderTargetTag = nullptr );
	void Set_ShaderOptionOnOff( const SHADEROPTION& _eShaderOption, const _bool& _bIsOnShaderOption );
#endif
public:	// Getter..
#ifdef KMH_MODIFY
	_bool Get_ShaderOptionOnOff( const SHADEROPTION& _eShaderOption ); // By.KMH
#endif
	void SetUp_ShadowDataOnShader( LPD3DXEFFECT _pEffect, const char* _pLightVPName, const char* _pLightVPBName, const char* _pCascadeRangeName );
public:
	HRESULT Ready_LoadShaderVarData( const _tchar* _pFilePath );
public:
	HRESULT Ready_Renderer(void);
	void	Render_RenderGroup(void);
	void	Release_StaticRenderGroup( void );	// By.KMH - 스태틱 렌더 그룹 삭제시켜주는 함수..
private:
	list<REDNEROBJECT>			m_RenderGroup[RENDER_END]; //By.PSH
	list<CGameObject*>			m_ShadowRenderGroup[Cascade_End]; // By.KMH - 쉐도우를 그리기 위한 렌더 그룹..
	STATICRENDERGROUP			m_StaticRenderGroup[STATICRENDER_END];
	typedef list<REDNEROBJECT>	RENDERGROUP;
private:
	CTarget_Manager*			m_pTarget_Manager = nullptr;
#ifndef KMH_MODIFY // By.KMH
	CShader*					m_pShader_Light = nullptr;
	CShader*					m_pShader_Blend = nullptr;
#endif
	CShader*					m_pShader_Distort = nullptr; // By.KJY
	
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;

	_matrix		m_matView;
	_matrix		m_matProj;

	_matrix		m_matOrthoView;
	_matrix		m_matOrthoProj;

	const _float	SHADOWMAP_WIDTH[Cascade_End] = { 2048.f, 512.f };
	const _float	SHADOWMAP_HEIGHT[Cascade_End] = { 2048.f, 512.f };

	_float		m_fCasCadeRange[Cascade_End];
	_matrix		m_matLightProj[Cascade_End];
	_matrix		m_matLightView[Cascade_End];
	_matrix		m_matLightVP[Cascade_End];
	_matrix		m_matLightVPB[Cascade_End];
	_matrix		m_matScaleBias[Cascade_End];

	CAMERADATA	m_CamData;

#ifdef KMH_MODIFY // By.KMH
private:
	CInput_Device*			m_pInput_Device = nullptr;
	CLight_Manager*			m_pLight_Manager = nullptr;
	CCamera_Manager*		m_pCam_Manager = nullptr;
	Engine::CFrustum*		m_pFrustum = nullptr;
	CGlareDef*				m_pGlareDef = nullptr;	// Cross 필터 등을 위해 필요한 클래스..
	Engine::CDecal_Manager*	m_pDecal_Manager = nullptr;	// Decal 을 모두 가지고 있는 매니저 클래스 ( Decal 들을 그릴 때 사용 )..
private:
	LPD3DXEFFECT	m_pEffectArr[Shader_End] = { nullptr };
	CShader*		m_pShaderArr[Shader_End] = { nullptr };
private:
	LPDIRECT3DTEXTURE9		m_pRandomTexture = nullptr;
	LPDIRECT3DTEXTURE9		m_pLUTTexture = nullptr;
private:
	// Shader on/off Option..
	_uint					m_iShaderOption = 0;
#ifdef DEBUGRENDERTARGET
private:
	_ubyte					m_iPassNum = RenderBackBuffer_Default;
	const _tchar*			m_pDebugRenderTargetTag = nullptr;
#endif
private:
	// 쉐이더의 전역 변수에 값을 채워 넣을 것들..
	_bool					m_bUseSepia = false;
	_vec4					m_vLightShaftValue = _vec4( 1.f, 1.f, 1.f, 1.f );
	SHADERVARDATA			m_ShaderVarData;
	_vec4					m_vSampleOffsetsArr[16];
private:
	const _tchar*			m_pLastRTName = nullptr;
	const _tchar*			m_pTempRTName = nullptr;
#ifdef ADD_GETTER_TARGETANDSHADER	// 모든 렌더타겟의 이름과 렌더러의 쉐이더를 저장하기 위한 용도 ( 디버깅용, 쉐이더툴용 )..
private:
	_uint					m_iTargetCount = 0;
	_uint					m_iEffectCount = 0;
	_tchar**				m_pAllTagetNameArr = nullptr;
	LPD3DXEFFECT*			m_pAllEffectArr = nullptr;
public:
	_uint Get_TargetCount( void ) { return m_iTargetCount; }
	_uint Get_EffectCount( void ) { return m_iEffectCount; }
	void Get_TargetName( _tchar* _pOut, const _uint& _iIndex ) { lstrcpy( _pOut, m_pAllTagetNameArr[_iIndex] ); }
	void Get_Effect( LPD3DXEFFECT* _ppEffect, const _uint& _iIndex ) { *_ppEffect = m_pAllEffectArr[_iIndex]; }
	_bool Set_ShaderVarData( const _uint& _iShaderIndex, EFFECTVARDATA* _pEffectVarData );
	EFFECTVARDATA* Get_ShaderVar( const _uint& _iIndex );
#endif
private:
	_bool						m_bIsShowAllTarget = false;
#ifdef RENDERERDEBUGING
private:
	list<std::function<void( void )>>	m_ActNodeList;
	std::function<void( void )>	m_ActFunction = nullptr;
	_bool						m_bIsDebugMode = false;
	_uint						m_iSelect = 0;
	_uint						m_iLimit = 2;
	_int						m_iWaitFrame = 0;
	_int						m_iDebugTargetOption = 0;
	enum { UpKey = DIK_I, DownKey = DIK_K, SelectKey = DIK_U, BackKey = DIK_P };
public:
	void SelectMoveKeyCheck( _uint& _iSelect );
	void PrevKeyCheck( void );
public:
	void Debug_ShowSelectOption( void );
	void Debug_ShowSelectRT( void );
	void Debug_ShowTargetOption( void );
	void Debug_ShowSelectShaderOption( void );
	void Debug_ShowShaderOptionYesOrNo( void );
	void Debug_ShowWaitFrame( void );
#endif
#endif

private:
	void Render_Priority(void);
	void Render_NonAlpha(void);
	void Render_Alpha(void);
	void Render_Shadow( void );
	void Render_Distortion(void);
	void Render_Decal( void );
	void Render_UI(void);
#ifdef KMH_MODIFY // By.KMH
private:
	HRESULT Ready_RenderTarget( const D3DVIEWPORT9& ViewPort );
	HRESULT Ready_ShaderCom( void );
	HRESULT Ready_Buffer( const D3DVIEWPORT9& ViewPort );
	HRESULT Ready_DefaultSetting( const D3DVIEWPORT9& ViewPort );
	///////////////////////// 수행하는 순서대로 함수 생성 되어 있음.. /////////////////////////
private:
	// Shadow Rendering..
private:
	// Before Render FrameBuffer..
	void Render_EdgeDection( void );
private:
	// Render FrameBuffer( Color 와 빛 연산을 하나의 렌더타겟에 )..
	void Render_FrameBuffer( void );
private:
	// Calculate HDR Progress..
	void Calc_HDRProgress( void );
	void Scene_To_ReductionBuffer( void );
	void Compute_AvgLuminance( void );
	void Extract_BrightPass( void );
	void Filter_To_BrightPass( const _uint& _iBlurCount );
	void Render_Star( void );
	void ToneMapping( void );
	//void Compute_Luminance( void );
private:
	// Priority 를 그리기 전에 객체들이 그려진 부분을 검은색 아닌 부분은 흰색으로 그려두자..
	void Render_LightOcclusion( void );
private:
	// PostProcessing 작업 전에 Priority 그룹과 씬의 렌더되어 있는 것들을 합치기 위한 함수..
	void Render_SumBuffer( void );
private:
	// Calculate PostProcessing..
	void Calc_PostProcessing( void );
	void Compute_MotionBlur( void );
	void Compute_RainAmbience( void );	// 비 온 느낌을 살리기 위해 블룸 등의 효과를 넣어줄 예정..
	void Compute_SSAO( void );
	void Compute_DOF( void );
	void Compute_Fog( void );
	void Compute_LightShaft( void );	// 빛의 산란을 눈으로 보여주는??
private:
	void Render_Final( void );
	void Render_FXAA( void );
	void Render_BackBuffer( void );
#ifdef KEYBOARDCHECK
private:
	void KeyCheck_DebugRenderTarget( void );
#endif
private:
	// 편의를 위한 용도의 함수들 ( 재사용을 위해 만든 것들 )..
	void ModifyCameraData( void );
	void ModifyShaderData( void );
	void Render_VIBuffer( void );
	void SettingViewProjMatrix( LPD3DXEFFECT _pEffect );
	void Shader_Begin( LPD3DXEFFECT _pEffect, const char* _pTechniqueName );
	void Shader_Begin( LPD3DXEFFECT _pEffect, EFFECTVARDATA* _pEffectVarData, const char* _pTechniqueName );
	void Shader_Draw( LPD3DXEFFECT _pEffect, const _uint& _iPassNum );
	void Shader_BeginDrawEnd( LPD3DXEFFECT _pEffect, const char* _pTechniqueName, const _uint& _iPassNum );
	void Shader_BeginDrawEnd( LPD3DXEFFECT _pEffect, EFFECTVARDATA* _pEffectVarData, const char* _pTechniqueName, const _uint& _iPassNum );
	void SetUp_ShaderData( LPD3DXEFFECT _pEffect, EFFECTVARDATA* _pEffectVarData );
private:
	// 알파 오브젝트 카메라 기준 소팅을 위한 함수..
	void SettingAlphaBlendRenderObjectSortValue( list<REDNEROBJECT>& _RenderObjectList );
	_int Compute_CascadeIndex( _vec3 _vPos, const _float& _fRadius );	// 현재 객체가 있는 케스케이드 영역을 구하기 위한 함수..
#endif
private:
	void Render_Deferred(void);
	void Render_LightAcc(void);
	void Render_Blend_Distortion(void);

private:
	void Clear_RenderGroup(void);
public:	
	virtual CComponent* Clone(void) final;
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphicDev);
#ifndef _FINAL
public:
	void Debug_Render();
private:
#endif
private:
	virtual _ulong Free(void);

};

END