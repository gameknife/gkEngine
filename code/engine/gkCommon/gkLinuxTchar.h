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



#ifndef _gkLinuxTchar_h_
#define _gkLinuxTchar_h_

#include "stdarg.h"
#include "stdio.h"

#ifdef OS_IOS

#include "string.h"

#endif

#ifndef  _UNICODE


/* Formatted i/o */

#define _tprintf        printf
#define _tprintf_l      _printf_l
#define _tprintf_s      printf_s
#define _tprintf_s_l    _printf_s_l
#define _tprintf_p      _printf_p
#define _tprintf_p_l    _printf_p_l
#define _tcprintf       _cprintf
#define _tcprintf_l     _cprintf_l
#define _tcprintf_s     _cprintf_s
#define _tcprintf_s_l   _cprintf_s_l
#define _tcprintf_p     _cprintf_p
#define _tcprintf_p_l   _cprintf_p_l
#define _vtcprintf      _vcprintf
#define _vtcprintf_l    _vcprintf_l
#define _vtcprintf_s    _vcprintf_s
#define _vtcprintf_s_l  _vcprintf_s_l
#define _vtcprintf_p    _vcprintf_p
#define _vtcprintf_p_l  _vcprintf_p_l
#define _ftprintf       fprintf
#define _ftprintf_l     _fprintf_l
#define _ftprintf_s     fprintf_s
#define _ftprintf_s_l   _fprintf_s_l
#define _ftprintf_p     _fprintf_p
#define _ftprintf_p_l   _fprintf_p_l
#define _stprintf       sprintf
#define _stprintf_l     _sprintf_l
#define _stprintf_s     sprintf_s
#define _stprintf_s_l   _sprintf_s_l
#define _stprintf_p     _sprintf_p
#define _stprintf_p_l   _sprintf_p_l
#define _sctprintf      _scprintf
#define _sctprintf_l    _scprintf_l
#define _sctprintf_p    _scprintf_p
#define _sctprintf_p_l  _scprintf_p_l
#define _sntprintf      _snprintf
#define _sntprintf_l    _snprintf_l
#define _sntprintf_s    _snprintf_s
#define _sntprintf_s_l  _snprintf_s_l
#define _vtprintf       vprintf
#define _vtprintf_l     _vprintf_l
#define _vtprintf_s     vprintf_s
#define _vtprintf_s_l   _vprintf_s_l
#define _vtprintf_p     _vprintf_p
#define _vtprintf_p_l   _vprintf_p_l
#define _vftprintf      vfprintf
#define _vftprintf_l    _vfprintf_l
#define _vftprintf_s    vfprintf_s
#define _vftprintf_s_l  _vfprintf_s_l
#define _vftprintf_p    _vfprintf_p
#define _vftprintf_p_l  _vfprintf_p_l
#define _vstprintf      vsprintf
#define _vstprintf_l    _vsprintf_l
#define _vstprintf_s    vsprintf_s
#define _vstprintf_s_l  _vsprintf_s_l
#define _vstprintf_p    _vsprintf_p
#define _vstprintf_p_l  _vsprintf_p_l
#define _vsctprintf     _vscprintf
#define _vsctprintf_l   _vscprintf_l
#define _vsctprintf_p   _vscprintf_p
#define _vsctprintf_p_l _vscprintf_p_l
#define _vsntprintf     _vsnprintf
#define _vsntprintf_l   _vsnprintf_l
#define _vsntprintf_s   _vsnprintf_s
#define _vsntprintf_s_l _vsnprintf_s_l

#define _tscanf         scanf
#define _tscanf_l       _scanf_l
#define _tscanf_s       scanf_s
#define _tscanf_s_l     _scanf_s_l
#define _tcscanf        _cscanf
#define _tcscanf_l      _cscanf_l
#define _tcscanf_s      _cscanf_s
#define _tcscanf_s_l    _cscanf_s_l
#define _ftscanf        fscanf
#define _ftscanf_l      _fscanf_l
#define _ftscanf_s      fscanf_s
#define _ftscanf_s_l    _fscanf_s_l
#define _stscanf        sscanf
#define _stscanf_l      _sscanf_l
#define _stscanf_s      sscanf_s
#define _stscanf_s_l    _sscanf_s_l
#define _sntscanf       _snscanf
#define _sntscanf_l     _snscanf_l
#define _sntscanf_s     _snscanf_s
#define _sntscanf_s_l   _snscanf_s_l


