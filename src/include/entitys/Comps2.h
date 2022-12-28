#pragma once
#include"comm.h"
#include"GProps.h"
#include <Gameprops0.h>
//#include <syss.hpp>
#include"units0.h"
namespace pj2::Objs {
	struct cmdbuf//:n_BasObj::obj<ECS::ECSclass<cmdbuf>>
	{
		std::atomic_int32_t members = 0;
		int cmdnum=0;
		std::array<GameProps::Cmd0, MaxCmdCache> _cmds0;
		std::vector<GameProps::Cmd0> cmds;
		cmdbuf(const cmdbuf& org) {
			members._Storage._Value = org.members._Storage._Value;
			cmds = std::move(org.cmds);
			_cmds0 = org._cmds0;
			cmdnum = org.cmdnum;
		}
		cmdbuf() {
		}
		inline const GameProps::Cmd0& getcmd(const int& i) {
			return i > (MaxCmdCache - 1) ? _cmds0[i] : cmds[i];
		}
		void init(Utility::span<GameProps::Cmd0> c){
			cmdnum = c.size();
			if (c.size() <= _cmds0.size()) {
				//std::memcpy()
				for (size_t i = 0; i < c.size(); i++)
				{
					_cmds0[i] = c[i];
				}
			}
			else
			{
				cmds.clear();
				for (size_t i = 0; i < c.size(); i++)
				{
					cmds.emplace_back(c[i]);
				}
			}
		}

		/*static int Build(Manager::t_Arch& achs, const int& id){
		
		}*/
	};
	
	//using t_unitcbf = n_BasObj::obj<ECS::ECSclass<cmdbuf>>;
}