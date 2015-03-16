#ifndef __MATHHELPER_HPP__
#define __MATHHELPER_HPP__

class MathHelper
{
	public:
		static const float Pi;
		static const float Epsilon;
		static inline int Abs(int pValue);
		static inline int Min(int pValueA, int pValueB);
		static inline int Max(int pValueA, int pValueB);
		static inline float Abs(float pValue);
		static inline float Min(float pValueA, float pValueB);
		static inline float Max(float pValueA, float pValueB);
};

#endif