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



/**
 * Copyright(c) 2012-2013, All Rights Reserved.
 *
 * @file strnormalize.h
 * @details Check GBK character you could do
 *     code >= 0x8000 && _pGbk2Utf16[code - 0x8000] != 0
 * @author cnangel
 * @version 1.0.0
 * @date 2012/10/09 11:44:58
 */

#ifndef __STRNORMALIZE_H__
#define __STRNORMALIZE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SNO_TO_LOWER        1
#define SNO_TO_UPPER        2
#define SNO_TO_HALF         4
#define SNO_TO_SIMPLIFIED   8

extern int str_normal_initialized;

void str_normalize_init();
void str_normalize_gbk(char *text, unsigned options);
void str_normalize_utf8(char *text, unsigned options);

int gbk_to_utf8(const char *from, unsigned int from_len, char **to, unsigned int *to_len);
int utf8_to_gbk(const char *from, unsigned int from_len, char **to, unsigned int *to_len);

#ifdef __cplusplus
}
#endif

#endif /* __STRNORMALIZE_H__ */
