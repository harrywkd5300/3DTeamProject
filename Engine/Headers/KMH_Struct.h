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
//#define SHADER_FULL_OPTION				// ���̴� Ǯ �ɼ����� �����ϰڴ�..
#define SSD_DEBUG		// Screen Space Decal �����..
#endif

//#define USEVELOCITYTARGET				// �ӵ� Ÿ���� ����� ���ΰ�??( ��� ���� �� �� )..

#define ADD_GETTER_TARGETANDSHADER		// <- ��� Ÿ�ٵ��� �̸��� ���������� ����ϴ� ���̴��� ������ ( ����� �뵵 �� ���̴��������� �Ѿ��� )..
#define LIGHT_DEBUGING
#define DEBUGRENDERTARGET
#define KMH_MODIFY
#define USELIGHTTOOL
#define KEYBOARDCHECK
#define VIEWPROJMATRIXSETTING_ONCE		// Render ó���� Renderer Ŭ�������� ����ϴ� ���̴����� View Projection ��ĵ��� �����ϰ� ����..
#define RENDERERDEBUGING				// <- ������ Ŭ���� ����� ( ���� Ÿ�� ���ų� �ɼ� �ٲٴ� �� ��� )..
#define RENDERTARGETNAME				// <- Ÿ�� ����� ���� ��� �� �̸����� ���� �߰Բ� ( ���� Ÿ�� ���ų� �ɼ� �ٲٴ� �� ��� )..
//#define RENDEREREFFECTHDR				// ������ ���� ���� Priority (�׷� ���� ������ ���İ� �ִ� ��ü�� HDR ���� ������ �ٲ�).. 
//#define USEAVGLUMINANCE					// ��� �ֵ��� �����ؼ� �װ��� ����� ���ΰ�??..
#define CHARACTER_COLLIDERMODIFY		// ĳ���� Ŭ������ �ݸ����� �� ������ ���� ���� �ݸ��� �׷�(�ݸ��� ������ == �ݸ��� �׷�)�� ������ �� �ְ� �ٲ�..

