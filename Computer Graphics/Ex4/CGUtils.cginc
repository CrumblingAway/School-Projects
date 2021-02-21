#ifndef CG_UTILS_INCLUDED
#define CG_UTILS_INCLUDED

#define PI 3.141592653

// A struct containing all the data needed for bump-mapping
struct bumpMapData
{ 
    float3 normal;       // Mesh surface normal at the point
    float3 tangent;      // Mesh surface tangent at the point
    float2 uv;           // UV coordinates of the point
    sampler2D heightMap; // Heightmap texture to use for bump mapping
    float du;            // Increment size for u partial derivative approximation
    float dv;            // Increment size for v partial derivative approximation
    float bumpScale;     // Bump scaling factor
};


// Receives pos in 3D cartesian coordinates (x, y, z)
// Returns UV coordinates corresponding to pos using spherical texture mapping
float2 getSphericalUV(float3 pos)
{
    float radius = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
    float theta = atan2(pos.z, pos.x);
    float phi = acos(pos.y / radius);
    
    float u = 0.5 + theta / (2 * PI);
    float v = 1 - phi / PI;
    
    return float2(u, v);
}

// Implements an adjusted version of the Blinn-Phong lighting model
fixed3 blinnPhong(float3 n, float3 v, float3 l, float shininess, fixed4 albedo, fixed4 specularity, float ambientIntensity)
{
    fixed3 Ambient = ambientIntensity * albedo.rgb;
    fixed3 Diffuse =  max(0,dot(n,l)) * albedo.rgb;
    fixed3 h = normalize(l + v);
    fixed3 Specular =  pow(max(0,dot(n,h)),shininess) * specularity.rgb;
    return Ambient + Diffuse + Specular;
}

// Returns the world-space bump-mapped normal for the given bumpMapData
float3 getBumpMappedNormal(bumpMapData i)
{
    // Calculate tangents 
    float t_u = (tex2D(i.heightMap, float2(i.uv.x + i.du, i.uv.y)) - tex2D(i.heightMap, i.uv)) / i.du;
    float t_v = (tex2D(i.heightMap, float2(i.uv.x, i.uv.y + i.dv)) - tex2D(i.heightMap, i.uv)) / i.dv;

    // Multiply tangents by scaling factor
    float3 n_h = normalize(float3(-t_u * i.bumpScale, -t_v * i.bumpScale, 1));

    // Bitangent vector
    float3 bitangent = normalize(cross(i.tangent, i.normal));

    // Translate from tangent space to world space
    return i.tangent * n_h.x + i.normal * n_h.z + bitangent * n_h.y;
}


#endif // CG_UTILS_INCLUDED
