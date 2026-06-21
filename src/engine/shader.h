#pragma once

#include <string>

#include "gl_common.h"
#include "logger.h"

class Shader {
public:
	std::string vertexCode = "#version 330 core \n"
		"layout (location = 0) in vec3 pos; \n" 
		"layout (location = 1) in vec2 texc;\n"
		"layout (location = 2) in vec3 norm;\n"
		""
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		""
		"out vec3 FragPos;"
		"out vec2 uv;"
		"out vec3 normal;"
		"void main() { \n" 
		"uv = texc;"
		"mat3 modelVector = mat3(transpose(inverse(model)));"

		"normal = normalize(modelVector * norm);"
		"FragPos = vec3(model * vec4(pos, 1.0));"
		"gl_Position = projection * view * model * vec4(pos,1.0); \n"
		"}";

	std::string fragmentCode =
		R"(#version 330 core

out vec4 FragColor;

uniform sampler2D text;
uniform bool useTexture;
uniform vec3 color;
uniform bool useLight;
uniform float shine;

uniform vec3 viewPos;

in vec3 FragPos;
in vec2 uv;
in vec3 normal;

struct PointLight {
	vec3 position;
	vec3 color;
	float strength;
	float radius;
};
uniform int pointLightsCount;
#define POINT_LIGHTS_ARR_SIZE 64
uniform PointLight pointLights[POINT_LIGHTS_ARR_SIZE];

vec3 calculateDiffuse(vec3 lightColor, vec2 uv, vec3 norm, vec3 lightDir) {
    vec3 diffuse;
    float diff = max(dot(norm, lightDir), 0.0);

    
    if (useTexture) {
        diffuse = diff * texture(text, vec2(uv.x, uv.y)).rgb * lightColor * color;
    } else {
        diffuse = diff * lightColor * color;
    }
    
    return diffuse;
}


vec3 calculateSpecular(vec3 lightColor, vec3 norm, vec3 lightDir, vec3 viewDir, vec2 uv) {
    float nDot = dot(norm, lightDir);

    float spec;

    if (false) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), shine);
    } else {
         vec3 reflectDir = reflect(lightDir, norm);
        spec = pow(max(dot(viewDir, -reflectDir), 0.0), shine);
    }

    
 
    vec3 specular = vec3(0.0, 0.0, 0.0);

    if (nDot > 0) {
		specular = spec * lightColor * nDot;
    }

    return specular;
}

vec4 calculatePointLight(PointLight light, vec3 norm, vec3 FragPos, vec3 viewDir, vec2 uv) 
{
    float distance = length(light.position - FragPos);
    float attenuation = smoothstep(light.radius, 0, distance);

    vec3 lightDir = normalize(light.position - FragPos);

    vec3 diffuse = calculateDiffuse(light.color, uv, norm, lightDir);
	vec3 specular = calculateSpecular(light.color, norm, lightDir, viewDir, uv);
	

    diffuse *= light.strength * attenuation;
    specular *= light.strength * attenuation;
    vec3 result = (diffuse + specular);
    return vec4(result, 1.0) ;
}

void main() {
	vec3 result = vec3(0.0, 0.0, 0.0);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	if (useLight) {
		for(int i = 0; i < pointLightsCount; i++) {
			result += calculatePointLight(pointLights[i], normal,
					FragPos, viewDir, uv).rgb;
		}
	} else {
		if (useTexture) {
			result = texture(text, vec2(uv.x, uv.y)).rgb;
		} else {
			result = color;
		}
	}
	FragColor = vec4(result, 1.0);
}	

)";
		

	const char* vCode = vertexCode.c_str();
	const char* fCode = fragmentCode.c_str();


	void recompile() {
		glDeleteProgram(id_);

		GLuint vertexShader;
		GLuint fragmentShader;

		const char* vCode = vertexCode.c_str();
		const char* fCode = fragmentCode.c_str();

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vCode, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "VERTEX");

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "FRAGMENT");

		id_ = glCreateProgram();
		glAttachShader(id_, vertexShader);
		glAttachShader(id_, fragmentShader);
		glLinkProgram(id_);
		checkCompileErrors(id_, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader() {
		GLuint vertexShader;
		GLuint fragmentShader;

		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vCode, NULL);
		glCompileShader(vertexShader);
		checkCompileErrors(vertexShader, "VERTEX");

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fCode, NULL);
		glCompileShader(fragmentShader);
		checkCompileErrors(fragmentShader, "FRAGMENT");

		id_ = glCreateProgram();
		glAttachShader(id_, vertexShader);
		glAttachShader(id_, fragmentShader);
		glLinkProgram(id_);
		checkCompileErrors(id_, "PROGRAM");

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	~Shader() {
		glDeleteProgram(id_);
	}

	void use() {
		glUseProgram(id_);
	}

	void checkCompileErrors(GLuint shader, std::string type) {
		int success;
		char infoLog[512];

		if (type == "PROGRAM") {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, 512, nullptr, infoLog);
				logError("[SHADER] ", type, " ", infoLog);
			}
		}
		else {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, 512, nullptr, infoLog);
				logError("[SHADER] ", type, " ", infoLog);
			}
		}
	}
	void setFloat(const std::string& name, const float value) const
	{
		glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
	}

	void setInt(const std::string& name, const int value) const
	{
		glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setVec3(const std::string& name, const glm::vec3& vec) const
	{
		glUniform3f(glGetUniformLocation(id_, name.c_str()), vec.x, vec.y, vec.z);
	}

private:
	unsigned int id_;
};