#pragma once
#include"GObjs0.h"
#include"json.h"

namespace pj2::n_seri {
	using EntityId = GameProps::EntityId;
	using t_ObjMap = std::unordered_map<std::string, math::int4>;
	using t_ObjMapSeir = phmap::parallel_flat_hash_map<EntityId, std::string>;
	//t_ObjMap _objmap;
	using json = nlohmann::json;

	static void to_json(json& j, const n_Objs::stateDesc& pp);
	static void from_json(const json& j, n_Objs::stateDesc& pp);
}