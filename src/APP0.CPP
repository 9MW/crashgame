#pragma once
#include "App0.h"
#include"turnsys.h"
#include"Common/ecs.hpp"
#include"json.h"
#include"UI0.h"
namespace cage {
	using json = nlohmann::json;

	warsys wsys;
	using t_rd = sStruct::inscmb;
	using t_unit = ECS::CompPackage<playerprop, t_rd>;
	t_unit units;
	UI0 uihandeler;
	using pj2::ResManager;
	using pj2::loader;
	using pj2::Paths;
	void convert(const json& j, float& f) {
		f = std::atof(j.get<string>().c_str());
	}
	context& getContext() {
		return context::main();
	}

	//load unit data from csv file
	//input output: container to store unit info parsed from file
	int unitcsv(std::vector<playerprop>& prc) {
		string fnm = "unitcfg.csv";
		std::ifstream infile(loader::Get(Paths::configs) / fnm);
		std::unordered_map<string, int> m_sk;
		for (size_t i = 0; i < getContext().skills.size(); i++)
		{
			auto& sk = getContext().skills[i];
			auto& input = sk.name;
			std::string firstWord = input.substr(0, input.find(" "));
			m_sk[firstWord] = i;
		}
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
			pi.skillnum = 0;
			size_t i = 0;
			for (; i < 8; i++)
			{
				prc.back().get((PropId)(i)) = std::atof(cfgele[i + 1].c_str());
			}
			i++;
			pi.get<PropId::attackrange>() = std::atof(cfgele[i++].c_str()) > 0 ? 0 : 1;
			if (std::atof(cfgele[i++].c_str()) > 0) //has intrinsick skill
			{
				auto skilmr = m_sk.find(pi.name);
				if (skilmr != m_sk.end())
					pi.skillequpaed[pi.skillnum++].id = skilmr->second;
			}
		}

