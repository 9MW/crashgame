#pragma once
#include"Gameprops0.h"
namespace pj2::GameProps {
	/*template<int bs,int bsize, class T,class T2>
	void setBitfield(T&i,T2& inum) {
		constexpr T mask = 0xf*bs;
		i &= (~(bsize << bs) & (inum << bs));
	}*/
	enum bytename
	{
		playerid, faction, visionRange, hp
	};
	struct gids
	{
		union 
		{
			int k;
			UCHAR kl[4];
			struct {
				UCHAR playerid, faction, visionRange, hp;
			};
		};
	};
	template<class T>
	inline int getPlayerId(T& i) {
		gids& gr = reinterpret_cast<gids&>(i);
		return gr.kl[playerid];
	}

	inline gids& getPlayerInfo(int& i) {
		return reinterpret_cast<gids&>(i);
	}
	inline gids& getPlayerInfo(uint& i) {
		return reinterpret_cast<gids&>(i);
	}
	inline int setPlayerId(uint& i,const int id) {
		gids& gr = reinterpret_cast<gids&>(i);
		gr.kl[playerid] = id;
		return i;// &= (~(0xff << bs) & (id << bs));
	}
	template<class T>
	inline T& sethpbar(T& i, const float normalhp) {
		gids& gr = reinterpret_cast<gids&>(i);
		gr.kl[hp] = (0xff * normalhp) ;
		return i;
	}

	/*template<int bs,int bsize, class T,class T2>
	void setBitfield(T&i,T2& inum) {
		constexpr T mask = 0xf*bs;
		i &= (~(bsize << bs) & (inum << bs));
	}*/

}