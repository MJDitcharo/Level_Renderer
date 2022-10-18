#pragma once
#include "h2bParser.h"
#include "Gateware.h"
#include <map>
#include <string>
#include <chrono>
#include <windows.h>
#include <shobjidl.h> 

constexpr auto PI = 3.14;


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
   
}


struct SCENE_DATA
{
	GW::MATH::GVECTORF sunDirection, sunColor, sunAmbience, cameraPos; // lighting info
	GW::MATH::GVECTORF pointLights[2];
	GW::MATH::GMATRIXF viewMatrix, projectionMatrix; // viewing info
	GW::MATH::GVECTORF padding[2]; // D3D12 requires 256 byte aligned constant buffers
};


std::string OpenFileDialogue()
{

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
        COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr))
    {
        IFileOpenDialog* pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
                        std::string tempString = converter.to_bytes(pszFilePath);
                        return tempString.c_str();


                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    return nullptr;
}

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