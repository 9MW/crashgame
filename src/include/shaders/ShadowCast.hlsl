//#pragma once

#include "ToolFuncs.fxh"
#include "VTYPES.fxh"

#define MAXI 0xFFFFFFFF
#define ZERO 0x00000000
#ifndef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 64
#endif
#ifndef QUEUE_SIZE
#define QUEUE_SIZE 1024*128
#endif
#ifndef AFM
#define AFM float
#endif
#ifndef QUEUE_BASE
#define QUEUE_BASE 1024*QUEUE_SIZE
#endif
#ifndef I32
#define I32 32
#endif
#ifndef N2
#define N2 2
#endif
#ifndef RECTPERC
#define RECTPERC 100
#endif
#ifndef NUM_BUf
#define NUM_BUf 1
#endif

#define RASELE 0xFFFFFF
#define RESELE RASELE+1
RWStructuredBuffer<SPHKV> Hash2Dtab;
StructuredBuffer<SPHKV> Hash2DtabSB;
StructuredBuffer<bufferinfo> instable;

RWStructuredBuffer<inscmb> Visibleout;
StructuredBuffer<tinfo> _tinfo;

StructuredBuffer<mtransfrom> s_objtrans;//[NUM_BUf];
StructuredBuffer<instData1>s_objProps;//[NUM_BUf];
//dynamic info for each render insatnce grout
StructuredBuffer<bufferinfo> so_bufferinfo;
//culled result offset for 0 for different  instance to render
//define max render instance per mesh
//StructuredBuffer<bufferinfo> co_bufferinfo;
//Buffer<uint> counts;
RWStructuredBuffer<mtransfrom> o_objtrans;//[NUM_BUf];
RWStructuredBuffer<instData1>o_objProps;//[NUM_BUf];
RWStructuredBuffer<inscmb> o_VfOIns;
RWTexture2D<lightType> lightMap;
RWStructuredBuffer<AroundTg>Nebbuf;
Texture2D<lightType> lightMapSRV;

RWTexture2D<float4> clearlightMap;
Texture2D<int4> resistanceMap;
RWTexture2D<TXIDMAP> GroupIdMap;
RWStructuredBuffer<objrep> objPropLit;
RWTexture2D<uint> resistanceMapW;
RWTexture2D<uint4> resistanceMapW1;
//RWTexture2D<double2> resistanceMapW2;
StructuredBuffer<objrep> moveProp; //map to m_deltaObjs
RWStructuredBuffer<objrep> movePropLocal; //map to m_deltaObjs
//StructuredBuffer<objrep> curProp;
cbuffer Constants
{
	#ifndef WriteGMap
	GlobalConstants g_Constants;
	#else
	GlobalConstants g_Constants;
	#endif
};

