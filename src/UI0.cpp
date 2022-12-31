#include "UI0.h"
#include"imgui.h"
#include<bitset>
#include<Common/FirstPersonCamera.hpp>
namespace cage {
	using pj2::Camera;
	void DrawProp(std::string&& name, float& v) {
		ImGui::Text((name + " : " + std::to_string(v)).c_str());
	}
	auto mul = [](auto& a, auto& b) {
		return a * b;
	};
	constexpr auto covertcoord = [](auto& a, auto& outv, const float w, const float h) {
		a.head<3>() *= 1 / a[3];
		//outv[0] = 2 * (a[0] / w) - 1;
		outv[0] = (a[0] + 1) * 0.5 * w;
		//outv[1] = 1 - 2 * (a[1] / h);
		outv[1] = (a[1] - 1) * -0.5 * h;//due to pretansform setting inversed y coordinate

	};
	auto equipskill = [](std::span<bskill> skills, playerprop& p) {
		for (size_t i = 0; i < p.skillnum; i++)
		{
			auto& sd = p.skillequpaed[i];
			auto& archskill = skills[p.skillequpaed[i].id];
			sd.effect = archskill.effect;
			sd.effnum = archskill.effnum;
			sd.probability = archskill.probability;
			sd.weight = archskill.weight;
			sd.trigger = archskill.trigger;
		}
	};
	void UI(playerprop& p) {
		bool op = 0;
		//ImGui::Begin("prop", &op, ImGuiWindowFlags_NoDecoration);
		int x = 0;
		ImGui::Text(("name : " + p.name).c_str());
		DrawProp("hp", p.get((PropId)x++));
		DrawProp("armor", p.get((PropId)x++));
		DrawProp("marmor", p.get((PropId)x++));
		DrawProp("speed", p.get((PropId)x++));
		DrawProp("damage", p.get((PropId)x++));
		DrawProp("mdamage", p.get((PropId)x++));
		DrawProp("agile", p.get((PropId)x++));
		DrawProp("critical", p.get((PropId)x++));
		DrawProp("attackdis", p.get((PropId)x++));

		//ImGui::End();
	}
	void ComboUI(int& select, std::span<const char*> items) {
		if (ImGui::BeginCombo("##AnimationClip", items[select])) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < items.size(); n++)
			{
				bool is_selected = n == select; // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected))
					select = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
	}

