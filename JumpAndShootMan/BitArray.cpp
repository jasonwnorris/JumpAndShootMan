// BitArray.cpp

// Project Includes
#include "BitArray.hpp"
// STL Includes
#include <assert.h>

BitArray::BitArray(int pLength)
{
	assert(pLength % 8 == 0);

	mLength = pLength;
	mData = new Uint8[mLength / 8];
	for (int i = 0; i < mLength / 8; ++i)
		mData[i] = false;
}

BitArray::~BitArray()
{
	delete [] mData;
}

bool BitArray::Get(int pIndex)
{
	if (pIndex < 0 || pIndex >= mLength)
		return false;

	return ((mData[pIndex / 8] >> (7 - (pIndex % 8))) & 0x01) == 0x01;
}

void BitArray::Set(int pIndex, bool pValue)
{
	// Zero the target bit.
	mData[pIndex / 8] &= ~(0x01 << (7 - (pIndex % 8)));
	// Assign the target bit.
	mData[pIndex / 8] |= ((pValue ? 0x01 : 0x00) << (7 - (pIndex % 8)));
}