struct CastArgBlock
{
	int size , reader ;
	CastArg data[I32];
};
RWStructuredBuffer<CastArg>Shadowqueue; //[QUEUE_SIZE];
RWStructuredBuffer<CullResults> Vf_out;
StructuredBuffer<VfProp> Vf_Props;
RWStructuredBuffer<CastArg> VfIn;
RWStructuredBuffer<VFoutArgs> VfArg;
RWStructuredBuffer<instData> VfOIns;
RWStructuredBuffer<VfGloable> VfG;
RWStructuredBuffer<Audio0> audio_insout;
RWStructuredBuffer<selectinfo> sele_out;
//RWStructuredBuffer<VFoutArgs> Vfoutinfo;
//CastArg Shadowqueue[QUEUE_SIZE];
static const uint quesz = QUEUE_SIZE;
static const uint MaxU32 = 0xFFFFFFFF;
RWBuffer<uint>
RWSign;
//groupshared int queuesize = 0;
static const int2 bases[] =
{
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

#ifdef Hash2DCAP
#define Hash2D(vec) ((vec[1]*73856093)^(vec[0]*83492791))%Hash2DCAP
int hash_address(const  int2 k){
    uint address = Hash2D(k), cv = 0,key=(k[0]<<16)|k[1],org=-2;

    while (cv++ < 20) {
        InterlockedCompareExchange(Hash2Dtab[address].mindis, 0xFFFFFFFF, key, org);
        Hash2Dtab[address].keypos = address;       //int2(544, org);
        Hash2Dtab[address].itid[7] = int2(key, org); //int2(544, org);
        //return address;
        if ((org == -1) | (org == key)) {
			break;
        }
        address++;
    }
    //Hash2Dtab[address].mindis = 5464;
 return cv<20?address:-1;
}
#endif
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void move(uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID)
{
	uint uiGlobalThreadIdx = Gid.x * THREAD_GROUP_SIZE+GTid.x;
    // objrep prerep = movePropLocal[uiGlobalThreadIdx];
    // uint2 xel;
    const uint2 sz = g_Constants.mapsize / 2;
    // uint2 lf = prerep.bx.zw;
    // uint2 tr = prerep.bx.xy;
    // int i;
    // for (i = lf[0]; i <= tr[0]; ++i)
    // {
    //     xel[0] = i;
    //     for (int p = lf[1]; p <= tr[1]; ++p)
    //     {
    //         xel[1] = p;
    //         xel += sz;
    //         resistanceMapW[xel] = 5;
    //     }
    // }
    // AllMemoryBarrier();
    if (uiGlobalThreadIdx >= g_Constants.uiNum)
		return;
    const objrep obj = moveProp[uiGlobalThreadIdx];
    uint2 xel = obj.pos.xy + sz;// + 512;
    //xel[0] = 530;
	//xel=int2(530,530);
	resistanceMapW[xel] =100;// obj.pos.z;
	//lf = obj.bx.zw;
	//tr = obj.bx.xy;

	//for ( i = lf[0]; i <= tr[0]; ++i)
	//{
	//    xel[0] = i;
	//    for (int p = lf[1]; p <= tr[1]; ++p)
	//    {
	//        xel[1] = p;
	//        xel += sz;
	//        resistanceMapW1[xel] = obj.groupids;
	//        //resistanceMapW[xel]   = obj.pos[2];
	//    }
	//}
	movePropLocal[uiGlobalThreadIdx] = obj;
	//resistanceMapW[obj.pos.xy] = hightdiff;
}
//unused
//filter to fix block return blockid and instance number
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void Vfilter0(uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID)
{
	uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE)+GTid.x;
	if (uiGlobalThreadIdx >= g_Constants.uiNum)
		return;
	const uint blocksize = g_Constants.pedd;
	//objrep prerep = movePropLocal[uiGlobalThreadIdx];
	int2 xel;
	const uint2 sz = g_Constants.mapsize / 2;
	int i;
	VfProp obj = Vf_Props[uiGlobalThreadIdx];
	//tid==-1 means inactive
	if (obj.insd.tid == -1) {
		return;
	}
#ifdef AudioIn
	if (obj.insd.audiosrc != -1) {
		Audio0 a0;
		InterlockedAdd(VfArg[0].audio_ins, 1, i);
		float4 dv4 = g_Constants.camerapos.xy - obj.insd.transform.pos.xy;
		a0.dissq = max(dot(dv4.xy, dv4.xy), 1);
		a0.aus = obj.insd.audiosrc;
		a0.aul = obj.insd.audiolength;
		//if (a0.aul == obj.insd.audiolength) {
		//
		//}
		audio_insout[i] = a0;
	}
#endif

	const int2 lf = obj.bx.zw;
	const int2 tr = obj.bx.xy;
	uint insid;
	int2 posi = (obj.insd.transform).pos.xy;
	int2 posi001 = (obj.insd.transform).pos.xy * RECTPERC;
#ifdef CHECKRECT
	//const instData=
	if (all(g_Constants.RECT.xy >= posi001) && all(g_Constants.RECT.xy <= posi001)) {
		InterlockedAdd(sele_out[0].x, 1, insid);
		sele_out[insid + 1] = int2(obj.insd.tid, obj.insd.insid);
	}
#endif
	if (any(lightMapSRV[posi + sz]))
	{
		InterlockedAdd(VfArg[obj.insd.tid].NumInstances, 1, insid);
		//VfArg[insid + 10].p1 = obj.insd.tid;
		//VfArg[insid + 10].p2 = obj.insd.id;
		//VfArg[insid + 10].FirstInstanceLocation = uiGlobalThreadIdx;
		VfOIns[VfArg[obj.insd.tid].FirstInstanceLocation + insid] = obj.insd;
	}
	return;
	for (i = lf[0]; i <= tr[0]; ++i)
	{
		xel[0] = i;
		for (int p = lf[1]; p <= tr[1]; ++p)
		{
			xel[1] = p;
			xel += sz;
			int4 res = lightMapSRV[xel];
			//visable and need render
			if (any(res > 0))
			{
				InterlockedAdd(VfArg[obj.insd.tid].NumInstances, 1, insid);

				//int bk = insid / blocksize;
				//uint nod = insid % blocksize;
				//if (nod == 0)
				//{
				//    InterlockedAdd(VfG[0].Ablock, 1,bk);
				//    InterlockedAdd(VfG[0].Args,1);
				//    InArgs arg0 = VfArg[obj.tid];
				//    arg0.NumInstances = blocksize;
				//    arg0.FirstInstanceLocation = insid;
				//    arg0.p1 = obj.tid;
				//    VfArg[bk] = arg0;
				//}
				//InterlockedAdd(Vfoutinfo[obj.tid].NumInstances, 1);
				//Vf_out[insid].id.xy = int2(obj.tid, obj.id);
				VfOIns[VfArg[obj.insd.tid].FirstInstanceLocation + insid] = obj.insd;
				//
				return;
				p = tr[1] + 1;
				i = tr[0] + 1;
			}
			//resistanceMapW[xel]   = obj.pos[2];
		}
	}
	//resistanceMapW[obj.pos.xy] = hightdiff;
}
StructuredBuffer<mtransfrom> objectTransfromBuffer;
StructuredBuffer<instData1> instDatas;
#ifdef VFILTER
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void Vfilter(uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID)
{
	uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE)+GTid.x;
	if (uiGlobalThreadIdx >= g_Constants.uiNum)
		return;
	const uint blocksize = g_Constants.pedd;
	//objrep prerep = movePropLocal[uiGlobalThreadIdx];
	int2 xel;
	const uint2 sz = g_Constants.mapsize / 2;
	int i;
    const mtransfrom transform=objectTransfromBuffer[uiGlobalThreadIdx];
