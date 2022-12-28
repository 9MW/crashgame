#include "BasicStructures.fxh"
#include "VTYPES.fxh"

#define MAXI 0xFFFFFFFF
#define ZERO 0x00000000
#ifndef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 64
#endif
#define RW_SIZE 32
#ifndef QUEUE_SIZE
#define QUEUE_SIZE 1024 * 128
#endif
#ifndef QUEUE_BASE
#define QUEUE_BASE 1024 * QUEUE_SIZE
#endif
#ifndef I32
#define I32 16
#endif
#ifndef N2
#define N2 2
#endif
// #ifndef NUM_BUf
// #define NUM_BUf 1
// StructuredBuffer<mtransfrom> objtrans[NUM_BUf];
// StructuredBuffer<objrep>objProps[NUM_BUf];
// Buffer<uint> counts;
// #endif
RWStructuredBuffer<objrep> objProp;
#define QueNum(id) RWSign.Load(1024 * 128 / 32 + id)
#define QueTop RWSign[1];
#define QueBot RWSign[2];
////#ifndef QueAdd(pos,val,res)
//#define QueAdd(id,val,res)  InterlockedAdd(RWSign[1024*128 /32+id], val, res)
//#define QueAdd1(id,val)  InterlockedAdd(RWSign[1024*128 /32+id], val)
////#define QueAdd(id,val,res)  InterlockedAdd(_queC[1024*128 /32+id], val, res)
//#define QueAdd1(id,val)  InterlockedAdd(_queC, val)
//#define QueAdd(id,val,res)  InterlockedAdd(RWSign[1024*128 /32], val, res)
//#endif
//#define queueFBl.Load(para) queueFBl.Load(para*4)
RWTexture2D<TLIT> lightMap;
Texture2D<uint> resistanceMap;
Texture2D<int4> GidMap;
// RWTexture2D<uint4> GroupMap;
// StructuredBuffer<objrep> curProp;
RWStructuredBuffer<AroundTg> targetProp;
cbuffer Constants
{
    GlobalConstants g_Constants;
};
// groupshared int lcl;
// cbuffer ConstantsL
//{
//    int3 pd;
//    int activeTh = 0;
//};
struct CastArgBlock
{
    int size, reader;
    CastArg data[I32];
};
// RWStructuredBuffer<uint4> queueFB;
RWStructuredBuffer<CastArg> Shadowqueue; //[QUEUE_SIZE];
// CastArg Shadowqueue[QUEUE_SIZE];
static const uint quesz = QUEUE_SIZE;
static const uint queblocksize = 32;
static const uint queblockcounter = 1024 * 128 / 32 + 1;
static const uint MaxU32 = 0xFFFFFFFF;
// last 256 byte for queue counter
RWBuffer<uint> RWSign;
// groupshared int queuesize = 0;
static const int2 bases[] = {
    { 1, 0 },
    { 0, 1 },
    { 1, 0 },
    { 0, -1 },
    { -1, 0 },
    { 0, -1 },
    { -1, 0 },
    { 0, 1 },
    { 0, 1 },
    { -1, 0 },
    { 0, 1 },
    { 1, 0 },
    { 0, -1 },
    { 1, 0 },
    { 0, -1 },
    { -1, 0 },
};
// void QueAdd(uint id, int val, inout uint res)
//{
//    queueFB.InterlockedAdd(id, val, res);
//}
// void QueAdd(uint id, int val)
//{
//    queueFB.InterlockedAdd(id, val);
//}
int dis(int2 a, int2 b)
{
    int2 ba = a - b;
    return ba[1] * QUEUE_BASE + ba[0];
}
void getPos(in uint2 tg, out uint original)
{
    InterlockedAdd(tg[0], 1, original);
    if ((original % QUEUE_BASE) == 0 && original > 0)
    {
        InterlockedAdd(tg[1], 1);
        InterlockedAdd(tg[0], -QUEUE_BASE);
    }
}
Texture2D<int> GroupIdMap;
StructuredBuffer<SPHKV> Hash2DtabSB;

