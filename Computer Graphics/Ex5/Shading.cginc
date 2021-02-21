// Implements an adjusted version of the Blinn-Phong lighting model
float3 blinnPhong(float3 n, float3 v, float3 l, float shininess, float3 albedo)
{
    n = normalize(n);
    v = normalize(v);
    l = normalize(l);
    float3 h = normalize(l + v);

    float3 specular = 0.4 * pow(max(0, dot(n, h)), shininess);
    float3 diffuse = max(0, dot(n, l)) * albedo;
    return specular + diffuse;
}

// Reflects the given ray from the given hit point
void reflectRay(inout Ray ray, RayHit hit)
{
    float3 n = hit.normal;
    float3 v = normalize(-ray.direction);
    float3 r = normalize(2 * dot(n, v) * n - v);
    ray.direction = r;
    ray.origin = hit.position + EPS * n;
    ray.energy *= hit.material.specular;
}

// Refracts the given ray from the given hit point
void refractRay(inout Ray ray, RayHit hit)
{
    float3 n = normalize(hit.normal);
    float3 i = normalize(ray.direction);

    float eta1 = 1;
    float eta2 = hit.material.refractiveIndex;
    float eta = eta1 / eta2;
    if (dot(n, i) > 0)
    {
        eta = eta2 / eta1;
        n = -n;
    }

    float c1 = abs(dot(n, i));
    float c2 = sqrt(1 - eta * eta * (1 - c1 * c1));
    float3 t = (i + n * c1) * eta - c2 * n;
    
    ray.direction = normalize(t);
    ray.origin = hit.position - EPS * n;
}

// Samples the _SkyboxTexture at a given direction vector
float3 sampleSkybox(float3 direction)
{
    float theta = acos(direction.y) / -PI;
    float phi = atan2(direction.x, -direction.z) / -PI * 0.5f;
    return _SkyboxTexture.SampleLevel(sampler_SkyboxTexture, float2(phi, theta), 0).xyz;
}