const instData1 insd=instDatas[uiGlobalThreadIdx];
	//tid==-1 means inactive
	if (insd.tid == -1) {
		return;
	}
#ifdef AudioIn
	if (insd.audiosrc != -1) {
		Audio0 a0;
		InterlockedAdd(VfArg[0].audio_ins, 1, i);
		float4 dv4 = g_Constants.camerapos.xy - insd.transform.pos.xy;
		a0.dissq = max(dot(dv4.xy, dv4.xy), 1);
		a0.aus = insd.audiosrc;
		a0.aul = insd.audiolength;
		//if (a0.aul == insd.audiolength) {
		//
		//}
		audio_insout[i] = a0;
	}
#endif

	const int2 lf = insd.bx.zw;
	const int2 tr = insd.bx.xy;
	uint insid;
	int2 posi = (transform).pos.xy;
	int2 posi001 = (transform).pos.xy * RECTPERC;
#ifdef CHECKRECT
	//const instData=
	if (all(g_Constants.RECT.xy >= posi001) && all(g_Constants.RECT.xy <= posi001)) {
		InterlockedAdd(sele_out[0].x, 1, insid);
		sele_out[insid + 1] = int2(insd.tid, insd.insid);
	}
#endif
	if (any(lightMapSRV[posi + sz]))
	{
		InterlockedAdd(VfArg[insd.tid].NumInstances, 1, insid);
		//VfArg[insid + 10].p1 = insd.tid;
		//VfArg[insid + 10].p2 = insd.id;
		//VfArg[insid + 10].FirstInstanceLocation = uiGlobalThreadIdx;
		VfOIns[VfArg[insd.tid].FirstInstanceLocation + insid] = insd;
	}
	return;
	for (i = lf[0]; i <= tr[0]; ++i)
	{
		xel[0] = i;
		for (int p = lf[1]; p <= tr[1]; ++p)
		{
			xel[1] = p;
			xel += sz;
			int4 res = lightMapSRV[xel];
			//visable and need render
			if (any(res > 0))
			{
				InterlockedAdd(VfArg[insd.tid].NumInstances, 1, insid);

				//int bk = insid / blocksize;
				//uint nod = insid % blocksize;
				//if (nod == 0)
				//{
				//    InterlockedAdd(VfG[0].Ablock, 1,bk);
				//    InterlockedAdd(VfG[0].Args,1);
				//    InArgs arg0 = VfArg[obj.tid];
				//    arg0.NumInstances = blocksize;
				//    arg0.FirstInstanceLocation = insid;
				//    arg0.p1 = obj.tid;
				//    VfArg[bk] = arg0;
				//}
				//InterlockedAdd(Vfoutinfo[obj.tid].NumInstances, 1);
				//Vf_out[insid].id.xy = int2(obj.tid, obj.id);
				VfOIns[VfArg[insd.tid].FirstInstanceLocation + insid] = insd;
				//
				return;
				p = tr[1] + 1;
				i = tr[0] + 1;
			}
			//resistanceMapW[xel]   = obj.pos[2];
		}
	}
	//resistanceMapW[obj.pos.xy] = hightdiff;
}
#endif
#ifdef Hash2DCAP


