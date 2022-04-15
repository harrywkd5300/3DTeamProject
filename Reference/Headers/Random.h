#pragma once

#include "Engine_Defines.h"
#include "base.h"

BEGIN(Engine)

class DLL_EXPORT CRandom :public CBase
{
	DECLARE_SINGLETON(CRandom)
private:
	explicit CRandom();
	virtual ~CRandom() = default;

protected:
	unsigned long m_states[16];
	unsigned int m_idxState;
	unsigned int SeedDiffer();

public:
	void InitSeedRandomize();
	void InitSeedRandomize(unsigned long *state);
	FORCEINLINE unsigned int GetValue();	
	FORCEINLINE unsigned int GetValue(unsigned int minValue, unsigned int maxValue); // 범위 : minValue ~ maxValue
	FORCEINLINE unsigned int GetValue(unsigned int maxValue); // 범위 : 0 ~ maxValue
	FORCEINLINE float GetValueF();
	FORCEINLINE float GetValueF(float minValue, float maxValue);// 범위 : minValue ~ maxValue
	FORCEINLINE float GetValueF(float maxValue); // 범위 : 0.f ~ maxValue
private:
	virtual _ulong Free();
};

END