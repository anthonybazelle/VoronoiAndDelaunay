#include "EsgiShader.h"
#include "glew.h"
#include <cstdio>

void EsgiShader::DestroyProgram()
{
	glDetachShader(m_ProgramObject, m_VertexShader);
	glDeleteShader(m_VertexShader);
	glDetachShader(m_ProgramObject, m_VertexShader);
	glDeleteShader(m_VertexShader);
	glDeleteProgram(m_ProgramObject);
}

bool EsgiShader::CreateProgram()
{
	m_ProgramObject = glCreateProgram();
	glAttachShader(m_ProgramObject, m_VertexShader);
	glAttachShader(m_ProgramObject, m_FragmentShader);
	glLinkProgram(m_ProgramObject);
	return true;
}

uint32_t EsgiShader::CompileShader(uint32_t type, const char* code)
{
	uint32_t shaderObject = glCreateShader(type);
	if (shaderObject != 0)
	{
		glShaderSource(shaderObject, 1, &code, nullptr);
		glCompileShader(shaderObject);
	}
	return shaderObject;
}

bool EsgiShader::LoadVertexShader(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	auto len = ftell(file);
	rewind(file);
	auto buffer = new char[len+1];
	fread(buffer, len, 1, file);
	buffer[len ] = '\0';
	m_VertexShader = CompileShader(GL_VERTEX_SHADER, buffer);
	delete[] buffer;
	return (m_VertexShader != 0);
}

bool EsgiShader::LoadFragmentShader(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	fseek(file, 0, SEEK_END);
	auto len = ftell(file);
	rewind(file);
	auto buffer = new char[len+1];
	fread(buffer, len, 1, file);
	buffer[len ] = '\0';
	m_FragmentShader = CompileShader(GL_FRAGMENT_SHADER, buffer);
	delete[] buffer;
	return (m_FragmentShader != 0);
}

