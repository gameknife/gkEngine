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



/* Lzma2Enc.h -- LZMA2 Encoder
2009-02-07 : Igor Pavlov : Public domain */

#ifndef __LZMA2_ENC_H
#define __LZMA2_ENC_H

#include "LzmaEnc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  CLzmaEncProps lzmaProps;
  size_t blockSize;
  int numBlockThreads;
  int numTotalThreads;
} CLzma2EncProps;

void Lzma2EncProps_Init(CLzma2EncProps *p);
void Lzma2EncProps_Normalize(CLzma2EncProps *p);

/* ---------- CLzmaEnc2Handle Interface ---------- */

/* Lzma2Enc_* functions can return the following exit codes:
Returns:
  SZ_OK           - OK
  SZ_ERROR_MEM    - Memory allocation error
  SZ_ERROR_PARAM  - Incorrect paramater in props
  SZ_ERROR_WRITE  - Write callback error
  SZ_ERROR_PROGRESS - some break from progress callback
  SZ_ERROR_THREAD - errors in multithreading functions (only for Mt version)
*/

typedef void * CLzma2EncHandle;

CLzma2EncHandle Lzma2Enc_Create(ISzAlloc *alloc, ISzAlloc *allocBig);
void Lzma2Enc_Destroy(CLzma2EncHandle p);
SRes Lzma2Enc_SetProps(CLzma2EncHandle p, const CLzma2EncProps *props);
Byte Lzma2Enc_WriteProperties(CLzma2EncHandle p);
SRes Lzma2Enc_Encode(CLzma2EncHandle p,
    ISeqOutStream *outStream, ISeqInStream *inStream, ICompressProgress *progress);

/* ---------- One Call Interface ---------- */

/* Lzma2Encode
Return code:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
  SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
*/

/*
SRes Lzma2Encode(Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, int writeEndMark,
    ICompressProgress *progress, ISzAlloc *alloc, ISzAlloc *allocBig);
*/

#ifdef __cplusplus
}
#endif

#endif
