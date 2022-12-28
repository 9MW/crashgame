#include"loader.h"
#include"json.h"
#include <ecs.hpp>
#include<taskflow/taskflow.hpp>
#undef GetObject

//#include"cista/cista.h"
#define _ENABLE_EXTENDED_ALIGNED_STORAGE
namespace pj2 {

	namespace fs = std::filesystem;
	inline std::string trim(const std::string& s)
	{
		char p[] = " \t\r\n";
		long sp = 0;
		long ep = s.length() - 1;
		for (; sp <= ep; ++sp)
			if (!strchr(p, s[sp])) break;
		for (; ep >= 0; --ep)
			if (!strchr(p, s[ep])) break;
		return s.substr(sp, ep - sp + 1);
	}

	inline
		std::string value(const std::string& s)
	{
		std::string c = "#;";
		std::string v = s;
		size_t ep = std::string::npos;
		for (size_t i = 0; i < c.size(); i++)
		{
			ep = s.find(c[i]);
			if (ep != std::string::npos)
				break;
		}
		if (ep != std::string::npos)
			v = s.substr(0, ep);
		return v;
	}
	loader::loader() {
		auto ps = fs::current_path().string();
		std::vector<char> p2(ps.size() + 100);
		ps.copy(&p2[0], ps.size());
		fs::path curdir(&p2[0]);
		curdir /= "assets";
		assetroot = curdir;
		//curdir /= "Soldier.glb";
		curdir /= "soldiercliped.glb";
		filepath = curdir;

		/*DBOUT(filepath.string().c_str());
		std::string err;
		std::vector<unsigned char> out;
		ReadWholeFile(out, &err, filepath.string());*/
		//bf.SetWorkingDirectory(curdir.c_str());
	}

	fs::path loader::Get(const Paths pth) {
		auto ps = fs::current_path().string();
		std::vector<char> p2(ps.size() + 100);
		ps.copy(&p2[0], ps.size());
		fs::path curdir(&p2[0]);
		curdir /= "assets";

		switch (pth)
		{
		case shader:
			curdir.remove_filename();
			curdir /= "src";
			curdir /= "shaders";
			return curdir;

		case model:
			curdir /= "model";
			return curdir;
		case Scene:
			curdir /= "Scene";
			fs::create_directory(curdir);
			return curdir;
			//case anim:
			//	curdir /= "anim";
			//	return curdir;
		case music:
			curdir /= "music";
			return curdir;
		case audios:
			curdir /= "audios";
			return curdir;
		case audioclip:
			curdir /= "audios";
			return curdir;
		case configs:
			curdir /= "configs";
			return curdir;

		default:
			break;
		}
	}

	inline bool loader::Get(const Paths pth, fs::path& curdir) {

		auto ps = fs::current_path().string();
		std::vector<char> p2(ps.size() + 100);
		ps.copy(&p2[0], ps.size());
		curdir = (&p2[0]);
		curdir /= "assets";

		switch (pth)
		{
		case shader:
			curdir.remove_filename();
			curdir /= "src";
			curdir /= "shaders";
			return true;

		case model:
			curdir /= "model";
			return true;

		case Scene:
			curdir /= "Scene";
			return true;
		case music:
			curdir /= "music";
			return true;
		case audios:
			curdir /= "audios";
			return true;
		case audioclip:
			curdir /= "audios";
			return true;
		case configs:
			curdir /= "configs";
			return true;
		default:
			return false;
			break;
		}
	}

	template<class F>
	void LoopDir(fs::path& path, F&& f) {

		for (const auto& entry : fs::directory_iterator(path)) {
			if (!entry.is_regular_file())
				continue;
			f(entry);
		}
	}
	void loader::Build(ResManager& rm, RenderContext& rt) {
		using Model = ModelImp::model1;
		ResManager jhjk;
		rm.~ResManager();
		/*if (&rm) {
			rm.~ResManager();
		}
		else {
			rm = *(ResManager*)malloc(sizeof(ResManager));
		}*/
		new (&rm)ResManager();
		rm.files[(int)Paths::audioclip].push_back("");
		std::unordered_map<std::string, Model>&_modelpool=rm._modelpool;
		static tf::Executor exc;
		ECS::visit<Paths::count>([&](auto I) {
			fs::path pth;
			constexpr Paths p = (Paths)I.value;
			if (loader::Get(p,pth)) {
				std::vector<std::string>& files = rm.files[I];
				if (!fs::exists(pth)) {
					fs::create_directories(pth);
				}
				LoopDir(pth, [&](const auto& entry) {
					const auto& name = entry.path().filename().string();
					files.push_back(name);
					if constexpr ((Paths)I.value == Paths::model)
					_modelpool[name]= Model();
					//rm.namtodir.try_emplace(name, entry.path());
					});

				if constexpr ((Paths)I.value == Paths::model) {
					LoopDir(pth, [&](const auto& entry) {
						const auto& name = entry.path().filename().string();
						Model& modl = _modelpool[name];
						exc.async([&, spth = entry.path().string()]() {
						new(&modl)Model(rt.m_pDevice, rt.m_pImmediateContext, spth);
							});
						});
				}
			}
			});
		exc.wait_for_all();
	}

	void exstr(const std::string& str, std::vector<std::string>& outstrs) {
		constexpr char sep = '|';
		size_t n = 0;
		while (true)
		{
			//n = n > 0 ? n+1:0;
			const auto n2 = str.find(sep, n);
			outstrs.emplace_back(str.substr(n, n2 - n));
			if (n2 == std::string::npos) {
				break;
			}
			n = n2 + 1;
		}

	}
	using resource = Paths;
	void loadmodel(ResManager& rm, fs::path path) {
		rm.dirs.emplace_back(path);
		/*LoopDir(path, [&](const auto& entry) {
			const auto& name = entry.path().filename().string();
			rm.namtodir.emplace(name, rm.dirs.size() - 1);
			});*/
	}
	const fs::path ResManager::GetPath(const std::string name, const Paths restype) {
		return loader::Get(restype) / name;
	}
	using json = nlohmann::json;
	inline void EditorSetting::store(void* p) {
		//json js = json{ {"fogtexture window prop"},{"pos",{}} }
	}

	inline void EditorSetting::read(void* p) {
	
	}

}