#include "StableHeader.h"
#include "gkVirtualAPI.h"

#ifdef RENDERAPI_GLES2
#include "glExtension.h"
#endif

void gkVirtualAPI::gkVAPI_MapBuffer(GLvoid* *data, GLenum target, uint32 size, MapFlag flag)
{
#ifdef RENDERAPI_GL330

	glGetError();

	GLenum realFlag = GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT /*| GL_MAP_FLUSH_EXPLICIT_BIT*/;
	switch( flag )
	{
	case eVAPI_MapWrite:
		realFlag |= GL_MAP_WRITE_BIT;
		break;
	case eVAPI_MapRead:
		realFlag |= GL_MAP_READ_BIT;
		break;
	case eVAPI_MapRW:
		realFlag |= (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		break;
	}

	glBufferData( target, size, 0, GL_STATIC_DRAW );
	*data = glMapBufferRange(target, 0,	size, realFlag);

	GLenum error = glGetError();
	if (error)
	{
		gkLogWarning(_T("MapBuffer Failed: %x."), error);
	}

#endif

#ifdef RENDERAPI_GLES2

	GLenum realFlag = 0;
	switch( flag )
	{
	case eVAPI_MapWrite:
		realFlag = GL_WRITE_ONLY_OES;
		break;
	case eVAPI_MapRead:
		realFlag = GL_WRITE_ONLY_OES;
		break;
	case eVAPI_MapRW:
		realFlag = GL_WRITE_ONLY_OES;
		break;

	}
	glBufferData( target, size, 0, GL_STATIC_DRAW );

	if (gkGLExtension::MAP_BUFFER)
	{
		*data = gkGLExtension::glMapBufferOES(target, realFlag);
	}
	else
	{
		*data = new uint8[size];
	}
#endif
}

void gkVirtualAPI::gkVAPI_UnMapBuffer(GLvoid* data, GLenum target, uint32 size)
{
#ifdef RENDERAPI_GL330
	//glFlushMappedBufferRange(target, 0,  size);
	glUnmapBuffer(target);
#endif

#ifdef RENDERAPI_GLES2
	if (gkGLExtension::MAP_BUFFER)
	{
		gkGLExtension::glUnmapBufferOES(target);
	}
	else
	{
		glBufferData(target, size, data, GL_STATIC_DRAW);
		delete[] data;
	}
#endif
}

void gkVirtualAPI::gen_or_bind_vao(GLuint& vao, GLuint& vbo)
{
#ifdef RENDERAPI_GL330
	if (vao == 0)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}
	else
	{
		glBindVertexArray(vao);
	}
#else
	if (gkGLExtension::EXT_VAO)
	{
		if (vao == 0)
		{
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
		}
		else
		{
			glBindVertexArray(vao);
		}
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}
#endif
}
void gkVirtualAPI::unbind_vao()
{
#ifdef RENDERAPI_GL330
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
#else
	if (gkGLExtension::EXT_VAO)
	{
		glBindVertexArray(0);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}
