// Unity built-in shader source. Copyright (c) 2016 Unity Technologies. MIT license (see license.txt)

// Simplified Bumped Specular shader. Differences from regular Bumped Specular one:
// - no Main Color nor Specular Color
// - specular lighting directions are approximated per vertex
// - writes zero to alpha channel
// - Normalmap uses Tiling/Offset of the Base texture
// - no Deferred Lighting support
// - no Lightmap support
// - fully supports only 1 directional light. Other lights can affect it, but it will be per-vertex/SH.

Shader "AnimationInstancing/Bumped Specular_instancing" {
Properties {
	_Shininess ("Shininess", Range (0.03, 1)) = 0.078125
	_MainTex ("Base (RGB) Gloss (A)", 2D) = "white" {}
	[NoScaleOffset] _BumpMap ("Normalmap", 2D) = "bump" {}
}
SubShader { 
	Tags { "RenderType"="Opaque" }
	LOD 250
	
CGPROGRAM

#include "AnimationInstancingBase.cginc"
#pragma vertex vert 
#pragma multi_compile_instancing
//DECLARE_VERTEX_SKINNING

#pragma surface surf MobileBlinnPhong exclude_path:prepass nolightmap noforwardadd halfasview interpolateview addshadow

inline float4 LightingMobileBlinnPhong (SurfaceOutput s, float3 lightDir, float3 halfDir, float atten)
{
	float diff = max (0, dot (s.Normal, lightDir));
	float nh = max (0, dot (s.Normal, halfDir));
	float spec = pow (nh, s.Specular*128) * s.Gloss;
	
	float4 c;
	c.rgb = (s.Albedo * _LightColor0.rgb * diff + _LightColor0.rgb * spec) * atten;
	UNITY_OPAQUE_ALPHA(c.a);
	return c;
}

sampler2D _MainTex;
sampler2D _BumpMap;
half _Shininess;

struct Input {
	float2 uv_MainTex;
};



void surf (Input IN, inout SurfaceOutput o) {
	float4 tex = tex2D(_MainTex, IN.uv_MainTex);
	o.Albedo = tex.rgb;
	o.Gloss = tex.a;
	o.Alpha = tex.a;
	o.Specular = _Shininess;
	o.Normal = UnpackNormal (tex2D(_BumpMap, IN.uv_MainTex));
}
ENDCG
}
//FallBack "Mobile/VertexLit"

}
