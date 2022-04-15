#pragma once
#include <list>

namespace Engine
{
	class CCollider;
	class CCharacter;

#ifdef _FINAL
#ifndef SHADER_FULL_OPTION
#define SHADER_FULL_OPTION
#endif
#else
//#define SHADER_FULL_OPTION				// 쉐이더 풀 옵션으로 시작하겠다..
#define SSD_DEBUG		// Screen Space Decal 디버깅..
#endif

//#define USEVELOCITYTARGET				// 속도 타겟을 사용할 것인가??( 모션 블러를 할 때 )..

#define ADD_GETTER_TARGETANDSHADER		// <- 모든 타겟들의 이름과 렌더러에서 사용하는 쉐이더를 저장함 ( 디버깅 용도 및 쉐이더툴에서는 켜야함 )..
#define LIGHT_DEBUGING
#define DEBUGRENDERTARGET
#define KMH_MODIFY
#define USELIGHTTOOL
#define KEYBOARDCHECK
#define VIEWPROJMATRIXSETTING_ONCE		// Render 처음에 Renderer 클래스에서 사용하는 쉐이더들의 View Projection 행렬들을 세팅하고 시작..
#define RENDERERDEBUGING				// <- 렌더러 클래스 디버깅 ( 렌더 타겟 보거나 옵션 바꾸는 거 등등 )..
#define RENDERTARGETNAME				// <- 타겟 디버깅 버퍼 출력 시 이름까지 같이 뜨게끔 ( 렌더 타겟 보거나 옵션 바꾸는 거 등등 )..
//#define RENDEREREFFECTHDR				// 렌더러 순서 변경 Priority (그룹 렌더 순서와 알파가 있는 객체와 HDR 렌더 순서가 바뀜).. 
//#define USEAVGLUMINANCE					// 평균 휘도를 추출해서 그것을 사용할 것인가??..
#define CHARACTER_COLLIDERMODIFY		// 캐릭터 클래스의 콜리더를 한 개에서 여러 개의 콜리더 그룹(콜리더 여러개 == 콜리더 그룹)을 저장할 수 있게 바꿈..

#define KMH_PLAYERCONTROLLER_MODIFY		// 플레이어 컨트롤러 수정..

#define MSG_BOX_CREATE_FAIL(CLASSNAME) MessageBox(0, TEXT("Create Failed!!"), TEXT(CLASSNAME), MB_OK)
#define MSG_BOX_CREATE_INSTANCE_RELEASE_FAIL(CLASSNAME) MessageBox(0, TEXT("Create Instance Release Failed!!"), TEXT(CLASSNAME), MB_OK)
#define CREATE_FAIL(pInstance, CLASSNAME)	{	MSG_BOX_CREATE_FAIL( CLASSNAME );							\
												if ( Engine::Safe_Release( pInstance ) )							\
													MSG_BOX_CREATE_INSTANCE_RELEASE_FAIL( CLASSNAME ); }

