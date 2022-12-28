#pragma once

#include"json.h"
#include"AllType.h"

namespace pj2::n_seri {
	using EntityId = GameProps::EntityId;
	using t_ObjMap = std::unordered_map<std::string, math::int4>;
	using t_ObjMapSeir = phmap::parallel_flat_hash_map<EntityId, std::string>;
	//t_ObjMap _objmap;
	using json = nlohmann::json;
	//void BuildMap(t_ObjMap& objmap, void* p_ct);
	static void from_json(const json& j, n_weapon::bullet0& p, t_ObjMap& objmap) {

	}
	void GetName(n_weapon::bullet0& b, std::vector<std::string>& prefnamw);
	void GetName(n_weapon::guns0& b, std::vector<std::string>& prefnamw);
	template<class T>
	void GetName(T& b, std::vector<std::string>& prefnamw) {}
	static void prefab_to_json(json& j, n_weapon::bullet0& pp);
	static void prefab_from_json(const json& j, n_weapon::bullet0& pp);
	static void to_json(json& j, Physics::VirtCollid& pp) {
		j.emplace_back(pp.ConvexR);
		j.emplace_back((float)pp.Activei);
		j.emplace_back(pp.InvMass);
		j.emplace_back(pp.MaxVelocity);
	};
	template<int I>
	static void to_json(json& j, Fix::fix<I>& pp) {
		for (size_t i = 0; i < I; i++)
		{
			j.emplace_back(Fix::toFloat(pp[i]));
		}
	};
	template<int I>
	static void from_json(const json& j, Fix::fix<I>& pp) {
		for (size_t i = 0; i < j.size(); i++)
		{
			pp[i]=(Fix::toFix(j[i].get<float>()));
		}
	};

	static void to_json(json& j, const Collid& pp) {
		j = json{ {"mass",1 / pp.InvMass},{"radius",pp.ConvexR},
			{"max speed",pp.MaxVelocity}};
		//j = json{ pp.elements };
	}
	static void from_json(const json& j, Collid& pp) {
		pp.InvMass = 1 / j["mass"].get<float>();
		pp.ConvexR = 1 / j["radius"].get<float>();
	}
	static void from_json(const json& j, sStruct::instData1& p) {
		auto* up = reinterpret_cast<uint*>(&p);
		for (size_t i = 0; i < 8; i++)
		{
			up[i] = j.at(0)[i].get<uint>();
		}
		up += 8;
		for (size_t i = 0; i < 4; i++)
		{
			up[i] = j.at(1)[i].get<int>();
		}
		for (size_t i = 0; i < 4; i++)
		{
			p.color[i] = j.at(2)[i].get<float>();
		}
	}
	//void from_json(const json& j, n_units::repoele& pp) {
	//	auto& objs = n_seri::p_ct->_pkgs;
	//	pp.num = j[0].get<int>();
	//	auto& eq = pp.eq;
	//	eq.Eqprefid.hashnum = 0;
	//	auto& ids = n_seri::objmap[j[1].get<std::string>()];
	//	eq.Eqprefid.id = ids[2];
	//	eq.Eqprefid.tid0 = ids[0];
	//	from_jarray(j[2], eq.d64);
	//}
	//static void to_json(json& j, const n_units::repoele& p) {
	//	auto& ent = p.eq.Eqprefid;
	//	auto& objs = n_seri::p_ct->_pkgs;
	//	ECS::visit_at<n_Registry::Registry>(ent.tid0, [&](auto I) {
	//		using T = n_Registry::Registry::t<I>;
	//		using namespace n_weapon;
	//		if constexpr (ECS::convertible<T, n_weapon::guns0>()) {
	//			auto& pg = objs.get<I>()[0];
	//			{
	//				auto&& [igun, wr] = pg.comps.get(ent.id);
	//				j = json{
	//					p.num,
	//					wr.name,
	//					p.eq.d64
	//				};
	//			}
	//		}
	//		});
	//}
	//static void to_json(json& j, const n_units::df_repo& pp) {
	//	for (auto&& ppe : pp.elements)
	//	{
	//		json jpp;
	//		to_json(jpp, ppe);
	//		j.push_back(jpp);

	//	}
	//	//j = json{ pp.elements };
	//}
	//static void from_json(const json& j, n_units::df_repo& pp) {
	//	for (size_t i = 0; i < pp.elements.size(); i++)
	//	{
	//		if (!j[i].is_null())
	//			from_json(j[i], pp.elements[i]);
	//	}
	//	//from_jarray(j, pp.elements.data());
	//}
	struct serio
	{
		int mode=0;
		json* jo;
		void* di;
		ResManager* rm;
	};
	struct desertab {
		//archtype map name to id
		std::map<std::string, GameProps::EntityId> s2e;
		std::unordered_map<std::string, int> mappropstr2i;
		//std::unordered_map<std::string, std::vector<n_BasObj::Renderble>> mappropstr2i;
		std::unordered_map<std::string, n_BasObj::Ubaseanim[1]> render_map;
		desertab();
		//std::map<std::string,GameProps::EntityId> s2e;
	};
	void Seri(serio s);
	void DeSeri(serio s);
	void from_json(const json& j, n_BasObj::Ubaseanim& p);
	template<class ...Ts>
	void Get(Ts&...args);
}