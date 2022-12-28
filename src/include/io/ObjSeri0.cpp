#include "ObjSeri0.h"
#include"knames.h"

namespace pj2 {
	float tophytime(int i) { return i * GameProps::MainProp().physicsdt; }
	int tosteptime(float i) { return i / GameProps::MainProp().physicsdt; }
}
void pj2::n_seri::to_json(json& j, const n_Objs::stateDesc& pp) {
	using namespace compname;
	j["name"] = pp.name;
	j["Desc"] = pp.Desc;
	j["duration"] = tophytime(pp.dur);
	for (size_t i = 0; i < _countof(pp.propid); i++)
	{
		j[propstrmap[pp.propid[i]]] = pp.effect[i];
	}
}

void pj2::n_seri::from_json(const json& j, n_Objs::stateDesc& pp) {
	pp.dur = tosteptime(j["duration"].get<float>());

}