#define EFAIL_MSGBOX(FunctionName)																		\
									{																	\
										MessageBox( 0, TEXT( "E_FAIL" ), FunctionName, MB_OK );			\
										return E_FAIL;													\
									}

	// 충돌 타입??? ( 한 번 충돌 했는지, 이미 충돌 된 상태에서 또 충돌했는지( 같은 콜리더와 ) )..
	enum COLLISIONTYPE { COLLISION_ENTER, COLLISION_STAY, COLLISION_END };

	enum STATICOPTIONKIND	// 콜리더 쪽에서 처리할 것들..
	{
		StaticOption_Push = 0	// 척력( 부딪혔을 때 밀어내는 힘??? )..
		,StaticOption_End
	};

	enum OPTIONKIND			// 데이터만 가지고 있을 것들..
	{
		Option_None = 0				// 아무 것도 없음..
		,Option_Attack = 0x02		// 공격 관련..
		,Option_End
	};

	enum // 콜리더 옵션에 디테일적으로 들어갈 값들..
	{
		// ------------------------- 밀어내는 것 관련 옵션들.. ------------------------- //
		PushOption_None = 0					// 밀어내는 옵션을 사용하지 않겠다..
		,PushOption_Fixed = 0x01			// 고정된 물체( 밀려나지 않는 옵션 )..
		,PushOption_BePushedOut = 0x02		// 밀려날 물체( 움직이는 오브젝트들은 움직일 때 이 옵션으로 )..


	};

	typedef struct tagCollisionData
	{
		D3DXVECTOR3		vCollisionPos = D3DXVECTOR3( 0.f, 0.f, 0.f );	// 충돌된 지점의 센터 좌표..
		D3DXVECTOR3		vDist = D3DXVECTOR3( 0.f, 0.f, 0.f );			// 나와 상대의 거리..
		D3DXVECTOR3		vRadius = D3DXVECTOR3( 0.f, 0.f, 0.f );		// 나와 상대의 총 반지름 크기
		D3DXVECTOR3		vOverlapSize = D3DXVECTOR3( 0.f, 0.f, 0.f );	// 나와 상대의 겹쳐져 있는 크기( 원의 경우 (TotalRadius - Distance) )..

	} COLLISIONDATA;

	typedef struct tagCollision
	{
		COLLISIONDATA		CollisionData;
		Engine::CCollider*	pCollider = nullptr;
		Engine::CCollider*	pOtherCollider = nullptr;
		COLLISIONTYPE		CollisionType;

		tagCollision( const COLLISIONDATA& _CollisionData, Engine::CCollider* _pCollider, Engine::CCollider* _pOtherCollider, const COLLISIONTYPE& _CollisionType )
			: CollisionData( _CollisionData )
			, pCollider( _pCollider )
			, pOtherCollider( _pOtherCollider )
			, CollisionType( _CollisionType )
		{}

	} Collision;

	typedef struct tagOptionData	// 콜리더가 가지고 있을 옵션( 이 콜리더는 공격 옵션이 있다 등등의 ) 들을 저장할 구조체..
	{
		unsigned long					dwOption = 0;		// OPTIONKIND 열거체의 값이 담기게 될 예정..
		std::list<unsigned long>		DetailDataList;		// Option 에 관한 세부적인 데이터..

	} OPTIONDATA;

	typedef struct tagStaticOptionData	// 콜리더 쪽에서 수행해주기 위해 필요한 옵션들..
	{
		unsigned long	dwDetailData = 0;		// 옵션에 대한 상세적인 정보..

		tagStaticOptionData( const unsigned long& _dwDetailData = 0 )
			: dwDetailData( _dwDetailData )
		{}

	} STATICOPTIONDATA;

	typedef struct tagInteractionOptionDataInfo
	{
		OPTIONDATA	OptionData;
		float		fAniPos = 0.f;
	} INTERACTIONOPTIONDATAINFO;

	typedef struct tagInteractionData
	{
		// 툴에서 파싱을 통해 가져올 데이터들..
		unsigned int					iAniIndex = 0;
		unsigned int					iOthersAniIndex = 0;	// 나와 상호작용을 할 상대방의 애니메이션 인덱스..
		float							fDist = 0.f;
		float							fAngle = 0.f;
		INTERACTIONOPTIONDATAINFO*		pOptionData = nullptr;
		unsigned char					iOptionCount = 0;

		HRESULT Add_Option( const INTERACTIONOPTIONDATAINFO& _InteractionOptionDataInfo )
		{
			INTERACTIONOPTIONDATAINFO* pTemp = new INTERACTIONOPTIONDATAINFO[iOptionCount + 1];
			for ( _ubyte i = 0; i < iOptionCount; ++i )
			{
				pTemp[i] = pOptionData[i];
			}

			pTemp[iOptionCount] = _InteractionOptionDataInfo;

			Engine::Safe_Delete_Array( pOptionData );

			pOptionData = pTemp;

			++iOptionCount;

			return NOERROR;
		}

		HRESULT Save( HANDLE& _Handle, unsigned long& _dwByte )
		{
			// 나와 상대방과의 거리 저장..
			WriteFile( _Handle, &fDist, sizeof( float ), &_dwByte, nullptr );

			// 나와 상대방과의 거리 저장..
			WriteFile( _Handle, &fAngle, sizeof( float ), &_dwByte, nullptr );

			// 나의 애니메이션 인덱스 정보 저장..
			WriteFile( _Handle, &iAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// 상대방의 애니메이션 인덱스 정보 저장..
			WriteFile( _Handle, &iOthersAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// 상대방에게 보낼 정보 개수 저장..
			WriteFile( _Handle, &iOptionCount, sizeof( unsigned char ), &_dwByte, nullptr );

			for ( unsigned char i = 0; i < iOptionCount; ++i )
			{
				// 시작할 애니메이션 포지션 시간 저장..
				WriteFile( _Handle, &pOptionData[i].fAniPos, sizeof( float ), &_dwByte, nullptr );


				// 상대방에게 보낼 정보 저장..
				WriteFile( _Handle, &pOptionData[i].OptionData.dwOption, sizeof( unsigned long ), &_dwByte, nullptr );

				unsigned int iCount = (unsigned int)pOptionData[i].OptionData.DetailDataList.size();

				WriteFile( _Handle, &iCount, sizeof( unsigned int ), &_dwByte, nullptr );

				for ( auto& DetailData : pOptionData[i].OptionData.DetailDataList )
				{
					WriteFile( _Handle, &DetailData, sizeof( unsigned long ), &_dwByte, nullptr );
				}
			}

			return NOERROR;
		}

		HRESULT Load( HANDLE& _Handle, unsigned long& _dwByte )
		{
			// 나와 상대방과의 거리 로드,..
			ReadFile( _Handle, &fDist, sizeof( float ), &_dwByte, nullptr );

			// 나와 상대방과의 거리 로드..
			ReadFile( _Handle, &fAngle, sizeof( float ), &_dwByte, nullptr );

			// 나의 애니메이션 인덱스 정보 로드..
			ReadFile( _Handle, &iAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// 상대방의 애니메이션 인덱스 정보 로드..
			ReadFile( _Handle, &iOthersAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// 상대방에게 보낼 정보 개수 로드..
			ReadFile( _Handle, &iOptionCount, sizeof( unsigned char ), &_dwByte, nullptr );

			if ( 0 == iOptionCount )
				return NOERROR;

			Engine::Safe_Delete_Array( pOptionData );

			pOptionData = new INTERACTIONOPTIONDATAINFO[iOptionCount];

			for ( unsigned char i = 0; i < iOptionCount; ++i )
			{
				// 시작할 애니메이션 포지션 시간 로드..
				ReadFile( _Handle, &pOptionData[i].fAniPos, sizeof( float ), &_dwByte, nullptr );


				// 상대방에게 보낼 정보 로드..
				ReadFile( _Handle, &pOptionData[i].OptionData.dwOption, sizeof( unsigned long ), &_dwByte, nullptr );

				unsigned int iCount = 0;

				ReadFile( _Handle, &iCount, sizeof( unsigned int ), &_dwByte, nullptr );

				for ( unsigned int j = 0; j < iCount; ++j )
				{
					unsigned long dwDetailData = 0;
					ReadFile( _Handle, &dwDetailData, sizeof( unsigned long ), &_dwByte, nullptr );

					pOptionData[i].OptionData.DetailDataList.push_back( dwDetailData );
				}
			}

			return NOERROR;
		}

		unsigned long Release( void )
		{
			Engine::Safe_Delete_Array( pOptionData );

			iOptionCount = 0;

			return 0;
		}

	} INTERACTIONDATA;

	typedef struct tagInteractionInfo
	{
		// 클라이언트에서 유동적으로 변경해줄 데이터들..
		Engine::CCharacter*				pCharacter = nullptr;
		D3DXVECTOR3						vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
		D3DXVECTOR3						vLook = D3DXVECTOR3( 0.f, 0.f, 0.f );

		// 툴에서 파싱을 통해 가져올 데이터들..
		INTERACTIONDATA*				pInteractionData = nullptr;
		_uint							iInteractionCount = 0;

		HRESULT Save( HANDLE& _Handle, unsigned long& _dwByte )
		{
			// 상호 작용 개수 저장..
			WriteFile( _Handle, &iInteractionCount, sizeof( _uint ), &_dwByte, nullptr );

			for ( _uint i = 0; i < iInteractionCount; ++i )
			{
				if ( FAILED( pInteractionData[i].Save( _Handle, _dwByte ) ) )
					return E_FAIL;
			}

			return NOERROR;
		}

		HRESULT Load( HANDLE& _Handle, unsigned long& _dwByte )
		{
			// 상호 작용 개수 저장..
			ReadFile( _Handle, &iInteractionCount, sizeof( unsigned int ), &_dwByte, nullptr );

			if ( iInteractionCount == 0 )
				return NOERROR;

			pInteractionData = new INTERACTIONDATA[iInteractionCount];

			for ( unsigned int i = 0; i < iInteractionCount; ++i )
			{
				if ( FAILED( pInteractionData[i].Load( _Handle, _dwByte ) ) )
					return E_FAIL;
			}

			return NOERROR;
		}

		unsigned long Release( void )
		{
			if ( nullptr != pInteractionData )
			{
				for ( unsigned int i = 0; i < iInteractionCount; ++i )
				{
					pInteractionData[i].Release();
				}

				Engine::Safe_Delete_Array( pInteractionData );
			}

			iInteractionCount = 0;

			return 0;
		}

	} INTERACTIONINFO;
}