#include "config.h"
#include"Render/fogRender.h"
#include"json.h"
#define JD(name, varb) varb=j[name].get<decltype(varb)>()
#define JD1(name,typ, varb) varb=j[name].get<typ>()
namespace pj2 {
	int64_t& cfg::cfgget(void* const p) {
		std::memset(p, 0, 8);
		return this->datas[0];
	}
	template<>
	int cfg::cfgexe<1, int>(int&& nun)
	{
		nun = this->datas[0];
		return nun;
	}
	using json = nlohmann::json;
	template<>
	cfg& cfg::cfgexe(json& js, n_Render::fogRender& fgr_)
	{
		GloProp::Prop& cfg_ = GloProp::Get();
		json& j = js["fogwar"];
		JD("MaxSOLnum", fgr_.MaxSOLnum);
		JD("mxselect", cfg_.maxSelect);
		JD("maxcullnum", cfg_.maxcullnum);
		JD("maxinstancenum", cfg_.maxinstancenum);
		JD("MaxTypenum", cfg_.MaxTypenum);
		JD1("testunitnum", int, datas[0]);
		return *this;
	}
	template<>
	cfg& cfg::cfgexe(int& i)
	{
		i = this->datas[0];
		return *this;
	}
	void cfgget(void* const p) {
		std::memset(p, 0, 8);
	}

}