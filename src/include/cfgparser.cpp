#include"cfgparser.h"
#include"Render/Mesh.h"
#undef GetObject
//#include"syss.hpp"
//#define _ENABLE_EXTENDED_ALIGNED_STORAGE
namespace pj2 {
    auto read_file(std::filesystem::path path) -> std::string {
        constexpr auto read_size = std::size_t{ 4096 };
        auto stream = std::ifstream{ path };
        stream.exceptions(std::ios_base::badbit);

        auto out = std::string{};
        auto buf = std::string(read_size, '\0');
        while (stream.read(&buf[0], read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        out.append(buf, 0, stream.gcount());
        return out;
    }
    void secaudio(cfgout& audioout, INI::Level& pser) {
        auto& amap = audioout.audios;
        auto path = loader::Get(Paths::audios);
        for (auto& name : pser.values)
        {
            amap[name.first] = path / name.second;
        }
    }
    struct modelprop
    {

    };
    void secmodels(cfgout& audioout, INI::Level& pser, RenderContext& rt_) {
        auto& amap = audioout.models;
        auto path = loader::Get(Paths::model);
        //std::vector<std::unique_ptr<GLTF::Model>>m_Models;
        //for (const auto& entry : fs::directory_iterator(path)) {
        //    if (fs::is_regular_file(entry)) {
        //       // amap[name.first] = file;
        //        m_Models.emplace_back().reset
        //        (new GLTF::Model(rt_.m_pDevice, rt_.m_pImmediateContext, entry.path().string()));

        //        std::unique_ptr<GLTF::Model>&m_Model=m_Models.back();
        //        LOGINFO("name={0} textures={1}", entry.path().filename(), m_Model->Textures.size());
        //    }
        //}
        
        LOGINFO("start load model");
        for (auto& name : pser.values)
        {
            const auto file = path/name.second;
            auto& s = file;
            LOGINFO("[[file {0}]] first={1} sec={2}",file,name.first,name.second);
            if (fs::is_regular_file(s))LOGINFO(" is a regular file");
            if (fs::is_directory(s))LOGINFO(" is a regular file 1");
            if (fs::is_block_file(s))LOGINFO(" is a regular file2");
            if (fs::is_character_file(s))LOGINFO(" is a regular file3");
            if (fs::is_fifo(s)) LOGINFO(" is a regular file4");
            if (fs::is_socket(s))LOGINFO(" is a regular file5");
            if (fs::is_symlink(s))LOGINFO(" is a regular file6");
            if (!fs::exists(s)) LOGINFO(" file not exist");
                if (fs::is_regular_file(file)) {
                    amap[name.first] = file;

                    /*m_Models.emplace_back().reset
                    (new GLTF::Model(rt_.m_pDevice,rt_.m_pImmediateContext,file.string()));*/
            }
        }
    }
     void Cfgs::init(cfgout& cout) {
        /*auto path = loader::Get(Paths::audios);
        auto text = read_file(path);
        std::istringstream input;
        input.str(text);*/
         cout.unitout.clear();
         cout.audios.clear();
         cout.models.clear();
         cout.mp_weapons.clear();
         
        return;
        // cout.
        const auto path = loader::Get(Paths::configs);

        for (const auto& entry : fs::directory_iterator(path)) {
            if (!entry.is_regular_file())
                continue;
            using std::ifstream; using std::ostringstream;
            ifstream ifs(entry.path().c_str());
            //const std::string str = loader::readFileIntoString(entry.path().string());
            INI::Parser pser(ifs);
            //pser.dump()
            size_t i = 0;
            {
                const std::string sec = "audios";
                auto& psec = pser.top()(sec);
                secaudio(cout, psec);
            }
            {
                const std::string sec = "models";
                auto& psec = pser.top()(sec);
                secmodels(cout, psec,_rt);
            }
            {
                const std::string sec = "weapon";
                auto& psec = pser.top()(sec);
                for (auto& name : psec.sections)
                {
                    weapondesc vweapons;// = cout.vweapons;
                    //const auto index = vweapons.size();
                    //weapondesc res;
                    auto& res = vweapons;// .emplace_back();
                    res.Name = name.first;
                    auto& prop = name.second.values;
                    res.shotaudio = prop["shotaudio"];
                    vweapons.wprop.freq = std::stoi(prop["cd"]);
                    cout.mp_weapons.emplace(res.Name, vweapons);
                    //cout.weapons[res.Name] = res;
                }
            }
            {
                const std::string sec = "unit";
                auto& psec = pser.top()(sec);
                for (auto& name : psec.sections)
                {
                    auto& res = cout.unitout.emplace_back();
                    res.Name = name.first;
                    auto& prop = name.second.values;
                    res.maxcap = std::stoi(prop["maxcap"]);
                    res.weapons= prop["weapon"];
                    res.render = prop["render"];
                   // res.audio = prop["shotaudio"];
                }
            }
            auto& us = cout.unitout;
           /* std::sort(us.begin(), us.end(), [&](unitdesc& a, unitdesc& b)
                {return ri[a.render] < ri[b.render]; });
            units.clear();*/
            /* for (auto & lw :wep.sections)
            {
            auto& im = resout.emplace_back();
            im.mark = sec;
            }*/
        }
        // std::string line;

        /* while (std::getline(text, line))
        {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '='))
        {
        std::string value;
        if (std::getline(is_line, value))
        store_line(key, value);
        }
        }*/
        //std::ifstream
    }
}