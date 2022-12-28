#include "BasicStructures.fxh"
struct InstanceProPerty
{
    float4 color;
};

StructuredBuffer<instData1> instDatas;
//StructuredBuffer<instData> VfOIns;
StructuredBuffer<float4x4> animreco;
StructuredBuffer<float4> map;
StructuredBuffer<mtransfrom> objectTransfromBuffer;

cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};


static matrix Identity =
{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};
static float3 Quad[] =
{
    float3(-1, -1, 0),
float3(-1, +1, 0),
float3(+1, +1, 0),
float3(+1, -1, 0)
};
static float4 one = (1, 1, 1, 1);
inline float4x4 loadAni(in uint base, in float boneid)
{
    //if (boneid > 18 || boneid < 0)
    //{
    //    return Identity;
    //}
    return animreco[base + int(boneid)];
}
uint Hash(uint s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

DfTex2D(g_ColorMap);
float Random(uint seed)
{
    return float(Hash(seed)) / 4294967295.0; // 2^32-1
}
SamplerState _MainTex_sampler; // By convention, texture samplers must use the '_sampler' suffix
void vert(in VertexInput v, out v2f o)
{

    //o.vertex = v.VertId / 3 + Pos[v.VertId % 3];
   
    o.uv = v.UV0;
    //Pos[v.VertId % 3];
    //uint InstanceID = instDatas[v.instance].transformid;
    //v.mBoneWeights = float4(0.25, 0.25, 0.25, 0.25);
    float4 posWorld = (float4(v.vertex, 1));
    //o.color =ani * posWorld*bs;
				//skinnedVertex *= objectPositionsBuffer[InstanceID].w;
    //v.vertex.xy *= float2(2, 4);
    
#ifdef SCMB
    const inscmb ic=instDatas[InstanceID];
    const mtransfrom trs=ic.transform;
    const instData1 ins=ic.insd;
#else
    const mtransfrom trs = objectTransfromBuffer[v.instance];
    const instData1 ins=instDatas[v.instance];
#endif
    posWorld = QuaternionMul(posWorld,trs.rotation);
    posWorld.xyz += trs.pos.xyz;
    posWorld.xyz = posWorld.xyz * trs.Scale.xyz;
    o.wps = posWorld;
    o.color =ins.color;
    //o.color.z = frac(dot(round(objectTransfromBuffer[InstanceID].pos.xy), float2(12.9090, 78.233)));
    //o.color.z = asfloat((int) posWorld.z << 10u);
    //o.color.xy *= frac(dot(round(objectTransfromBuffer[InstanceID].pos.xy), float2(12.9090, 78.233)));;
    //o.color.a =0;
    //posWorld = (v.VertId % 100, v.VertId / 100, 0, 1);
    o.vertex = mul(g_CameraAttribs.mViewProj, posWorld);
    //o.uv = v.uv;
    //o.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
	/* float3 normalSkinningRotated = mul(combinedMatrix, float4(v.normal.xyz, 0));
	float3 normalWorld = QuaternionMul(float4(normalSkinningRotated, 1), objectRotationsBuffer[InstanceID]); */
   
}

struct VSInputl
{
float4 Position;
float4 color;
};
StructuredBuffer<VSInputl> LineVertex;
void vert1(in uint gl_VertexID : SV_VertexID, out v2f o)
{


    int line_i = gl_VertexID / 6;
    int tri_i  = gl_VertexID % 6;
float u_thickness= 0.01;
const float2 u_resolution=g_CameraAttribs.f4ViewportSize.xy;
    float4 va[4];
    o.color=LineVertex[line_i].color;
    for (int i=0; i<4; ++i)
    {
        va[i] =  mul(g_CameraAttribs.mViewProj,float4(LineVertex[line_i+i].Position.xyz,1));
        va[i].xyz /= va[i].w;
        va[i].xy = (va[i].xy + 1.0) * 0.5 *  u_resolution;
    }
    //break line
    bool vl=(LineVertex[line_i+1].Position.w==0xa0|LineVertex[line_i+2].Position.w==0xa0);
    float2 v_line  = normalize(va[2].xy - va[1].xy);
    float2 nv_line = float2(-v_line.y, v_line.x);

    float4 pos;
    if (tri_i <=1 || tri_i == 3)
    {
        float2 v_pred  = va[1].xy == va[0].xy?0:normalize(va[1].xy - va[0].xy);
        float2 v_miter =normalize(nv_line + float2(-v_pred.y, v_pred.x));

        pos = va[1];
        pos.xy += v_miter * u_thickness * (tri_i == 1 ? -0.5 : 0.5) / dot(v_miter, nv_line);
    }
    else
    {
        float2 v_succ  = va[3].xy == va[2].xy?0:normalize(va[3].xy - va[2].xy);
        float2 v_miter = normalize(nv_line + float2(-v_succ.y, v_succ.x));

        pos = va[2];
        pos.xy +=v_miter * u_thickness * (tri_i == 5 ? 0.5 : -0.5) / dot(v_miter, nv_line);
    }

    pos.xy = pos.xy / u_resolution * 2.0 - 1.0;
    pos.xyz *= pos.w;
    o.vertex =vl?-1:pos;//clip out from NDA space
}
//line render
void vertline(in uint gl_VertexID : SV_VertexID, out v2f o)
{


    int line_i = gl_VertexID / 6;
    int tri_i  = gl_VertexID % 6;
float u_thickness= 0.01;
const float2 u_resolution=g_CameraAttribs.f4ViewportSize.xy;
    float4 va[4];
  o.color =LineVertex[line_i].color;
    for (int i=0; i<4; ++i)
    {
        va[i] =  mul(g_CameraAttribs.mViewProj,float4(LineVertex[line_i+i].Position.xyz,1));
        va[i].xyz /= va[i].w;
        va[i].xy = (va[i].xy + 1.0) * 0.5 *  u_resolution;
    }

    float2 v_line  = normalize(va[2].xy - va[1].xy);
    float2 nv_line = float2(-v_line.y, v_line.x);

    float4 pos;
    if (tri_i == 0 || tri_i == 1 || tri_i == 3)
    {
        float2 v_pred  = va[1].xy == va[0].xy?0:normalize(va[1].xy - va[0].xy);
        float2 v_miter = normalize(nv_line + float2(-v_pred.y, v_pred.x));

        pos = va[1];
        pos.xy += v_miter * u_thickness * (tri_i == 1 ? -0.5 : 0.5) / dot(v_miter, nv_line);
    }
    else
    {
        float2 v_succ  = va[3].xy == va[2].xy?0:normalize(va[3].xy - va[2].xy);
        float2 v_miter = normalize(nv_line + float2(-v_succ.y, v_succ.x));

        pos = va[2];
        pos.xy += v_miter * u_thickness * (tri_i == 5 ? 0.5 : -0.5) / dot(v_miter, nv_line);
    }

    pos.xy = pos.xy / u_resolution * 2.0 - 1.0;
    pos.xyz *= pos.w;
    o.vertex = pos;
}
float4 frag(in v2f i) : SV_Target
{
    return i.color;
    //DfSap2D(g_ColorMap, i.uv);
    //return float4(1,0,0.5,1);
}

DfTex2D(inTexg_ColorMap);
DfTex2D(fowTexg_ColorMap);
float4 FOWfrag(in v2f i) : SV_Target
{
    
    return i.color;
    //DfSap2D(g_ColorMap, i.uv);
    //return float4(0,0,0,0);
}
