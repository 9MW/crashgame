//#ifndef _BASIC_STRUCTURES_FXH_
//#define _BASIC_STRUCTURES_FXH_


#ifdef __cplusplus
#pragma once

#   ifndef BOOL
#      define BOOL int32_t // Do not use bool, because sizeof(bool)==1 !
#   endif

#   ifndef CHECK_STRUCT_ALIGNMENT
        // Note that defining empty macros causes GL shader compilation error on Mac, because
        // it does not allow standalone semicolons outside of main.
        // On the other hand, adding semicolon at the end of the macro definition causes gcc error.
#       define CHECK_STRUCT_ALIGNMENT(s) static_assert( sizeof(s) % 16 == 0, "sizeof(" #s ") is not multiple of 16" )
#   endif

#   ifndef DEFAULT_VALUE
#       define DEFAULT_VALUE(x) =x
#   endif

//#ifndef NAMES
//#define NAMES pj2
//#   endif
//#ifndef NAMEEND
//#define NAMEEND }
//#   endif
//namespace NAMES{

#else
#include"math.hlsl"
#   ifndef BOOL
#       define BOOL bool
#   endif

#   ifndef DEFAULT_VALUE
#       define DEFAULT_VALUE(x)
#   endif
#ifndef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 64
#endif
#endif


//struct CascadeAttribs
//{
//	float4 f4LightSpaceScale;
//	float4 f4LightSpaceScaledBias;
//    float4 f4StartEndZ;

//    // Cascade margin in light projection space ([-1, +1] x [-1, +1] x [-1(GL) or 0, +1])
//    float4 f4MarginProjSpace;
//};
//#ifdef CHECK_STRUCT_ALIGNMENT
//    CHECK_STRUCT_ALIGNMENT(CascadeAttribs);
//#endif

//#define SHADOW_MODE_PCF 1
//#define SHADOW_MODE_VSM 2
//#define SHADOW_MODE_EVSM2 3
//#define SHADOW_MODE_EVSM4 4
//#ifndef SHADOW_MODE
//#   define SHADOW_MODE SHADOW_MODE_PCF
//#endif

//#define MAX_CASCADES 8
//struct ShadowMapAttribs
//{
//    // 0
//#ifdef __cplusplus
//    float4x4 mWorldToLightViewT; // Matrices in HLSL are COLUMN-major while float4x4 is ROW major
//#else
//    matrix mWorldToLightView;  // Transform from view space to light projection space
//#endif
//    // 16
//    CascadeAttribs Cascades[MAX_CASCADES];

//#ifdef __cplusplus
//    float4x4 mWorldToShadowMapUVDepthT[MAX_CASCADES];
//    float fCascadeCamSpaceZEnd[MAX_CASCADES];
//#else
//    matrix mWorldToShadowMapUVDepth[MAX_CASCADES];
//    float4 f4CascadeCamSpaceZEnd[MAX_CASCADES/4];
//#endif

//    float4 f4ShadowMapDim;    // Width, Height, 1/Width, 1/Height

//    // Number of shadow cascades
//    int   iNumCascades                  DEFAULT_VALUE(0);
//    float fNumCascades                  DEFAULT_VALUE(0);
//    // Do not use bool, because sizeof(bool)==1 !
//	BOOL  bVisualizeCascades            DEFAULT_VALUE(0);
//    BOOL  bVisualizeShadowing           DEFAULT_VALUE(0);

//    float fReceiverPlaneDepthBiasClamp  DEFAULT_VALUE(10);
//    float fFixedDepthBias               DEFAULT_VALUE(1e-5f);
//    float fCascadeTransitionRegion      DEFAULT_VALUE(0.1f);
//    int   iMaxAnisotropy                DEFAULT_VALUE(4);

//    float fVSMBias                      DEFAULT_VALUE(1e-4f);
//    float fVSMLightBleedingReduction    DEFAULT_VALUE(0);
//    float fEVSMPositiveExponent         DEFAULT_VALUE(40);
//    float fEVSMNegativeExponent         DEFAULT_VALUE(5);

//    BOOL  bIs32BitEVSM                  DEFAULT_VALUE(1);
//    int   iFixedFilterSize              DEFAULT_VALUE(3); // 3x3 filter
//    float fFilterWorldSize              DEFAULT_VALUE(0);
//    bool  fDummy;
//};
//#ifdef CHECK_STRUCT_ALIGNMENT
//    CHECK_STRUCT_ALIGNMENT(ShadowMapAttribs);
//#endif
//struct LightAttribs
//{
//    float4 f4Direction      DEFAULT_VALUE(float4(0, 0,-1, 0));
//    float4 f4AmbientLight   DEFAULT_VALUE(float4(0, 0, 0, 0));
//    float4 f4Intensity      DEFAULT_VALUE(float4(1, 1, 1, 1));

//    ShadowMapAttribs ShadowAttribs;
//};
//#ifdef CHECK_STRUCT_ALIGNMENT
//    CHECK_STRUCT_ALIGNMENT(LightAttribs);
//#endif
struct Audio0{
    float dissq;
    uint pd0,aus,aul;
};
struct Selects{
float4 maxin;
float4 org;//reprent color when render selection rect
float4 dir;
uint inum,tmax,p2,p3;
//float4 ped;
};

struct CameraAttribs
{
    float4 f4Position;     // Camera world position
    float4 f4ViewportSize; // (width, height, 1/width, 1/height)

