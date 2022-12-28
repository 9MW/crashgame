#pragma once
//#include"DiligentTools/AssetLoader/interface/GLTFLoader.hpp"
//#undef GetCurrentDirectory
#undef GetObject
#include <filesystem>
#include <iostream>
#include<string>
#include "Platforms/interface/FileSystem.hpp"
#include "FileWrapper.hpp"
#include <sstream>
#include <fstream>
#include"io/AudioManager.h"
#include"AssetLoader/GlTFLD.h"
namespace  pj2{
	//using path = Diligent::Char*;
	using namespace Diligent;
	namespace fs = std::filesystem;

	enum Paths {
		shader,
		model,
		anim,
		music,Scene,
		audios, audioclip, texture,
		configs,count
	};
	static fs::path assetroot;
	struct gModel {
		math::v4f bbox, offset;
	};
	struct ResManager
	{
		std::vector<fs::path> dirs;
		std::array<std::vector<std::string>, Paths::count> files;
		//std::unordered_map<std::string, math::ui64> namtodir;
		using Model = ModelImp::model1;

		std::unordered_map<std::string, Model>_modelpool;
		std::unordered_map<std::string, gModel>_modelpool1;
		const fs::path GetPath(const std::string name, const Paths restype);
		pj2::Audio::AudioManager am;
		~ResManager() = default;
		ResManager() = default;
		ResManager(const ResManager& rmp) = delete;
		/*ResManager(ResManager&& rmp)noexcept {
			this->dirs = rmp.dirs;
			this->files = rmp.files;
			this->_modelpool =std::move(rmp._modelpool);
		}*/
		static  ResManager& Get() {
			static ResManager srmgr;
			return srmgr;
		}
	};
	struct loader
	{
		fs::path filepath;
		loader();
		static fs::path Get(const Paths pth);
		static bool Get(const Paths pth, fs::path& curdir);
		template<Paths pth>
		static bool path(fs::path& curdir) {
			auto ps = fs::current_path().string();
			std::vector<char> p2(ps.size() + 100);
			ps.copy(&p2[0], ps.size());
			curdir=(&p2[0]);
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

				//case anim:
				//	curdir /= "anim";
				//	return true;
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
		/*static fs::path Get(const Paths pth) {
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

			case anim:
				curdir /= "anim";
				return curdir;
			default:
				break;
			}
		}*/
		static void Build(ResManager& rm,RenderContext&rt);
		bool ReadWholeFile(std::vector<unsigned char>& out,
			std::string* err,
			const std::string& filepath)
		{
			FileWrapper pFile(filepath.c_str(), EFileAccessMode::Read);
			if (!pFile)
			{
				if (err)
				{
					(*err) += FormatString("Unable to open file ", filepath, "\n");
				}
				return false;
			}

			auto size = pFile->GetSize();
			if (size == 0)
			{
				if (err)
				{
					(*err) += FormatString("File is empty: ", filepath, "\n");
				}
				return false;
			}

			out.resize(size);
			pFile->Read(out.data(), size);

			return true;
		}

		static inline std::string readFileIntoString(const std::string& path) {
			using std::ifstream; using std::ostringstream;
			ifstream input_file(path);
			if (!input_file.is_open()) {
				std::cerr << "Could not open the file - '"
					<< path << "'" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		}
	};

	inline bool FileExists(const std::string& abs_filename)
	{
		return FileSystem::FileExists(abs_filename.c_str());
	}
	struct EditorSetting
	{
		float fogtexpos[2];
		void store(void* p);
		void read(void* p);
	};
}