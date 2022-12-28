#pragma once

#include "App0.h"
#include"turnsys.h"
#include"Common/ecs.hpp"
#include"json.h"
namespace cage {
	using json = nlohmann::json;
	context turnctx;
	warsys wsys;
	using t_rd = sStruct::inscmb;
	using t_unit = ECS::CompPackage<playerprop, t_rd>;
	t_unit units;
	using pj2::ResManager;
	using pj2::loader;
	using pj2::Paths;
	void convert(const json& j, float& f) {
		f = std::atof(j.get<string>().c_str());
	}
	int unitcsv(std::vector<playerprop>& prc) {
		string fnm = "unitcfg.csv";
		std::ifstream infile(loader::Get(Paths::configs) / fnm);
		std::vector<string> cfg, cfgele;
		string line;
		std::getline(infile, line);//discard first line 
		prc.reserve(10);
		int n = 0;
		while (std::getline(infile, line)) {
			cfgele.clear();
			auto lstream = std::istringstream(line);
			while (std::getline(lstream, cfgele.emplace_back(), ','));
			prc.emplace_back();
			n++;
			auto& pi = prc.back();
			pi.name = cfgele[0];
			for (size_t i = 0; i < 8; i++)
			{
				prc.back().get((PropId)(i)) = std::atof(cfgele[i+1].c_str());
			}
		}
		return n;
	}
	void buildunitfromjson(const json& j, int i) {
		auto&& [prc, plg] = units.get< playerprop, sStruct::inscmb>(i);
		string l1 = R"(name,hp,armor,marmor,speed,damage,mdamage,agile,critical,attackdis
warrior,52,1,0,1,2,0,0,,melee
hunter,50,0,0,1.25,2,0,10,10,melee
)";
		std::istringstream input,inpele;
		input.str(l1);
		std::vector<string> cfg,cfgele;
		string fef = "";
		float gh=std::atof(fef.c_str());
		while (std::getline(input, cfg.emplace_back())) {
		}
		
		for (auto i = 0; i < cfg.size(); i++)
		{
			cfgele.clear();
			inpele.clear();
			inpele.str(cfg[i]);

			while (std::getline(inpele, cfgele.emplace_back(), ',')) {
			}
		}
		//fill array with 0
		std::fill(prc.props.begin(), prc.props.end(), 0);
		int x = 0;
		prc.name = j["name"];
		convert(j["hp"], prc.get((PropId)x++));
		convert(j["armor"], prc.get((PropId)x++));
		convert(j["marmor"], prc.get((PropId)x++));
		convert(j["speed"], prc.get((PropId)x++));
		convert(j["damage"], prc.get((PropId)x++));
		convert(j["mdamage"], prc.get((PropId)x++));
		convert(j["agile"], prc.get((PropId)x++));
		convert(j["critical"], prc.get((PropId)x++));
		convert(j["attackdis"], prc.get((PropId)x++));
	}
	void load() {
		string fnm = "cfg.json";
		json jo;
		std::ifstream infile(loader::Get(Paths::configs) / fnm);
		infile >> jo;
		auto& junits = jo["units"];
		auto& rm = ResManager::Get();
	}
	std::unordered_map<string, int> _mmodeld;
	void App0::SetData(void* d) {}
	void App0::init(pj2::Imp::RenderContext& rt_) {
		float dissqp = 2;
		rct = rt_;
		mtransform::v2 facor(0,2);
		int iu=unitcsv(units.get<playerprop>());
		units.resize(iu);
		load();
		auto& Bxr_ = Renders.get<n_Render::FixRender>();
		auto& skr_ = Renders.get<n_Render::SkinRender>();
		auto& olr_ = Renders.get<n_Render::OlskRender>();
		auto& lir_ = Renders.get<n_Render::LineRender>();
		IBuffer* bhg;
		{
			using namespace Diligent;
			BufferDesc BuffDesc;
			ShaderMacroHelper sm;
			std::vector<StateTransitionDesc> decc_;
			int socap = 20;//model instance
			decc_.reserve(32);
			auto& rc = rt_;
			{
				using namespace pj2::RenderUtil;
				using ts = sStruct::inscmb;
				auto&& buf =skr_._instbuf;
				BuildBDesc<ts>(BuffDesc, socap, "Visibleout",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC,BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_WRITE);
				buf.reserve(rc, BuffDesc);
			}
			if(decc_.size()>0)
			rc.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
		}
		skr_.init(rt_, 30);
		lir_.init(rt_);
		{
			auto& rm = ResManager::Get();
			auto& rds = units.get<t_rd>();
			for (size_t i = 0; i < units.size(); i++)
			{
				auto&& [prc, plg] = units.get< playerprop, t_rd>(i);
				prc.defaulttrans.pos4.head<2>() = facor;
				plg.transform.pos4 = prc.defaulttrans.pos4;
				facor = -facor;
				_mmodeld[prc.name]=skr_.LoadModel(rt_, rm._modelpool[prc.name+".glb"]);
			}
		}
		context::main();

		//skr_.LoadModel(rt_, rm._modelpool[tb.model]);
	}

	void App0::loadcfg()
	{
	}

	//input big instance buffer with offset table to info  type corresponding position size
	//filter to result vbuffer store result with previous offset

	void App0::render() {
		Renders.get<n_Render::SkinRender>().Render();
	}

	void App0::Update() {
		using namespace Diligent;
		using namespace pj2::n_Render;
		auto& skr_ = Renders.get<n_Render::SkinRender>();
		static int c = 0;
		Diligent::MapHelper<t_rd> sout(rct.m_pImmediateContext
			, skr_._instbuf, MAP_WRITE, MAP_FLAG_DISCARD);
		auto&randinfo=units.get<t_rd>();
		memcpy(sout, randinfo.data(), randinfo.size() * sizeof(t_rd));
		for (size_t i = 0; i < units.size(); i++) {
			auto&& [prc, plg] = units.get< playerprop, t_rd>(i);
			static int redi = 100;
			if (redi == i) {
				continue;
			}
			////model id get from loadmodel,instance count,FirstInstanceLocation,
			std::array<UINT32, 4> drawarg = { (UINT32)_mmodeld[prc.name],(UINT32)1,i,(UINT32)i };
			auto& mdin = skr_._meshtable[drawarg[0]];
			auto&anim=skr_._AniReco[mdin.propadss[modeloffset::anim]];
			static int module = 120;
			plg.insd.frameIndex = anim.base+anim.bonenum * (c% module);
			plg.insd.base = anim.base;
			skr_.push(drawarg);
		}
		c++;
	}
}