		return n;
	}
	//unused
	void buildunitfromjson(const json& j, int i) {
		auto&& [prc, plg] = units.get< playerprop, sStruct::inscmb>(i);
		string l1 = R"(name,hp,armor,marmor,speed,damage,mdamage,agile,critical,attackdis
warrior,52,1,0,1,2,0,0,,melee
hunter,50,0,0,1.25,2,0,10,10,melee
)";
		std::istringstream input, inpele;
		input.str(l1);
		std::vector<string> cfg, cfgele;
		string fef = "";
		float gh = std::atof(fef.c_str());
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
	//expact used to load game setting
	void load() {
		string fnm = "cfg.json";
		json jo;
		std::ifstream infile(loader::Get(Paths::configs) / fnm);
		infile >> jo;
		auto& junits = jo["units"];
		auto& rm = ResManager::Get();
	}
	std::unordered_map<string, int> _mmodeld;
	//load unit data from csv file
	//input :skr_
    //output: animation info stored in skr_
	void configAnimation(n_Render::SkinRender& skr_, playerprop& prc) {
		using namespace pj2::n_Render;
		auto modelid = _mmodeld[prc.name];
		auto& mdin = skr_._meshtable[modelid];
		
		thread_local std::unordered_map<string, AnimationClip> _manim;
		_manim.clear();
		size_t i = mdin.propadss[modeloffset::anim];
		size_t iEND = (1 + modelid) == skr_._meshtable.size() ? skr_._AniReco.size() :
			((&mdin)+1)->propadss[modeloffset::anim]- i;
		auto& anim0 = skr_._AniReco[i];
		prc.renderinfo.bonenum = anim0.bonenum;
		prc.renderinfo.modelid = modelid;
		//assum parse last model's animation
		for (; i < iEND; i++)
		{
			auto& anim = skr_._AniReco[i];
			auto& aclip = _manim[anim.name];
			aclip.beg = anim.base;
			aclip.size = anim.size;
			aclip.speed = 1;
		}
		prc.anims.resize((int)agentstate::count);
		auto setanim = [&](string&& name, int i) {
			auto kv = _manim.find(name);
			if (kv != _manim.end())
				prc.anims[i] = kv->second;
			else
			{
				prc.anims[i].beg = anim0.base;
			}
		};
		setanim("attack", agentstate::attack);
		setanim("move", agentstate::move);
		setanim("idel", agentstate::idel);
	}
	//event processor called when game beginning.swap player location to 0 for player first action
	void eventbegain(void* p) {
		units.swap(0, getContext().playerid);
	}
	void App0::SetData(void* d) {}
	//config unit data based on csv file
	void configunit(App0&ap, pj2::Imp::RenderContext& rt_)
	{
		units = decltype(units)();
		_mmodeld.clear();
		auto& skr_ = ap.Renders.get<n_Render::SkinRender>();
		mtransform::v2 facor(0, 2);
		int iu = unitcsv(units.get<playerprop>());
		units.resize(iu);
		auto& rm = ResManager::Get();
		auto& rds = units.get<t_rd>();
		for (size_t i = 0; i < units.size(); i++)
		{
			auto&& [prc, plg] = units.get< playerprop, t_rd>(i);
			prc.defaulttrans.pos4.head<2>() = facor;
			plg.transform.pos4 = prc.defaulttrans.pos4;
			facor = -facor;
			_mmodeld[prc.name] = skr_.LoadModel(rt_, rm._modelpool[prc.name + ".glb"]);
			configAnimation(skr_, prc);
			prc.state.SetState(cage::agentstate::idel);
		}
	}
	void App0::init(pj2::Imp::RenderContext& rt_) {
		float dissqp = 2;
		rct = rt_;
		load();
		auto& Bxr_ = Renders.get<n_Render::FixRender>();
		auto& skr_ = Renders.get<n_Render::SkinRender>();
		auto& olr_ = Renders.get<n_Render::OlskRender>();
		auto& lir_ = Renders.get<n_Render::LineRender>();
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
				auto&& buf = skr_._instbuf;
				BuildBDesc<ts>(BuffDesc, socap, "Visibleout",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_WRITE);
				buf.reserve(rc, BuffDesc);
			}
			if (decc_.size() > 0)
				rc.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
		}
		skr_.init(rt_, 30);
		lir_.init(rt_);
		configunit(*this, rt_);
		getContext().gstate.running = false;
		wsys.ctx = &context::main();
	}

	void App0::loadcfg()
	{
	}
	//render UI and scene
	void App0::render() {
		auto& io = ImGui::GetIO();
		auto& playerprops = units.get<playerprop>();
		SetWindowCenter(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f));
		if (ImGui::BeginPopup("result", ImGuiWindowFlags_NoTitleBar & ImGuiWindowFlags_NoDecoration)) {
			ImGui::Text(playerprops[0].state == agentstate::died ? "you lost" : "you win");
			if (ImGui::Button("restart")) {
				//load unit data
				ImGui::CloseCurrentPopup();
				configunit(*this, this->rct);

			}
			ImGui::EndPopup();
			return;
		}
		if (wsys.activer < 0) {
			ImGui::OpenPopup("result");
			getContext().gstate.running = false;
			reset(wsys);
			wsys.ctx = &context::main();
		}
		uihandeler._pdata[0] = &getContext();
		uihandeler._pdata[1] = &playerprops;
		auto& skr_ = Renders.get<n_Render::SkinRender>();
		if (getContext().gstate.running)
			for (size_t i = 0; i < units.size(); i++) {
				auto&& [prc, plg] = units.get< playerprop, t_rd>(i);
				std::array<UINT32, 4> drawarg = { (UINT32)_mmodeld[prc.name],(UINT32)1,i,(UINT32)i };
				skr_.push(drawarg);
			}
		else
		{
			uihandeler.events[0]= eventbegain;
			using namespace ImGui;
			SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			uihandeler.update(0, nullptr);
		}

		Renders.get<n_Render::SkinRender>().Render();
	}

	//update turn logic
	void App0::Update() {
		using namespace Diligent;
		using namespace pj2::n_Render;
		if (!getContext().gstate.running)
		{
			return;
		}
		uihandeler.update(1, nullptr);
		{
			//write render data
			auto& skr_ = Renders.get<n_Render::SkinRender>();
			Diligent::MapHelper<t_rd> sout(rct.m_pImmediateContext
				, skr_._instbuf, MAP_WRITE, MAP_FLAG_DISCARD);
			auto& randinfo = units.get<t_rd>();
			memcpy(sout, randinfo.data(), randinfo.size() * sizeof(t_rd));
		}
		for (size_t i = 0; i < units.size(); i++) {
			auto&& [prc, plg] = units.get< playerprop, t_rd>(i);
			wsys.proppaie[i] = &prc;
			wsys.transpair[i] = &plg;
		}
		for (size_t i = 0; i < units.size(); i++)
			wsys.update(i);
	}
}