void castLight(CastArg carg)
//int xx, int xy, int yx, int yy)
{
	//castLight(carg.distance, carg.se, carg.pos, carg.step0, carg.step1);
}
#define MAXLit 1000
RWStructuredBuffer<objrep> objPropRW;
StructuredBuffer<objrep> objProp;
RWStructuredBuffer<uint4> CastGxyz;
groupshared uint mindis=0;
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void WriteGMap(uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID)
{
    uint uiGlobalThreadIdx = Gid.x * THREAD_GROUP_SIZE + uint(GTid.x), orv;
	const uint InstanceID=uiGlobalThreadIdx;
    const instData1 insd = s_objProps[InstanceID];
    if (uiGlobalThreadIdx >= g_Constants.uiNum | (insd.tid == -1))
        return;
    //InterlockedAdd(mindis, 1, orv);
    //orv = 1 << (GTid.x % 32);
    const float3 Pos = s_objtrans[InstanceID].pos;
    const int2 xels = (Pos).xy + g_Constants.mapsize * 0.5;
    const uint2 xel = uint2(xels);
    const uint gid = (insd.gt01id) & 0xff, fracid = (insd.gt01id >> 8) & 0xff, visiondis = ((insd.gt01id >> (16)) & 0xff);
    orv = 1 << fracid;
    uint oo ,ooi=0;
    objrep olrp;
	//if((1<<gid)|g_Constants.litmask[0])
    {
        olrp.pos = int4(Pos, visiondis);
        olrp.pos.xy = xel;
        olrp.targetfilter[0] = fracid;
        olrp.targetfilter[1] = 5 * 5;// visiondis* visiondis;
        olrp.tid = uiGlobalThreadIdx;
        olrp.id = insd.id;

        InterlockedAdd(CastGxyz[0].x, 1, ooi);
        if (visiondis>0)
           {
			 objPropRW[ooi] = olrp;
			}
    }
#if 0
    InterlockedAdd(GroupIdMap[uint2(0, 0)], 1, oo);
    GroupIdMap[uint2(0, 0)] = 1515;
    GroupIdMap[uint2(gid, 0)] = 1515;
    GroupIdMap[xel.xy] = gid;
    InterlockedMax(GroupIdMap[xel.xy] ,gid);
    InterlockedOr(GroupIdMap[int2(0, 0)], orv);
#endif
    InterlockedOr(GroupIdMap[xel.xy], orv);
    const int hpadss = Hash2D(xel.xy);
    InterlockedAdd(Hash2Dtab[hpadss].num, 1, oo);
    Hash2Dtab[hpadss].itid[9] = int2(insd.tid, insd.tid==0xFFFFFFFF);
        //Hash2Dtab[0].mindis = 4545;
	if(oo<10)
    {
        Hash2Dtab[hpadss].itid[oo] = int2(insd.tid, insd.id);
        Hash2Dtab[hpadss].itid[8] = xel; //(Pos).xy ;
        Hash2Dtab[hpadss].itid[7] = int2(Pos.x, InstanceID) ;
	}
    //Hash2Dtab[hpadss].num = 1;
}
//Gid.xy= shadowmapsz / 32 & shadowmapsz>=1024 => uiGlobalThreadIdx>=65536
[numthreads(8, 8, 1)]
void reset(uint3 Gid : SV_GroupID,
	uint3 GTid : SV_GroupThreadID)
{
    uint3 texpos = Gid * 32 + GTid * 4;
    uint uiGlobalThreadIdx = texpos.x + texpos.y;
	for (int distance = texpos.x; distance < texpos.x + 4; distance++)
	{
		for (int deltaX = texpos.y; deltaX < texpos.y + 4; deltaX++)
        {
            uint hid = min(Hash2DCAP, distance*1024+deltaX);
            Hash2Dtab[hid].mindis = 0xFFFFFFFF;
            Hash2Dtab[hid].num = 0;
            Hash2Dtab[hid].keypos = -1;
			const uint2 coordinate=uint2(distance, deltaX);
			clearlightMap[coordinate] = 0;
			GroupIdMap[coordinate]= 0;
		}
    }
#ifdef Hash2DCAPh
    for (int i = uiGlobalThreadIdx; i)
    if (Hash2DCAP > (uiGlobalThreadIdx)) {
        Hash2Dtab[uiGlobalThreadIdx].mindis = 0xFFFFFFFF;
        Hash2Dtab[uiGlobalThreadIdx].num = 0;
        Hash2Dtab[uiGlobalThreadIdx].keypos = -1;
        Hash2Dtab[hpadss].itid[7]=-1U>>1
    }
#endif
	if (uiGlobalThreadIdx >= g_Constants.pedd)
		return;
	VfArg[uiGlobalThreadIdx].NumInstances = 0;

#ifdef CHECKRECT
	sele_out[0] = 0;
#endif
}
#endif
//[numthreads(THREAD_GROUP_SIZE, 1, 1)]
//void resetArg(uint3 Gid : SV_GroupID,
//          uint3 GTid : SV_GroupThreadID)
//{
//    uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE) + GTid.x * 8 + GTid[1];

