
#include "BasicStructures.fxh"

#define MAXI 0xFFFFFFFF
#define ZERO 0x00000000
#ifndef THREAD_GROUP_SIZE
#define THREAD_GROUP_SIZE 64
#define TIN instData
cbuffer Constants
{
	GlobalConstants g_Constants;
};
StructuredBuffer<TIN>insProp;
StructuredBuffer<float4x4> animreco;
RWStructuredBuffer<float4x4> animout;
[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void skin0(uint3 Gid : SV_GroupID,
    uint3 GTid : SV_GroupThreadID){
    
	uint uiGlobalThreadIdx = Gid.x * uint(THREAD_GROUP_SIZE)+GTid.x;
    TIN cff=insProp[uiGlobalThreadIdx];
	if (uiGlobalThreadIdx >= g_Constants.uiNum)
		return;
		const uint bonenum=g_Constants.pedd;
		uint ic=0;
		for(uint i=cff.frameIndex;i<bonenum;i++,ic++){
			animout[uiGlobalThreadIdx*bonenum+ic]=animreco[i];
		}
    }