#include "BaseTask.h"

USING(Engine)

CBaseTask::CBaseTask()
	:m_Status(Status::Invalid)
{
}

CBaseTask::~CBaseTask()
{
}

Status CBaseTask::Tick(_float fTimeDelta)
{
	return Status();
}
