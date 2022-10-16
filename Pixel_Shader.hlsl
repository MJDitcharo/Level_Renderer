
struct PS_IN { 
    float4 posH : SV_POSITION; // homogeneous projection space
    float3 nrmW : NORMAL; // normal in world space (for lighting)
    float3 posW : WORLD; // position in world space (for lighting)
    float3 tex : TEXTCOORD;
};



struct OBJ_ATTRIBUTES
{
    float3 Kd; // diffuse reflectivity
    float d; // dissolve (transparency) 
    float3 Ks; // specular reflectivity
    float Ns; // specular exponent
    float3 Ka; // ambient reflectivity
    float sharpness; // local reflection map sharpness
    float3 Tf; // transmission filter
    float Ni; // optical density (index of refraction)
    float3 Ke; // emissive reflectivity
    unsigned int illum; // illumination model
};

struct OUTPUT_TO_RASTERIZER
{
    float4 posH : SV_POSITION; // homogeneous projection space
    float3 nrmW : NORMAL; // normal in world space (for lighting)
    float3 posW : WORLD; // position in world space (for lighting)
};

struct SCENE_DATA
{
    float4 sunDirection, sunColor, sunAmbience, cameraPos;
    float4 pointLights[2];
    float4x4 viewMatrix, projectionMatrix;
    float4 padding[2];
};

struct MESH_MATRIX
{
    float4x4 world;
};

struct MESH_MATERIAL
{
    OBJ_ATTRIBUTES material;
};

ConstantBuffer<SCENE_DATA> cameraAndLights : register(b0);
ConstantBuffer<MESH_MATRIX> meshMatrix : register(b1);
ConstantBuffer<MESH_MATERIAL> meshMaterial : register(b2);



float4 main(PS_IN input) : SV_TARGET
{
    
    float3 viewDir = normalize(cameraAndLights.cameraPos.xyz - input.posW);
    float3 halfVector = normalize((-cameraAndLights.sunDirection.xyz) + viewDir);
    float intensity = (max(pow(saturate(dot(input.nrmW, halfVector)), meshMaterial.material.Ns), 0)) * 0.75;
    float reflectedLight = cameraAndLights.sunColor.xyz * meshMaterial.material.Ks * intensity;
    
    
    float lightRatio = saturate(dot(-cameraAndLights.sunDirection.xyz, input.nrmW));
    float3 result = ((lightRatio * cameraAndLights.sunColor.xyz) + cameraAndLights.sunAmbience.xyz) * (meshMaterial.material.Kd + reflectedLight);

    return float4(result, 1);
}