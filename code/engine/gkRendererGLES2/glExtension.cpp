#include "StableHeader.h"
#include "glExtension.h"

#if defined(OS_IOS)
// the extensions supported on the iPhone are treated as core functions of gl
// so use this macro to assign the function pointers in this class appropriately.
#define GLEXTGetProcAddress(x) ::x

#else

#define GLEXTGetProcAddress(x) eglGetProcAddress(#x)

#endif

PFNGLMAPBUFFEROESPROC                   gkGLExtension::glMapBufferOES = 0;
 PFNGLUNMAPBUFFEROESPROC                  gkGLExtension::glUnmapBufferOES = 0;
 PFNGLGETBUFFERPOINTERVOESPROC            gkGLExtension::glGetBufferPointervOES = 0;

// GL_EXT_discard_framebuffer
 PFNGLDISCARDFRAMEBUFFEREXTPROC			 gkGLExtension::glDiscardFramebufferEXT = 0;
 bool gkGLExtension::EXT_VAO = false;
 bool gkGLExtension::COMPRESS_VERTEX = false;
 bool gkGLExtension::INDEX_32BIT = false;

 PFNGLBINDVERTEXARRAYOESPROC  gkGLExtension::glBindVertexArrayOES = 0;
 PFNGLDELETEVERTEXARRAYSOESPROC  gkGLExtension::glDeleteVertexArraysOES = 0;
 PFNGLGENVERTEXARRAYSOESPROC  gkGLExtension::glGenVertexArraysOES = 0;
 PFNGLISVERTEXARRAYOESPROC  gkGLExtension::glIsVertexArrayOES = 0;

 const GLubyte * gkGLExtension::EXTENSIONS = NULL;

void gkGLExtension::Init()
{
	EXTENSIONS = glGetString(GL_EXTENSIONS);

	
	TCHAR* str = (TCHAR*)EXTENSIONS;

#ifdef UNICODE
	const char* org = (const char*)EXTENSIONS;
	str = new TCHAR[ strlen(org) + 1 ];

	MultiByteToWideChar( CP_ACP, NULL, org, -1, str, strlen(org) + 1);

#endif

	gkLogMessage( _T("GLEXT: \n --------------------------- \n %s"), str );
	gkLogMessage( _T("--------------------------- \n") );

	// VAO EXT
	glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC) GLEXTGetProcAddress(glBindVertexArrayOES);
	glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC) GLEXTGetProcAddress(glDeleteVertexArraysOES);
	glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC) GLEXTGetProcAddress(glGenVertexArraysOES);
	glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC) GLEXTGetProcAddress(glIsVertexArrayOES);

	if (glBindVertexArrayOES || glDeleteVertexArraysOES || glGenVertexArraysOES || glIsVertexArrayOES )
	{
		gkLogMessage( _T("RendererGLES2: Device support ext [VAO].") );
#ifdef OS_IOS
		EXT_VAO = true;
#endif
	}
    
    int max_vs_uniforms = 0;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS , &max_vs_uniforms);
    int max_ps_uniforms = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS , &max_ps_uniforms);
    
    gkLogMessage( _T("RendererGLES2: max uniforms [vs:%d] [ps:%d]."), max_vs_uniforms, max_ps_uniforms );

#if defined( GL_OES_vertex_half_float ) && defined( GL_OES_vertex_type_10_10_10_2 )
		COMPRESS_VERTEX = true;
#endif

	if ( IsGLExtensionSupported("GL_OES_element_index_uint") )
	{
		INDEX_32BIT = true;
	}

#ifdef OS_ANDROID
	//COMPRESS_VERTEX = false;
	EXT_VAO = false;
#endif

#ifdef OS_WIN32
	COMPRESS_VERTEX = false;
#endif
	//////////////////////////////////////////////////////////////////////////
	// check ext support
	bool bIsPVRTCSupported = gkGLExtension::IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc");
#ifndef OS_IOS
	bool bIsBGRA8888Supported  = gkGLExtension::IsGLExtensionSupported("GL_IMG_texture_format_BGRA8888");
#else
	bool bIsBGRA8888Supported  = gkGLExtension::IsGLExtensionSupported("GL_APPLE_texture_format_BGRA8888");
#endif
	bool bIsFloat16Supported = gkGLExtension::IsGLExtensionSupported("GL_OES_texture_half_float");
	bool bIsFloat32Supported = gkGLExtension::IsGLExtensionSupported("GL_OES_texture_float");
#ifndef OS_IOS
	bool bIsETCSupported = gkGLExtension::IsGLExtensionSupported("GL_OES_compressed_ETC1_RGB8_texture");
#endif
}

/*!***********************************************************************
@Function			IsGLExtensionSupported
@Input				extension extension to query for
@Returns			True if the extension is supported
@Description		Queries for support of an extension
*************************************************************************/
bool gkGLExtension::IsGLExtensionSupported(const char * const extension)
{
	// The recommended technique for querying OpenGL extensions;
	// from http://opengl.org/resources/features/OGLextensions/

	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;

	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings, etc. */
	start = EXTENSIONS;
	for (;;) {
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}

	return false;
}