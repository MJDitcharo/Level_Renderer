#pragma once
#include "Define.h"


class Model
{
public:

	H2B::Parser parser;
	MESH_DATA mesh;
	unsigned gpuAddress;


	D3D12_VERTEX_BUFFER_VIEW					vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		vertexBuffer;
	D3D12_INDEX_BUFFER_VIEW						indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>		constantBuffer;




	Model();
	~Model();

private:


	void levelParse(const char* fileName, std::vector<GW::MATH::GMATRIXF>* output, std::vector<std::string>* h2bOutput)
	{
		std::fstream f;
		std::vector<float> values;
		f.open(fileName, std::ios::in);

		while (!f.eof())
		{
			std::string str, h2b, strTemp;
			std::getline(f, str, '\n');
			if (std::strcmp(str.c_str(), "MESH") == 0/* || std::strcmp(str.c_str(), "LIGHT") == 0 || std::strcmp(str.c_str(), "CAMERA") == 0*/)
			{
				std::cout << str << std::endl;

				// Get and Print .h2b
				std::getline(f, h2b, '\n');
				std::cout << h2b << std::endl;
				h2bOutput->push_back(h2b);

				// First Matrix Row
				{
					std::getline(f, strTemp, '(');
					std::cout << strTemp;
					std::getline(f, strTemp, ')');
					std::cout << '(' << strTemp << ')';
					std::string numString = "";
					strTemp.append(")");
					for (std::string::iterator it = strTemp.begin(); it != strTemp.end(); ++it)
					{
						if (*it != ',' && *it != ')')
							numString += *it;
						else
						{
							float numOutput = std::stof(numString);
							values.push_back(numOutput);
							numString = "";
						}
					}
				}

				// Second Matrix Row
				{
					std::getline(f, strTemp, '(');
					std::cout << strTemp;
					std::getline(f, strTemp, ')');
					std::cout << '(' << strTemp << ')';
					std::string numString = "";
					strTemp.append(")");
					for (std::string::iterator it = strTemp.begin(); it != strTemp.end(); ++it)
					{
						if (*it != ',' && *it != ')')
							numString += *it;
						else
						{
							float numOutput = std::stof(numString);
							values.push_back(numOutput);
							numString = "";
						}
					}
				}

				// Third Matrix Row
				{
					std::getline(f, strTemp, '(');
					std::cout << strTemp;
					std::getline(f, strTemp, ')');
					std::cout << '(' << strTemp << ')';
					std::string numString = "";
					strTemp.append(")");
					for (std::string::iterator it = strTemp.begin(); it != strTemp.end(); ++it)
					{
						if (*it != ',' && *it != ')')
							numString += *it;
						else
						{
							float numOutput = std::stof(numString);
							values.push_back(numOutput);
							numString = "";
						}
					}
				}

				// Last Matrix Row
				{
					std::getline(f, strTemp, '(');
					std::cout << strTemp;
					std::getline(f, strTemp, ')');
					std::cout << '(' << strTemp << ')';
					std::string numString = "";
					strTemp.append(")");
					for (std::string::iterator it = strTemp.begin(); it != strTemp.end(); ++it)
					{
						if (*it != ',' && *it != ')')
							numString += *it;
						else
						{
							float numOutput = std::stof(numString);
							values.push_back(numOutput);
							numString = "";
						}
					}
				}
				std::cout << '\n';
			}
		}
		f.close();


		GW::MATH::GMATRIXF matTemp;
		for (size_t j = 0; j < values.size(); j += 16)
		{
			for (size_t i = 0; i < 16; i += 4)
			{

				if (i < 4)
				{
					matTemp.row1.x = values[i];
					matTemp.row1.y = values[i + 1];
					matTemp.row1.z = values[i + 2];
					matTemp.row1.w = values[i + 3];
				}
				else if (i < 8)
				{
					matTemp.row2.x = values[i];
					matTemp.row2.y = values[i + 1];
					matTemp.row2.z = values[i + 2];
					matTemp.row2.w = values[i + 3];
				}
				else if (i < 12)
				{
					matTemp.row3.x = values[i];
					matTemp.row3.y = values[i + 1];
					matTemp.row3.z = values[i + 2];
					matTemp.row3.w = values[i + 3];
				}
				else if (i < 16)
				{
					matTemp.row4.x = values[i];
					matTemp.row4.y = values[i + 1];
					matTemp.row4.z = values[i + 2];
					matTemp.row4.w = values[i + 3];
					output->push_back(matTemp);
				}
			}
		}

	}


};

Model::Model()
{
}

Model::~Model()
{
}