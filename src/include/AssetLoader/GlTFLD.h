#pragma once

#include "MapHelper.hpp"
#include "CommonlyUsedStates.h"
#include "DataBlobImpl.hpp"
#include "Image.h"
#include "FileSystem.hpp"
#include "FileWrapper.hpp"
#include "GraphicsAccessories.hpp"
#include "TextureLoader.h"
#include<thread>
//#include"../Render/RenderUtil.h"
//import DiliGFloader;
#include "ThirdParty/GLTFLoader.hpp"
#include <cassert>
#define EXP 
namespace pj2::ModelImp {
	using namespace Diligent;
	struct  model1;
	EXP struct model1 : public Diligent::GLTF::Model
	{
		std::vector<Uint32>         IndexBuffer;
		std::vector<VertexAttribs0> VertexData0;
		std::vector<VertexAttribs1> VertexData1;
		std::vector<VertexAttribs0> tmp_VertexData0;
		std::vector<VertexAttribs1> tmp_VertexData1;
		model1(
			IRenderDevice* pDevice,
			IDeviceContext* pContext,
			const std::string& filename,
			TextureCacheType* pTextureCache = nullptr) {
			this->LoadModel(pDevice, pContext, filename, pTextureCache);
		}
		model1() {
			tmp_VertexData0.reserve(100000);
			tmp_VertexData1.reserve(100000);
		}
	/*	model1(model1&& mi) {

		}
		void operator =(model1&& mi) {

		}*/
		//void SetTexture (GLTF::Material::TEXTURE_ID TexId, ITextureView* pDefaultTexSRV, const char* VarName) //
		//{
		//	RefCntAutoPtr<ITextureView> pTexSRV;

		//	auto TexIdx = Material.TextureIds[TexId];
		//	if (TexIdx >= 0)
		//	{
		//		if (auto* pTexture = Model.GetTexture(TexIdx))
		//		{
		//			if (pTexture->GetDesc().Type == RESOURCE_DIM_TEX_2D_ARRAY)
		//				pTexSRV = pTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		//			else
		//			{
		//				TextureViewDesc SRVDesc;
		//				SRVDesc.ViewType = TEXTURE_VIEW_SHADER_RESOURCE;
		//				SRVDesc.TextureDim = RESOURCE_DIM_TEX_2D_ARRAY;
		//				pTexture->CreateView(SRVDesc, &pTexSRV);
		//			}
		//		}
		//	}

		//	if (pTexSRV == nullptr)
		//		pTexSRV = pDefaultTexSRV;

		//	if (auto* pVar = pSRB->GetVariableByName(SHADER_TYPE_PIXEL, VarName))
		//		pVar->Set(pTexSRV);
		//};
		//void LoadSkins(const tinygltf::Model& gltf_model)
		//{
		//	for (const auto& source : gltf_model.skins)
		//	{
		//		std::unique_ptr<Skin> NewSkin(new Skin{});
		//		NewSkin->Name = source.name;

		//		// Find skeleton root node
		//		if (source.skeleton > -1)
		//		{
		//			NewSkin->pSkeletonRoot = NodeFromIndex(source.skeleton);
		//		}

		//		// Find joint nodes
		//		for (int jointIndex : source.joints)
		//		{
		//			Node* node = NodeFromIndex(jointIndex);
		//			if (node)
		//			{
		//				NewSkin->Joints.push_back(NodeFromIndex(jointIndex));
		//			}
		//		}

		//		// Get inverse bind matrices from buffer
		//		if (source.inverseBindMatrices > -1)
		//		{
		//			const tinygltf::Accessor& accessor = gltf_model.accessors[source.inverseBindMatrices];
		//			const tinygltf::BufferView& bufferView = gltf_model.bufferViews[accessor.bufferView];
		//			const tinygltf::Buffer& buffer = gltf_model.buffers[bufferView.buffer];
		//			NewSkin->InverseBindMatrices.resize(accessor.count);
		//			memcpy(NewSkin->InverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(float4x4));
		//		}

		//		Skins.push_back(std::move(NewSkin));
		//	}
		//}
		//using IRenderDevice = Diligent::IRenderDevice;
		void LoadModel(
			IRenderDevice* pDevice,
			IDeviceContext* pContext,
			const std::string& filename,
			TextureCacheType* pTextureCache = nullptr);
		void CalBBox(float3& min,float3&max) {
			min = float3{ +FLT_MAX, +FLT_MAX, +FLT_MAX };
			max = float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			for (auto& node:LinearNodes)
			{
				assert(!node->_Mesh && !node->IsValidBVH);
				if (node->IsValidBVH){
					min = std::min(dimensions.min, node->BVH.Min);
					max = std::max(dimensions.max, node->BVH.Max);
				}
			}
			//std::array<std::thread, 4> th;
			//std::array<float3[2], th.size()> thbox;
			//std::vector<BoundBox> bbxs(LinearNodes.size());
			//int vseg = this->LinearNodes.size() / 4;// this->VertexData0.size() / th.size();
			//for (size_t i = 0; i < th.size(); i++)
			//{
			//	auto& t = th[i];
			//	t = std::thread([&]() {
			//		const auto& s= i * vseg;
			//		const auto& e= i<3? this->LinearNodes.size() :s+vseg;
			//		float3 min=float3{ +FLT_MAX, +FLT_MAX, +FLT_MAX };
			//		float3 max=float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			//		for (size_t i1 = s; i1 < e; i1++)
			//		{
			//			auto nod = LinearNodes[i1];
			//			if (nod->_Mesh) {
			//				auto& mesh = nod->_Mesh;
			//				auto& pri=mesh->Primitives[0];
			//				for (size_t i2 = 0; i2 < pri.IndexCount; i2++)
			//				{
			//					auto& idx = this->IndexBuffer[pri.FirstIndex + i2];
			//					auto& vt = this->VertexData0[idx];
			//					max = std::max(max, vt.pos);
			//					min = std::min(min, vt.pos);
			//				}
			//				if (!(min == mesh->BB.Min && max == mesh->BB.Max)) {
			//					i = i;
			//				}
			//				nod->BVH.Max = max;
			//				nod->BVH.Min= min;
			//				//assert(min == mesh->BB.Min && max == mesh->BB.Max);
			//			}
			//			auto& bx = bbxs[i1];
			//			bx.Max = max;
			//			bx.Min= min;
			//		}
			//		thbox[i][0] = max;
			//		thbox[i][1] = min;
			//		});
			//	t.join();
			//}
			//auto predim = this->dimensions;
			//this->GetSceneDimensions();
			//auto cdim = this->dimensions;
			//float3 min = float3{ +FLT_MAX, +FLT_MAX, +FLT_MAX };
			//float3 max = float3{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			//for (size_t i = 0; i < thbox.size(); i++)
			//{
			//	max = std::max(max, thbox[i][0]);
			//	min = std::min(min, thbox[i][0]);
			//	max = std::max(max, thbox[i][1]);
			//	min = std::min(min, thbox[i][1]);
			//}

		}
		void Build(IRenderDevice* pDevice,
			IDeviceContext* pContext) {

				{
					VERIFY_EXPR(!VertexData0.empty());
					BufferDesc VBDesc;
					VBDesc.Name = "GLTF vertex attribs 0 buffer";
					VBDesc.Size = static_cast<Uint32>(VertexData0.size() * sizeof(VertexData0[0]));
					VBDesc.BindFlags = BIND_VERTEX_BUFFER;
					VBDesc.Usage = USAGE_IMMUTABLE;

					BufferData BuffData(VertexData0.data(), VBDesc.Size);
					pDevice->CreateBuffer(VBDesc, &BuffData, &pVertexBuffer[0]);
				}

				{
					VERIFY_EXPR(!VertexData1.empty());
					BufferDesc VBDesc;
					VBDesc.Name = "GLTF vertex attribs 1 buffer";
					VBDesc.Size = static_cast<Uint32>(VertexData1.size() * sizeof(VertexData1[0]));
					VBDesc.BindFlags = BIND_VERTEX_BUFFER;
					VBDesc.Usage = USAGE_IMMUTABLE;

					BufferData BuffData(VertexData1.data(), VBDesc.Size);
					pDevice->CreateBuffer(VBDesc, &BuffData, &pVertexBuffer[1]);
				}


				if (!IndexBuffer.empty())
				{
					BufferDesc IBDesc;
					IBDesc.Name = "GLTF inde buffer";
					IBDesc.Size = static_cast<Uint32>(IndexBuffer.size() * sizeof(IndexBuffer[0]));
					IBDesc.BindFlags = BIND_INDEX_BUFFER;
					IBDesc.Usage = USAGE_IMMUTABLE;

					BufferData BuffData(IndexBuffer.data(), IBDesc.Size);
					pDevice->CreateBuffer(IBDesc, &BuffData, &pIndexBuffer);
				}

				StateTransitionDesc Barriers[3];
				new(&Barriers[0])StateTransitionDesc(pVertexBuffer[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
				new(&Barriers[1])StateTransitionDesc(pVertexBuffer[1], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
				new(&Barriers[2])StateTransitionDesc(pIndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
				pContext->TransitionResourceStates(3, Barriers);
				vfree(this->VertexData0);
				vfree(this->VertexData1);
				vfree(this->IndexBuffer);
		}
		//std::vector<modelid>models;
	private:
		template<class T>
		void vfree(std::vector<T>& v) {
			std::vector<T> temp;
			v.swap(temp);
		}
	};
}
namespace pj2::Model {
	using ModelImp::model1;
}