
#include "BasicStructures.fxh"
#include "VStruct.hlsl"

#define DfTex(name) Texture2D name;SamplerState name##_sampler
#define DfTexs(name,num) Texture2D name[num];SamplerState name##_sampler
#define DfSap(name,uv) name.Sample(name##_sampler,uv)
#define DfSaps(name,textureId,uv) name[textureId].Sample(name##_sampler, uv)
#ifndef NUM_TEXTURES
#define NUM_TEXTURES 1
#endif
//#define BOSTRIDE 17
struct pixin
{
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
    int2 texid : TEXCOORD2;
    float4 vertex : SV_POSITION;
};
DfTex(_boneTexture);
Texture2D     g_Texture[NUM_TEXTURES];
cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};
#ifdef NUM_PRIM
cbuffer cbPrim
{
    skinedcbe cmeshs[NUM_PRIM];
};
#endif
#ifdef OL
cbuffer cbOl
{
    outlinecb col;
};
#endif
#ifdef PRETRANSFORM
cbuffer cbpretrs
{
   mtransfrom pretrs;
};
#endif
StructuredBuffer<mtransfrom> objectTransfromBuffer;
#ifdef SCMB
StructuredBuffer<inscmb> instDatas;
#else
StructuredBuffer<instData1> instDatas;
#endif
StructuredBuffer<skinedcbe> cmes;
StructuredBuffer<float4x4> animreco;
inline float4x4 loadAni(in uint base, in float boneid)
{
    //if (boneid > 18 || boneid < 0)
    //{
    //    return Identity;
    //}
    return animreco[base+ int(boneid)];
}
SamplerState _MainTex_sampler; // By convention, texture samplers must use the '_sampler' suffix
#ifndef Bar
#ifndef FIXMESH
void vert(in VertexInputSkinnin v, out pixin o)
#else
void vert(in VertexInput1 v, out pixin o)
#endif
{

    const uint InstanceID = (v.instance&0xffff);//Up to 100k
    const uint primiId=v.instance>>BOSTRIDE;
#ifdef SCMB
    const inscmb ic=instDatas[InstanceID];
    const mtransfrom transform=ic.transform;
    const instData1 ins=ic.insd;
#else
    const mtransfrom transform=objectTransfromBuffer[InstanceID];
const instData1 ins=instDatas[InstanceID];
#endif

    o.color = float4(0.2, 0.1, 0.2, 0.6);
    o.texid = -1;
    #ifdef VTXCOLOR
    o.color=ins.color;
    #endif
    //const inscmb ic=instDatas[InstanceID];
    //o.vertex = v.VertId / 3 + Pos[v.VertId % 3];
    o.uv = v.UV0;
    //Pos[v.VertId % 3];
    //v.mBoneWeights = float4(0.25, 0.25, 0.25, 0.25);
    float4 posWorld =float4(v.vertex, 1);
#ifndef FIXMESH
    uint bs =ins.frameIndex;
   const skinedcbe cmesh=cmeshs[primiId];
    o.texid.x=cmesh.texid;
    float4x4 ani =
      v.mBoneWeights.x*loadAni(bs,v.mBoneIndices.x)
     +v.mBoneWeights.y*loadAni(bs,v.mBoneIndices.y)
     +v.mBoneWeights.z*loadAni(bs,v.mBoneIndices.z)
     +v.mBoneWeights.w*loadAni(bs,v.mBoneIndices.w);
     o.texid.y=ani[0][0];
    posWorld =mul(mul(cmesh.NodeMatrix,ani),posWorld);
#else
#endif

    posWorld = posWorld / posWorld.w;
#ifdef OL
posWorld*=col.scf;
#endif
    //unused
    #ifdef FIXMESHd
    //posWorld.xyz *=(ins.bx/10000.0);
    #endif
#ifdef PRETRANSFORM
    posWorld.xyz *=pretrs.Scale;
#endif
    posWorld.xyz *=transform.Scale;
    posWorld = QuaternionMul(posWorld, transform.rotation);
    posWorld.xyz +=transform.pos;
    
#ifdef PRETRANSFORM
posWorld.xyz +=pretrs.pos;
#endif
    //posWorld.w = 1.0;
    //o.color.z = frac(dot(round(posWorld.xy), float2(12.9090, 78.233)));
    //o.color.z += asfloat((int) posWorld.z << 10u);
    //o.color.xy = round(posWorld.xy);
    //o.color.w = 1;
    //posWorld = (v.VertId % 100, v.VertId / 100, 0, 1);
    o.vertex = mul(g_CameraAttribs.mViewProj, posWorld);
    //o.uv = v.uv;
    //o.vertex = mul(posWorld, g_CameraAttribs.mViewProj);
	/* float3 normalSkinningRotated = mul(combinedMatrix, float4(v.normal.xyz, 0));
	float3 normalWorld = QuaternionMul(float4(normalSkinningRotated, 1), objectRotationsBuffer[InstanceID]); */

}
#endif
//#define Bar
#ifdef Bar

void vert(
   // in VertexInput1 v,
    in uint VertexID : SV_VertexID,
    in uint InstanceID : SV_INSTANCEID,
     out pixin o){
    //uint InstanceID=v.instance;
#ifdef SCMB
    const inscmb ic=instDatas[InstanceID];
    const mtransfrom transform=ic.transform;
    const instData1 ins=ic.insd;
#else
    const mtransfrom transform=objectTransfromBuffer[InstanceID];
const instData1 ins=instDatas[InstanceID];
#endif

    o.color = float4(0.2, 0.1, 0.2, 0.6);
    o.texid = -1;
    #ifdef VTXCOLOR
    o.color=ins.color;
    #endif
    //const inscmb ic=instDatas[InstanceID];
    //o.vertex = v.VertId / 3 + Pos[v.VertId % 3];
    o.uv = QuadStripUV[VertexID];
    //Pos[v.VertId % 3];
    //v.mBoneWeights = float4(0.25, 0.25, 0.25, 0.25);
    float4 posWorld =float4(transform.pos, 1);
    o.vertex = mul(g_CameraAttribs.mViewProj, posWorld);
    o.vertex /=o.vertex.w ;
    //o.vertex.z=0;
    o.vertex.xy+=QuadStrip[VertexID].xy*0.1*float2(0.2,0.02)+float2(0,0.05);
    o.uv.y=(float)((ins.gt01id>>24)/(float)0xff);
    o.vertex=o.uv.y==0?float4(1,1,1,0):o.vertex;
}
#endif

//g_NormalMap
DfTexs(g_ColorMap,NUM_TEXTURES);
DfTexs(g_NormalMap,NUM_TEXTURES);
float4 frag(in pixin i) : SV_Target
{
    
    #ifdef Bar
    i.color=float4(1,1,0.1,1);
    return  i.uv.x<i.uv.y? float4(1,0,0,1):i.color;
    #endif

    #ifdef VTXCOLOR
    return i.color;
    #else
    float4 dr= DfSaps(g_ColorMap,i.texid.x,i.uv);
    dr[3]=1;
     return dr;;
    #endif
}