Shader "CG/BlinnPhong"
{
    Properties
    {
        _DiffuseColor ("Diffuse Color", Color) = (0.14, 0.43, 0.84, 1)
        _SpecularColor ("Specular Color", Color) = (0.7, 0.7, 0.7, 1)
        _AmbientColor ("Ambient Color", Color) = (0.05, 0.13, 0.25, 1)
        _Shininess ("Shininess", Range(0.1, 50)) = 10
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

                // From UnityCG
                uniform fixed4 _LightColor0; 

                // Declare used properties
                uniform fixed4 _DiffuseColor;
                uniform fixed4 _SpecularColor;
                uniform fixed4 _AmbientColor;
                uniform float _Shininess;

                struct appdata
                { 
                    float4 vertex : POSITION;
                    float3 normal : NORMAL;
                };

                struct v2f
                {
                    float4 pos : SV_POSITION;
                    float3 normal: NORMAKL;
                };


                v2f vert (appdata input)
                {
                    v2f output;
                    output.pos = UnityObjectToClipPos(input.vertex);
                    output.normal = input.normal;
                    return output;
                }


                fixed4 frag (v2f input) : SV_Target
                {
                    float3 vector_l = normalize(_WorldSpaceLightPos0.xyz);
                    float3 vector_v = normalize(_WorldSpaceCameraPos);
                    float3 vector_h = normalize((vector_l + vector_v) / 2);
                    float3 vector_n = normalize(mul(unity_ObjectToWorld, input.normal));
                    
                    float3 color_d = max(dot(vector_l, vector_n), 0) * _DiffuseColor * _LightColor0;
                    float3 color_s = pow(max(dot(vector_n, vector_h), 0), _Shininess) * _SpecularColor * _LightColor0;
                    float3 color_a = _AmbientColor * _LightColor0;
                    fixed4 blinnPhongColor = fixed4(color_d + color_s + color_a, 1.0);
                    
                    return blinnPhongColor;
                }

            ENDCG
        }
    }
}
