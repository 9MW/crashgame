#define THREAD_GROUP_SIZE 256
#include "BasicStructures.fxh"
#ifndef AFM
#define AFM float4
#endif
#define ATELE Gid[1]
#define ALH 2048
cbuffer Constants { GlobalConstants g_Constants; };
cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};

StructuredBuffer<VFoutArgs> VfArg;
StructuredBuffer<AFM> audio_bufin;
// StructuredBuffer<AFM> audio_bufin;
RWStructuredBuffer<AFM> audio_bufout;
StructuredBuffer<Audio0> audio_inst;
groupshared AFM accum[THREAD_GROUP_SIZE];
//#define groupthreads 32 * 1024 / (THREAD_GROUP_SIZE * 16)
// static const int elebk = 32 * 1024 / (THREAD_GROUP_SIZE * 16);
// cbuffer cbCS : register(b0) {
//   uint4 g_param; // g_param.x is the actual elements contained in Input
//                  // g_param.y is the x dimension of the Dispatch call
// };

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void mixer0(uint3 Gid
                                             : SV_GroupID, uint3 DTid
                                             : SV_DispatchThreadID, uint GI
                                             : SV_GroupIndex) {


  const Audio0 ast0 = audio_inst[0];
  if (Gid.x < ALH) {
    // Audio0 obj = audio_inst[Gid[0]];
    uint j = ceil(ast0.pd0 / (float)THREAD_GROUP_SIZE); // b=Gid.x*elebk;
    accum[GI] = 0;
    if (j > 0) {
      for (int i = 0; i < j; i++) {
        const uint si = THREAD_GROUP_SIZE * i + GI;
        const Audio0 Audio = audio_inst[si];
        const bool nz = (Audio.aus + Gid.x) < Audio.aul;
        accum[GI] +=
            (si < ast0.pd0)&nz ? audio_bufin[audio_inst[si].aus + Gid.x]: 0;
      }
    }
  } else
    return;
  // Parallel reduction algorithm follows

  GroupMemoryBarrierWithGroupSync();
  if (GI < 128)
    accum[GI] += accum[128 + GI];
  GroupMemoryBarrierWithGroupSync();
  if (GI < 64)
    accum[GI] += accum[64 + GI];

  GroupMemoryBarrierWithGroupSync();
  if (GI < 32)
    accum[GI] += accum[32 + GI];

  GroupMemoryBarrierWithGroupSync();
  if (GI < 16)
    accum[GI] += accum[16 + GI];

  GroupMemoryBarrierWithGroupSync();
  if (GI < 8)
    accum[GI] += accum[8 + GI];

  GroupMemoryBarrierWithGroupSync();
  if (GI < 4)
    accum[GI] += accum[4 + GI];

  GroupMemoryBarrierWithGroupSync();
  if (GI < 2)
    accum[GI] += accum[2 + GI];

  GroupMemoryBarrierWithGroupSync();
  if (GI < 1){
    accum[0] += accum[1];
    audio_bufout[Gid.x] = accum[0];
    }
}

StructuredBuffer<VfProp> Vf_Props;

RWStructuredBuffer<Audio0> audio_inst_out;
RWStructuredBuffer<VFoutArgs> RWVfArg;
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void Vfilter(uint3 Gid : SV_GroupID,
    uint3 GTid : SV_GroupThreadID) {
    uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE)+GTid.x;
    /*if (uiGlobalThreadIdx >= sele.inum)
        return;*/
    const VfProp obj = Vf_Props[uiGlobalThreadIdx];
    if (obj.insd.tid == -1) {
        //audio_inst_out[i].aul = 0;
        return;
    }

    if (obj.insd.audiolength != 0) {
        Audio0 a0;
        uint i;
        InterlockedAdd(RWVfArg[0].audio_ins, 1, i);
        float3 dv4 = g_CameraAttribs.f4Position.xyz - obj.insd.transform.pos;
        a0.dissq = max(dot(dv4.xy, dv4.xy), 1);
        a0.aus = obj.insd.audiosrc;
        a0.aul = obj.insd.audiolength;
        //if (a0.aul == obj.insd.audiolength) {
        //
        //}
        audio_inst_out[i] = a0;
    }
}
// [numthreads(groupthreads, 1, 1)] void CSMain(uint3 Gid
//                                               : SV_GroupID, uint3 DTid
//                                               : SV_DispatchThreadID, uint GI
//                                               : SV_GroupIndex) {
//   const VFoutArgs ast = VfArg[0];
//   if (Gid.x < ast.audio_ins)
//     accum[GI] = Input[DTid.x];
//   else
//     accum[GI] = 0;
//   Audio0 obj = audio_inst[Gid[0]];
//   for (int i = asl / 2; i > 0; i /= 2) {
//     val += g.shfl_down(val, i);
//   }
//   // Parallel reduction algorithm follows

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 128)
//     accum[GI] += accum[64 + GI];
//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 64)
//     accum[GI] += accum[64 + GI];

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 32)
//     accum[GI] += accum[32 + GI];

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 16)
//     accum[GI] += accum[16 + GI];

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 8)
//     accum[GI] += accum[8 + GI];

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 4)
//     accum[GI] += accum[4 + GI];

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 2)
//     accum[GI] += accum[2 + GI];

//   GroupMemoryBarrierWithGroupSync();
//   if (GI < 1)
//     accum[GI] += accum[1 + GI];

//   if (GI == 0) {
//     Result[Gid.x] = accum[0];
//   }
// }