#define KMH_PLAYERCONTROLLER_MODIFY		// �÷��̾� ��Ʈ�ѷ� ����..

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

	// �浹 Ÿ��??? ( �� �� �浹 �ߴ���, �̹� �浹 �� ���¿��� �� �浹�ߴ���( ���� �ݸ����� ) )..
	enum COLLISIONTYPE { COLLISION_ENTER, COLLISION_STAY, COLLISION_END };

	enum STATICOPTIONKIND	// �ݸ��� �ʿ��� ó���� �͵�..
	{
		StaticOption_Push = 0	// ô��( �ε����� �� �о�� ��??? )..
		,StaticOption_End
	};

	enum OPTIONKIND			// �����͸� ������ ���� �͵�..
	{
		Option_None = 0				// �ƹ� �͵� ����..
		,Option_Attack = 0x02		// ���� ����..
		,Option_End
	};

	enum // �ݸ��� �ɼǿ� ������������ �� ����..
	{
		// ------------------------- �о�� �� ���� �ɼǵ�.. ------------------------- //
		PushOption_None = 0					// �о�� �ɼ��� ������� �ʰڴ�..
		,PushOption_Fixed = 0x01			// ������ ��ü( �з����� �ʴ� �ɼ� )..
		,PushOption_BePushedOut = 0x02		// �з��� ��ü( �����̴� ������Ʈ���� ������ �� �� �ɼ����� )..


	};

	typedef struct tagCollisionData
	{
		D3DXVECTOR3		vCollisionPos = D3DXVECTOR3( 0.f, 0.f, 0.f );	// �浹�� ������ ���� ��ǥ..
		D3DXVECTOR3		vDist = D3DXVECTOR3( 0.f, 0.f, 0.f );			// ���� ����� �Ÿ�..
		D3DXVECTOR3		vRadius = D3DXVECTOR3( 0.f, 0.f, 0.f );		// ���� ����� �� ������ ũ��
		D3DXVECTOR3		vOverlapSize = D3DXVECTOR3( 0.f, 0.f, 0.f );	// ���� ����� ������ �ִ� ũ��( ���� ��� (TotalRadius - Distance) )..

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

	typedef struct tagOptionData	// �ݸ����� ������ ���� �ɼ�( �� �ݸ����� ���� �ɼ��� �ִ� ����� ) ���� ������ ����ü..
	{
		unsigned long					dwOption = 0;		// OPTIONKIND ����ü�� ���� ���� �� ����..
		std::list<unsigned long>		DetailDataList;		// Option �� ���� �������� ������..

	} OPTIONDATA;

	typedef struct tagStaticOptionData	// �ݸ��� �ʿ��� �������ֱ� ���� �ʿ��� �ɼǵ�..
	{
		unsigned long	dwDetailData = 0;		// �ɼǿ� ���� ������ ����..

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
		// ������ �Ľ��� ���� ������ �����͵�..
		unsigned int					iAniIndex = 0;
		unsigned int					iOthersAniIndex = 0;	// ���� ��ȣ�ۿ��� �� ������ �ִϸ��̼� �ε���..
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
			// ���� ������� �Ÿ� ����..
			WriteFile( _Handle, &fDist, sizeof( float ), &_dwByte, nullptr );

			// ���� ������� �Ÿ� ����..
			WriteFile( _Handle, &fAngle, sizeof( float ), &_dwByte, nullptr );

			// ���� �ִϸ��̼� �ε��� ���� ����..
			WriteFile( _Handle, &iAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// ������ �ִϸ��̼� �ε��� ���� ����..
			WriteFile( _Handle, &iOthersAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// ���濡�� ���� ���� ���� ����..
			WriteFile( _Handle, &iOptionCount, sizeof( unsigned char ), &_dwByte, nullptr );

			for ( unsigned char i = 0; i < iOptionCount; ++i )
			{
				// ������ �ִϸ��̼� ������ �ð� ����..
				WriteFile( _Handle, &pOptionData[i].fAniPos, sizeof( float ), &_dwByte, nullptr );


				// ���濡�� ���� ���� ����..
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
			// ���� ������� �Ÿ� �ε�,..
			ReadFile( _Handle, &fDist, sizeof( float ), &_dwByte, nullptr );

			// ���� ������� �Ÿ� �ε�..
			ReadFile( _Handle, &fAngle, sizeof( float ), &_dwByte, nullptr );

			// ���� �ִϸ��̼� �ε��� ���� �ε�..
			ReadFile( _Handle, &iAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// ������ �ִϸ��̼� �ε��� ���� �ε�..
			ReadFile( _Handle, &iOthersAniIndex, sizeof( unsigned int ), &_dwByte, nullptr );

			// ���濡�� ���� ���� ���� �ε�..
			ReadFile( _Handle, &iOptionCount, sizeof( unsigned char ), &_dwByte, nullptr );

			if ( 0 == iOptionCount )
				return NOERROR;

			Engine::Safe_Delete_Array( pOptionData );

			pOptionData = new INTERACTIONOPTIONDATAINFO[iOptionCount];

			for ( unsigned char i = 0; i < iOptionCount; ++i )
			{
				// ������ �ִϸ��̼� ������ �ð� �ε�..
				ReadFile( _Handle, &pOptionData[i].fAniPos, sizeof( float ), &_dwByte, nullptr );


				// ���濡�� ���� ���� �ε�..
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
		// Ŭ���̾�Ʈ���� ���������� �������� �����͵�..
		Engine::CCharacter*				pCharacter = nullptr;
		D3DXVECTOR3						vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
		D3DXVECTOR3						vLook = D3DXVECTOR3( 0.f, 0.f, 0.f );

		// ������ �Ľ��� ���� ������ �����͵�..
		INTERACTIONDATA*				pInteractionData = nullptr;
		_uint							iInteractionCount = 0;

		HRESULT Save( HANDLE& _Handle, unsigned long& _dwByte )
		{
			// ��ȣ �ۿ� ���� ����..
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
			// ��ȣ �ۿ� ���� ����..
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