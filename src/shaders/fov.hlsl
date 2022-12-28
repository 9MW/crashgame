
#include "BasicStructures.fxh"
 
#define MAXI 0xFFFFFFFF
#ifndef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 64
#endif

#ifndef UPDATE_SPEED
#define UPDATE_SPEED 0
#endif
RWTexture2D<int4> lightMap;
Texture2D<float4> lightMapfow;
//Texture2D<float4> lightMaprt;
Texture2D<float4> Rendertg0;
SamplerState lightMapfow_sampler; // By convention, texture samplers must use the '_sampler' suffix
DfTex2D(fowresult);//render result layer
DfTex2D(lightMaprt);//light map layer
RWTexture2D<int4> clearlightMap;
globallycoherent RWTexture2D<int4>resistanceMapW;
Texture2D<int4> resistanceMapR;
RWTexture2D<uint4> GroupMap;
StructuredBuffer<objrep> curProp;
RWStructuredBuffer<AroundTg> targetProp;
cbuffer Constants
{
    GlobalConstants g_Constants;
};
/**
* Calculates the Field Of View for the provided map from the given x, y
* coordinates. Returns a lightmap for a result where the values represent a
* percentage of fully lit.
*
* A value equal to or below 0 means that cell is not in the
* field of view, whereas a value equal to or above 1 means that cell is
* in the field of view.
*
* @param resistanceMapR the grid of cells to calculate on where 0 is transparent and 1 is opaque
* @param startx the horizontal component of the starting location
* @param starty the vertical component of the starting location
* @param radius the maximum distance to draw the FOV
* @param radiusStrategy provides a means to calculate the radius as desired
* @return the computed light grid
*/
void tryAddtarget(const objrep cur, const objrep tg, int id)
{
	
    //if (targetProp[id].size < MaxTargetCap-1&&
    //    (cur.targetfilter & tg.groupids) > 0)
		

}

// Divide the 2D-Dispatch_Grid into tiles of dimension [N, DipatchGridDim.y]
// ��CTA�� (Cooperative Thread Array) == Thread Group in DirectX terminology
//uint2 ThreadGroupTilingX(
//	const uint2 dipatchGridDim, // Arguments of the Dispatch call (typically from a ConstantBuffer)
//	const uint2 ctaDim, // Already known in HLSL, eg:[numthreads(8, 8, 1)] -> uint2(8, 8)
//	const uint maxTileWidth, // User parameter (N). Recommended values: 8, 16 or 32.
//	const uint2 groupThreadID, // SV_GroupThreadID
//	const uint2 groupId // SV_GroupID
//)
//{
//	// A perfect tile is one with dimensions = [maxTileWidth, dipatchGridDim.y]
//    const uint Number_of_CTAs_in_a_perfect_tile = maxTileWidth * dipatchGridDim.y;

//	// Possible number of perfect tiles
//    const uint Number_of_perfect_tiles = dipatchGridDim.x / maxTileWidth;

//	// Total number of CTAs present in the perfect tiles
//    const uint Total_CTAs_in_all_perfect_tiles = Number_of_perfect_tiles * maxTileWidth * dipatchGridDim.y - 1;
//    const uint vThreadGroupIDFlattened = dipatchGridDim.x * groupId.y + groupId.x;

//	// Tile_ID_of_current_CTA : current CTA to TILE-ID mapping.
//    const uint Tile_ID_of_current_CTA = vThreadGroupIDFlattened / Number_of_CTAs_in_a_perfect_tile;
//    const uint Local_CTA_ID_within_current_tile = vThreadGroupIDFlattened % Number_of_CTAs_in_a_perfect_tile;
//    uint Local_CTA_ID_y_within_current_tile;
//    uint Local_CTA_ID_x_within_current_tile;

//    if (Total_CTAs_in_all_perfect_tiles < vThreadGroupIDFlattened)
//    {
//		// Path taken only if the last tile has imperfect dimensions and CTAs from the last tile are launched. 
//        uint X_dimension_of_last_tile = dipatchGridDim.x % maxTileWidth;
//#ifdef DXC_STATIC_DISPATCH_GRID_DIM
//		X_dimension_of_last_tile = max(1, X_dimension_of_last_tile);
//#endif
//        Local_CTA_ID_y_within_current_tile = Local_CTA_ID_within_current_tile / X_dimension_of_last_tile;
//        Local_CTA_ID_x_within_current_tile = Local_CTA_ID_within_current_tile % X_dimension_of_last_tile;
//    }
//    else
//    {
//        Local_CTA_ID_y_within_current_tile = Local_CTA_ID_within_current_tile / maxTileWidth;
//        Local_CTA_ID_x_within_current_tile = Local_CTA_ID_within_current_tile % maxTileWidth;
//    }

