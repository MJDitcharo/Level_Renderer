#pragma pack_matrix(row_major)


struct VS_IN {
	float3 pos : POSITION;
	float3 tex : TEXTCOORD;
	float3 norm : NORMAL;
};
struct VS_OUT { 
    float4 posH : SV_POSITION; // homogeneous projection space
    float3 nrmW : NORMAL; // normal in world space (for lighting)
    float3 posW : WORLD; // position in world space (for lighting)
	float3 tex : TEXTCOORD;
};


struct OBJ_ATTRIBUTES
{
	float3           Kd; // diffuse reflectivity
	float	          d; // dissolve (transparency) 
	float3           Ks; // specular reflectivity
	float            Ns; // specular exponent
	float3           Ka; // ambient reflectivity
	float     sharpness; // local reflection map sharpness
	float3           Tf; // transmission filter
	float            Ni; // optical density (index of refraction)
	float3           Ke; // emissive reflectivity
	unsigned int    illum; // illumination model
};

struct SCENE_DATA
{
	float4 sunDirection, sunColor, sunAmbience, cameraPos;
	float4x4 viewMatrix, projectionMatrix;
	float4 padding[4];
};

struct MESH_DATA
{
	float4x4 world;
	OBJ_ATTRIBUTES material;
	unsigned int padding[28];

};

struct OUTPUT_TO_RASTERIZER
{
    float4 posH : SV_POSITION; // homogeneous projection space
    float3 nrmW : NORMAL; // normal in world space (for lighting)
    float3 posW : WORLD; // position in world space (for lighting)
};

ConstantBuffer<SCENE_DATA> cameraAndLights   : register(b0, Space0);
ConstantBuffer<MESH_DATA>  meshInfo			 : register(b1, Space0);



VS_OUT main(VS_IN input)
{

	
    VS_OUT output = (VS_OUT) 0;
    output.posW = input.pos;
	
    output.posH = mul(float4(output.posW, 1), meshInfo.world);
    output.posH = mul(output.posH, cameraAndLights.viewMatrix);
    output.posH = mul(output.posH, cameraAndLights.projectionMatrix);
	
    output.posW = mul(float4(input.pos, 1), meshInfo.world);
    output.nrmW = mul(input.norm, meshInfo.world);
	
	output.tex = input.tex;
	

	return output;
	// TODO: Part 4b
}