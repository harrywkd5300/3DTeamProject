#pragma once

namespace Engine
{
#define MATRIX_ROTATIONXYZ(matRot,AngleX,AngleY,AngleZ) \
(D3DXMatrixRotationYawPitchRoll(matRot,D3DXToRadian(AngleY),D3DXToRadian(AngleX),D3DXToRadian(AngleZ)))



	typedef struct tagRecordData
	{
		enum MOVETYPE { TYPE_CURVE, TYPE_QUOTIENT, TYPE_TIME, TYPE_COSTANT };

		_vec3 vEye;
		_vec3 vAt;			
		_float fTime;
		_float fSpeed;

		MOVETYPE eType;
		
	}RECORDDATA;

}