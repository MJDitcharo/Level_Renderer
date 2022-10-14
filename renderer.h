// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft
#include "Level.h"




class Renderer
{
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GDirectX12Surface d3d;
	GW::MATH::GMatrix mat;

	// Const buffer shit
	IDXGISwapChain4* swapChain;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	GW::MATH::GVector vecProxy;
	// Camera Proxy
	GW::INPUT::GInput gInput;
	GW::INPUT::GController gController;


	// what we need at a minimum to draw a triangle
	D3D12_VERTEX_BUFFER_VIEW					vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		vertexBufferTemp;
	D3D12_INDEX_BUFFER_VIEW						indexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		indexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>		constantBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeap;

	Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>	pipeline;

	// World, View, and Projection
	GW::MATH::GMATRIXF world;
	GW::MATH::GMATRIXF view;
	GW::MATH::GMATRIXF projection;

	Level level;

public:
	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX12Surface _d3d)
	{

		/////////////////////////////////////////////////////////////////////////////////
		// Initialize Proxies
		win = _win;
		d3d = _d3d;
		ID3D12Device* creator;
		d3d.GetDevice((void**)&creator);
		d3d.GetSwapchain4((void**)&swapChain);
		swapChain->GetDesc(&swapChainDesc);
		mat.Create();
		gController.Create();
		gInput.Create(_win);
		/////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////
		// Init View and Projection Matrix
		//mat.IdentityF(view);
		float fov = angleToRadian(65);
		float nPlane = 0.1f;
		float fPlane = 100.0f;
		float aspectRatio;
		d3d.GetAspectRatio(aspectRatio);
		mat.IdentityF(projection);
		GW::MATH::GVECTORF eye = { 0.0f, 3.0f, -9.0f, 1.0f };
		GW::MATH::GVECTORF at =  { 0.0f, 0.0f,  0.0f, 1.0f };
		GW::MATH::GVECTORF up =  { 0.0f, 1.0f, 0.0f, 1.0f };
		mat.LookAtLHF(eye, at, up, view);
		mat.ProjectionDirectXLHF(fov, aspectRatio, nPlane, fPlane, projection);
		/////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////
		//  Initialize Level and Create Vertex/Index Buffers
		level.levelParse("../TestLevel.txt");

		UINT offset = 0;

		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
		descHeapDesc.NumDescriptors = 1 + (2 * level.uniqueMeshes.size());
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		creator->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));

		SCENE_DATA data = {};
		data.viewMatrix = view;
		data.projectionMatrix = projection;
		CreateConstantBuffer(creator, nullptr, sizeof(data), data);
		CreateConstantBufferSceneView(creator, offset);

		for (auto it = level.uniqueMeshes.begin(); it != level.uniqueMeshes.end(); ++it)
		{
			CreateVertexBuffer(creator, &it->second);
			CreateIndexBuffer(creator, &it->second);
			CreateConstantBuffer(creator, &it->second);
			CreateConstantBufferModelView(creator, &it->second, offset);
		}
		/////////////////////////////////////////////////////////////////////////////////

		//UINT desc_heap_size = _creator->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);





		







		// Create Vertex Shader
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif

		std::string VS = ShaderAsString("../Vertex_Shader.hlsl");
		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
		if (FAILED(D3DCompile(VS.c_str(), strlen(VS.c_str()),
			nullptr, nullptr, nullptr, "main", "vs_5_1", compilerFlags, 0,
			vsBlob.GetAddressOf(), errors.GetAddressOf())))
		{
			std::cout << (char*)errors->GetBufferPointer() << std::endl;
			abort();
		}
		// Create Pixel Shader

		std::string PS = ShaderAsString("../Pixel_Shader.hlsl");
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob; errors.Reset();
		if (FAILED(D3DCompile(PS.c_str(), strlen(PS.c_str()),
			nullptr, nullptr, nullptr, "main", "ps_5_1", compilerFlags, 0,
			psBlob.GetAddressOf(), errors.GetAddressOf())))
		{
			std::cout << (char*)errors->GetBufferPointer() << std::endl;
			abort();
		}

		// TODO: Part 1e
		// Create Input Layout
		D3D12_INPUT_ELEMENT_DESC format[] = {

			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXTCOORD",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }

		};


		CD3DX12_ROOT_PARAMETER rootParams[3];
		rootParams[0].InitAsConstantBufferView(0);
		rootParams[1].InitAsConstantBufferView(1);
		rootParams[2].InitAsConstantBufferView(2);

		// create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(ARRAYSIZE(rootParams), rootParams, 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		D3D12SerializeRootSignature(&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errors);
		creator->CreateRootSignature(0, signature->GetBufferPointer(),
			signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

		// create pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psDesc;
		ZeroMemory(&psDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psDesc.InputLayout = { format, ARRAYSIZE(format) };
		psDesc.pRootSignature = rootSignature.Get();
		psDesc.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
		psDesc.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
		psDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psDesc.SampleMask = UINT_MAX;
		psDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psDesc.NumRenderTargets = 1;
		psDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psDesc.SampleDesc.Count = 1;
		creator->CreateGraphicsPipelineState(&psDesc, IID_PPV_ARGS(&pipeline));
		// free temporary handle
		creator->Release();
	}
	void Render()
	{

		// grab the context & render target
		ID3D12GraphicsCommandList* cmd;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		d3d.GetCommandList((void**)&cmd);
		d3d.GetCurrentRenderTargetView((void**)&rtv);
		d3d.GetDepthStencilView((void**)&dsv);


		// setup the pipeline
		cmd->SetGraphicsRootSignature(rootSignature.Get());




		cmd->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		cmd->SetPipelineState(pipeline.Get());


		// now we can draw
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		cmd->SetGraphicsRootConstantBufferView(0, constantBuffer->GetGPUVirtualAddress());	// A

		for (const auto& model : level.uniqueMeshes)
		{
			cmd->IASetVertexBuffers(0, 1, &model.second.vertexView);
			cmd->IASetIndexBuffer(&model.second.indexView);
			
			for (size_t i = 0; i < model.second.worldMatrices.size(); i++)
			{
				cmd->SetGraphicsRootConstantBufferView(1, model.second.worldConstBuffer->GetGPUVirtualAddress() + (i * sizeof(GW::MATH::GMATRIXF)));	// B
				for (const auto& submesh : model.second.parser.meshes)
				{
					// C
					UINT material_index = submesh.materialIndex;
					cmd->DrawIndexedInstanced(submesh.drawInfo.indexCount, 1, submesh.drawInfo.indexOffset, 0, 0);
				}
			}
		}


		//cmd->DrawInstanced(3, 1, 0, 0); // TODO: Part 1c

		// release temp handles
		cmd->Release();
	}
	~Renderer()
	{
		// ComPtr will auto release so nothing to do here 
	}

	/*void InitBuffers(ID3D12Device* _creator)
	{
		CreateVertexBuffer(_creator);
		CreateIndexBuffer(_creator);
	}*/

	inline UINT Renderer::CalculateConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
	};

	void CreateVertexBuffer(ID3D12Device* _creator, Model* _model)
	{
		unsigned vertBufferSize = sizeof(H2B::VERTEX) * _model->parser.vertexCount;

		HRESULT hr = _creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(vertBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(_model->vertexBuffer.GetAddressOf()));
		if (FAILED(hr)) {
			hr = _creator->GetDeviceRemovedReason();
			throw(std::runtime_error::runtime_error("Error creating a vertex buffer."));
		}

		UINT8* transferMemoryLocation;
		_model->vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&transferMemoryLocation));
		memcpy(transferMemoryLocation, _model->parser.vertices.data(), vertBufferSize);
		_model->vertexBuffer->Unmap(0, nullptr);


		// Create a vertex View to send to a Draw() call.
		_model->vertexView.BufferLocation = _model->vertexBuffer->GetGPUVirtualAddress();
		_model->vertexView.StrideInBytes = sizeof(H2B::VERTEX);
		_model->vertexView.SizeInBytes = vertBufferSize;
	}

	void CreateIndexBuffer(ID3D12Device* _creator, Model* _model)
	{
		unsigned indexBufferSize = sizeof(unsigned) * _model->parser.indexCount;

		HRESULT hr = _creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(_model->indexBuffer.GetAddressOf()));
		if (FAILED(hr))
			throw(std::runtime_error::runtime_error("Error creating an index buffer."));

		UINT8* transferMemoryLocation;
		_model->indexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&transferMemoryLocation));
		memcpy(transferMemoryLocation, _model->parser.indices.data(), indexBufferSize);
		_model->indexBuffer->Unmap(0, nullptr);

		// Create IndexView 
		_model->indexView.BufferLocation = _model->indexBuffer->GetGPUVirtualAddress();
		_model->indexView.SizeInBytes = indexBufferSize;
		_model->indexView.Format = DXGI_FORMAT_R32_UINT;
	}

	void CreateConstantBuffer(ID3D12Device* _creator, Model* _model, unsigned constBuffMemory, SCENE_DATA sceneData)
	{


		HRESULT hr = _creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(CalculateConstantBufferByteSize(constBuffMemory)),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(constantBuffer.GetAddressOf()));
		if (FAILED(hr))
			throw(std::runtime_error::runtime_error("Error creating a scene const buffer."));

		UINT8* transferMemoryLocation;
		constantBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&transferMemoryLocation));
		memcpy(transferMemoryLocation, &sceneData, sizeof(SCENE_DATA));
		constantBuffer->Unmap(0, nullptr);
	}

	void CreateConstantBuffer(ID3D12Device* _creator, Model* _model)
	{

		//UINT desc_heap_size = _creator->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		/////////////////////////////////////////////////////////////////////////////////
		// World Const Buffer
		{

			unsigned constantBufferSize = CalculateConstantBufferByteSize(sizeof(GW::MATH::GMATRIXF) * _model->worldMatrices.size());
			HRESULT hr = _creator->CreateCommittedResource( // using UPLOAD heap for simplicity
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
				D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(_model->worldConstBuffer.GetAddressOf()));
			if (FAILED(hr))
				throw(std::runtime_error::runtime_error("Error creating a world const buffer."));

			UINT8* transferMemoryLocation;
			_model->worldConstBuffer->Map(0, &CD3DX12_RANGE(0, 0),
				reinterpret_cast<void**>(&transferMemoryLocation));
			memcpy(transferMemoryLocation, _model->worldMatrices.data(), constantBufferSize);
			_model->worldConstBuffer->Unmap(0, nullptr);
		}
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		// Material Const Buffer
		{
			unsigned constantBufferSize = CalculateConstantBufferByteSize(sizeof(H2B::MATERIAL) * _model->parser.materialCount);
			HRESULT hr = _creator->CreateCommittedResource( // using UPLOAD heap for simplicity
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
				D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(_model->materialConstBuffer.GetAddressOf()));
			if (FAILED(hr))
				throw(std::runtime_error::runtime_error("Error creating a material const buffer."));

			UINT8* transferMemoryLocation;
			_model->materialConstBuffer->Map(0, &CD3DX12_RANGE(0, 0),
				reinterpret_cast<void**>(&transferMemoryLocation));
			memcpy(transferMemoryLocation, _model->parser.materials.data(), constantBufferSize);
			_model->materialConstBuffer->Unmap(0, nullptr);
		}
		/////////////////////////////////////////////////////////////////////////////////




	}

	void CreateConstantBufferSceneView(ID3D12Device* _creator, UINT& offset)
	{

		UINT desc_heap_size = _creator->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_CONSTANT_BUFFER_VIEW_DESC bufferViewWorld;
		bufferViewWorld.BufferLocation = constantBuffer->GetGPUVirtualAddress();
		bufferViewWorld.SizeInBytes = CalculateConstantBufferByteSize(sizeof(GW::MATH::GMATRIXF) * 2);

		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(descHeap->GetCPUDescriptorHandleForHeapStart(), offset, desc_heap_size);
		_creator->CreateConstantBufferView(&bufferViewWorld, handle);

		offset++;
	}
	void CreateConstantBufferModelView(ID3D12Device* _creator, Model* _model, UINT& offset)
	{
		UINT desc_heap_size = _creator->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		D3D12_CONSTANT_BUFFER_VIEW_DESC bufferViewWorld;
		bufferViewWorld.BufferLocation = _model->worldConstBuffer->GetGPUVirtualAddress();
		bufferViewWorld.SizeInBytes = CalculateConstantBufferByteSize(_model->worldMatrices.size() * sizeof(GW::MATH::GMATRIXF));

		_model->descHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), offset, desc_heap_size);
		_creator->CreateConstantBufferView(&bufferViewWorld, _model->descHandle);

		offset++;

		D3D12_CONSTANT_BUFFER_VIEW_DESC bufferView;
		bufferView.BufferLocation = _model->materialConstBuffer->GetGPUVirtualAddress();
		bufferView.SizeInBytes = CalculateConstantBufferByteSize(sizeof(H2B::MATERIAL) * _model->parser.materialCount);

		//_model->descHandle = descHeap->GetCPUDescriptorHandleForHeapStart();
		_creator->CreateConstantBufferView(&bufferViewWorld, CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeap->GetCPUDescriptorHandleForHeapStart(), offset, desc_heap_size));

		offset++;
	}

};

