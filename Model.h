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


	Model(GW::GRAPHICS::GDirectX12Surface _d3d);
	Model();
	~Model();

	void initBuffers();
	void createVertexBuffer();
	void createIndexBuffer();


private:


	GW::GRAPHICS::GDirectX12Surface d3d;
	ID3D12Device* creator;

};



Model::Model(GW::GRAPHICS::GDirectX12Surface _d3d)
{
	d3d = _d3d;
	d3d.GetDevice((void**)&creator);

}

Model::Model()
{

}

Model::~Model()
{
}

void Model::initBuffers() 
{

}
void Model::createVertexBuffer()
{

	unsigned vertBufferSize = sizeof(H2B::VERTEX) * parser.vertexCount;

	creator->CreateCommittedResource( // using UPLOAD heap for simplicity
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
		D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vertBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));

	UINT8* transferMemoryLocation;
	vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
		reinterpret_cast<void**>(&transferMemoryLocation));
	memcpy(transferMemoryLocation, parser.vertices.data(), vertBufferSize);
	vertexBuffer->Unmap(0, nullptr);


	// Create a vertex View to send to a Draw() call.
	vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexView.StrideInBytes = sizeof(H2B::VERTEX);
	vertexView.SizeInBytes = vertBufferSize;

}
void Model::createIndexBuffer()
{

}