Shader "CG/Earth"
{
    Properties
    {
        [NoScaleOffset] _AlbedoMap ("Albedo Map", 2D) = "defaulttexture" {}
        _Ambient ("Ambient", Range(0, 1)) = 0.15
        [NoScaleOffset] _SpecularMap ("Specular Map", 2D) = "defaulttexture" {}
        _Shininess ("Shininess", Range(0.1, 100)) = 50
        [NoScaleOffset] _HeightMap ("Height Map", 2D) = "defaulttexture" {}
        _BumpScale ("Bump Scale", Range(1, 100)) = 30
        [NoScaleOffset] _CloudMap ("Cloud Map", 2D) = "black" {}
        _AtmosphereColor ("Atmosphere Color", Color) = (0.8, 0.85, 1, 1)
    }
    SubShader
    {
        Pass
        {
            Tags { "LightMode" = "ForwardBase" }

            CGPROGRAM

                #pragma vertex vert
                #pragma fragment frag
                #include "UnityCG.cginc"
                #include "CGUtils.cginc"

                // Declare used properties
                uniform sampler2D _AlbedoMap;
                uniform float _Ambient;
                uniform sampler2D _SpecularMap;
                uniform float _Shininess;
                uniform sampler2D _HeightMap;
                uniform float4 _HeightMap_TexelSize;
                uniform float _BumpScale;
                uniform sampler2D _CloudMap;
                uniform fixed4 _AtmosphereColor;

                struct appdata
                { 
                    float4 vertex : POSITION;
                };

                struct v2f
                {
                    float4 pos : SV_POSITION;
                    float4 pos_world : TEXCOORD0;
                    float4 vertex : TEXCOORD1;
                    float3 normal : NORMAL;
                    float3 tangent : TANGENT;
                };

                v2f vert (appdata input)
                {
                    v2f output;
                    output.pos = UnityObjectToClipPos(input.vertex);
                    output.pos_world = mul(unity_ObjectToWorld, input.vertex);
                    output.vertex = input.vertex;
                    output.normal = normalize(mul(unity_ObjectToWorld, input.vertex.xyz));
                    output.tangent = normalize(cross(output.normal, float3(0, 1, 0)));
                    return output;
                }

                fixed4 frag (v2f input) : SV_Target
                {
                    // Calculate bump map data
                    float3 normal = normalize(input.normal);
                    float3 tangent = normalize(input.tangent);
                    float2 uv = getSphericalUV(input.vertex.xyz);
                    float du = _HeightMap_TexelSize.x;
                    float dv = _HeightMap_TexelSize.y;
                    bumpMapData my_bumps = {normal, tangent, uv, _HeightMap, du, dv, _BumpScale / 10000};
                    float3 temp_n = normalize(getBumpMappedNormal(my_bumps));

                    // Blinn Phong
                    fixed4 Albedo = tex2D(_AlbedoMap, uv);
                    fixed4 Specular = tex2D(_SpecularMap, uv);
                    float3 n = normalize((1 - Specular) * temp_n + Specular * normal); // Flatten water
                    float3 l = normalize(_WorldSpaceLightPos0.xyz);
                    float3 v = normalize(_WorldSpaceCameraPos - input.pos_world);
                    fixed4 blinnPhongColor = fixed4(blinnPhong(n, v, l, _Shininess, Albedo, Specular, _Ambient) ,1.0);

                    // Atmosphere
                    float lambert = max(0, dot(normal, l));
                    fixed4 atmosphere = (1 - max(0, dot(normal, v))) * sqrt(lambert) * _AtmosphereColor;
                    fixed4 clouds = tex2D(_CloudMap, uv) * (sqrt(lambert) + _Ambient);

                    return blinnPhongColor + atmosphere + clouds;
                }

            ENDCG
        }
    }
}
