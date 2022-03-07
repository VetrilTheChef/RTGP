class GLUtils
{
	const char * static GLErrorToString(GLenum error)
	{
		switch (error)
		{
			case GL_NO_ERROR:                      return "GL_NO_ERROR";
			case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
			case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
			case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
			case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
			case 0x8031: /* not core */            return "GL_TABLE_TOO_LARGE_EXT";
			case 0x8065: /* not core */            return "GL_TEXTURE_TOO_LARGE_EXT";
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
			default:
				assert(!"Unhandled GL error code");
				return NULL;
		}
	};
}