/* Unformatted i/o */

#define _fgettc         fgetc
#define _fgettc_nolock  _fgetc_nolock
#define _fgettchar      _fgetchar
#define _fgetts         fgets
#define _fputtc         fputc
#define _fputtc_nolock  _fputc_nolock
#define _fputtchar      _fputchar
#define _fputts         fputs
#define _cputts         _cputs
#define _gettc          getc
#define _gettc_nolock   _getc_nolock
#define _gettch         _getch
#define _gettch_nolock  _getch_nolock
#define _gettche        _getche
#define _gettche_nolock _getche_nolock
#define _gettchar       getchar
#define _gettchar_nolock _getchar_nolock
#define _getts          gets
#define _getts_s        gets_s
#define _cgetts         _cgets
#define _cgetts_s       _cgets_s
#define _puttc          putc
#define _puttc_nolock   _putc_nolock
#define _puttchar       putchar
#define _puttchar_nolock _putchar_nolock
#define _puttch         _putch
#define _puttch_nolock  _putch_nolock
#define _putts          puts
#define _ungettc        ungetc
#define _ungettc_nolock _ungetc_nolock
#define _ungettch       _ungetch
#define _ungettch_nolock _ungetch_nolock

/* String conversion functions */

#define _tcstod     strtod
#define _tcstol     strtol
#define _tcstoul    strtoul
#define _ttof       atof
#define _tstof      atof
#define _tstol      atol
#define _tstoi      atoi
#define _tstoi64    _atoi64
#define _tcstod_l     _strtod_l
#define _tcstol_l     _strtol_l
#define _tcstoul_l    _strtoul_l
#define _tstof_l      _atof_l
#define _tstol_l      _atol_l
#define _tstoi_l      _atoi_l
#define _tstoi64_l    _atoi64_l

#define _itot_s     _itoa_s
#define _ltot_s     _ltoa_s
#define _ultot_s    _ultoa_s
#define _itot       _itoa
#define _ltot       _ltoa
#define _ultot      _ultoa
#define _ttoi       atoi
#define _ttol       atol

#define _ttoi64     _atoi64
#define _tcstoi64   _strtoi64
#define _tcstoi64_l   _strtoi64_l
#define _tcstoui64  _strtoui64
#define _tcstoui64_l  _strtoui64_l
#define _i64tot_s   _i64toa_s
#define _ui64tot_s  _ui64toa_s
#define _i64tot     _i64toa
#define _ui64tot    _ui64toa

/* String functions */

/* Note that _mbscat, _mbscpy and _mbsdup are functionally equivalent to 
   strcat, strcpy and strdup, respectively. */

#define _tcscat     strcat
#define _tcscat_s   strcat
#define _tcscpy     strcpy
#define _tcscpy_s   strcpy_s
#define _tcsdup     _strdup
#define _tcslen     strlen
#define _tcsnlen    strnlen
#define _tcsxfrm    strxfrm
#define _tcsxfrm_l    _strxfrm_l
#define _tcserror   strerror
#define _tcserror_s   strerror_s
#define __tcserror  _strerror
#define __tcserror_s  _strerror_s

#ifdef _DEBUG
#define _tcsdup_dbg _strdup_dbg
#endif

/* Execute functions */

#define _texecl     _execl
#define _texecle    _execle
#define _texeclp    _execlp
#define _texeclpe   _execlpe
#define _texecv     _execv
#define _texecve    _execve
#define _texecvp    _execvp
#define _texecvpe   _execvpe

#define _tspawnl    _spawnl
#define _tspawnle   _spawnle
#define _tspawnlp   _spawnlp
#define _tspawnlpe  _spawnlpe
#define _tspawnv    _spawnv
#define _tspawnve   _spawnve
#define _tspawnvp   _spawnvp
#define _tspawnvpe  _spawnvpe

#define _tsystem    system


/* Time functions */

#define _tasctime   asctime
#define _tctime     ctime
#define _tctime32   _ctime32
#define _tctime64   _ctime64
#define _tstrdate   _strdate
#define _tstrtime   _strtime
#define _tutime     _utime
#define _tutime32   _utime32
#define _tutime64   _utime64
#define _tcsftime   strftime
#define _tcsftime_l _strftime_l

#define _tasctime_s   asctime_s
#define _tctime_s     ctime_s
#define _tctime32_s   _ctime32_s
#define _tctime64_s   _ctime64_s
#define _tstrdate_s   _strdate_s
#define _tstrtime_s   _strtime_s

/* Directory functions */

#define _tchdir             _chdir
#define _tgetcwd            _getcwd
#define _tgetdcwd           _getdcwd
#define _tgetdcwd_nolock    _getdcwd_nolock
#define _tmkdir             _mkdir
#define _trmdir             _rmdir

#ifdef _DEBUG
#define _tgetcwd_dbg        _getcwd_dbg
#define _tgetdcwd_dbg       _getdcwd_dbg
#define _tgetdcwd_lk_dbg    _getdcwd_lk_dbg
#endif

/* Environment/Path functions */

#define _tfullpath      _fullpath
#define _tgetenv        getenv
#define _tgetenv_s      getenv_s
#define _tdupenv_s      _dupenv_s
#define _tmakepath      _makepath
#define _tmakepath_s    _makepath_s
#define _tpgmptr        _pgmptr
#define _get_tpgmptr    _get_pgmptr
#define _tputenv        _putenv
#define _tputenv_s      _putenv_s
#define _tsearchenv     _searchenv
#define _tsearchenv_s   _searchenv_s
#define _tsplitpath     _splitpath
#define _tsplitpath_s   _splitpath_s

#ifdef _DEBUG
#define _tfullpath_dbg  _fullpath_dbg
#define _tdupenv_s_dbg  _dupenv_s_dbg
#endif

/* Stdio functions */

#ifdef  _POSIX_
#define _tfdopen    fdopen
#else
#define _tfdopen    _fdopen
#endif
#define _tfsopen    _fsopen
#define _tfopen     fopen
#define _tfopen_s   fopen_s
#define _tfreopen   freopen
#define _tfreopen_s freopen_s
#define _tperror    perror
#define _tpopen     _popen
#define _ttempnam   _tempnam
#define _ttmpnam    tmpnam
#define _ttmpnam_s  tmpnam_s

#ifdef _DEBUG
#define _ttempnam_dbg   _tempnam_dbg
#endif


/* Io functions */

#define _tchmod     _chmod
#define _tcreat     _creat
#define _tfindfirst      _findfirst
#define _tfindfirst32    _findfirst32
#define _tfindfirst64    _findfirst64
#define _tfindfirsti64   _findfirsti64
#define _tfindfirst32i64 _findfirst32i64
#define _tfindfirst64i32 _findfirst64i32
#define _tfindnext       _findnext
#define _tfindnext32     _findnext32
#define _tfindnext64     _findnext64
#define _tfindnexti64    _findnexti64
#define _tfindnext32i64  _findnext32i64
#define _tfindnext64i32  _findnext64i32
#define _tmktemp            _mktemp
#define _tmktemp_s          _mktemp_s

#ifdef  _POSIX_
#define _topen      open
#define _taccess    access
#else
#define _topen      _open
#define _taccess    _access
#define _taccess_s  _access_s
#endif

#define _tremove    remove
#define _trename    rename
#define _tsopen     _sopen
#define _tsopen_s   _sopen_s
#define _tunlink    _unlink

#define _tfinddata_t      _finddata_t
#define _tfinddata32_t    _finddata32_t
#define _tfinddata64_t    __finddata64_t
#define _tfinddatai64_t   _finddatai64_t
#define _tfinddata32i64_t _finddata32i64_t
#define _tfinddata64i32_t _finddata64i32_t

/* ctype functions */
#define _istascii       __isascii
#define _istcntrl       iscntrl
#define _istcntrl_l     _iscntrl_l
#define _istxdigit      isxdigit
#define _istxdigit_l    _isxdigit_l

/* Stat functions */
#define _tstat      _stat
#define _tstat32    _stat32
#define _tstat32i64 _stat32i64
#define _tstat64    _stat64
#define _tstat64i32 _stat64i32
#define _tstati64   _stati64


