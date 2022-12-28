
#include "VTYPES.fxh"
#include "BasicStructures.fxh"
cbuffer Constants
{
    GlobalConstants g_Constants;
};
struct factioninfo {
    uint4 targetfilter;
};
//StructuredBuffer<tinfo> _tinfo;
RWStructuredBuffer<AroundTg> targetProp;
StructuredBuffer<objrep> objProp;
StructuredBuffer<factioninfo> factioninfos;
Texture2D<TXIDMAP> GroupIdMap;
Texture2D<float4> lightMap;
StructuredBuffer<SPHKV> Hash2DtabSB;
StructuredBuffer<bufferinfo> instable;

groupshared uint4 dis;
groupshared uint mindis;
groupshared AroundTg gard;
//groupshared factioninfo fi;
#define CSQ 8
#undef TOUTH
#ifdef TOUTH
RWTexture2D<float4> DETMap;
#endif

#define Hash2DCAP ( 12288 * 2);
#define Hash2D(vec) ((vec[1] * 73856093) ^ (vec[0] * 83492791)) % Hash2DCAP
int hash_address(const int2 k) {
    uint address = Hash2D(k);
    uint cv = 0, key = (k[0] << 16) | k[1];
    while ((cv++ < 20 )&(Hash2DtabSB[address].mindis != key)) {
        address++;
    }
    return cv < 20 ? address : -1;
}
[numthreads(CSQ, CSQ, 1)]
void Scanner(uint3 Gid: SV_GroupID, uint3 GTid: SV_GroupThreadID) {

    uint uiGlobalThreadIdx = Gid.x;// * THREAD_GROUP_SIZE / 8 + uint(GTid.x / 8);
    if (uiGlobalThreadIdx >= g_Constants.uiNum)
        return;
    dis = 0xFFFFFFFF;
    objrep obj = objProp[uiGlobalThreadIdx];
    factioninfo fi = factioninfos[obj.targetfilter.x];
    mindis = 0xFFFFFFFF;
    int lit = lightMap[obj.pos.xy].x;
    uint2 visiondis = uint2(obj.pos[3], obj.pos[3]);
    uint vdsq = visiondis[0] * visiondis[1];
     uint2 block = ceil(2 * visiondis / (float)CSQ);
    //const int2 tr = max(g_Constants.mapsize-1, obj.pos.xy + obj.pos[3]);
    int2 lf = (((obj.pos.xy - visiondis) + block * GTid.xy));
    gard.pos[GTid.x] = 2147483647;//GTid.x == 0 ? 2147483647 : GTid.xy;
    lf= clamp(lf, uint2(0, 0), g_Constants.mapsize);
    uint2 xel; //= clamp(GTid.xy * block, uint2(0, 0), g_Constants.mapsize);
    block = min(g_Constants.mapsize, lf + block);
#if 0
    gard.pos[2] = 0;
    if (uiGlobalThreadIdx == 0) {
        gard.pos[3] = g_Constants.mapsize;
        gard.pos[4] = visiondis;

        gard.pos[5] = fi.targetfilter.x;
        gard.pos[6] = GroupIdMap[uint2(1, 0)];
        gard.pos[7] = GroupIdMap[obj.pos.xy];
        //gard.pos[2][1] = max(gard.pos[2][1], (int)lightMap[obj.pos].x);
    }
#endif
    AllMemoryBarrier();
    for (xel[0] = lf[0]; xel[0] < block[0]; ++xel[0])
    {
        for (xel[1] = lf[1]; xel[1] < block[1]; ++xel[1])
        {

#ifdef TOUTH
             DETMap[xel]=float4(0,1,0.2,1);
#endif
             uint xelid = GroupIdMap[xel.xy];
             float4 lmv = lightMap[xel];
             float litf = (lightMap[xel].x);
             if (((lightMap[xel].x) > 0) & ((xelid & fi.targetfilter.x) > 0)) {
                {
                    int2 disv = obj.pos.xy - xel;
                    uint coordinateshash = (disv[0] + disv[1]) == 0 ? max(disv[0], disv[1]) : disv[0] + disv[1]+512;
                    //expect distance won't exceed 30
                    uint mdo,cl=coordinateshash, dissq = ((dot(disv, disv)));//Warning: prental percision collision
                    coordinateshash |= (dissq << 10);
                    InterlockedMin(mindis, coordinateshash, mdo);
                    if ((mdo > coordinateshash)&(vdsq >= dissq))
                         {
                        uint address = Hash2D(xel.xy);
                        SPHKV eles = Hash2DtabSB[address];
                        gard.pos[7] = Hash2DtabSB[address].itid[0]; // int2(address, Hash2DtabSB[address].num);
                        //gard.pos[6] = int2(cl, eles.num);
                        gard.pos[6] = int2(vdsq, obj.targetfilter.y);       //;
                        gard.pos[5] = int2(asint(lmv.y), asint(litf));//eles.itid[7]; //int2(xel);//int2(Hash2DtabSB[address].mindis,uint((xel[0] << 16) | xel[1]));
                        dis.xy = xel;
                        // for (int i = 7; i > 0; i--) {
                        //     gard.pos[i] = gard.pos[i-1];
                        // }
                        gard.pos[0] = int2(xel) - int2(g_Constants.mapsize * 0.5); //int2(g_Constants.mapsize * 0.5);
                        gard.pos[3] = gard.pos[0];
                        gard.pos[2] = disv; //eles.itid[8];
                        gard.pos[1] = int2(eles.num, coordinateshash );
                        for (uint i = 4; (i-4) < eles.num & i < 8;i++)
                        gard.pos[4] = int2(eles.itid[0][1], eles.itid[1].y);
                    }
                }
            }
        }
    }
    AllMemoryBarrier();

    //gard.pos[0] = dis.xy - int2(g_Constants.mapsize * 0.5);
    gard.pos[7][0] = mindis;
    targetProp[obj.tid].pos[GTid.x] = gard.pos[GTid.x];
}
//x ^ 2 + y ^ 2 = ss
//2xy+x^2+y^2=s^2+2xy
//x+y=s
