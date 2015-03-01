//////////////////////////////////////////////////////////////////////////
/*
Copyright (c) 2011-2015 Kaiming Yi
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
	
*/
//////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   crc32.h
//  Version:     v1.00
//  Created:     31/10/2002 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __crc32_h__
#define __crc32_h__

#if _MSC_VER > 1000
#pragma once
#endif

class Crc32Gen 
{
public:
	Crc32Gen();

	void Init() {
		if (!m_bInited)
		{
			m_bInited = true;
			init_CRC32_Table();
		}
	}

	//! Creates a CRC from a text string 
	unsigned int GetCRC32( const char *text ) const;
	unsigned int GetCRC32( const char *data,int size,unsigned int ulCRC ) const;

	unsigned int GetCRC32Lowercase( const char *text ) const;
	unsigned int GetCRC32Lowercase( const char *data,int size,unsigned int ulCRC ) const;

protected:
	unsigned int crc32_table[256];  //!< Lookup table array
	bool m_bInited;

	void init_CRC32_Table();  //!< Builds lookup table array 
	unsigned int reflect( unsigned int ref, char ch); //!< Reflects CRC bits in the lookup table 
	unsigned int get_CRC32( const char *data,int size,unsigned int ulCRC ) const;
	unsigned int get_CRC32Lowercase( const char *data,int size,unsigned int ulCRC ) const;

};

inline unsigned int Crc32Gen::GetCRC32( const char *text ) const
{
	int len = strlen(text);
	return GetCRC32( text,len,0xffffffff );
}

inline unsigned int Crc32Gen::GetCRC32( const char *data, int size, unsigned int ulCRC ) const
{
	return get_CRC32( data,size,ulCRC );
}

inline unsigned int Crc32Gen::GetCRC32Lowercase( const char *text ) const
{
	int len = strlen(text);
	return GetCRC32Lowercase( text,len,0xffffffff );
}

inline unsigned int Crc32Gen::GetCRC32Lowercase( const char *data, int size, unsigned int ulCRC ) const
{
	return get_CRC32Lowercase( data,size,ulCRC );
}

#define _ascii_tolower(c)     ( (((c) >= L'A') && ((c) <= L'Z')) ? ((c) - L'A' + L'a') : (c) )

inline unsigned int Crc32Gen::get_CRC32Lowercase( const char *data,int size,unsigned int ulCRC ) const
{	
	int len; 
	unsigned char* buffer;

	// Get the length. 
	len = size; 

	// Save the text in the buffer. 
	buffer = (unsigned char*)data; 
	// Perform the algorithm on each character in the string, using the lookup table values. 

	while(len--) 
	{
		unsigned char c = *buffer++;
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ _ascii_tolower(c)]; 
	}
	// Exclusive OR the result with the beginning value. 
	return ulCRC ^ 0xffffffff; 
} 

inline unsigned int Crc32Gen::get_CRC32( const char *data, int size, unsigned int ulCRC ) const
{
	// Pass a text string to this function and it will return the CRC.

	// Once the lookup table has been filled in by the two functions above,
	// this function creates all CRCs using only the lookup table.

	// Be sure to use unsigned variables, because negative values introduce high bits where zero bits are required.

	// Start out with all bits set high.
	int len; 
	unsigned char* buffer;

	// Get the length. 
	len = size; 

	// Save the text in the buffer. 
	buffer = (unsigned char*)data; 
	// Perform the algorithm on each character in the string, using the lookup table values. 

	while(len--) 
		ulCRC = (ulCRC >> 8) ^ crc32_table[(ulCRC & 0xFF) ^ *buffer++]; 
	// Exclusive OR the result with the beginning value. 
	return ulCRC ^ 0xffffffff; 
} 

inline Crc32Gen::Crc32Gen()
{
	m_bInited = false;
	Init();
}

inline unsigned int Crc32Gen::reflect(unsigned int ref, char ch)
{// Used only by Init_CRC32_Table().
	unsigned int value = 0;

	// Swap bit 0 for bit 7
	// bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}


// Call this function only once to initialize the CRC table.
inline void Crc32Gen::init_CRC32_Table()
{
	// This is the official polynomial used by CRC-32
	// in PKZip, WinZip and Ethernet.
	unsigned int ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		crc32_table[i] = reflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1U << 31) ? ulPolynomial : 0);
		crc32_table[i] = reflect(crc32_table[i], 32);
	}
}


#endif // __crc32_h__
