#include "CompositeTask.h"
#include "..\Headers\ActionAttack.h"
#include "Character.h"
#include "Controller.h"

USING(Engine)

bool CActionAttack::operator()(void * agent)
{

	return static_cast<CCharacter*>(agent)->Get_Controller()->Attack_Target();

	//return true;
}

CActionAttack::CActionAttack()
{

}

CActionAttack::~CActionAttack()
{

}

Status CActionAttack::Tick(_float fTimeDelta)
{

	return Status();
}
