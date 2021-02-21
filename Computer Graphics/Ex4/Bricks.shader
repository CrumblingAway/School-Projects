Shader "CG/Bricks"
{
    Properties
    {
        [NoScaleOffset] _AlbedoMap ("Albedo Map", 2D) = "defaulttexture" {}
        _Ambient ("Ambient", Range(0, 1)) = 0.15
        [NoScaleOffset] _SpecularMap ("Specular Map", 2D) = "defaulttexture" {}
        _Shininess ("Shininess", Range(0.1, 100)) = 50
        [NoScaleOffset] _HeightMap ("Height Map", 2D) = "defaulttexture" {}
        _BumpScale ("Bump Scale", Range(-100, 100)) = 40
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

                struct appdata
                { 
                    float4 vertex   : POSITION;
                    float3 normal   : NORMAL;
                    float4 tangent  : TANGENT;
                    float2 uv       : TEXCOORD0;
                };

                struct v2f
                {
                    float4 pos : SV_POSITION;
                    float4 pos_world : TEXCOORD0;
                    float3 normal   : NORMAL;
                    float4 tangent  : TANGENT;
                    float2 uv : TEXCOORD1;
                };

                v2f vert (appdata input)
                {
                    v2f output;
                    output.pos = UnityObjectToClipPos(input.vertex);
                    output.pos_world = mul(unity_ObjectToWorld, input.vertex); 
                    output.normal = mul(unity_ObjectToWorld,input.normal);
                    output.tangent = mul(unity_ObjectToWorld,input.tangent);
                    output.uv = input.uv;
                    return output;
                }

                fixed4 frag (v2f input) : SV_Target
                {
                    // Calculate normal for bump map
                    float3 normal = normalize(input.normal);      
                    float3 tangent = normalize(input.tangent.xyz);     
                    float2 uv = input.uv; 
                    float du = _HeightMap_TexelSize.x;
                    float dv = _HeightMap_TexelSize.y;
                    bumpMapData my_bumps = {normal, tangent, uv, _HeightMap, du, dv, _BumpScale / 10000};
                    float3 n = normalize(getBumpMappedNormal(my_bumps));

                    // Blinn Phong
                    fixed4 Albedo = tex2D(_AlbedoMap, uv);
                    fixed4 Specular = tex2D(_SpecularMap, uv);
                    float3 l = normalize(_WorldSpaceLightPos0.xyz);
                    float3 v = normalize(_WorldSpaceCameraPos - input.pos_world.xyz);
                    fixed4 color = fixed4(blinnPhong(n, v, l, _Shininess, Albedo, Specular, _Ambient), 1.0);

                    return color;
                }

            ENDCG
        }
    }
}
