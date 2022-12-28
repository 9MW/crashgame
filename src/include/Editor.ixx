
#pragma once
#include<imgui.h>
#include<imgui_internal.h>
#include"Gameprops0.h"
#include"Common/ecs.hpp"
#include"AllType.h"
#include "editor.h"
//import RenderSet;
export module Editor;
namespace pj2 {
	export class Editor {
		int item_current = 0;
		const std::string poppf = "popupbpref";
		const std::string popmod = "model", winSceneitems = "Sceneitem";
		static thread_local inline int node_clicked = -1, insid = -1;
	public:
		uicallback callbacks;
		void* p_cont = NULL;
		void init() {
			auto& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigDockingAlwaysTabBar = true;
		}
		void update() {
			 
			//ImGui::ShowDemoWindow();
			using rtpkg = n_Registry::Registry::t_Pkgs;
			auto& objs = *((rtpkg*)p_cont);

			auto viewport = ImGui::GetMainViewport();
			constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
			float height = ImGui::GetFrameHeight();

			static thread_local bool opened = false, hov = false;
			thread_local ImVec2 poppos;
			if (ImGui::BeginViewportSideBar("##SecondaryMenuBar", viewport, ImGuiDir_Up, height, window_flags)) {
				if (ImGui::BeginMenuBar()) {
					if (ImGui::BeginMenu("IO")) {

						if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
						if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
						if (ImGui::MenuItem("Close", "Ctrl+W")) {}
						ImGui::EndMenu();
					}
					ImGui::Text("Happy secondary menu bar");
					ImGui::EndMenuBar();
				}

				if (ImGui::BeginMenuBar()) {

					ImGui::Text(poppf.c_str());
					hov = ImGui::IsItemHovered();
					if (hov && !opened)
					{
						//ImGui::OpenPopup(poppf.c_str());
						opened = true;
						new (&poppos)ImVec2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMin().y);

					}
					ImGui::EndMenuBar();
				}
				ImGui::End();
			}

			//if (opened)
				//ImGui::OpenPopup(poppf.c_str());
			if (opened) {//ImGui::BeginPopup(poppf.c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::SetNextWindowPos(poppos);
				ImGui::Begin("popup prefab", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

				ECS::visit_all_convertible<n_Registry::Registry, n_BasObj::Ubase>([&](auto I) {
					if (ImGui::Button(typeid(n_Registry::Registry::t<I>).name())) {
						auto& vecunit = objs.get<I>();
						vecunit.emplace_back().prefabs.emplace_back();
						opened = false;
					}
					});
				//ImGui::EndMenu();
				/*if (!ImGui::IsWindowHovered() &&!hov) {
					opened = false;
					ImGui::CloseCurrentPopup();
				}*/
				ImGui::End();
			}
			thread_local int node_clicked = -1, insid = -1;

			ImGuiID dockspace_id = ImGui::GetID("App0::Update");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
			ImGui::Begin("Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			if (node_clicked >= 00 && insid >= 0)
			{
				ECS::visit_convertible<n_Registry::Registry, n_BasObj::Ubase>(node_clicked >> 16, [&](auto I) {
					auto& vecunit = objs.get<I>();
					auto& pref = vecunit[0xFFFF & node_clicked].prefabs[insid];
					Drawunit(pref, callbacks);
					});
				//ImGui::ListBox(fmt::format("select : {0}",item_current).c_str(), &item_current, behavors);
			}
			ImGui::End();

			ImGui::SetNextWindowSize(ImVec2(550, 680));
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::Begin(winSceneitems.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize);
			constexpr ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			if (ImGui::TreeNode("units"))
			{
				thread_local int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
				int i0 = 0, i = 0;
				ECS::visit_all_convertible<n_Registry::Registry, n_BasObj::Ubase>([&](auto I) {
					auto& vecunit = objs.get<I>();
					if (vecunit.size() > 0) {
						for (auto& ug : vecunit) {

							for (auto& pref : ug.prefabs)
							{
								//Drawunit(pref, callbacks);
								ImGuiTreeNodeFlags node_flags = base_flags;
								//const bool is_selected = (selection_mask & (1 << i)) != 0;
								const int cid = I << 16 | i0;
								if (node_clicked == cid && insid == i0)
									node_flags |= ImGuiTreeNodeFlags_Selected;
								node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
								bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "%d Node %d-%d", typeid(n_Registry::Registry::t<I>).name(), i0, i);
								if (ImGui::IsItemClicked()) {
									node_clicked = cid;
									insid = i;
								}
								/*	if (node_open)
									{
										ImGui::BulletText(fmt::format("I:{0} i:{1}", I, i).c_str());
										ImGui::TreePop();
									}*/
								i++;
							}
							i0++;
						}
					}
					});
				ImGui::TreePop();
				//ImGui::ListBox(fmt::format("select : {0}",item_current).c_str(), &item_current, behavors);
			}
			ImGui::End();
		}
		void updateUI() {
			//unit panel
			{

			}
		}
	};
}