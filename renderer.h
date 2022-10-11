// minimalistic code to draw a single triangle, this is not part of the API.
// required for compiling shaders on the fly, consider pre-compiling instead
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#include "d3dx12.h" // official helper file provided by microsoft
#include "Gateware.h"
#include "h2bParser.h"




struct VECTOR {
	float x, y, z;
};

struct VERTEX {
	VECTOR pos, uvw, nrm;
};

struct alignas(void*) ATTRIBUTES {
	VECTOR Kd; float d;
	VECTOR Ks; float Ns;
	VECTOR Ka; float sharpness;
	VECTOR Tf; float Ni;
	VECTOR Ke; unsigned illum;
};

struct BATCH {
	unsigned indexCount, indexOffset;
};

struct MATERIAL {
	ATTRIBUTES attrib;
	const char* name;
	const char* map_Kd;
	const char* map_Ks;
	const char* map_Ka;
	const char* map_Ke;
	const char* map_Ns;
	const char* map_d;
	const char* disp;
	const char* decal;
	const char* bump;
	const void* padding[2];
};

struct MESH {
	const char* name;
	BATCH drawInfo;
	unsigned materialIndex;
};


struct SCENE_DATA
{
	GW::MATH::GVECTORF sunDirection, sunColor, sunAmbience, cameraPos; // lighting info
	GW::MATH::GMATRIXF viewMatrix, projectionMatrix; // viewing info
	GW::MATH::GVECTORF padding[4]; // D3D12 requires 256 byte aligned constant buffers
};

struct MESH_DATA
{
	// per sub-mesh transform and material data
	GW::MATH::GMATRIXF world; // final world space transform
	ATTRIBUTES material; // color/texture of surface
	unsigned padding[28];
};





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



class Renderer
{
	// proxy handles
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GDirectX12Surface d3d;
	// what we need at a minimum to draw a triangle
	D3D12_VERTEX_BUFFER_VIEW					vertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource>		vertexBuffer;
	// TODO: Part 1g
	// TODO: Part 2c
	// TODO: Part 2e
	Microsoft::WRL::ComPtr<ID3D12RootSignature>	rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState>	pipeline;
	// TODO: Part 2a
	// TODO: Part 2b
	// TODO: Part 4f
public:
	Renderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX12Surface _d3d)
	{
		win = _win;
		d3d = _d3d;
		ID3D12Device* creator;
		d3d.GetDevice((void**)&creator);
		// TODO: part 2a
		// TODO: part 2b
		// TODO: Part 4f
		// TODO: Part 1c
		// Create Vertex Buffer
		float verts[] = {
			   0,   0.5f,
			 0.5f, -0.5f,
			-0.5f, -0.5f
		};
		creator->CreateCommittedResource( // using UPLOAD heap for simplicity
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), // DEFAULT recommend  
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(verts)),
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
		// Transfer triangle data to the vertex buffer.
		UINT8* transferMemoryLocation;
		vertexBuffer->Map(0, &CD3DX12_RANGE(0, 0),
			reinterpret_cast<void**>(&transferMemoryLocation));
		memcpy(transferMemoryLocation, verts, sizeof(verts));
		vertexBuffer->Unmap(0, nullptr);
		// Create a vertex View to send to a Draw() call.
		vertexView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexView.StrideInBytes = sizeof(float) * 2; // TODO: Part 1e
		vertexView.SizeInBytes = sizeof(verts); // TODO: Part 1d
		// TODO: Part 1g
		// TODO: Part 2d
		// TODO: Part 2e
		// TODO: Part 2f

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
		// TODO: Part 2g
		// create root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr,
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
		// TODO: Part 2a
		// TODO: Part 4d
		// grab the context & render target
		ID3D12GraphicsCommandList* cmd;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		d3d.GetCommandList((void**)&cmd);
		d3d.GetCurrentRenderTargetView((void**)&rtv);
		d3d.GetDepthStencilView((void**)&dsv);
		// setup the pipeline
		cmd->SetGraphicsRootSignature(rootSignature.Get());
		// TODO: Part 2h
		// TODO: Part 4e
		cmd->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		cmd->SetPipelineState(pipeline.Get());
		// now we can draw
		cmd->IASetVertexBuffers(0, 1, &vertexView);
		cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// TODO: Part 1h
		// TODO: Part 3b
			// TODO: Part 3c
			// TODO: Part 4e

		cmd->DrawInstanced(3, 1, 0, 0); // TODO: Part 1c
		// release temp handles
		cmd->Release();
	}
	~Renderer()
	{
		// ComPtr will auto release so nothing to do here 
	}
};
