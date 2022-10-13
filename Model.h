#pragma once
#include "Define.h"


class Model
{
public:

	H2B::Parser parser;
	std::vector<GW::MATH::GMATRIXF> worldMatrices;
	unsigned* gpuAddress;


	D3D12_VERTEX_BUFFER_VIEW					vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		vertexBuffer;
	D3D12_INDEX_BUFFER_VIEW						indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>		constantBuffer;


	Model();
	~Model();


private:


	

};


Model::Model()
{

}

Model::~Model()
{
}


