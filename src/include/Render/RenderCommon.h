#pragma once
#include"RenderUtil.h"
#include"AssetLoader/RenderStr0.h"
#define PREF(t) Diligent::RefCntAutoPtr<t>
namespace pj2::n_Render {
	struct rdbatch
	{
		DrawIndexedAttribs DRG;
		std::array<Diligent::IBuffer*, 4> Bufs;
		std::array<RefCntAutoPtr<ITexture>, 4> Texs;
		
		PREF(Diligent::IShaderResourceBinding) SRB;
	};
	struct BatchRenderb :Render {
		void push(){}
		int getmodelid(std::string s) { return -1; }
	};
	
	struct FilterVB {
		enum MyEnum
		{
			transvb,insvb,count
		};
		using t_Ss = std::tuple< math::mtransform, sStruct::instData1>;
		using t_VB = dyvbf;// RefCntAutoPtr<IBuffer>;
		std::array<t_VB, 2> _VB;
		void init(RenderContext& rc,int mxinstancenum) {
			std::array<StateTransitionDesc, count> decc_;

			BufferDesc BuffDesc;
			{
				constexpr int IB = transvb;
				using ts = std::tuple_element_t<IB,t_Ss>;
				auto& buf = _VB[IB];
				RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "buffer mtransfrom",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
				buf.reserve(rc, BuffDesc);
				//decc_[IB] = { buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE };

			}
			{
				constexpr int IB = insvb;
				using ts = std::tuple_element_t<IB, t_Ss>;
				auto& buf = _VB[IB];
				RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "buffer ",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
				buf.reserve(rc, BuffDesc);
				//decc_[IB] = { buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE };

			}
		}
		template<MyEnum E>
		t_VB Get() { return _VB[E]; }
	};
	struct RenderAttr {
		RefCntAutoPtr<IPipelineState> _PSO;
		RefCntAutoPtr<IBuffer>                m_VertexBuffer[2];
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;
		std::array<RefCntAutoPtr<IBuffer>, 8>                _Buffers;
		std::array<RefCntAutoPtr<ITexture>, 8>                _Textures;
		std::vector<rdbatch> prims;
		std::vector<modelidn> modins;
	}; 
	struct drawunit {
		modelidn modins;
		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		RefCntAutoPtr<IBuffer> instancebuf;
		RefCntAutoPtr<IBuffer> m_pTransAttribsBuffer;
		RefCntAutoPtr<ITexture> g_ColorMap;
		DrawIndexedAttribs DRG;

	};
	enum class ENUMPSO :int {
		Cull,Skin,Station
	};
}