#pragma once

#include "Engine_Defines.h"
#include "Defines.h"
#include "Base.h"

namespace Engine
{
	class CRect_Texture;
	class CTexture;
	class CShader;
	class CTransform;
}

class CMyFont : public CBase
{
	enum SHADERTYPE{TYPE_2D,TYPE_3D,TYPE_NOTDEPTH_3D};
	
	typedef struct tagFontInfo
	{
		_int					iStartAscllCode = 32;
		_int					iEndAscllCode = 90;

		_float					fMaxWidthNum = 0.f;
		_float					fMaxHeightNum = 0.f;

		_float					fHeight = 0.f;
		_float					fWidth = 0.f;
		_float					fSpace = 0.f;
		_float					fDir = 1.f;
		_vec4					vColor;

		char					strText[MAX_PATH];		
	}FONTINFO;

private:
	explicit CMyFont(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMyFont() = default;


public:
	const FONTINFO*		Get_FontInfo() { return &m_tFontInfo; }

public:
	HRESULT Ready_Component(_tchar* pComponentTag);
	HRESULT Ready_Font(_tchar * pComponentTag,const _float& fMaxWidthNum, const _float& fMaxHeightNum,const _int& iStartAscllCode = 32, const _int& iEndAscllCode = 90);

public:
	void Set_FontSetting( const _float& fWidth, const _float& fHeight, const _float& fSpace,const _vec4& vColor,const _float& fDir = 1.f);
public:
	void Draw2DFont(const _float& fX, const _float& fY, const char* strTextFormat ,...);
	void Draw3DFont(const _vec3* pScale,const _vec3* pAngle,const _vec3* pPos, const char* strTextFormat, ...);
	void Draw3DFont_NotDepth(const _matrix* pMatrix, const char* strTextFormat, ...);
private:
	HRESULT SetUp_2DFontConstantTable(LPD3DXEFFECT pEffect, const _int& iFontNum, const _int& iCurNum,const _float& fX, const _float& fY);
	HRESULT SetUp_3DFontConstantTable(LPD3DXEFFECT pEffect, const _int& iFontNum, const _int& iCurNum, const _vec3* pScale, const _vec3* pAngle, const _vec3* pPos);
	HRESULT SetUp_3DFontConstantTable(LPD3DXEFFECT pEffect, const _int& iFontNum, const _int& iCurNum, const _matrix* pMatrix);
private:
	FONTINFO				m_tFontInfo;
private:
	Engine::CTransform*		m_pTransformCom = nullptr;
	Engine::CRect_Texture*	m_pBufferCom = nullptr;
	Engine::CTexture*		m_pTextureCom = nullptr;
	Engine::CShader*		m_pShaderCom = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;

public:
	static CMyFont*	Create(LPDIRECT3DDEVICE9 pGraphicDev,_tchar* pComponentTag, const _float& fMaxWidthNum, const _float& fMaxHeightNum);
private:
	virtual _ulong Free();
};