//    const uint Swizzled_vThreadGroupIDFlattened =
//		Tile_ID_of_current_CTA * maxTileWidth +
//		Local_CTA_ID_y_within_current_tile * dipatchGridDim.x +
//		Local_CTA_ID_x_within_current_tile;

//    uint2 SwizzledvThreadGroupID;
//    SwizzledvThreadGroupID.y = Swizzled_vThreadGroupIDFlattened / dipatchGridDim.x;
//    SwizzledvThreadGroupID.x = Swizzled_vThreadGroupIDFlattened % dipatchGridDim.x;

//    uint2 SwizzledvThreadID;
//    SwizzledvThreadID.x = ctaDim.x * SwizzledvThreadGroupID.x + groupThreadID.x;
//    SwizzledvThreadID.y = ctaDim.y * SwizzledvThreadGroupID.y + groupThreadID.y;

//    return SwizzledvThreadID.xy;
//}

cbuffer Cbuf
{
    CameraAttribs g_CameraAttribs;
};

struct ShadowMapVisPSOutput
{
    float4 Color : SV_TARGET;
};

struct v2f1
{
    float2 uv : TEX_COORD;
    float4 color : ATTRIB0;
    float4 wps : ATTRIB1;
    float4 vertex : SV_POSITION;
};
/*
   Hyllian's 5xBR v3.7c (rounded) Shader
   
   Copyright (C) 2011/2012 Hyllian/Jararaca - sergiogdb@gmail.com

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

const static float coef = 2.0;
const static float eq_threshold = 15.0;
//const static float3 dtt           = float3(65536,255,1);
//const static float y_weight        = 48.0;
//const static float u_weight        = 7.0;
//const static float v_weight        = 6.0;
//const static float3 yuv          = float3(0.299, 0.587, 0.114);
const static half3 yuv_weighted = half3(14.352, 28.176, 5.472);

float4 df(float4 A, float4 B)
{
    return abs(A - B);
}
/*
bool eq(float A, float B)
{
	return bool(abs(A-B) < eq_threshold);
}
*/
bool4 eq(float4 A, float4 B)
{
    return bool4(abs(A.x - B.x) < eq_threshold, abs(A.y - B.y) < eq_threshold, abs(A.z - B.z) < eq_threshold, abs(A.w - B.w) < eq_threshold);
}

float4 weighted_distance(float4 a, float4 b, float4 c, float4 d, float4 e, float4 f, float4 g, float4 h)
{
    return (df(a, b) + df(a, c) + df(d, e) + df(d, f) + 4.0 * df(g, h));
}

struct out_vertex {
    float4 vertex : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float4 t1 : TEXCOORD1;
    float4 t2 : TEXCOORD2;
    float4 t3 : TEXCOORD3;
    float4 t4 : TEXCOORD4;
    float4 t5 : TEXCOORD5;
    float4 t6 : TEXCOORD6;
#ifndef HLSL_4
    float4 Color : COLOR;
#endif
};
void fowv(in VertexInput1 v,
          out out_vertex PSOut)
{
    //uint2 pos = (uint2) round(v.vertex);
    //uint4 sd = g_ShadowMap[pos];
    //objrep InstanceID = curProp[v.instance];
    //float4 Pos[4];
    //Pos[0] = float4(-100, -100, 0.0, 1.0);
    //Pos[1] = float4(-100, +100, 0.0, 1.0);
    //Pos[2] = float4(+100, -100, 0.0, 1.0);
    //Pos[3] = float4(+100, +100, 0.0, 1.0);
    float2 ps = 1/4096*0.5;// g_CameraAttribs.f4ViewportSize.zw;
    float4 posWorld = (float4(v.vertex, 1));
    PSOut.vertex = mul(g_CameraAttribs.mViewProj, posWorld);
    //PSOut.wps = posWorld;
    PSOut.texCoord = v.UV0;
    //PSOut.color = float4(ps * float2(1, 0), ps * float2(0, 1));

    float2 dg1 =  ps;
    float x = dg1.x;
    float y = dg1.y;
    float2 dg2 = float2(-x, y);
    float2 sd1 = dg1 * 0.5;
    float2 sd2 = dg2 * 0.5;
    float2 ddx = float2(x, 0.0);
    float2 ddy = float2(0.0, y);

    PSOut.t1 = float4(PSOut.texCoord - sd1, PSOut.texCoord - ddy);
    PSOut.t2 = float4(PSOut.texCoord - sd2, PSOut.texCoord + ddx);
    PSOut.t3 = float4(PSOut.texCoord + sd1, PSOut.texCoord + ddy);
    PSOut.t4 = float4(PSOut.texCoord + sd2, PSOut.texCoord - ddx);
    PSOut.t5 = float4(PSOut.texCoord - dg1, PSOut.texCoord - dg2);
    PSOut.t6 = float4(PSOut.texCoord + dg1, PSOut.texCoord + dg2);
}