#define Hash2D(vec) ((vec[1] * 73856093) ^ (vec[0] * 83492791)) % Hash2DCAP
int hash_address(const int2 k)
{
    uint address = Hash2D(k), cv = 0;
    while (cv++ < 20 && any(Hash2DtabSB[address].keypos.xy != k.xy))
    {
        address++;
    }
    return cv < 20 ? address : -1;
}
#define GQS 512
#define AtomW 32
#define EMPTY 0x0
#define STORED 0x2
#define USE 0x1
#define TSSIZE 8

#define GetStatusId(pin, i, ibit) \
    ibit = (pin % 16) * 2;        \
    i = pin % GQS
groupshared AroundTg gat[THREAD_GROUP_SIZE / 8];
groupshared uint4 gatdis[THREAD_GROUP_SIZE / 8];
groupshared uint _gSign[GQS], _queBlock[GQS], _queBase = 0, gqa = 0, gqb = 0;
groupshared int _queC, _queCL, process = 0, _QueBnum = 0;
groupshared uint odsq=MAXI;
groupshared CastArg gcargs[GQS]; // 16k size of the stack
static uint threadid=0,thgid=-1;
#define CalQueBase(i) (i / queblocksize) % THREAD_GROUP_SIZE;
int getQueuepos(uint i)
{
    return _queBlock[(i / queblocksize) % THREAD_GROUP_SIZE] + i % queblocksize;
}
void QueueAdd(int i, out uint res)
{
    InterlockedAdd(RWSign[1024 * 128 / 32 + 1], i, res);
}
uint compact16(const int2 v)
{
    return v[0] << 16 | v[1];
}
#undef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 8
groupshared uint4 nebCounters[THREAD_GROUP_SIZE / 8]; // count mindis instID
RWStructuredBuffer<AroundTg> Nebbuf;
void castLight(int distance, float2 se, const int4 pos, int2 step0, int2 step1,
               const int nebCounteri)