//}

//struct ParticleAttribs
//{
//	float2 f2Pos;
//	float2 f2NewPos;

//	float2 f2Speed;
//	float2 f2NewSpeed;

//	float fSize;
//	float fTemperature;
//	int iNumCollisions;
//	float fPadding0;
//};
//cbuffer Constants
//{
//	GlobalConstants g_Constants;
//};
//RWStructuredBuffer<ParticleAttribs> g_Particles;
//RWBuffer<int /*format=r32i*/> g_ParticleListHead;
//RWBuffer<int /*format=r32i*/> g_ParticleLists;

//[numthreads(THREAD_GROUP_SIZE, 1, 1)]
//void main(uint3 Gid : SV_GroupID,
//          uint3 GTid : SV_GroupThreadID)
//{
//	uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE) + GTid.x;
//	if (uiGlobalThreadIdx >= g_Constants.uiNum)
//		return;

//	int iParticleIdx = int(uiGlobalThreadIdx);

//	ParticleAttribs Particle = g_Particles[iParticleIdx];
//	Particle.f2Pos = Particle.f2NewPos;
//	Particle.f2Speed = Particle.f2NewSpeed;
//	Particle.f2Pos += Particle.f2Speed * g_Constants.f2Scale * 0.02;
//	//Particle.fTemperature -= Particle.fTemperature * min(g_Constants.fDeltaTime * 2.0, 1.0);

//	//ClampParticlePosition(Particle.f2Pos, Particle.f2Speed, Particle.fSize, g_Constants.f2Scale);
//	//g_Particles[iParticleIdx] = Particle;

// //   // Bin particles
//	int GridIdx = 100;
//	int OriginalListIdx;
//	InterlockedExchange(g_ParticleListHead[GridIdx], iParticleIdx, OriginalListIdx);
//	g_ParticleLists[iParticleIdx] = OriginalListIdx;
//}