#define _tcschr         strchr
#define _tcscspn        strcspn
#define _tcsncat        strncat
#define _tcsncat_s      strncat_s
#define _tcsncat_l      _strncat_l
#define _tcsncat_s_l    _strncat_s_l
#define _tcsncpy        strncpy
#define _tcsncpy_s      strncpy_s
#define _tcsncpy_l      _strncpy_l
#define _tcsncpy_s_l    _strncpy_s_l
#define _tcspbrk        strpbrk
#define _tcsrchr        strrchr
#define _tcsspn         strspn
#define _tcsstr         strstr
#define _tcstok         strtok
#define _tcstok_s       strtok_s
#define _tcstok_l       _strtok_l
#define _tcstok_s_l     _strtok_s_l

#define _tcsnset        _strnset
#define _tcsnset_s      _strnset_s
#define _tcsnset_l      _strnset_l
#define _tcsnset_s_l    _strnset_s_l
#define _tcsrev         _strrev
#define _tcsset         _strset
#define _tcsset_s       _strset_s
#define _tcsset_l       _strset_l
#define _tcsset_s_l     _strset_s_l

#define _tcscmp         strcmp
#define _tcsicmp        strcasecmp
#define _stricmp        strcasecmp
#define stricmp        strcasecmp
#define _tcsicmp_l      _stricmp_l
#define _tcsnccmp       strncmp
#define _tcsncmp        strncmp
#define _tcsncicmp      _strnicmp
#define _tcsncicmp_l    _strnicmp_l
#define _tcsnicmp       _strnicmp
#define _tcsnicmp_l     _strnicmp_l

#define _tcscoll        strcoll
#define _tcscoll_l      _strcoll_l
#define _tcsicoll       _stricoll
#define _tcsicoll_l     _stricoll_l
#define _tcsnccoll      _strncoll
#define _tcsnccoll_l    _strncoll_l
#define _tcsncoll       _strncoll
#define _tcsncoll_l     _strncoll_l
#define _tcsncicoll     _strnicoll
#define _tcsncicoll_l   _strnicoll_l
#define _tcsnicoll      _strnicoll
#define _tcsnicoll_l    _strnicoll_l

/* "logical-character" mappings */

#define _tcsclen        strlen
#define _tcscnlen       strnlen
#define _tcsclen_l(_String, _Locale) strlen(_String)
#define _tcscnlen_l(_String, _Max_count, _Locale) strnlen((_String), (_Max_count))
#define _tcsnccat       strncat
#define _tcsnccat_s     strncat_s  
#define _tcsnccat_l     _strncat_l  
#define _tcsnccat_s_l   _strncat_s_l
#define _tcsnccpy       strncpy
#define _tcsnccpy_s     strncpy_s  
#define _tcsnccpy_l     _strncpy_l  
#define _tcsnccpy_s_l   _strncpy_s_l
#define _tcsncset       _strnset
#define _tcsncset_s     _strnset_s
#define _tcsncset_l     _strnset_l
#define _tcsncset_s_l   _strnset_s_l

/* MBCS-specific functions */

#define _tcsdec     _strdec
#define _tcsinc     _strinc
#define _tcsnbcnt   _strncnt
#define _tcsnccnt   _strncnt
#define _tcsnextc   _strnextc
#define _tcsninc    _strninc
#define _tcsspnp    _strspnp

#define _tcslwr     _strlwr
#define _tcslwr_l   _strlwr_l
#define _tcslwr_s   _strlwr_s
#define _tcslwr_s_l _strlwr_s_l
#define _tcsupr     _strupr
#define _tcsupr_l   _strupr_l
#define _tcsupr_s   _strupr_s
#define _tcsupr_s_l _strupr_s_l
#define _tcsxfrm    strxfrm
#define _tcsxfrm_l  _strxfrm_l

#endif



inline int strcpy_s(char* dst, size_t size, const char* src)
{
	strcpy(dst, src);
	//dst[size - 1]=0;
	return 0;
}

inline int strcpy_s(char* dst, const char* src)
{
	strcpy(dst, src);
	return 0;
}

inline int sprintf_s(char* buffer, size_t size, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	int err = vsnprintf(buffer, size, format, args);
	va_end(args);

	return err;
}


#endif