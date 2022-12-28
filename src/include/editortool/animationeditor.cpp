#include "animationeditor.h"
#include"Render/LineRender.h"
#include "ImGui.h"

namespace pj2 {
	namespace Editor {

		ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5);
		ImTextureID rtargetid;
		//RefCntAutoPtr<ITexture> Tex2d;
		std::vector<math::int2> animaddres;
		int animtotalsize = 0,modid;
		n_Render::LineRender __ird;
		n_Render::FixRender Bxr_;
		mtransform trs;
		sStruct::instData1 ins;
		sStruct::inscmb icb;
		void Anim::init(void*& pimage) {
			return;
			using namespace n_Render;
			auto prt = (RenderContext*)pimage;

			BufferDesc VertBuffDesc;
			VertBuffDesc.Name = "transform buffer";
			VertBuffDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
			VertBuffDesc.BindFlags = BIND_SHADER_RESOURCE;
			VertBuffDesc.Size = sizeof(mtransform);

			{
				VertBuffDesc.Name = "inst buffer";
				VertBuffDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
				VertBuffDesc.BindFlags = BIND_SHADER_RESOURCE;
				VertBuffDesc.Size = sizeof(sStruct::instData1);
				BufferData VBData;
				VBData.pData = &trs;
				VBData.DataSize = sizeof(sStruct::instData1);
				Bxr_._transbuf.reserve(*prt, VertBuffDesc);
			}
			Bxr_._transbuf.updateat(&trs, *prt, 0);
			Bxr_._instbuf.updateat(&ins, *prt, 0);
			//TextureDesc TexDesc;
			//TexDesc.Type = RESOURCE_DIM_TEX_2D;
			//TexDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
			////TexDesc.Usage = Diligent::USAGE::USAGE_DYNAMIC;
			//TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;
			//TexDesc.Width = 480;
			//TexDesc.Height = 480;
			//TexDesc.Format = TEX_FORMAT_RGBA8_UNORM;
			//TexDesc.MipLevels = 1;
			////TextureDesc TexDesc(RESOURCE_DIM_TEX_2D, shadowmapsz, shadowmapsz, 1, TEX_FORMAT_RGBA32_UINT, USAGE_DEFAULT, BIND_SHADER_RESOURCE);
			//TexDesc.Name = "anim_render_tex";
			//_rt.m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
			//rtargetid = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
			icb.transform.reset(math::mtransform::v3(0,0,0));
			icb.insd.frameIndex = 0;
			auto& back = _Skr->_AniReco.back();
			animaddres.resize(_Skr->_meshtable.size());
			for (size_t i = 0; i < animaddres.size(); i++)
			{
				auto& mtb = _Skr->_meshtable[i];
				auto& amat = animaddres[i];
				ui64 boneoffset = mtb.propadss[modeloffset::bone];
				ui64 boneoffsetnex =i+1<animaddres.size()?
					_Skr->_meshtable[i+1].propadss[modeloffset::bone]:
					_Skr->PropsSum()[modeloffset::bone];
				amat[0] = boneoffset;
				amat[1] = boneoffsetnex-boneoffset;
			}
			animtotalsize = back.size + back.base;

			auto& unitCube = geomatrys::GetColl().uCube0;
			Bxr_.init(*prt, 1, 8);
			Bxr_.Ldgomatry(unitCube.m_CubeVertexBuffer, unitCube.m_CubeIndexBuffer, 36);

		}
		n_Render:: SkinRender::srDrawArg DAG[1];
		int frm = 0;
		float animperc = 0;
		int Anim::Update(AnimContext& ac) {
			size_t i = 0, iani = 0,dc=0;
			modid = ac.modid;
			for (; i < _Skr->_models.size(); i++)
			{
				if (ImGui::TreeNodeEx(ac.modelNames[i].c_str())) {
					auto& mod = *_Skr->_models[i];
					for (size_t i1 = 0; i1 < mod.Animations.size(); i1++)
					{
						if (ImGui::TreeNodeEx(mod.Animations[i1].Name.c_str()))
						{
							modid = i;
							auto&rec = _Skr->_AniReco[iani];

							if (ImGui::Button("reset")) {
								icb.insd.base = rec.base;
							}
							if (ImGui::Button("apply")) {
								ac.size = rec.size/ rec.bonenum;
								icb.insd.base = rec.base;
								dc++;
							}
							ac.rep = &icb.insd;
							ImGui::SliderFloat("frame percent", &animperc, 0,1);
							(ImGui::SliderInt("beg", (int*)&rec.base, 0, ac.size));
							ImGui::InputInt("size", (int*) &rec.size,0, animaddres [modid][1]);
							ImGui::Text("frame : %d frame num : %d",icb.insd.frameIndex, rec.size / rec.bonenum);
							icb.insd.frameIndex = icb.insd.base + (int)(rec.size / rec.bonenum * animperc) * rec.bonenum;

							ImGui::TreePop();
							if (dc > 0){
								ImGui::TreePop();
								return iani;
							}
							break;
						}
						iani++;
					}
					ImGui::TreePop();
				}
			}
			DAG[0] = { 1,0,modid,0 };
			n_Render::SkinRender::srDrawArg drgcub[1];
			auto& dg8 = drgcub[0];
			dg8.NumInstances = 1;
			dg8.FirstInstanceLocation = 0;// ccull.BaseInst(g1.tid);
			dg8.modiid = 0;
			Bxr_.Render(drgcub, 36);
			return -1;
		}
		int Anim::Render() {
			return 0;
			_rt.m_pImmediateContext->UpdateBuffer(_Skr->_instbuf, 0, sizeof(icb), &icb, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

			_Skr->Render(DAG);
		}
	}
}