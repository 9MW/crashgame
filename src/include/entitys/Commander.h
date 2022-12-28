#pragma once

#include "comm.h"
namespace pj2 {
	namespace n_eneity{
		constexpr int maxcmd = 255;
		template<class T,int Size>
		struct cmdarray:std::array<T, Size>
		{
			using par = std::array<T, Size>;
			using reference = T&;
			int c = 0;
			T& emplace_back() { return par::at(c++); }
			T& emplace_back(const T& t) {
				par::at(c++) = t;
				return par::operator[](c);
			}
			void clear() { c = 0; }
			constexpr reference operator[](const size_t& _Pos) noexcept /* strengthened */ {
				return par::operator[](_Pos);
			}
		};
		struct cmdlist
		{
			int units,activetargetpointer;
			cmdarray<GameProps::Cmd0,maxcmd> cmds;
			cmdarray<int, maxcmd> invalidcmd;
		};
		struct ucmdid {
			struct hashcd
			{
				inline const size_t& operator()(const CollideID& cd)const {
					return cd.hash();
				}
			};
			phmap::flat_hash_set<CollideID, hashcd> activeunits;
			std::vector<CollideID> invalidcmdunit, invalidcmdlast;

		};
		using t_subunit = ucmdid;// td::vector<CollideID>;
		using cmdcomp = ECS::ECSclass<cmdlist, t_subunit>;
		struct instructor:n_BasObj::poolobj<cmdcomp>
		{
			//std::vector<cmdlist>cmdlists;
		};
	}
}