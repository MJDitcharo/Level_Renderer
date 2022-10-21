
struct PS_IN
{
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

struct POINT_LIGHT
{
    float4 color;
    float4 pos;
};

struct OUTPUT_TO_RASTERIZER
{
    float4 posH : SV_POSITION; // homogeneous projection space
    float3 nrmW : NORMAL; // normal in world space (for lighting)
    float3 posW : WORLD; // position in world space (for lighting)
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

ConstantBuffer<SCENE_DATA> cameraAndLights : register(b0);
ConstantBuffer<MESH_MATRIX> meshMatrix : register(b1);
ConstantBuffer<MESH_MATERIAL> meshMaterial : register(b2);



float4 main(PS_IN input) : SV_TARGET
{
    input.nrmW = normalize(input.nrmW);
    
    float3 viewDir = normalize(cameraAndLights.cameraPos[view].xyz - input.posW);
    float3 halfVector = normalize((-cameraAndLights.sunDirection.xyz) + viewDir);
    
    float exponent = meshMaterial.material.Ns == 0 ? 96 : meshMaterial.material.Ns;
    float power = pow(saturate(dot(input.nrmW, halfVector)), exponent);
    float intensity = (max(power, 0));
    float3 reflectedLight = cameraAndLights.sunColor.xyz * meshMaterial.material.Ks * intensity;
    
    
    float3 pointLightResult = { 0, 0, 0 };
    
    for (int i = 0; i < pLightCount; i++)
    {
        float3 pointDir = normalize(cameraAndLights.pointLights[i].pos.xyz - input.posW);
        float pointRatio = saturate(dot(pointDir, input.nrmW));
        float pointAttenuation = 1.0 - saturate(length(cameraAndLights.pointLights[i].pos.xyz - input.posW) / cameraAndLights.pointLights[i].color.w);
        pointLightResult += pointAttenuation * pointRatio * cameraAndLights.pointLights[i].color.xyz * meshMaterial.material.Kd;
    }
    
    
    
    float lightRatio = saturate(dot(-cameraAndLights.sunDirection.xyz, input.nrmW));
    float3 result = (saturate(lightRatio * cameraAndLights.sunColor.xyz) + cameraAndLights.sunAmbience.xyz) * meshMaterial.material.Kd + reflectedLight;

    result += pointLightResult;
    return float4(result, 1);
}