Shader "CG/Water"
{
    Properties
    {
        _CubeMap("Reflection Cube Map", Cube) = "" {}
        _NoiseScale("Texture Scale", Range(1, 100)) = 10 
        _TimeScale("Time Scale", Range(0.1, 5)) = 3 
        _BumpScale("Bump Scale", Range(0, 0.5)) = 0.05
    }
    SubShader
    {
        Pass
        {
            CGPROGRAM

                #pragma vertex vert
                #pragma fragment frag
                #include "UnityCG.cginc"
                #include "CGUtils.cginc"
                #include "CGRandom.cginc"

                #define DELTA 0.01

                // Declare used properties
                uniform samplerCUBE _CubeMap;
                uniform float _NoiseScale;
                uniform float _TimeScale;
                uniform float _BumpScale;

                struct appdata
                { 
                    float4 vertex   : POSITION;
                    float3 normal   : NORMAL;
                    float4 tangent  : TANGENT;
                    float2 uv       : TEXCOORD0;
                };

                struct v2f
                {
                    float4 pos      : SV_POSITION;
                    float3 normal   : NORMAL;
                    float2 uv       : TEXCOORD0;
                    float4 tangent  : TANGENT;
                    float4 pos_world : TEXCOORD1;
                };

                // Returns the value of a noise function simulating water, at coordinates uv and time t
                float waterNoise(float2 uv, float t)
                {
                    float noise1 = perlin3d(0.5 * float3(uv, t));
                    float noise2 =  0.5 * perlin3d(float3(uv, t));
                    float noise3 = 0.2 * perlin3d(float3(2 * uv, 3 * t));
                    return noise1 + noise2 + noise3;
                }

                // Returns the world-space bump-mapped normal for the given bumpMapData and time t
                float3 getWaterBumpMappedNormal(bumpMapData i, float t)
                {
                    // Calculate tangents 
                    float t_u = (waterNoise(float2(i.uv.x + i.du, i.uv.y), t) - waterNoise(i.uv, t)) / i.du;
                    float t_v = (waterNoise(float2(i.uv.x, i.uv.y + i.dv), t) - waterNoise(i.uv, t)) / i.dv;

                    // Multiply tangents by scaling factor
                    float3 n_h = normalize(float3(-t_u * i.bumpScale, -t_v * i.bumpScale, 1));

                    // Bitangent vector
                    float3 bitangent = normalize(cross(i.tangent, i.normal));

                    // Translate from tangent space to world space
                    return i.tangent * n_h.x + i.normal * n_h.z + bitangent * n_h.y;
                }


                v2f vert (appdata input)
                {
                    v2f output;

                    // Recalculate vertex after displacment
                    float perlin = waterNoise(input.uv * _NoiseScale, _Time.y * _TimeScale);
                    float4 vertex = input.vertex + float4(input.normal * perlin * _BumpScale, 0);

                    output.pos = UnityObjectToClipPos(vertex);
                    output.pos_world = mul(unity_ObjectToWorld, vertex);
                    output.uv = input.uv;
                    output.normal = input.normal;
                    output.tangent = input.tangent;
                    
                    return output;
                }

                fixed4 frag (v2f input) : SV_Target
                {
                    // Calculate normal for bump map
                    bumpMapData myBumps;
                    myBumps.normal = normalize(input.normal);
                    myBumps.tangent = normalize(input.tangent.xyz);
                    myBumps.uv = input.uv * _NoiseScale;
                    myBumps.du = DELTA;
                    myBumps.dv = DELTA;
                    myBumps.bumpScale = _BumpScale; 
                    float3 normal = normalize(getWaterBumpMappedNormal(myBumps, _Time.y*_TimeScale));
                    
                    // Calculate the ReflectedColor
                    float3 v = normalize(_WorldSpaceCameraPos - input.pos_world.xyz);
                    float3 r = normalize(2 * dot(v,normal) * normal - v);
                    float4 ReflectedColor = texCUBE(_CubeMap, r);
                    
                    return (1 - max(0, dot(normal, v)) + 0.2) * ReflectedColor;
                }

            ENDCG
        }
    }
}
