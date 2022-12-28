struct gidmap{
uint4 gids;
};
#define TXIDMAP uint
#define TLIT float4
#define TUSMAP float
#define INT_MAX (0xFFFFFFFF >>1)
#define accessGid(i,map) map[i].gids[0]