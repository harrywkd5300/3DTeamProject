#include "Random.h"

IMPLEMENT_SINGLETON(CRandom)


unsigned int CRandom::SeedDiffer()
{
	return 5 + (rand() % 500);
}

CRandom::CRandom()
{
	InitSeedRandomize();
}

void CRandom::InitSeedRandomize()
{
	m_idxState = 0;
	unsigned int randseed = 0;
	for (int i = 0; i < 16; ++i)
	{
		randseed = SeedDiffer();
		bool soleness = true;
		while (1)
		{
			soleness = true;
			for (int j = 0; j < i; ++j)
			{
				if (randseed == m_states[j])
				{
					soleness = false;
					break;
				}
			}
			if (false == soleness) { randseed = SeedDiffer(); }
			else { break; }
		}

		m_states[i] = randseed;
	}
}

void CRandom::InitSeedRandomize(unsigned long * state)
{
	for (int i = 0; i < 16; ++i)
	{
		m_states[i] = state[i];
	}

}

unsigned int CRandom::GetValue()
{
	unsigned int a, b, c, d;

	a = m_states[m_idxState];
	c = m_states[(m_idxState + 13) & 15];
	b = a ^ c ^ (a << 16) ^ (c << 15);
	c = m_states[(m_idxState + 9) & 15];
	c = c ^ (c >> 11);
	a = m_states[m_idxState] = b ^ c;
	d = a ^ ((a << 15) & 0xda442d24U);
	m_idxState = (m_idxState + 15) & 15;
	a = m_states[m_idxState];
	m_states[m_idxState] = a ^ b ^ d ^ (a << 2) ^ (b << 18) ^ (c << 28);

	return m_states[m_idxState];
}

unsigned int CRandom::GetValue(unsigned int minValue, unsigned int maxValue)
{
	return minValue + (GetValue() % (maxValue - minValue));
}

unsigned int CRandom::GetValue(unsigned int maxValue)
{
	return GetValue() % maxValue;
}

float CRandom::GetValueF()
{
	unsigned int randValue = GetValue();

	union { unsigned long ul; float f; } p;
	p.ul = (((randValue = randValue * 16807) & 0x007fffff) - 1) | 0x3f800000;

	return p.f - 1.f;
}

float CRandom::GetValueF(float minValue, float maxValue)
{
	return minValue + ((maxValue - minValue) * GetValueF());
}

float CRandom::GetValueF(float maxValue)
{
	return GetValueF()* maxValue;
}

_ulong CRandom::Free()
{
	_ulong dwRefCount = 0;


	return dwRefCount;
}