// int xx, int xy, int yx, int yy)
{
    int countero;
    const uint4 nID = nebCounters[nebCounteri];
    // se = se.yx;
    // InterlockedAdd(activeTh, 1);
    float newStart = 0.0f, start = se[0], end = se[1];

    if (start < end)
    {
        return;
    }
    bool blocked = false;
    const float radius = pos[3];
    for (; distance <= radius && !blocked; distance++)
    {
        int2 delta = -distance;
        // int deltaY = -distance;
        // int2 stepy = deltaY * step1;
        uint mod = distance % 2;
        // refer to
        // https://gamedev.stackexchange.com/questions/73759/can-i-prevent-diagonal-movement-from-exploring-more-of-the-map/73787#73787
        uint diagonal = ((distance) / 2) * 3 + mod;
        // - diagonal + ((diagonal - distance) * 2 - ~mod)
        delta[0] = (diagonal > radius) ? -2 * (radius + delta[0] + 1) : delta[0];

        for (; delta[0] <= 0; delta[0]++)
        {
            float2x2 mti2 = { step0, step1 };
            // delta.x = delta.x * step0[0] + delta.y * step1[0];
            // delta.y = delta.x * step0[1] + delta.y * step1[1];
            int2 re3 = mul(mti2, delta);

            int2 currPos = pos.xy + re3;
            // currPos += deltaX * step0;
            const int currentX = currPos.x;
            // startx + deltaX * xx + deltaY * xy;
            const int currentY = currPos.y;
            // starty + deltaX * yx + deltaY * yy;
            float leftSlope = (delta[0] - 0.5f) / (delta[1] + 0.5f);
            float rightSlope = (delta[0] + 0.5f) / (delta[1] - 0.5f);
            // lightMap[currPos] =int4(1,1,1,1);
            if (!(all(currPos >= 0 && currPos < g_Constants.mapsize)) ||
                start < rightSlope)
            {
                continue;
            }
            if (end > leftSlope)
            {
                break;
            }
            //if (GroupIdMap[currPos] > 0)
            if (false)
            {
                uint dissq = dot(re3, re3),odq;
                const int hpadss = hash_address(currPos);
                int2 thidxy = int2(thgid / 8, thgid % 8);
                dot(gat[thgid / 8].pos[4], gat[thgid / 8].pos[4]);
                //const SPHKV kvp = Hash2DtabSB[hpadss];

                if (gatdis[thidxy.x].x >= dissq) {
                    InterlockedMin(gatdis[thidxy.x].x, dissq, odq);
                    if (odq != dissq) {
                        gatdis[thidxy.x].y = 1;
                        gat[thidxy.x].pos[0] = currPos;
                    } else {
                        InterlockedAdd(gatdis[thidxy.x].y, 1, odq);
                        int cli = min(TSSIZE - 1, odq);
                        gat[thidxy.x].pos[cli] = currPos;
                    }
                //InterlockedMin(nebCounters[nebCounteri].y, dissq, odsq);
                }

                // InterlockedAdd(nebCounters[nebCounteri].x, 1, countero);
                // if (dissq == odsq)
                // {
                //     InterlockedAdd(nebCounters[nebCounteri].x, 1, countero);
                //     Nebbuf[nID.z].pos[compact16(Nebbuf[nID.z].pos[0]) < compact16(currPos)
                //                           ? min(countero, 7)
                //                           : 0] = currPos;
                // }
                // else if (dissq < odsq)
                // {
                //     InterlockedAdd(nebCounters[nebCounteri].x, 1, countero);
                //     Nebbuf[nID.z].pos[0] = currPos;
                // }
            }
            int2 delt = delta; // currPos - (pos.xy);(all(currPos==int2(530,530))|
            // check if it's within the lightable area and light if needed
            bool bk = (resistanceMap[currPos] == 100); // > hightdiff;
            // if (!bk)
            {
                // float bright = (float) (1 - (rStrat.radius(deltaX, deltaY) /
                // radius));
                lightMap[currPos] = 1; // int4(1,1,1,1);// g_Constants.lightcolor;
            }
            if (blocked)
            { // previous cell was a blocking one
                if (bk)
                { // hit a wall
                    newStart = rightSlope;
                    continue;
                }
                else
                {
                    blocked = false;
                    start = newStart;
                }
            }
            else
            {
                if (bk) //&& distance < radius)
                {       // hit a wall within sight line
                    blocked = true;
                    uint original;
                    // getPos(queueFBl.zw, original);
                    // queueFB.InterlockedAdd((0 + N2) * 4, 1, original);
                    // QueAdd(2, 1, original);
                    // InterlockedAdd(RWSign[QUEUE_SIZE / 8 + 2], 1, original);
                    // if ((original % QUEUE_BASE) == 0 && original > 0)
                    //{
                    //    //InterlockedAdd(queueFBl[3], 1);
                    //    //queueFBl.InterlockedAdd((0 + N2) * 4, -QUEUE_BASE);
                    //    //queueFBl.InterlockedAdd((1 + N2) * 4, 1);
                    //    original = 0;

                    //}
                    InterlockedAdd(gqb, 1, original);
                    uint shift = ((original % I32) * 2);
                    uint id = USE << shift;
                    uint p = (original % GQS);
                    //InterlockedOr(_gSign[p], id, original);
                    // write to log memory
                    uint exist = (original >> shift) & 0x3;

                    CastArg carg;
                    carg.id = original;
                    InterlockedAdd(_QueBnum, 1, original);
                    #if 0
                    carg.distance = original-gqa;
                    Shadowqueue[threadid] = carg;
                    #endif
                    carg.se = float2(start, leftSlope);
                    carg.distance = distance + 1;
                    carg.pos = pos;
                    carg.step0 = step0;
                    carg.step1 = step1;
                    //original = _gSign[p];
                    //p = 1000;
                    InterlockedCompareExchange(_gSign[p], EMPTY, USE, original);
                    while (original != EMPTY) {
                        InterlockedCompareExchange(_gSign[p], EMPTY, USE, original);
                    }
                    gcargs[p] = carg;
                    InterlockedExchange(_gSign[p], STORED,original) ;
                    int ioff = 0;
                    // write to groupshared memory
                    while (0 & _queC > 0 && (_queC != MAXI) && ioff < GQS)
                    {
                        ioff = (_queC & 0xff) != 0xff ? 0 : 16;
                        uint ofr = 1u << ioff++;
                        InterlockedOr(_queC, ofr, original);
                        if ((ofr & original) == 0)
                        {

                            InterlockedOr(_queCL, ofr, original);
                            while (ofr & original)
                                ;
                            gcargs[ioff - 1] = carg;
                            ofr = ~ofr;
                            InterlockedAnd(_queCL, ofr, original);
                            break;
                        }
                    }
                    // if(ioff>=GQS)
                    if (false)
                    {
                        // write to global memory
                        InterlockedAdd(_QueBnum, 1, original);
                        while (original >= QUEUE_SIZE)
                            ;
                        QueueAdd(1, original);
                        Shadowqueue[original % QUEUE_SIZE] = carg;
                    }
                    // lightMap[uint2(500, 550) + uint2(id, p)] = g_Constants.lightcolor;
                    // castLight(distance + 1, start, leftSlope, pos, step0, step1, ard);
                    newStart = rightSlope;
                }
            }
        }
    }
    // InterlockedAdd(activeTh, -1);
}
static uint nebctid = 0;
void castLight(const CastArg carg)
// int xx, int xy, int yx, int yy)
{
    // queueFBl.InterlockedAdd((0 + N2) * 4, 1);
     castLight(carg.distance, carg.se, carg.pos, carg.step0, carg.step1,nebctid);
}
// groupshared AroundTg nebs[THREAD_GROUP_SIZE/8];
#define SMAX 2147483647
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void fov(uint3 Gid: SV_GroupID, uint3 GTid: SV_GroupThreadID)
{
    uint uiGlobalThreadIdx = Gid.x;//* THREAD_GROUP_SIZE / 8 + uint(GTid.x / 8);
    if (uiGlobalThreadIdx >= g_Constants.uiNum)
        return;
    //lightMap[currPos] = 255
    CastArg carg;
    carg.id = Gid.x * THREAD_GROUP_SIZE + GTid.x;
    carg.id = uiGlobalThreadIdx;
#if 0
    carg.step1[0] = nebctid;
    nebctid=carg.id;
    carg.step1[1] = nebctid;
    carg.step0[1] = threadid;
    RWSign[carg.id] = threadid;
    thgid = GTid.x;
    threadid = uiGlobalThreadIdx;
    carg.step0[0] = threadid;
    carg.pos[3] = process;
#endif
    RWSign[uiGlobalThreadIdx + 1000] = threadid;
    int cls = uiGlobalThreadIdx;
    objrep obj = objProp[uiGlobalThreadIdx];

    gat[thgid / 8].pos[thgid%8] = MaxU32;
    gatdis[thgid / 8] = uint4(MaxU32,0,0,0);
    
    lightMap[obj.pos.xy] = hightdiff;
    uint octant = GTid.x % 8;
    /*RWSign[QueId(2)] = 0;
  RWSign[QueId(0)] = 0;*/
    // RWSign[queblockcounter] = 0;
    const int shcnt = GQS / THREAD_GROUP_SIZE;
    for (int i = 0; i < shcnt; i++) {
        _gSign[GTid.x * shcnt + i] = EMPTY;
    }
    nebctid = GTid.x / 8;
    nebCounters[nebctid] = uint4(0, 0xffffffff, uiGlobalThreadIdx, octant);
    // nebCounters[nebctid]=uint4(0,0xffffffff,0,octant);
    //Nebbuf[uiGlobalThreadIdx].pos[octant] = int2(SMAX, SMAX);
    _queC = 0;
    _queCL = 0;
    _QueBnum = 0;
    _queBase = 0;
    // lcl = 0;
    AllMemoryBarrier();
    InterlockedAdd(process, 1,cls);
    // InterlockedAdd(RWSign[0], -1);
    castLight(1, float2(1, 0), obj.pos, bases[(octant * 2) & 15],
              bases[(octant * 2 + 1) & 15], nebctid);
    uint2 quef = { 0, 0 };

    InterlockedAdd(process, -1, cls);
    // if (QueNum(3) == g_Constants.lightcolor[1])
    //    lightMap[uint2(521, 550) + uint2(uiGlobalThreadIdx, octant)] =
    //    g_Constants.lightcolor;

    // if (QueNum(3) > g_Constants.lightcolor[1])
    //    lightMap[uint2(530, 550) + uint2(uiGlobalThreadIdx, octant)] =
    //    g_Constants.lightcolor;
    // if (RWSign[0] > 0)
    //    lightMap[uint2(514, 550) + uint2(uiGlobalThreadIdx, octant)] =
    //    g_Constants.lightcolor;
    // InterlockedAnd(RWSign[p], ~(1 << 10));
    // AllMemoryBarrier();
    // if (RWSign[0] > 0)
    //    lightMap[uint2(515, 550) + uint2(uiGlobalThreadIdx, octant)] =
    //    g_Constants.lightcolor;
    // queueFBl.InterlockedAdd((0 + N2) * 4, 1, uiGlobalThreadIdx);
    // InterlockedAdd(activeTh, 1, uiGlobalThreadIdx);
    // InterlockedAdd(activeTh, 1);
    // if (activeTh > 0)
    uint p = 0, id = 0, shift;
    uint f = 0, ioff = 1000;
    carg.pos=obj.pos;
    carg.id=obj.id;
    //Shadowqueue[carg.id] = carg;
    Shadowqueue[uiGlobalThreadIdx] = carg;
    uint original = -1;
    InterlockedAdd(gqa, 1, original);
    GetStatusId(original, p, shift);
    // write to groupshared memory
    do
    {

        InterlockedCompareExchange(_gSign[p], STORED, USE, original);
        if (original == STORED) {
            InterlockedAdd(process, 1);
            carg = gcargs[p];
            InterlockedExchange(_gSign[p], EMPTY, original);
            ioff = original != USE ? 1 : ioff;
            //    _gSign[p] = EMPTY;
            castLight(carg);
            //InterlockedAdd(_QueBnum, -1, cls);
            InterlockedAdd(process, -1, cls);
            InterlockedAdd(gqa, 1, original);
        GetStatusId(original, p,shift);
        }
        ioff--;
        carg.distance = cls;
    } while (process > 0 | ioff == 0);
    //carg.se[0] = ioff==0? -100:0;
    //Shadowqueue[carg.id] = carg;
// InterlockedOr(RWSign[octant], 1 << octant);
//////InterlockedAnd(RWSign[octant], ~(1 << octant));
// if (VfArg[obj.tid].NumInstances == QueNum(0) && g_Constants.lightcolor[3] ==
// 0)
//    lightMap[uint2(500, 550) + uint2(uiGlobalThreadIdx, octant)] =
//    g_Constants.lightcolor;
// if (VfArg[obj.tid].NumInstances < QueNum(0) && g_Constants.lightcolor[3] ==
// 1)
//        lightMap[uint2(501, 550) + uint2(uiGlobalThreadIdx, octant)] =
//        g_Constants.lightcolor;
// if (VfArg[obj.tid].NumInstances > QueNum(0) && g_Constants.lightcolor[3] ==
// 2)
//    lightMap[uint2(502, 550) + uint2(uiGlobalThreadIdx, octant)] =
//    g_Constants.lightcolor;
// QueAdd(6, -1, f);
// uint max = queueFB.Load(off * 4);
// queueFB.Store((0 + N2) * 4, 0);
// bool existtarget = resistanceMap[curpix] == obj.targetfilter;
}