const static float3 dt = float3(1.0, 1.0, 1.0);
const float mx = 0;        // start smoothing wt.
const float k = 0;         // wt. decrease factor
const float max_w = 100;   // max filter weigth
const float min_w = 0.03;  // min filter weigth
const float lum_add = 100; // effects smoothing
#define COMPAT_SamplePoint(tex, uv) lightMapfow.Sample(lightMapfow_sampler, i.uv).x
//sample fog texture
float4 fowf(in out_vertex i) : SV_Target0
{
    float c = COMPAT_SamplePoint(decal, texCoord);
    float i1 = COMPAT_SamplePoint(decal, t1.xy);
    float i2 = COMPAT_SamplePoint(decal, t2.xy);
    float i3 = COMPAT_SamplePoint(decal, t3.xy);
    float i4 = COMPAT_SamplePoint(decal, t4.xy);
    float o1 = COMPAT_SamplePoint(decal, t5.xy);
    float o3 = COMPAT_SamplePoint(decal, t6.xy);
    float o2 = COMPAT_SamplePoint(decal, t5.zw);
    float o4 = COMPAT_SamplePoint(decal, t6.zw);
    float s1 = COMPAT_SamplePoint(decal, t1.zw);
    float s2 = COMPAT_SamplePoint(decal, t2.zw);
    float s3 = COMPAT_SamplePoint(decal, t3.zw);
    float s4 = COMPAT_SamplePoint(decal, t4.zw);

    float ko1 = dot(abs(o1 - c), dt);
    float ko2 = dot(abs(o2 - c), dt);
    float ko3 = dot(abs(o3 - c), dt);
    float ko4 = dot(abs(o4 - c), dt);

    float k1 = min(dot(abs(i1 - i3), dt), max(ko1, ko3));
    float k2 = min(dot(abs(i2 - i4), dt), max(ko2, ko4));

    float w1 = k2; if (ko3 < ko1) w1 *= ko3 / ko1;
    float w2 = k1; if (ko4 < ko2) w2 *= ko4 / ko2;
    float w3 = k2; if (ko1 < ko3) w3 *= ko1 / ko3;
    float w4 = k1; if (ko2 < ko4) w4 *= ko2 / ko4;

    c = (w1 * o1 + w2 * o2 + w3 * o3 + w4 * o4 + 0.001 * c) / (w1 + w2 + w3 + w4 + 0.001);
    w1 = k * dot(abs(i1 - c) + abs(i3 - c), dt) / (0.125 * dot(i1 + i3, dt) + lum_add);
    w2 = k * dot(abs(i2 - c) + abs(i4 - c), dt) / (0.125 * dot(i2 + i4, dt) + lum_add);
    w3 = k * dot(abs(s1 - c) + abs(s3 - c), dt) / (0.125 * dot(s1 + s3, dt) + lum_add);
    w4 = k * dot(abs(s2 - c) + abs(s4 - c), dt) / (0.125 * dot(s2 + s4, dt) + lum_add);

    w1 = clamp(w1 + mx, min_w, max_w);
    w2 = clamp(w2 + mx, min_w, max_w);
    w3 = clamp(w3 + mx, min_w, max_w);
    w4 = clamp(w4 + mx, min_w, max_w);

    //uint2 rdres = (uint2) ((round(i.wps / g_CameraAttribs.f4ExtraData[0]) + g_CameraAttribs.f4ExtraData[1]).xy);
    float lig = lightMapfow.Sample(lightMapfow_sampler, i.texCoord).x;
    float4 terr = float4(1, 1, 1, 1); //+clamp( resistanceMapR.Sample(lightMapfow_sampler, i.uv),0,0.001);
    //return (w1 * (i1 + i3) + w2 * (i2 + i4) + w3 * (s1 + s3) + w4 * (s2 + s4) + c) / (2.0 * (w1 + w2 + w3 + w4) + 1.0);

    terr =min(1 - lig,0.8);
    return terr;
    //(lightMapfow.Sample(fowresult_sampler, i.uv)) + 1;
   // Dfsap2D(fowresult, i.color.xy);
}


