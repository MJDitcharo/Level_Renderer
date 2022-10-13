#pragma once
#include "Model.h"
class Level
{
public:

	std::map<std::string, Model> uniqueMeshes;

	void levelParse(const char* fileName);


	Level();
	~Level();

private:

	GW::MATH::GMATRIXF createMatrix(std::vector<float> values);
};

Level::Level()
{
}

Level::~Level()
{
}


void Level::levelParse(const char* fileName)
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
			// Get and Print .h2b
			std::getline(f, h2b, '\n');

			// First Matrix Row
			{
				std::getline(f, strTemp, '(');
				std::getline(f, strTemp, ')');
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
				std::getline(f, strTemp, ')');
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
				std::getline(f, strTemp, ')');
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
				std::getline(f, strTemp, ')');
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


			size_t lastindex = h2b.find_last_of(".");
			std::string rawName = h2b.substr(0, lastindex);

			auto iter = uniqueMeshes.find(rawName);

			if (uniqueMeshes.end() != iter)
			{
				uniqueMeshes[rawName].worldMatrices.push_back(createMatrix(values));
			}
			else
			{
				std::string filePath = "../assets/OBJ/" + rawName + ".h2b";

				Model modelTemp;
				if (modelTemp.parser.Parse(filePath.c_str()))
				{
					modelTemp.worldMatrices.push_back(createMatrix(values));
					uniqueMeshes[rawName] = modelTemp;
				}
			}

			values.clear();
		}
	}
	f.close();
}


GW::MATH::GMATRIXF Level::createMatrix(std::vector<float> values)
{
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
			}
		}
	}

	return matTemp;

}
