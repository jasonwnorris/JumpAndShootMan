// BitField.cpp

// Project Includes
#include "BitField.hpp"

BitField::BitField()
{
	mWidth = 0;
	mHeight = 0;
	mCount = 0;
	mSize = 0;
}

BitField::BitField(int pWidth, int pHeight)
{
	Resize(pWidth, pHeight);
}

BitField::~BitField()
{
}

int BitField::GetCount() const
{
	return mCount;
}

int BitField::GetSize() const
{
	return mSize;
}

void BitField::Resize(int pWidth, int pHeight)
{
	mWidth = pWidth;
	mHeight = pHeight;
	mCount = mWidth * mHeight;
	mSize = (mCount + 7) / 8;
	mData.resize(mSize);
}

bool BitField::GetBit(int pX, int pY) const
{
	if (pX < 0 || pX >= mWidth || pY < 0 || pY >= mHeight)
		return false;

	return ((mData[(pX + pY * mWidth) / 8] >> (7 - ((pX + pY * mWidth) % 8))) & 0x01) == 0x01;;
}

void BitField::SetBit(int pX, int pY, bool pValue)
{
	// Zero the target bit.
	mData[(pX + pY * mWidth) / 8] &= ~(0x01 << (7 - ((pX + pY * mWidth) % 8)));
	// Assign the target bit.
	mData[(pX + pY * mWidth) / 8] |= ((pValue ? 0x01 : 0x00) << (7 - ((pX + pY * mWidth) % 8)));
}

Uint8 BitField::GetByte(int pIndex)
{
	if (pIndex < 0 || pIndex >= mSize)
		return -1;

	return mData[pIndex];
}

void BitField::SetByte(int pIndex, Uint8 pValue)
{
	if (pIndex < 0 || pIndex >= mSize)
		return;

	mData[pIndex] = pValue;
}