cbuffer cScreenRect {
	Selects para;
};
//render to screen
void fowv1(in uint VertexID: SV_VertexID,
           out v2f PSInput)
{
	const float4 mn = para.maxin;
    //uint2 pos = (uint2) round(v.vertex);
    //uint4 sd = g_ShadowMap[pos];
    //objrep InstanceID = curProp[v.instance];
    
    //float2 UV[4];
    //UV[0] = float2(+0.0, +0.0);
    //UV[1] = float2(+0.0, +1.0);
    //UV[2] = float2(+1.0, +0.0);
    //UV[3] = float2(+1.0, +1.0);
    float4 Pos[8];
    Pos[0] = float4(-1.0, -1.0, 0.0, 1.0);
    Pos[1] = float4(-1.0, +1.0, 0.0, 1.0);
    Pos[2] = float4(+1.0, -1.0, 0.0, 1.0);
    Pos[3] = float4(+1.0, +1.0, 0.0, 1.0);
    
    
	Pos[0+4] = float4(mn.zw, 0.0, 1.0);
	Pos[1+4] = float4(mn.zy, 0.0, 1.0);
	Pos[2+4] = float4(mn.xw, 0.0, 1.0);
	Pos[3+4] = float4(mn.xy, 0.0, 1.0);
	PSInput.color =VertexID>3? float4(para.org.xyz,0.3):float4(0,0,0,0);

    float2 UV[8];
    UV[0] = float2(+0.0, +1.0);
    UV[1] = float2(+0.0, +0.0);
    UV[2] = float2(+1.0, +1.0);
    UV[3] = float2(+1.0, +0.0);

	UV[0+4] = float2(((mn.zw*float2(1,-1)*0.5+0.5)));
	UV[1+4] = float2(((mn.zy*float2(1,-1)*0.5+0.5)));
	UV[2+4] = float2(((mn.xw*float2(1,-1)*0.5+0.5)));
	UV[3+4] = float2(((mn.xy*float2(1,-1)*0.5+0.5)));
    PSInput.vertex = Pos[VertexID];
    PSInput.uv = UV[VertexID];
    PSInput.color = 0;
    PSInput.wps = 0;
}

float4 fowf1(in v2f i) : SV_Target
{
    //float4 r=1;
    float4 d = (lightMaprt.Sample(fowresult_sampler, i.uv));
    float4 viewunfog = fowresult.Sample(fowresult_sampler, i.uv);

    viewunfog.xyz = lerp(viewunfog.xyz, float3(0, 0, 0), d[0]);
    //viewunfog.xyz = lerp(viewunfog.xyz, float3(1, 1, 1), d[0]>0?0.001:0);
    //uint2 rdres = (uint2) ((round(i.wps / g_CameraAttribs.f4ExtraData[0]) + g_CameraAttribs.f4ExtraData[1]).xy);
    return viewunfog;//(float4((fowresult.Sample(fowresult_sampler, i.uv).xyz-d.xyz*0.1)*(1-i.color[3])+(i.color[3])*i.color.xyz,1));
   // Dfsap2D(fowresult, i.color.xy);
}
void bv(in uint VertId : SV_VertexID,
          out PSInput PSIn)
{
    float4 Pos[4];
    Pos[0] = float4(-1.0, -1.0, 0.0, 1.0);
    Pos[1] = float4(-1.0, +1.0, 0.0, 1.0);
    Pos[2] = float4(+1.0, -1.0, 0.0, 1.0);
    Pos[3] = float4(+1.0, +1.0, 0.0, 1.0);

    float2 UV[4];
    UV[0] = float2(+0.0, +1.0);
    UV[1] = float2(+0.0, +0.0);
    UV[2] = float2(+1.0, +1.0);
    UV[3] = float2(+1.0, +0.0);

    PSIn.Pos = Pos[VertId];
    PSIn.UV = UV[VertId];
}

DfTex2D(g_Texture0);
DfTex2D(g_Texture1);
//struct ShadowMapVisPSOutput
//{
//    float4 Color : SV_TARGET;
//};

float4 bp(in PSInput PSIn) : SV_Target
{
    float4 c1 = DfSap2D(g_Texture0,PSIn.UV);
    float4 c2 = DfSap2D(g_Texture1,PSIn.UV);
    return c1 * c2;
    //g_ShadowMap.Sample(g_ShadowMap_sampler, PSIn.UV).r;
}

