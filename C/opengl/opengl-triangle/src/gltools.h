#ifndef __GL_TOOLS
#define __GL_TOOLS 1

#include <glad/gl.h>
#include <stdio.h>
#include <stdlib.h>

static void glCheckError_(const char *file, int line)
{
	GLenum gl_error;
	while ((gl_error = glGetError()) != GL_NO_ERROR) {
		const char *gl_error_name = NULL;
		switch (gl_error) {
		case GL_INVALID_ENUM:
			gl_error_name = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			gl_error_name = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			gl_error_name = "GL_INVALID_OPERATION";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			gl_error_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			gl_error_name = "GL_OUT_OF_MEMORY";
			break;
		default:
			gl_error_name = "GL_UNKNOWN";
		}

		fprintf(stderr, "%s(%d) | %s (%d)\n", gl_error_name, gl_error, file,
				line);
	}
}

#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif
