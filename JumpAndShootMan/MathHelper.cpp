#include "MathHelper.hpp"

const float MathHelper::Pi = 3.141592f;
const float MathHelper::Epsilon = 0.0001f;

int MathHelper::Abs(int pValue)
{
	return (pValue < 0) ? -pValue : pValue;
}

int MathHelper::Min(int pValueA, int pValueB)
{
	return (pValueA < pValueB) ? pValueA : pValueB;
}

int MathHelper::Max(int pValueA, int pValueB)
{
	return (pValueA > pValueB) ? pValueA : pValueB;
}

float MathHelper::Abs(float pValue)
{
	return (pValue < 0.0f) ? -pValue : pValue;
}

float MathHelper::Min(float pValueA, float pValueB)
{
	return (pValueA < pValueB) ? pValueA : pValueB;
}

float MathHelper::Max(float pValueA, float pValueB)
{
	return (pValueA > pValueB) ? pValueA : pValueB;
}