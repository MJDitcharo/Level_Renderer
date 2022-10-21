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

cbuffer INDEXES : register(b3)
{
    uint view;
    uint proj;
    uint pLightCount;
};

struct POINT_LIGHT
{
    float4 color;
    float4 posAndRadius;
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
    float4 sunDirection, sunColor, sunAmbience;
    POINT_LIGHT pointLights[16];
    float4 cameraPos[4];
    float4x4 viewMatrix[4];
    float4x4 projectionMatrix[2];
};

struct MESH_MATRIX
{
	float4x4 world;
};

struct MESH_MATERIAL
{
    OBJ_ATTRIBUTES material;
};


ConstantBuffer<SCENE_DATA>    cameraAndLights    : register(b0);
ConstantBuffer<MESH_MATRIX>   meshMatrix		 : register(b1);
ConstantBuffer<MESH_MATERIAL> meshMaterial       : register(b2);


VS_OUT main(VS_IN input)
{
	
    VS_OUT output = (VS_OUT) 0;
    output.posW = input.pos;
	
    output.posH = mul(float4(output.posW, 1), meshMatrix.world);
    output.posH = mul(output.posH, cameraAndLights.viewMatrix[view]);
    output.posH = mul(output.posH, cameraAndLights.projectionMatrix[proj]);
	
    output.posW = mul(float4(input.pos, 1), meshMatrix.world).xyz;
    output.nrmW = mul(float4(input.norm, 0), meshMatrix.world).xyz;
	output.tex = input.tex;
	

	return output;
}