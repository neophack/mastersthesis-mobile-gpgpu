#include "shader.h"

#include "../cv_accar.h"

CvAccARShader::CvAccARShader() {
	programId = -1;
}

CvAccARShader::~CvAccARShader() {
	if (programId > 0) {
		glDeleteProgram(programId);
	}
}

bool CvAccARShader::buildFromSrc(char *vshSrc, char *fshSrc) {
	programId = create(vshSrc, fshSrc, &vshId, &fshId);

	return (programId > 0);
}

void CvAccARShader::use() {
	glUseProgram(programId);
}

GLint CvAccARShader::getParam(CvAccARShaderParamType type, const char *name) {
	GLint id = (type == ATTR) ?
			glGetAttribLocation(programId, name) :
			glGetUniformLocation(programId, name);

	if (id < 0) {
		LOGERR("CvAccARShader: Could not get parameter id for param %s", name);
	}

	return id;
}

GLuint CvAccARShader::create(char *vshSrc, char *fshSrc, GLuint *vshId, GLuint *fshId) {
	*vshId = compile(GL_VERTEX_SHADER, vshSrc);
	*fshId = compile(GL_FRAGMENT_SHADER, fshSrc);

	GLuint programId = glCreateProgram();

	if (programId == 0) {
		LOGERR("CvAccARShader: Could not create shader program.");
		return -1;
	}

	glAttachShader(programId, *vshId);   // add the vertex shader to program
	glAttachShader(programId, *fshId);   // add the fragment shader to program
	glLinkProgram(programId);

	// check link status
	GLint linkStatus;
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		LOGERR("CvAccARShader: Could not link shader program:");
		GLchar infoLogBuf[1024];
		GLsizei infoLogLen;
		glGetProgramInfoLog(programId, 1024, &infoLogLen, infoLogBuf);
		LOGERR("%s", infoLogBuf);

		glDeleteProgram(programId);

		return -1;
	}

	return programId;
}

GLuint CvAccARShader::compile(GLenum type, char *src) {
	GLuint shId = glCreateShader(type);

	if (shId == 0) {
		LOGERR("CvAccARShader: Could not create shader.");
		return -1;
	}

    glShaderSource(shId, 1, (const GLchar**)&src, NULL);
    glCompileShader(shId);

    // check compile status
    GLint compileStatus;
    glGetShaderiv(shId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus != GL_TRUE) {
		LOGERR("CvAccARShader: Could not compile shader:");
		GLchar infoLogBuf[1024];
		GLsizei infoLogLen;
		glGetShaderInfoLog(shId, 1024, &infoLogLen, infoLogBuf);
		LOGERR("%s", infoLogBuf);

		glDeleteShader(shId);

		return -1;
	}

	return shId;
}
