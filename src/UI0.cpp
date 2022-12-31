#include "UI0.h"
#include"imgui.h"
#include"cagentity.h"
#include<bitset>

namespace cage {

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
	void ComboUI(const char* lable,int& select, T& items, FS&& f) {
		if (ImGui::BeginCombo(lable, f(items[select]))) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < items.size(); n++)
			{
				bool is_selected = n == select; // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(f(items[select]), is_selected))
					select = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
	}
	void DrawProp(std::string&& name, float& v) {
		ImGui::Text((name + " : " + std::to_string(v)).c_str());
	}
	std::string cob(std::string& a, std::string& b) {
		return a + b;
	}
	void SetSkill(std::vector<bskill>& skills, playerprop& p) {
		bool skillselects[64];
		auto equipskill = [&](skilldesc& sd, int archid) {
			auto& archskill = skills[archid];
			sd.id = archid;
			sd.effect = archskill.effect;
			sd.effnum = archskill.effnum;
			sd.probability = archskill.probability;
			sd.weight = archskill.weight;
			sd.trigger = archskill.trigger;
		};
		std::memset(skillselects, 0, sizeof(skillselects));
		for (size_t i = 0; i < p.skillnum; i++)
		{
			skillselects[p.skillequpaed[i].id] = 1;
		}
		std::bitset<64> bs;
		static int sele = 0;
		auto& weight = p.maxskillweight;
		int skilnumrem = p.maxskill - p.skillnum;
		ImGui::Text(("weight remain : " + std::to_string(weight)).c_str());
		ImGui::Text(("skill remain : " + std::to_string(skilnumrem)).c_str());
		for (size_t i = 0; i < skills.size(); i++)
		{
			bool disenable = skilnumrem > 0 && (weight - skills[i].weight) < 0 && !skillselects[i];
			if (disenable)
				ImGui::BeginDisabled();
			if (ImGui::Checkbox(("skill : " + skills[i].name).c_str(), skillselects + i)) {
				//weight += (skillselects[i] ? 1 : -1)*skills[i].weight;
				if (skillselects[i]) {
					weight -= skills[i].weight;
					p.skillequpaed[p.skillnum++].id = i;
				}
				else
				{
					weight += skills[i].weight;
					p.skillequpaed[p.skillnum++].id = i;
					bool findskill = 0;
					for (size_t isk = 0; isk < p.skillnum; isk++)
					{
						if (p.skillequpaed[isk].id == i) {
							p.skillequpaed[isk].id = p.skillequpaed[--p.skillnum].id;
							findskill = 1;
							break;
						}
					}
					assert(findskill);
				}
			}
			if (disenable)
				ImGui::EndDisabled();
			sele += skillselects[i];
		}
	}
	void UI(context& ct, std::span<playerprop> ps) {
		bool op = 0;
		static int sle = 0;
		ImGui::Begin("player select", &op, ImGuiWindowFlags_NoDecoration);
		ComboUI("unit type",sle, ps, [](playerprop& prop) {return prop.name.c_str(); });
		SetSkill(ct.skills, ps[sle]);
		if (ImGui::Button("start game")) {

		}
		ImGui::End();
	}
	void UI(playerprop& p) {
		bool op = 0;
		ImGui::Begin("prop", &op, ImGuiWindowFlags_NoDecoration);
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

		ImGui::End();
	}
	template<class ...T>
	void Draw(void* p...) {
		UI(*reinterpret_cast<T*>(p)...);
	}
	void UI0::init(void*)
	{
	}
	void UI0::update(int i, void* p)
	{
		switch (i)
		{
		case 0:
			Draw<cage::playerprop>(p);
			break;
		case 1:
			Draw<cage::playerprop>(p);
			break;
		case 2:
			Draw<cage::playerprop>(p);
			break;
		case 3:
			Draw<cage::playerprop>(p);
			break;
		default:
			break;
		}
	}

}