    float2 f2ViewportOrigin; // (min x, min y)
    float fNearPlaneZ; 
    float fFarPlaneZ; // fNearPlaneZ < fFarPlaneZ

#ifdef __cplusplus
    float4x4 mViewT;
    float4x4 mProjT;
    float4x4 mViewProjT;
    float4x4 mViewInvT;
    float4x4 mProjInvT;
    float4x4 mViewProjInvT;
#else
    matrix mView;
    matrix mProj;
    matrix mViewProj;
    matrix mViewInv;
    matrix mProjInv;
    matrix mViewProjInv;
#endif

    float4 f4ExtraData[5]; // Any appliation-specific data
    // Sizeof(CameraAttribs) == 256*2
};
#ifdef CHECK_STRUCT_ALIGNMENT
    CHECK_STRUCT_ALIGNMENT(CameraAttribs);
#endif

//#endif //_BASIC_STRUCTURES_FXH_

#ifndef __cplusplus
#define DfTex2D(name) Texture2D name;SamplerState name##_sampler
#define DfSap2D(name,uv) name.Sample(name##_sampler,uv)
struct mtransfrom
{
    float3 pos;
    float pd;
    float3 Scale;
    float od;
    float4 rotation;
};
#endif
#ifdef __cplusplus
using mtransfrom=math::mtransform;
//using int2=math::int2;
#endif
struct instData
{
    uint bonelength;
    uint frameIndex;
    //uint transformid;
    uint textureId;
    uint base;
    //set tid to -1 mean not render
    uint tid, id,audiosrc,audiolength;
   // float transitionProgress;
    float4 color;
    mtransfrom transform;
};
struct visualiresult{
float4 disvec;
};
struct bufferinfo{
    int org, count;
};
struct outlinecb{
float scf,p1,p2,p3;
};
struct instData1
{
//gt01id>>(16)&0xff result vision radius up to 256
    uint gt01id;
    uint frameIndex;
    uint textureId;
    uint base;
    //set tid to -1 sign to stop render
    uint tid, id,audiosrc,audiolength;
    int4 bx;
   // float transitionProgress;
    float4 color;
};

struct skinedcbe{
int texid,normaltexid,bonenumb,p0;
float4x4 NodeMatrix;
};
struct inscmb
{
    instData1 insd;
    mtransfrom transform;
};
struct seleout{
int itid,tid;
};
struct VisionProp
{
#ifdef __cplusplus
using uint4=math::uint4;
#endif
	uint4 bp;
};
struct objrep
{
	uint tid, id;
    //xyz and vision range
	int4 pos;
    uint4 targetfilter;
    //int4 bx;
};
struct VfGloable
{
    uint Ablock ,Args,selectitem,ped;
    //uint insnumber[1024*4];
};
struct VfProp
{
    instData insd;
    int4 bx;
    //uint tid, id;
};
struct seleFilter{
    mtransfrom transform;
    float4 halfextent;
};
struct tinfo{
float4 halfext;
float4 offset;
};
struct GlobalConstants
{
    uint uiNum,pedd;//pedd: Bone number
    uint2 mapsize;
    
    float radius;
    float fpd;
    float2 f2Scale;
    uint2 litmask;//if true populate to light source buffer
    int2 i2ParticleGridSize;
    int4 lightcolor;
    //accuracy at 0.01
    int4 RECT;
    uint4 targetfilter;
    float4 camerapos;
};
struct selectinfo{
    int2 id;
};
#define hightdiff 3
#define MaxTargetCap 4
#define PerGidObj 8
struct AroundTg
{
    int2 pos[8];
};
struct SPHKV{
int num;uint mindis; int2 keypos;int2 itid[10];// tid,insid;
};
struct mapele
{
    int4 pos;
    uint4 gid;
};
struct CastArg
{
    int id;
    int distance;
    int2 step0, step1;
    float2 se; //start end slope
    int4 pos;
//#ifndef __cplusplus
//    RWByteAddressBuffer queueFB;
//    #else
//    int2 p_buf;
//#endif
    
    //AroundTg at;
};
struct CullResults
{
    int4 id;
};
struct VFoutArgs
{
    uint NumIndices,
			NumInstances,
			FirstIndexLocation,
			BaseVertex,
			FirstInstanceLocation, audio_ins, audio_insout, p3;
    //uint3 peed;
};
//struct ScreenRect{
//    float4 maxmin;
//	float4 color;
//};
#ifndef __cplusplus
struct insData
{
    float4x3 trans;
};

struct v2f
{
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
    float4 wps : TEXCOORD2;
    float4 vertex : SV_POSITION;
};

struct VertexInputSkinnin
{
    
    float3 vertex : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV0 : ATTRIB2;
    float2 UV1 : ATTRIB3;
    float4 mBoneIndices : ATTRIB4;
    float4 mBoneWeights : ATTRIB5;
    uint instance : SV_INSTANCEID;
    uint VertId : SV_VertexID;
};

struct VertexInput1
{
    
    float3 vertex : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV0    : ATTRIB2;
    float2 UV1 : ATTRIB3;
    uint instance : SV_INSTANCEID;
    uint VertId : SV_VertexID;
};
struct VertexInput
{
    
    float3 vertex : ATTRIB0;
    float3 Normal : ATTRIB1;
    float2 UV0 : ATTRIB2;
    uint instance : SV_INSTANCEID;
    uint VertId : SV_VertexID;
};
struct VSInput
{
    uint VertexID : SV_VertexID;
};

struct PSInput
{
    float4 Pos : SV_POSITION;
    float2 UV : TEX_COORD;
};
struct PSOut0
{
    float4 o1 : SV_Target0;
    uint2 o2 : SV_Target1;
};
#define lightType uint4
#endif

//#ifdef __cplusplus
//NAMEEND
//#endif