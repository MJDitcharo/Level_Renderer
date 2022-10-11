
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
    float4x4 viewMatrix, projectionMatrix;
    float4 padding[4];
};

struct MESH_DATA
{
    float4x4 world;
    OBJ_ATTRIBUTES material;
    unsigned int padding[28];

};

//ConstantBuffer<SCENE_DATA> cameraAndLights : register(b0, Space0);
//ConstantBuffer<MESH_DATA> meshInfo : register(b1, Space0);


// TODO: Part 2b
// TODO: Part 4f
// TODO: Part 4b

float4 main(PS_IN input) : SV_TARGET
{
    
    //float3 viewDir = normalize(cameraAndLights.cameraPos.xyz - input.posW);
    //float3 halfVector = normalize((-cameraAndLights.sunDirection.xyz) + viewDir);
    //float intensity = max(pow(saturate(dot(input.nrmW, halfVector)), meshInfo.material.Ns), 0);
    //float reflectedLight = cameraAndLights.sunColor.xyz * meshInfo.material.Ks * intensity;
    
    
    //float lightRatio = saturate(dot(-cameraAndLights.sunDirection.xyz, input.nrmW));
    //float3 result = ((lightRatio * cameraAndLights.sunColor.xyz) + cameraAndLights.sunAmbience.xyz) * (meshInfo.material.Kd + reflectedLight);

    // TODO: Part 4g
    return float4(input.nrmW, 1); // TODO: Part 1a
}