/*    VERTEX_SHADER  
out_vertex main_vertex(COMPAT_IN_VERTEX)
{
    out_vertex OUT;
#ifdef HLSL_4
    float4 position = VIN.position;
    float2 texCoord = VIN.texCoord;
#else
    OUT.Color = color;
#endif
    OUT.position = mul(modelViewProj, position);
    OUT.texCoord = texCoord;

    float x = 0.5 * (1.0 / COMPAT_texture_size.x);
    float y = 0.5 * (1.0 / COMPAT_texture_size.y);
    float2 dg1 = float2(x, y);
    float2 dg2 = float2(-x, y);
    float2 sd1 = dg1 * 0.5;
    float2 sd2 = dg2 * 0.5;
    float2 ddx = float2(x, 0.0);
    float2 ddy = float2(0.0, y);

    OUT.t1 = float4(texCoord - sd1, texCoord - ddy);
    OUT.t2 = float4(texCoord - sd2, texCoord + ddx);
    OUT.t3 = float4(texCoord + sd1, texCoord + ddy);
    OUT.t4 = float4(texCoord + sd2, texCoord - ddx);
    OUT.t5 = float4(texCoord - dg1, texCoord - dg2);
    OUT.t6 = float4(texCoord + dg1, texCoord + dg2);

    return OUT;
}

const static float3 dt = float3(1.0, 1.0, 1.0);
const float mx = 1.0;       // start smoothing wt.
const float k = -1.10;      // wt. decrease factor
const float max_w = 0.75;   // max filter weigth
const float min_w = 0.03;   // min filter weigth
const float lum_add = 0.33; // effects smoothing

float4 FourxScaleHQ(float4 t1, float4 t2, float4 t3, float4 t4, float4 t5, float4 t6, float2 texCoord, COMPAT_Texture2D(decal))
{

    float3 c = COMPAT_SamplePoint(decal, texCoord).xyz;
    float3 i1 = COMPAT_SamplePoint(decal, t1.xy).xyz;
    float3 i2 = COMPAT_SamplePoint(decal, t2.xy).xyz;
    float3 i3 = COMPAT_SamplePoint(decal, t3.xy).xyz;
    float3 i4 = COMPAT_SamplePoint(decal, t4.xy).xyz;
    float3 o1 = COMPAT_SamplePoint(decal, t5.xy).xyz;
    float3 o3 = COMPAT_SamplePoint(decal, t6.xy).xyz;
    float3 o2 = COMPAT_SamplePoint(decal, t5.zw).xyz;
    float3 o4 = COMPAT_SamplePoint(decal, t6.zw).xyz;
    float3 s1 = COMPAT_SamplePoint(decal, t1.zw).xyz;
    float3 s2 = COMPAT_SamplePoint(decal, t2.zw).xyz;
    float3 s3 = COMPAT_SamplePoint(decal, t3.zw).xyz;
    float3 s4 = COMPAT_SamplePoint(decal, t4.zw).xyz;

    float ko1 = dot(abs(o1 - c), dt);
    float ko2 = dot(abs(o2 - c), dt);
    float ko3 = dot(abs(o3 - c), dt);
    float ko4 = dot(abs(o4 - c), dt);

    float k1 = min(dot(abs(i1 - i3), dt), max(ko1, ko3));
    float k2 = min(dot(abs(i2 - i4), dt), max(ko2, ko4));

    float w1 = k2; if (ko3 < ko1) w1 *= ko3 / ko1;
    float w2 = k1; if (ko4 < ko2) w2 *= ko4 / ko2;
    float w3 = k2; if (ko1 < ko3) w3 *= ko1 / ko3;
    float w4 = k1; if (ko2 < ko4) w4 *= ko2 / ko4;

    c = (w1 * o1 + w2 * o2 + w3 * o3 + w4 * o4 + 0.001 * c) / (w1 + w2 + w3 + w4 + 0.001);
    w1 = k * dot(abs(i1 - c) + abs(i3 - c), dt) / (0.125 * dot(i1 + i3, dt) + lum_add);
    w2 = k * dot(abs(i2 - c) + abs(i4 - c), dt) / (0.125 * dot(i2 + i4, dt) + lum_add);
    w3 = k * dot(abs(s1 - c) + abs(s3 - c), dt) / (0.125 * dot(s1 + s3, dt) + lum_add);
    w4 = k * dot(abs(s2 - c) + abs(s4 - c), dt) / (0.125 * dot(s2 + s4, dt) + lum_add);

    w1 = clamp(w1 + mx, min_w, max_w);
    w2 = clamp(w2 + mx, min_w, max_w);
    w3 = clamp(w3 + mx, min_w, max_w);
    w4 = clamp(w4 + mx, min_w, max_w);

    return float4((w1 * (i1 + i3) + w2 * (i2 + i4) + w3 * (s1 + s3) + w4 * (s2 + s4) + c) / (2.0 * (w1 + w2 + w3 + w4) + 1.0), 1.0);
}
  */