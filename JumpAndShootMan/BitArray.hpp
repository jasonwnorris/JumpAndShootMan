// BitArray.hpp

#ifndef __BITARRAY_HPP__
#define __BITARRAY_HPP__

typedef unsigned char Uint8;

class BitArray
{
	public:
		BitArray(int pLength);
		~BitArray();

		bool Get(int pIndex);
		void Set(int pIndex, bool pValue);

	private:
		int mLength;
		Uint8* mData;
};

#endif