	template<class T, class FS>
	void ComboUI(const char* lable, int& select, T& items, FS&& f) {
		if (ImGui::BeginCombo(lable, f(items[select]))) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < items.size(); n++)
			{
				bool is_selected = n == select; // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(f(items[n]), is_selected))
					select = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
	}
	void TextCentered(std::string text) {
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::Text(text.c_str());
	}
	std::string cob(std::string& a, std::string& b) {
		return a + b;
	}
	void configAIplayer(playerprop& p, std::vector<bskill>& skills) {
		int intrinsicsk = p.skillequpaed[0].id;
		auto& weight = p.maxskillweight;
		for (size_t i = 0; i < p.maxskill; i++)
		{
			int skidr = std::rand();
			if (skidr == intrinsicsk)
				continue;
			auto& skrand = skills[skidr % skills.size()];
			int weightrem = weight - skrand.weight;
			if (weightrem >= 0) {
				p.skillequpaed[p.skillnum++].id = skidr;
			}
			else if (weightrem == 0)
			{
				break;
			}
		}
		equipskill(skills, p);
	}
	void SetSkill(std::vector<bskill>& skills, playerprop& p) {
		std::array<bool, 64> skillselects;
		std::memset(&skillselects, 0, sizeof(skillselects));
		for (size_t i = 1; i < p.skillnum; i++)
		{
			skillselects[p.skillequpaed[i].id] = 1;
		}

		auto& weight = p.maxskillweight;
		int skilnumrem = p.maxskill - p.skillnum;
		ImGui::Text(("weight remain : " + std::to_string(weight)).c_str());
		ImGui::Text(("skill remain : " + std::to_string(skilnumrem)).c_str());
		int intrinsicsk = p.skillequpaed[0].id, skchanged = 0;
		skillselects[intrinsicsk] = 1;
		for (size_t i = 0; i < skills.size(); i++)
		{
			auto& skill = skills[i];
			bool disenable = intrinsicsk == i || (skilnumrem > 0 && (weight - skills[i].weight) < 0 && !skillselects[i]);

			if (disenable)
				ImGui::BeginDisabled();
			if (ImGui::Checkbox(("skill : " + skills[i].name).c_str(), skillselects.data() + i)) {
				skchanged = 1;
				//weight += (skillselects[i] ? 1 : -1)*skills[i].weight;
				if (skillselects[i]) {
					weight -= skills[i].weight;
					auto& eqskill = p.skillequpaed[p.skillnum++];
					eqskill.id = i;
				}
				else
				{
					weight += skills[i].weight;
					bool findskill = 0;
					for (size_t isk = 0; isk < p.skillnum; isk++)
					{
						auto& eqskill = p.skillequpaed[isk];
						if (eqskill.id == i) {
							p.skillequpaed[isk] = p.skillequpaed[--p.skillnum];
							p.skillequpaed[p.skillnum].id = -1;
							findskill = 1;
							break;
						}
					}
					assert(findskill);
				}
			}
			if (disenable)
				ImGui::EndDisabled();
		}
	}
	float uif = 3;
	void DrawProps(playerprop& p, Camera& cam) {
		bool op = 0;
		mtransform& trans = p.defaulttrans;
		mtransform::v4 pos;

		const float& w = cam.m_ProjAttribs.Width, & h = cam.m_ProjAttribs.Height;
		std::memcpy(&pos, &trans.pos4, sizeof(pos));
		pos.w() = 1;
		pos.z() += 3;//top of player
		pos.head<2>() *= uif;//left or right side
		pos = mul(cam.CameraViewProj, pos);
		ImVec2 screenpos;
		covertcoord(pos, screenpos, w, h);
		SetWindowCenter(screenpos);
		ImGui::Begin((p.name + "props:" + std::to_string(p.state.st)).c_str(), &op, ImGuiWindowFlags_NoDecoration);
		UI(p);
		ImGui::End();
	}
	int sle = 0;
	template<class F>
	void UISele(context& ct, std::vector<playerprop>& ps, F begfunc) {
		bool op = 0;
		ImGui::Begin("player select", &op, ImGuiWindowFlags_NoDecoration);
		ComboUI("unit type", sle, ps, [](playerprop& prop) {return prop.name.c_str(); });
		SetSkill(ct.skills, ps[sle]);
		UI(ps[sle]);
		if (ImGui::Button("start game")) {
			if (ps[sle].skillnum < ct.minequpskill) {
				(ImGui::OpenPopup("sneed"));
			}
			else
			{
				ct.playerid = sle;
				for (auto& pp : ps)
				{
					equipskill(ct.skills, pp);
				}
				begfunc();
				ct.gstate.running = 1;
			}
		}
		ImGui::End();
		if (ImGui::BeginPopup("sneed", ImGuiWindowFlags_NoTitleBar & ImGuiWindowFlags_NoDecoration)) {
			TextCentered("need select more skill");
			if (ImGui::Button("yes")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	void UI(context& txreal, std::span<playerprop> playersprops, Camera& cam) {
		auto& texs = txreal.textppool;
		mtransform::v4 pos;
		ImVec2 lastpos(-1,0);
		bool op = 0;
		ImGui::Begin("ui cfg");
		ImGui::DragFloat("sepf", &uif);
		ImGui::End();
		for (size_t i = 0; i < playersprops.size(); i++)
		{
			DrawProps(playersprops[i], cam);
		}
		if (texs.size() == 0)
			return;
		auto& [meg, ids] = txreal.textppool[0];
		const float& w = cam.m_ProjAttribs.Width, & h = cam.m_ProjAttribs.Height;
		auto& dynamictext = txreal.textppool;

		auto hight = ImGui::GetItemRectSize().y;
		//for (int64_t i = dynamictext.size()-1; i >0 ; i--)
		//{
		//	auto& [mesg, ida] = txreal.textppool[i];
		//	auto& [meg, idb] = txreal.textppool[i-1];
		//	auto dis =math::square(ida[0] - idb[0])*10000+ (ida[1] - idb[1]);
		//	//elevate if overlap
		//	if (dis ==0) {
		//		ida[2] = hight;
		//	}
		//}

		for (int i = txreal.textppool.size() - 1; i >= 0; i--)
		{
			auto& [msg, ids] = txreal.textppool[i];
			auto& meg = msg;
			auto& trans = playersprops[ids[0]].defaulttrans;
			std::memcpy(&pos, &trans.pos, sizeof(pos));
			pos.w() = 1;
			pos.z() += 3;//top of player
			pos = mul(cam.CameraViewProj, pos);
			ImVec2 screenpos;
			covertcoord(pos, screenpos, w, h);
			////equal return 0
			//if (!std::memcmp(&lastpos ,&screenpos,sizeof(lastpos))) {
			//	
			//}
			float arpha = ids[2] / (hight * 8);
			if (arpha >= 1) {
				txreal.textppool.erase(txreal.textppool.begin());
				break;
			}
			screenpos.y -= ids[2];
			SetWindowCenter(screenpos);
			ImGui::Begin("dmg", &op, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
			ImGui::TextColored(ImVec4(1, 0, 0, 1 - arpha), meg.c_str());
			ids[2]++;
			ImGui::End();
		}
	}
	template<class ...T>
	void Draw(void* p) {
		UI(*reinterpret_cast<T*>(p)...);
	}
	template<class ...T>
	void Draw(void** p) {
		int i = sizeof...(T);
		UI((*static_cast<T*>(p[--i]))...);
	}
	template<class ...T>
	void Draw(std::initializer_list<void*>p) {
		int i = sizeof...(T);
		UI((*static_cast<T*>(p[--i]))...);
	}
	void UI0::init(std::any a)
	{
	}
	void UI0::update(int i, std::any p)
	{
		auto& ctx = *(context*)_pdata[0];
		if (ctx.gstate.running) {
			_pdata[2] = &Camera::Main();
		}
		auto vprop = static_cast<std::vector<cage::playerprop>*>(_pdata[1]);
		void** px = _pdata.data();
		auto startgame = [&]() {
			events[0](this);
		};
		switch (i)
		{
		case 0:
			UISele(ctx,*vprop,startgame);
			break;
		case 1:
			Draw<context, std::vector<cage::playerprop>, Camera>(px);//draw player property
			break;

		default:
			break;
		}
	}

}