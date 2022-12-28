
#include "ToolFuncs.fxh"
#include "VTYPES.fxh"

#define MAXI 0xFFFFFFFF
#define ZERO 0x00000000
#ifndef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 64
#endif
#ifndef RECTPERC
#define RECTPERC 100
#endif

#define RASELE 0xFFFFFF
#define RESELE RASELE + 1
#undef lightType
#define lightType float4
StructuredBuffer<bufferinfo> instable;

RWStructuredBuffer<inscmb> Visibleout;
StructuredBuffer<tinfo> _tinfo;

StructuredBuffer<mtransfrom> s_objtrans; //[NUM_BUf];
StructuredBuffer<instData1> s_objProps;  //[NUM_BUf];
//dynamic info for each render insatnce grout
StructuredBuffer<bufferinfo> so_bufferinfo;
//culled result offset for 0 for different  instance to render
//define max render instance per mesh
//StructuredBuffer<bufferinfo> co_bufferinfo;
//Buffer<uint> counts;
RWStructuredBuffer<mtransfrom> o_objtrans; //[NUM_BUf];
RWStructuredBuffer<instData1> o_objProps;  //[NUM_BUf];
RWStructuredBuffer<inscmb> o_VfOIns;
//#define queueFB.Load(para) queueFB.Load(para*4)
RWTexture2D<lightType> lightMap;
RWStructuredBuffer<AroundTg> Nebbuf;
Texture2D<lightType> lightMapSRV;
RWStructuredBuffer<VFoutArgs> VfArg;
RWStructuredBuffer<Audio0> audio_insout;
RWStructuredBuffer<instData> VfOIns;
RWStructuredBuffer<VfGloable> VfG;
RWStructuredBuffer<selectinfo> sele_out;
RWStructuredBuffer<CullResults> Vf_out;
StructuredBuffer<VfProp> Vf_Props;
RWStructuredBuffer<CastArg> VfIn;
cbuffer Constants
{
#ifndef WriteGMap
    GlobalConstants g_Constants;
#else
    GlobalConstants g_Constants;
#endif
};
cbuffer cbSelect
{
    Selects sele[2];
};
cbuffer cbCameraAttribs
{
    CameraAttribs g_CameraAttribs;
};
RWStructuredBuffer<seleout> in_sele_out;

//buffer used render selection UI
RWStructuredBuffer<seleFilter> sele_ov;
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void sCull(uint3 Gid: SV_GroupID,
           uint3 GTid: SV_GroupThreadID)
{
    uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE) + GTid.x;
    uint flatid;
    if (uiGlobalThreadIdx >= sele[0].inum)
        return;
    const uint InstanceID = uiGlobalThreadIdx;
    const mtransfrom transform = s_objtrans[InstanceID];
    const instData1 insd = s_objProps[InstanceID];
    const uint tid = insd.tid;
    //tid==-1 means inactive
    if (tid == -1)
        return;
    const uint blocksize = g_Constants.pedd;
    uint2 xel;
    const uint2 sz = g_Constants.mapsize * 0.5;

#ifdef FiltAudio
    if (insd.audiolength != 0) {
        Audio0 a0;
        uint i;
        InterlockedAdd(audio_insout[0].pd0, 1, i);
        float3 dv4 = g_CameraAttribs.f4Position - transform.pos;
        a0.dissq = max(dot(dv4.xy, dv4.xy), 1);
        a0.aus = insd.audiosrc;
        a0.aul = insd.audiolength;
        audio_insout[i] = a0;
    }
#endif
    const int2 lf = insd.bx.zw;
    const int2 tr = insd.bx.xy;
    uint insid;
    xel = (transform).pos.xy+ g_Constants.mapsize * 0.5;;
    int2 posi001 = (transform).pos.xy * RECTPERC;
#ifdef CHECKRECT
    //const instData=
    if (all(g_Constants.RECT.xy >= posi001) && all(g_Constants.RECT.xy <= posi001)) {
        InterlockedAdd(sele_out[0].x, 1, insid);
        sele_out[insid + 1] = int2(insd.tid, insd.insid);
    }
#endif
    // if (any(lightMapSRV[posi + sz]))
    // {
    // 	InterlockedAdd(VfArg[obj.tid].NumInstances, 1, insid);
    // 	//VfArg[insid + 10].p1 = obj.insd.tid;
    // 	//VfArg[insid + 10].p2 = obj.insd.id;
    // 	//VfArg[insid + 10].FirstInstanceLocation = uiGlobalThreadIdx;
    // 	//VfOIns[VfArg[obj.tid].FirstInstanceLocation + insid] = obj.insd;
    // }
    //return;
    const bufferinfo finf = instable[tid];
//selectable if visible
#ifdef TSELECT
    int si;
    seleout so;
    const Selects usele = sele[1];

    seleFilter sVbuff;
    if (usele.inum == RASELE) {

        if (raysele(transform, usele, _tinfo[tid])) {
            InterlockedAdd(in_sele_out[0].itid, 1, si);
            so.itid = insd.id;
            so.tid = insd.tid;
            in_sele_out[si + 1] = so;

            const float4 he = _tinfo[tid].halfext;
            sVbuff.transform = transform;
            sVbuff.halfextent = he;
            sele_ov[si] = sVbuff;
        }
    } else if (usele.inum == RESELE) {
        if (rectsele(transform, usele, _tinfo[tid], g_CameraAttribs)) {
            InterlockedAdd(in_sele_out[0].itid, 1, si);
            so.itid = insd.id;
            so.tid = insd.tid;
            in_sele_out[si + 1] = so;

            const float4 he = _tinfo[tid].halfext;
            sVbuff.transform = transform;
            sVbuff.halfextent = he;
            sele_ov[si] = sVbuff;
        }
    }
#endif
     float res = lightMapSRV[xel].x;
    //for (int i = lf[0]; i <= tr[0]; ++i)
    {
        //for (int p = lf[1]; p <= tr[1]; ++p)
        {
            VFoutArgs vd;
            vd.audio_ins = asuint((transform).pos[0]);
            vd.audio_insout = asuint((transform).pos[1]);
            vd.FirstInstanceLocation = asuint(res);
            vd.NumIndices = insd.id;
            vd.NumInstances = 0;
            vd.FirstIndexLocation = xel.x;
            vd.BaseVertex = xel.y;
            VfArg[InstanceID+6] = vd;
            //visable and need render
            if ((res > 0))
            {
                InterlockedAdd(VfArg[insd.tid].NumInstances, 1, insid);
                InterlockedMax(VfArg[0].p3, insd.id);
                VfArg[insid].p3 =  insd.id;
                inscmb insout;
                insout.insd = insd;
                insout.transform = transform;
                Visibleout[insid + finf.org] = insout;
                return;
            }
            //resistanceMapW[xel]   = obj.pos[2];
        }
    }

    //resistanceMapW[obj.pos.xy] = hightdiff;
}