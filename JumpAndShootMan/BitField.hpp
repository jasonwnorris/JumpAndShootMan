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
		BitField(int p_Width, int p_Height);
		~BitField();

		int GetCount() const;
		int GetSize() const;

		void Resize(int p_Width, int p_Height);

		bool GetBit(int p_X, int p_Y) const;
		void SetBit(int p_X, int p_Y, bool p_Value);

		Uint8 GetByte(int p_Index);
		void SetByte(int p_Index, Uint8 p_Value);

	private:
		int m_Width;
		int m_Height;
		int m_Count;
		int m_Size;
		std::vector<Uint8> m_Data;
};

#endif
