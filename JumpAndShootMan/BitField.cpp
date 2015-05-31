// BitField.cpp

// Project Includes
#include "BitField.hpp"

BitField::BitField()
{
	m_Width = 0;
	m_Height = 0;
	m_Count = 0;
	m_Size = 0;
}

BitField::BitField(int p_Width, int p_Height)
{
	Resize(p_Width, p_Height);
}

BitField::~BitField()
{
}

int BitField::GetCount() const
{
	return m_Count;
}

int BitField::GetSize() const
{
	return m_Size;
}

void BitField::Resize(int p_Width, int p_Height)
{
	m_Width = p_Width;
	m_Height = p_Height;
	m_Count = m_Width * m_Height;
	m_Size = (m_Count + 7) / 8;
	m_Data.resize(m_Size);
}

bool BitField::GetBit(int p_X, int p_Y) const
{
	if (p_X < 0 || p_X >= m_Width || p_Y < 0 || p_Y >= m_Height)
		return false;

	return ((m_Data[(p_X + p_Y * m_Width) / 8] >> (7 - ((p_X + p_Y * m_Width) % 8))) & 0x01) == 0x01;;
}

void BitField::SetBit(int p_X, int p_Y, bool p_Value)
{
	// Zero the target bit.
	m_Data[(p_X + p_Y * m_Width) / 8] &= ~(0x01 << (7 - ((p_X + p_Y * m_Width) % 8)));
	// Assign the target bit.
	m_Data[(p_X + p_Y * m_Width) / 8] |= ((p_Value ? 0x01 : 0x00) << (7 - ((p_X + p_Y * m_Width) % 8)));
}

Uint8 BitField::GetByte(int p_Index)
{
	if (p_Index < 0 || p_Index >= m_Size)
		return -1;

	return m_Data[p_Index];
}

void BitField::SetByte(int p_Index, Uint8 p_Value)
{
	if (p_Index < 0 || p_Index >= m_Size)
		return;

	m_Data[p_Index] = p_Value;
}
