#pragma once
#include "h2bParser.h"
#include "Gateware.h"
#include <map>
#include <string>
#include <chrono>



constexpr auto PI = 3.14;

struct SCENE_DATA
{
	GW::MATH::GVECTORF sunDirection, sunColor, sunAmbience, cameraPos; // lighting info
	GW::MATH::GMATRIXF viewMatrix, projectionMatrix; // viewing info
	GW::MATH::GVECTORF padding[4]; // D3D12 requires 256 byte aligned constant buffers
};


float angleToRadian(float input)
{
	return (input * PI) / 180;
}

std::string ShaderAsString(const char* shaderFilePath) {
	std::string output;
	unsigned int stringLength = 0;
	GW::SYSTEM::GFile file; file.Create();
	file.GetFileSize(shaderFilePath, stringLength);
	if (stringLength && +file.OpenBinaryRead(shaderFilePath)) {
		output.resize(stringLength);
		file.Read(&output[0], stringLength);
	}
	else
		std::cout << "ERROR: Shader Source File \"" << shaderFilePath << "\" Not Found!" << std::endl;
	return output;
}