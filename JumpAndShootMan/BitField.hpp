// BitField.hpp

#ifndef __BITFIELD_HPP__
#define __BITFIELD_HPP__

// STL Includes
#include <vector>

typedef unsigned char Uint8;

class BitField
{
	public:
		BitField();
		BitField(int pWidth, int pHeight);
		~BitField();

		int GetCount() const;
		int GetSize() const;

		void Resize(int pWidth, int pHeight);

		bool GetBit(int pX, int pY);
		void SetBit(int pX, int pY, bool pValue);

		Uint8 GetByte(int pIndex);
		void SetByte(int pIndex, Uint8 pValue);

	private:
		int mWidth;
		int mHeight;
		int mCount;
		int mSize;
		std::vector<Uint8> mData;
};

#endif