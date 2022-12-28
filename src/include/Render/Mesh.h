#pragma once
#include"PCH.h"
//#include<unordered_map>
//#include <locale>
//#include <codecvt>
//#include"Common/Math1.h"
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include"assimp/Importer.hpp"
//#include <assimp/pbrmaterial.h>
#include"Common/span.hpp"
#include"Image.h"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include"BasicMath.hpp"
#include "FileSystem.hpp"
#include "FileWrapper.hpp"
#include <DataBlobImpl.hpp>
#include"AssetLoader/ThirdParty/GLTFLoader.hpp"
//namespace {
//	using namespace Assimp;
//	template<typename T, typename F>
//	using map = std::unordered_map<T, F>;
//	using namespace std;
//	using namespace Diligent;
//	using Matrix4f = math::Matrix4f;
//
//	struct MydataBlob : public Diligent::ObjectBase<IDataBlob>
//	{
//		typedef ObjectBase<IDataBlob> TBase;
//		MydataBlob(IReferenceCounters* pRefCounters, void* p, uint sz) :m_DataBuff(p), size(sz) ,TBase(pRefCounters){}
//
//		virtual void DILIGENT_CALL_TYPE QueryInterface(const INTERFACE_ID& IID, IObject** ppInterface) override { std::abort(); }
//
//		/// Sets the size of the internal data buffer
//		virtual void DILIGENT_CALL_TYPE Resize(size_t NewSize) override{std::abort();}
//
//		/// Returns the size of the internal data buffer
//		virtual size_t DILIGENT_CALL_TYPE GetSize() const override { return size; };
//
//		/// Returns the pointer to the internal data buffer
//		virtual void* DILIGENT_CALL_TYPE GetDataPtr() override { return m_DataBuff; }
//
//		/// Returns const pointer to the internal data buffer
//		virtual const void* DILIGENT_CALL_TYPE GetConstDataPtr() const override { return m_DataBuff; };
//
//		void* m_DataBuff;
//		uint size;
//	}; 
//	struct material
//	{
//		static constexpr size_t MaxMaterialName = 100;
//		char                    Name[MaxMaterialName];
//
//		// Use MaterialInstancePath
//		static constexpr size_t MaxMaterialPath = 260;
//		char                    MaterialInstancePath[MaxMaterialPath];
//
//		// Or fall back to d3d8-type materials
//		static constexpr size_t MaxTextureName = 260;
//		char                    DiffuseTexture[MaxTextureName];
//		char                    NormalTexture[MaxTextureName];
//		char                    SpecularTexture[MaxTextureName];
//
//		Diligent::float4 Diffuse;
//		Diligent::float4 Ambient;
//		Diligent::float4 Specular;
//		Diligent::float4 Emissive;
//		float  Power;
//
//		union
//		{
//			math::ui64    Force64_1; //Force the union to 64bits
//			ITexture* pDiffuseTexture = nullptr;
//		};
//		union
//		{
//			math::ui64    Force64_2; //Force the union to 64bits
//			ITexture* pNormalTexture = nullptr;
//		};
//		union
//		{
//			math::ui64    Force64_3; //Force the union to 64bits
//			ITexture* pSpecularTexture = nullptr;
//		};
//
//		union
//		{
//			math::ui64        Force64_4; //Force the union to 64bits
//			ITextureView* pDiffuseRV = nullptr;
//		};
//		union
//		{
//			math::ui64        Force64_5; //Force the union to 64bits
//			ITextureView* pNormalRV = nullptr;
//		};
//		union
//		{
//			math::ui64        Force64_6; //Force the union to 64bits
//			ITextureView* pSpecularRV = nullptr;
//		};
//	};
//	struct Vertex
//	{
//		math::float4 mPosition;
//		math::float4 mNormal;
//		math::float4 mUV;
//		math::uint4  mBoneIndices;
//		math::float4 mBoneWeights;
//	};
//
//	struct animData { int start, length; };
//	 math::Ref<const Matrix4f> Map2Eigen(const aiMatrix4x4& aimtx) { return  math::Map<const Matrix4f>(aimtx[0]); }
//	struct Meshs
//	{
//		std::vector<animData> _anims;
//		uint FindPosition(float p_animation_time, const aiNodeAnim* p_node_anim)
//		{
//			// ����� ���� ������� ����� ����� ����� ������� ���������� ����� ������ ��������
//			for (uint i = 0; i < p_node_anim->mNumPositionKeys - 1; i++) // �������� ����� ��������
//			{
//				if (p_animation_time < (float)p_node_anim->mPositionKeys[i + 1].mTime) // �������� �� �������� ��������� !!!
//				{
//					return i; // �� ������� ������ �������� !!!!!!!!!!!!!!!!!! ����������������������������
//				}
//			}
//
//			assert(0);
//			return 0;
//		}
//
//		uint FindRotation(float p_animation_time, const aiNodeAnim* p_node_anim)
//		{
//			// ����� ���� ������� ����� ����� ����� ������� ���������� ����� ������ ��������
//			for (uint i = 0; i < p_node_anim->mNumRotationKeys - 1; i++) // �������� ����� ��������
//			{
//				if (p_animation_time < (float)p_node_anim->mRotationKeys[i + 1].mTime) // �������� �� �������� ��������� !!!
//				{
//					return i; // �� ������� ������ �������� !!!!!!!!!!!!!!!!!! ����������������������������
//				}
//			}
//
//			assert(0);
//			return 0;
//		}
//
//		uint FindScaling(float p_animation_time, const aiNodeAnim* p_node_anim)
//		{
//			// ����� ���� ������� ����� ����� ����� ������� ���������� ����� ������ ��������
//			for (uint i = 0; i < p_node_anim->mNumScalingKeys - 1; i++) // �������� ����� ��������
//			{
//				if (p_animation_time < (float)p_node_anim->mScalingKeys[i + 1].mTime) // �������� �� �������� ��������� !!!
//				{
//					return i; // �� ������� ������ �������� !!!!!!!!!!!!!!!!!! ����������������������������
//				}
//			}
//
//			assert(0);
//			return 0;
//		}
//
//		template<typename T, typename T2>
//		inline uint Replacesmallest(T& ay, const uint length,const T2 v) {
//			int smallest = 0;
//			T2 cu = v;
//			for (size_t i = 0; i < length; i++)
//			{
//				if (ay[i] < cu){
//					cu = ay[i];
//					smallest = i;
//				}
//			}
//			if (ay[smallest] < v) {
//				ay[smallest] = v;
//				return smallest;
//			}
//			return -1;
//		}
//		struct MeshEntry {
//			MeshEntry()
//			{
//				NumIndices = 0;
//				BaseVertex = 0;
//				BaseIndex = 0;
//				MaterialIndex = 0xFFFFFFFF;
//			}
//
//			unsigned int NumIndices;
//			unsigned int BaseVertex;
//			unsigned int BaseIndex;
//			unsigned int NumVertices;
//			unsigned int MaterialIndex;
//		};
//
//		const aiScene* m_pScene;
//		Importer m_Importer;
//		std::vector<MeshEntry> _subMesh;
//		std::vector<ITexture*> m_Textures;
//
//
//		std::unordered_map<std::string, std::pair<aiNodeAnim*, uint>> m_BoneMapping;
//		//std::unordered_map<std::string, std::pair<aiNodeAnim*, uint>> m_BoneMapping;
//		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
//		{
//			for (uint i = 0; i < pAnimation->mNumChannels; i++) {
//				const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
//
//				if (string(pNodeAnim->mNodeName.data) == NodeName) {
//					return pNodeAnim;
//				}
//			}
//
//			return NULL;
//		}
//
//		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
//		{
//			if (pNodeAnim->mNumPositionKeys == 1) {
//				Out = pNodeAnim->mPositionKeys[0].mValue;
//				return;
//			}
//
//			uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
//			uint NextPositionIndex = (PositionIndex + 1);
//			assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
//			float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
//			float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
//			assert(Factor >= 0.0f && Factor <= 1.0f);
//			const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
//			const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
//			aiVector3D Delta = End - Start;
//			Out = Start + Factor * Delta;
//		}
//
//
//		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
//		{
//			// we need at least two values to interpolate...
//			if (pNodeAnim->mNumRotationKeys == 1) {
//				Out = pNodeAnim->mRotationKeys[0].mValue;
//				return;
//			}
//
//			uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
//			uint NextRotationIndex = (RotationIndex + 1);
//			assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
//			float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
//			float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
//			assert(Factor >= 0.0f && Factor <= 1.0f);
//			const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
//			const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
//			aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
//			Out = Out.Normalize();
//		}
//
//
//		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
//		{
//			if (pNodeAnim->mNumScalingKeys == 1) {
//				Out = pNodeAnim->mScalingKeys[0].mValue;
//				return;
//			}
//			uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
//			uint NextScalingIndex = (ScalingIndex + 1);
//			assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
//			float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
//			float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
//			assert(Factor >= 0.0f && Factor <= 1.0f);
//			const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
//			const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
//			aiVector3D Delta = End - Start;
//			Out = Start + Factor * Delta;
//		}
//		map<string, aiAnimation*> Animap;
//		unordered_map<string, std::vector<Matrix4f>> trased;
//		aiBone** currBones;
//		const float dt=0.02;
//			float	_animdt,dur;
//		std::vector<nonstd::span<Matrix4f>> _boneanim;
//
//		float TicksPerSecond;
//		void ReadNodeHeirarchy(const aiNode* pNode, const nonstd::span<Matrix4f> ParentTransform, const aiScene* m_pScene)
//		{
//			std::string NodeName(pNode->mName.data);
//			Matrix4f NodeTransformation=(math::Map<Matrix4f>((float*)(pNode->mTransformation[0])));
//
//			
//			auto boneani = m_BoneMapping.find(NodeName);
//			math::uint boneid = -1;
//			std::vector<Matrix4f> bonerec(ParentTransform.size());
//			//nonstd::span<Matrix4f> bonerec = cnode;
//			if (boneani != m_BoneMapping.end()) {
//				int i = 0;
//				boneid = boneani->second.second;
//
//				const auto pNodeAnim = boneani->second.first;
//				float AnimationTime = pNodeAnim? pNodeAnim->mPositionKeys[0].mTime:NAN;
//					if (boneid != -1&&boneani->second.first) {
//
//						auto& bonematrix = currBones[boneid]->mOffsetMatrix;// .Inverse();
//						auto bonetrs = _boneanim[boneid].subspan(currad.start, currad.length);
//						do
//						{
//							aiVector3D Scaling;
//							CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
//							// Interpolate rotation and generate rotation transformation matrix
//							aiQuaternion RotationQ;
//							CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//							// Interpolate translation and generate translation transformation matrix
//							aiVector3D Translation;
//							CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
//
//							// Combine the above transformations
//							NodeTransformation =  (math::Translation3f(math::Map< math::float3>(&Translation[0])) *
//								math::quaternion(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z) *
//								Eigen::Scaling(math::Map< math::float3>(&Scaling[0]))).matrix();
//							const Matrix4f boninv = math::Map<const Matrix4f>(bonematrix[0]);// .inverse();
//							//bonerec[i] = boninv.inverse();
//							bonerec[i] =  ParentTransform[i]* NodeTransformation;
//							bonetrs [i]=m_GlobalInverseTransform* bonerec[i]* boninv;
//							i++;
//							assert(bonerec.size() <= ParentTransform.size());
//							AnimationTime += _animdt;
//						} while (AnimationTime < dur);
//					}
//					else {
//						
//						do
//						{
//							if (boneid != -1) {
//
//								auto& bonematrix = currBones[boneid]->mOffsetMatrix;// .Inverse();
//								auto bonetrs = _boneanim[boneid].subspan(currad.start, currad.length);
//								const auto pr = NodeTransformation * math::Map<const Matrix4f>(bonematrix[0]);
//								for (size_t i = 0; i < bonerec.size(); i++)
//								{
//									bonerec[i] = ParentTransform[i] * NodeTransformation;
//									bonetrs[i] = m_GlobalInverseTransform * ParentTransform[i] * pr;
//								}
//								break;
//							}
//							aiVector3D Scaling;
//							CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
//							// Interpolate rotation and generate rotation transformation matrix
//							aiQuaternion RotationQ;
//							CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//							// Interpolate translation and generate translation transformation matrix
//							aiVector3D Translation;
//							CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
//
//							// Combine the above transformations
//							NodeTransformation = (math::Translation3f(math::Map< math::float3>(&Translation[0])) *
//								math::quaternion(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z) *
//								Eigen::Scaling(math::Map< math::float3>(&Scaling[0]))).matrix();
//							bonerec[i] =  ParentTransform[i]* NodeTransformation;
//							i++;
//							assert(bonerec.size() <= ParentTransform.size());
//							AnimationTime += _animdt;
//						} while (AnimationTime < dur);
//					}
//				
//			}
//			else
//			{
//					if ((NodeTransformation.matrix() != math::Matrix4f::Identity())) {
//
//						for (size_t i = currad.start; i < currad.length; i++)
//						{
//							bonerec[i] = ParentTransform[i] * NodeTransformation;
//						}
//					}
//					else
//					{
//						for (size_t i = 0; i < bonerec.size(); i++)
//						{
//							bonerec[i] = ParentTransform[i];
//						}
//					}
//			}
//
//			for (uint i = 0; i < pNode->mNumChildren; i++) {
//				ReadNodeHeirarchy(pNode->mChildren[i], bonerec, m_pScene);
//			}
//		}
//		animData currad;
//	 math::Matrix4f	m_GlobalInverseTransform;
//	 nonstd::span<Matrix4f> boneTransform;
//	 //void Update(aiMesh* _Mesh)
//	 //{
//		// auto AnimationTime = 0.0;
//		// if (_Mesh)
//		// {
//		//	 _Mesh->Transforms.matrix = GetMatrix();
//		//		 // Update join matrices
//		//		const auto   InverseTransform = m_GlobalInverseTransform; // TODO: do not use inverse tranform here
//		//		 size_t numJoints = std::min((uint32_t)_Skin->Joints.size(), Uint32{ Mesh::TransformData::MaxNumJoints });
//		//		 for (size_t i = 0; i < _Mesh->mNumBones; i++)
//		//		 {
//		//			 auto JointNode = _bonmap.find(_Mesh->mBones[i]->mName);
//		//			 auto pNodeAnim = std::get<1>(JointNode->second);
//		//			 auto nd= std::get<0>(JointNode->second);
//		//			 math::Transform<float, 3, Eigen::Affine> trs;
//		//			 aiVector3D Scaling;
//		//			 CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
//		//			 // Interpolate rotation and generate rotation transformation matrix
//		//			 aiQuaternion RotationQ;
//		//			 CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//		//			 // Interpolate translation and generate translation transformation matrix
//		//			 aiVector3D Translation;
//		//			 CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
//
//		//			 // Combine the above transformations
//		//			auto mat = math::Translation3f(math::Map< math::float3>(&Translation[0])) *
//		//				 math::quaternion(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z) *
//		//				 Eigen::Scaling(math::Map< math::float3>(&Scaling[0]));
//		//			while (nd->mParent)
//		//			{
//		//				nd = nd->mParent;
//		//				JointNode = _bonmap.find(_Mesh->mBones[i]->mName);
//		//				mat =Map<math::Matrix4f>( std::get<0>(JointNode->second)->mTransformation[0]);
//		//				if (JointNode != _bonmap.end()) {
//		//					pNodeAnim = std::get<1>(JointNode->second);
//		//					if (pNodeAnim) {
//		//						aiVector3D Scaling;
//		//						CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
//		//						// Interpolate rotation and generate rotation transformation matrix
//		//						aiQuaternion RotationQ;
//		//						CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//		//						// Interpolate translation and generate translation transformation matrix
//		//						aiVector3D Translation;
//		//						CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
//
//		//						// Combine the above transformations
//		//						 mat = mat* math::Translation3f(math::Map< math::float3>(&Translation[0])) *
//		//							math::quaternion(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z) *
//		//							Eigen::Scaling(math::Map< math::float3>(&Scaling[0]));
//		//					}
//		//				}
//		//			}
//		//			 const auto jm = JointNode->GetMatrix();
//		//			 auto  JointMat = _Skin->InverseBindMatrices[i] * jm * InverseTransform;
//
//		//			 _Mesh->Transforms.jointMatrix[i] = JointMat;
//		//		 }
//		// }
//
//		// for (auto& child : Children)
//		// {
//		//	 child->Update();
//		// }
//	 //}
//	 float CalDur(const aiAnimation* mAnimations) {
//		 float s=0, e=0;
//		 s = mAnimations->mChannels[0]->mPositionKeys[0].mTime;
//		 s = mAnimations->mChannels[0]->mRotationKeys[0].mTime<s? mAnimations->mChannels[0]->mRotationKeys[0].mTime:s;
//		 s = mAnimations->mChannels[0]->mRotationKeys[0].mTime<s? mAnimations->mChannels[0]->mScalingKeys[0].mTime:s;
//		 const auto max1 = mAnimations->mChannels[0]->mNumPositionKeys-1;
//		 const auto max2 = mAnimations->mChannels[0]->mNumRotationKeys-1;
//		 const auto max3 = mAnimations->mChannels[0]->mNumScalingKeys- 1;
//		 e = mAnimations->mChannels[0]->mPositionKeys[max1].mTime;
//		 e = mAnimations->mChannels[0]->mRotationKeys[max2].mTime > e ? mAnimations->mChannels[0]->mRotationKeys[max2].mTime :e;
//		 e = mAnimations->mChannels[0]->mRotationKeys[max3].mTime > e ? mAnimations->mChannels[0]->mScalingKeys[max3].mTime : e;
//		 return e - s;
//	 }
//		void ProcessAnim(aiMesh* ms) {
//			currad.start = 0;
//			currad.length = 0;
//			/*std::vector<aiNode*> nds;
//			nds.push_back(m_pScene->mRootNode);
//			while (nds.size()>0)
//			{
//				nds.pop_back();
//			}*/
//			m_GlobalInverseTransform =Map<math::Matrix4f> ((float*)&(m_pScene->mRootNode->mTransformation.Inverse()));
//			int total = 0;
//			std::vector<aiAnimation*> currAnims;
//
//			for (size_t i = 0; i < m_pScene->mNumAnimations; i++)
//			{
//				auto& pAnimation = m_pScene->mAnimations[i];
//				const auto currBonesnum = ms->mNumBones;
//				//if (pAnimation->mNumChannels == currBonesnum)
//				{
//					currAnims.push_back(pAnimation);
//					
//					TicksPerSecond = (float)(pAnimation->mTicksPerSecond != 0 ? pAnimation->mTicksPerSecond ==1?1000:pAnimation->mTicksPerSecond : 25.0f);
//					auto dth = dt * TicksPerSecond;
//					const auto dur = pAnimation->mDuration;
//					currad.start = currad.start + currad.length;
//					auto truedur = dur;// CalDur(pAnimation);
//					currad.length = std::ceil(truedur / dth);
//					total += currad.length;
//				}
//			}
//			for (size_t i = 0; i < _boneanim.size(); i++)
//			{
//				auto& sp =  _boneanim[i];
//				sp=std::move(nonstd::span(new Matrix4f[total](),total));
//			}
//			currad.start = 0;
//			currad.length = 0;
//			//nonstd::span<Matrix4f> gy(totale);
//			for (size_t i = 0; i < m_pScene->mNumAnimations; i++)
//			{
//				auto& pAnimation = m_pScene->mAnimations[i];
//				
//				for (size_t i = 0; i < pAnimation->mNumChannels; i++)
//				{
//					std::string cnam(pAnimation->mChannels[i]->mNodeName.data);
//					m_BoneMapping[cnam].first = pAnimation->mChannels[i];
//				}
//				
//				TicksPerSecond = (float)(pAnimation->mTicksPerSecond != 0 ? pAnimation->mTicksPerSecond == 1 ? 1000 : pAnimation->mTicksPerSecond : 25.0f);
//				const auto dth = dt * TicksPerSecond;
//				dur = pAnimation->mDuration;
//				currad.start = currad.start + currad.length;
//				const auto truedur = dur;//CalDur(pAnimation);
//				currad.length = std::ceil(truedur / dth);
//				_animdt = dt * TicksPerSecond;
//				_anims.emplace_back(currad);
//				const auto pNodeAnim = m_BoneMapping.find(string(m_pScene->mRootNode->mName.data));
//				vector<Matrix4f> bonerec(currad.length);
//				if (pNodeAnim!=m_BoneMapping.end()) {
//					std::abort();
//				}
//				else
//				{
//					for (size_t i = 0; i < currad.length; i++)
//					{
//						bonerec[i].setIdentity();
//					}
//				}
//				ReadNodeHeirarchy(m_pScene->mRootNode, bonerec, m_pScene);
//			}
//			/*for (size_t i = 0; i < _boneanim.size(); i++)
//			{
//				auto bm = m_BoneMapping.find(ms->mBones[i]->mName.C_Str());
//
//				if (bm->second.first == NULL) {
//					auto& bb = _boneanim[i];
//					for (size_t i3 = 0; i3 < bb.size(); i3++)
//					{
//						bb[i3].setIdentity();
//					}
//				}
//			}*/
//		}
//		void aiTex2Tex(aiTexture* tex, std::tuple<TextureData, TextureDesc >& outTex) {
//			auto& TexDesc = std::get<1>(outTex);
//			auto& data = std::get<0>(outTex);
//			TexDesc.Type = RESOURCE_DIM_TEX_2D;
//			TexDesc.Format = TEX_FORMAT_RGBA32_FLOAT;
//			TexDesc.Width = tex->mWidth;
//			TexDesc.Height = tex->mHeight;
//			TexDesc.MipLevels = 1;
//			TexDesc.BindFlags = BIND_SHADER_RESOURCE;
//			TexDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
//			data.pSubResources[0].Stride = tex->mWidth * 16;
//			data.pSubResources[0].pData = tex->pcData;
//		}
//		RefCntAutoPtr<ITexture>  loadMaterial(IRenderDevice* m_pDevice,aiMesh* pMesh) {
//			std::vector<ITexture> textures;
//			aiMaterial* mat = m_pScene->mMaterials[pMesh->mMaterialIndex];
//			aiString path;
//
//			TextureLoadInfo loadInfo;
//			loadInfo.IsSRGB = true;
//			RefCntAutoPtr<ITexture> Tex;
//			for (size_t i = 0; i < mat->GetTextureCount(aiTextureType_DIFFUSE); i++)
//			{
//				mat->GetTexture(aiTextureType_DIFFUSE, i, &path);
//				CreateTextureFromFile(path.data, loadInfo, m_pDevice, &Tex);
//			}
//			return Tex;
//		}
//		//std::unordered_map<aiString, std::tuple<aiNode*,aiNodeAnim*>> _bonmap;
//		void processMesh(aiMesh* pMesh, const aiScene* scene, std::vector<Vertex>& outvtx,
//			std::vector<UINT>& indices, int MeshIndex) {
//			// Data to fill
//			currBones = pMesh->mBones;
//
//			auto sz = sizeof(math::float3);
//			 sz = sizeof(math::float2);
//			 sz = sizeof(Vertex);
//			 sz = sizeof(math::uint4);
//			for (size_t i = 0; i < pMesh->mNumVertices; i++)
//			{
//				outvtx.emplace_back();
//				Vertex& vertex = outvtx[i];
//				vertex.mPosition[0] = pMesh->mVertices[i].x;
//				vertex.mPosition[1] = pMesh->mVertices[i].y;
//				vertex.mPosition[2] = pMesh->mVertices[i].z;
//			}
//			if (pMesh->HasNormals()) {
//				for (size_t i = 0; i < pMesh->mNumVertices; i++) {
//					//outvtx[i].mNormal = reinterpret_cast<decltype(Vertex::mNormal)&>(pMesh->mNormals[i]);
//					outvtx[i].mNormal[0] = pMesh->mNormals[i][0];
//					outvtx[i].mNormal[1] = pMesh->mNormals[i][1];
//					outvtx[i].mNormal[2] = pMesh->mNormals[i][2];
//				}
//			}
//			if (pMesh->HasTextureCoords(0)) {
//				auto& Tcoords = pMesh->mTextureCoords[0];
//				for (size_t i = 0; i < pMesh->mNumVertices; i++)
//				{
//					auto& uv = Tcoords[i];
//					outvtx[i].mUV[0] = uv[0];
//					outvtx[i].mUV[1] = uv[1];
//				}
//			}
//			for (UINT i = 0; i < pMesh->mNumFaces; i++) {
//				aiFace face = pMesh->mFaces[i];
//
//				for (UINT j = 0; j < face.mNumIndices; j++)
//					indices.push_back(face.mIndices[j]);
//			}
//			//std::vector<float[6]> boneiw2(pMesh->mNumVertices);
//			/*std::vector<int> boneiw(pMesh->mNumVertices);
//			std::memset(boneiw.data(), 0, boneiw.size() * sizeof(int));*/
//			m_BoneMapping.clear();
//			for (uint i = 0; i < pMesh->mNumBones; i++) {
//				std::string BoneName(pMesh->mBones[i]->mName.data);
//				m_BoneMapping[BoneName].second=i;
//				//_bonmap[pMesh->mBones[i]->mName]._Myfirst = NULL;
//				for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
//					uint VertexID = _subMesh[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
//					const float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
//					auto& vertex = outvtx[VertexID];
//					//auto& vbr = boneiw[VertexID];
//					//boneiw2[VertexID][vbr] = Weight;
//					/*vertex.mBoneIndices[vbr] = i;
//					vertex.mBoneWeights[vbr] = Weight;*/
//					//vbr++;
//					const auto replaceindex = Replacesmallest(vertex.mBoneWeights, 4, Weight);
//					if (replaceindex !=-1)
//					{
//						vertex.mBoneIndices[replaceindex] = i;
//					}
//				}
//			}
//			/*for (size_t i = 0; i < m_pScene->mNumAnimations; i++)
//			{
//				auto& pAnimation = m_pScene->mAnimations[i];
//				if (pAnimation->mNumChannels != pMesh->mNumBones)
//					continue;
//				for (size_t i = 0; i < pAnimation->mNumChannels; i++)
//				{
//
//					auto ch = pAnimation->mChannels[i];
//					std::get<1>(_bonmap[ch->mNodeName]) = ch;
//				}
//			}
//			std::vector<aiNode*> nodesdk;
//			nodesdk.push_back(m_pScene->mRootNode);
//			while (nodesdk.size()>0)
//			{
//				const auto f1 = nodesdk[nodesdk.size()-1];
//				if (_bonmap.find(f1->mName) != _bonmap.end()) {
//					std::get<0>(_bonmap[f1->mName]) = f1;
//				}
//				nodesdk.pop_back();
//				for (size_t i = 0; i < f1->mNumChildren; i++)
//				{
//					const auto ff = f1->mChildren[i];
//					nodesdk.push_back(ff);
//				}
//			}*/
//			/*for (size_t i = 0; i < outvtx.size(); i++)
//			{
//				const auto vv = outvtx[i];
//				float v=0;
//				for (size_t i = 0; i < 4; i++)
//				{
//					v += vv.mBoneWeights[i];
//				}
//				if (v !=1)
//					ERROR("d");
//			}*/
//			if (m_BoneMapping.size() > 0) {
//				_boneanim.resize(pMesh->mNumBones);
//				ProcessAnim(pMesh);
//			}
//			else
//			{
//
//			}
//
//		}
//
//		void LoadTex(const aiMaterial* pMaterial, aiTextureType textype, IRenderDevice* pDevice, const Char* ResourceDirectory,
//			ITexture** outTex) {
//			int matc = pMaterial->GetTextureCount(textype);
//			aiString Name0;
//			for (size_t i = 0; i < matc; i++)
//			{
//				aiString Name;
//				SamplerDesc SamDesc;
//				aiTextureMapping* mapping	 =NULL;
//					unsigned int* uvindex	 =NULL;
//					ai_real* blend			 =NULL;
//					aiTextureOp* op			 =NULL;
//					aiTextureMapMode* mapmode=NULL;
//					TEXTURE_ADDRESS_MODE ft=TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
//					auto p=pMaterial->mProperties[0];
//				if (pMaterial->GetTexture(textype, i, &Name, mapping, uvindex, blend, op, mapmode) == AI_SUCCESS) {
//					if (Name0 == Name)
//						continue;
//					else
//						Name0 = Name;
//					if(mapmode){
//						switch (*mapmode)
//						{
//						case aiTextureMapMode::aiTextureMapMode_Wrap:
//							ft = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
//							break;
//						case aiTextureMapMode::aiTextureMapMode_Clamp:
//							ft = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_CLAMP;
//							break;
//						case aiTextureMapMode::aiTextureMapMode_Mirror:
//							ft = TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_MIRROR;
//							break;
//						}
//					}
//					SamDesc.MagFilter = FILTER_TYPE_LINEAR;
//					SamDesc.MinFilter = FILTER_TYPE_LINEAR;
//					SamDesc.MipFilter = FILTER_TYPE_LINEAR;
//					SamDesc.AddressU = ft;
//					SamDesc.AddressV = ft;
//					SamDesc.AddressW = SamDesc.AddressV;
//					RefCntAutoPtr<ISampler> pSampler;
//					pDevice->CreateSampler(SamDesc, &pSampler);
//					TextureSamplers.push_back(std::move(pSampler));
//					std::string FullPath = ResourceDirectory;
//					if (auto emtex = m_pScene->GetEmbeddedTexture(Name.C_Str())) {
//						if (emtex->mHeight == 0) {
//							auto ImgFileFormat = Diligent::Image::GetFileFormat(reinterpret_cast<Uint8*>(emtex->pcData), emtex->mWidth);
//							if (ImgFileFormat == IMAGE_FILE_FORMAT_PNG ||
//								ImgFileFormat == IMAGE_FILE_FORMAT_JPEG ||
//								ImgFileFormat == IMAGE_FILE_FORMAT_TIFF)
//							{
//								ImageLoadInfo ImgLoadInfo;
//								ImgLoadInfo.Format = ImgFileFormat;
//								RefCntAutoPtr<IDataBlob> pData(MakeNewRCObj<MydataBlob>()(emtex->pcData, emtex->mWidth));
//
//								RefCntAutoPtr<Image>    pImage; 
//
//								TextureLoadInfo LoadInfo;
//								LoadInfo.IsSRGB = true;
//								Image::CreateFromDataBlob(pData, ImgLoadInfo, &pImage);
//								CreateTextureFromImage(pImage, LoadInfo, pDevice, outTex);
//								continue;
//							}
//						}
//						else
//						{
//							//need imp
//							std::abort();
//						}
//					}
//					if (!FullPath.empty() && FullPath.back() != FileSystem::GetSlashSymbol())
//						FullPath.push_back(FileSystem::GetSlashSymbol());
//					FullPath.append(Name.data);
//					if (FileSystem::FileExists(FullPath.c_str()))
//					{
//						TextureLoadInfo LoadInfo;
//						LoadInfo.IsSRGB = true;
//						CreateTextureFromFile(FullPath.c_str(), LoadInfo, pDevice, outTex);
//						
//						if (outTex == nullptr)
//						{
//							LOG_ERROR("Failed to load texture ", Name.data);
//						}
//					}
//				}
//
//			}
//		}
//		std::vector<RefCntAutoPtr<ITexture>> Textures;
//		std::vector<RefCntAutoPtr<ISampler>> TextureSamplers;
//		std::vector<material> _materials;
//		bool InitMaterials(const Diligent::Char* ResourceDirectory, IRenderDevice* pDevice, IDeviceContext* pDeviceCtx)
//		{
//			// Extract the directory part from the file name
//			
//
//			bool Ret = true;
//			_materials.resize(m_pScene->mNumMaterials);
//			// Initialize the materials
//			for (uint e = 0; e < m_pScene->mNumMaterials; e++) {
//				const aiMaterial* pMaterial = m_pScene->mMaterials[e];
//				for (size_t i = 1; i <= aiTextureType_UNKNOWN; i++)
//				{
//					aiTextureType textype = (aiTextureType)i;
//					auto& mat = _materials[e];
//					if (pMaterial->GetTextureCount(textype) > 0) {
//						ITexture* tex=nullptr;
//						LoadTex(pMaterial, (aiTextureType)i, pDevice, ResourceDirectory,&tex);
//						switch (textype)
//						{
//						case aiTextureType_UNKNOWN:
//						case aiTextureType_DIFFUSE:
//								_materials[e].pDiffuseTexture=tex;
//								mat.pDiffuseRV = (*tex).GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
//								
//							break;
//						case aiTextureType_NORMALS:
//							_materials[e].pNormalTexture = tex;
//							mat.pNormalRV = (*tex).GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
//							
//						default:
//							break;
//						}
//					}
//				}
//				
//			}
//
//			return Ret;
//		}
//		RefCntAutoPtr<IBuffer> pVertexBuffer;
//		RefCntAutoPtr<IBuffer> pIndexBuffer;
//		std::vector<uint> Indices;
//		std::vector<Vertex> Vertices;
//		void InitFromScene(const aiScene* pScene, IRenderDevice* pDevice)
//		{
//			_subMesh.resize(pScene->mNumMeshes);
//			m_Textures.resize(pScene->mNumMaterials);
//			m_pScene = pScene;
//			uint NumVertices = 0;
//			uint NumIndices = 0;
//
//			// Count the number of vertices and indices
//			for (uint i = 0; i < _subMesh.size(); i++) {
//				_subMesh[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
//				_subMesh[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
//				_subMesh[i].BaseVertex = NumVertices;
//				_subMesh[i].BaseIndex = NumIndices;
//				_subMesh[i].NumVertices = pScene->mMeshes[i]->mNumVertices;
//				NumVertices += pScene->mMeshes[i]->mNumVertices;
//				NumIndices += _subMesh[i].NumIndices;
//			}
//
//			// Reserve space in the vectors for the vertex attributes and indices
//			Vertices.reserve(NumVertices);
//			Indices.reserve(NumIndices);
//			uint baseIndex = 0;
//			// Initialize the meshes in the scene one by one
//			for (uint i = 0; i < _subMesh.size(); i++) {
//				aiMesh* paiMesh = pScene->mMeshes[i];
//				processMesh(paiMesh, pScene, Vertices, Indices, i);
//			}
//
//			{
//				VERIFY_EXPR(!VertexData0.empty());
//				BufferDesc VBDesc;
//				VBDesc.Name = " vertex  buffer";
//				VBDesc.Size = static_cast<Uint32>(Vertices.size() * sizeof(Vertices[0]));
//				VBDesc.BindFlags = BIND_VERTEX_BUFFER;
//				VBDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
//
//				BufferData BuffData(Vertices.data(), VBDesc.Size);
//				pDevice->CreateBuffer(VBDesc, &BuffData, &pVertexBuffer);
//			}
//
//			if (!Indices.empty())
//			{
//				BufferDesc IBDesc;
//				IBDesc.Name = " index buffer";
//				IBDesc.Size = static_cast<Uint32>(Indices.size() * sizeof(Indices[0]));
//				IBDesc.BindFlags = BIND_INDEX_BUFFER;
//				IBDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
//
//				BufferData BuffData(Indices.data(), IBDesc.Size);
//				pDevice->CreateBuffer(IBDesc, &BuffData, &pIndexBuffer);
//			}
//			/*if (!InitMaterials(pScene, Filename)) {
//				return false;
//			}*/
//		}
//		void InitFromScene(std::filesystem::path Resource, IRenderDevice* pDevice) {
//			Assimp::Importer m_Importer;
//			m_pScene = m_Importer.ReadFile(Resource.string(),
//				aiProcess_CalcTangentSpace |
//				aiProcess_Triangulate |
//				aiProcess_JoinIdenticalVertices |
//				aiProcess_SortByPType);
//			auto recDir1 = Resource;
//			auto recDir = recDir1.remove_filename().string();
//			InitMaterials(recDir.c_str(), pDevice, NULL);
//			InitFromScene(m_pScene, pDevice);
//		}
//		void loadgltf(std::filesystem::path Resource, IRenderDevice* pDevice,
//			IDeviceContext* pContext) {
//			GLTF::Model m_Model(pDevice, pContext, Resource.string().c_str());
//			auto ff = 0;
//		}
//	};
//}

//
//
//
//
//Meshs m_Mesh;
//std::vector<Uint32>                                m_PSOIndex;
//std::vector<RefCntAutoPtr<IPipelineState>>         m_RenderMeshPSO;
//std::vector<RefCntAutoPtr<IPipelineState>>         m_RenderMeshShadowPSO;
//std::vector<RefCntAutoPtr<IShaderResourceBinding>> m_SRBs;
//std::vector<RefCntAutoPtr<IShaderResourceBinding>> m_ShadowSRBs;
//void CreatePipelineStates()
//{
//	ShaderCreateInfo                               ShaderCI;
//	RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
//	auto shaderpath = loader::Get(shader);
//	m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(shaderpath.string().c_str(), &pShaderSourceFactory);
//	ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
//	ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
//	ShaderCI.UseCombinedTextureSamplers = true;
//
//	ShaderMacroHelper Macros;
//	// clang-format on
//	ShaderCI.Macros = Macros;
//
//	ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
//	ShaderCI.Desc.Name = "Mesh VS";
//	ShaderCI.EntryPoint = "vert";
//	ShaderCI.FilePath = "main.vsh";
//	RefCntAutoPtr<IShader> pVS;
//	m_pDevice->CreateShader(ShaderCI, &pVS);
//
//	ShaderCI.Desc.Name = "Mesh PS";
//	ShaderCI.EntryPoint = "frag";
//	ShaderCI.FilePath = "main.vsh";
//	ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
//	RefCntAutoPtr<IShader> pPS;
//	m_pDevice->CreateShader(ShaderCI, &pPS);
//
//
//	m_PSOIndex.resize(m_Mesh._subMesh.size());
//	m_RenderMeshPSO.clear();
//	m_RenderMeshShadowPSO.clear();
//	for (Uint32 vb = 0; vb < m_Mesh._subMesh.size(); ++vb)
//	{
//		auto& material = m_Mesh._materials[vb];
//		PipelineStateCreateInfo PSOCreateInfo;
//		PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
//		LayoutElement inputDesc[] =
//		{
//			{0, 0, 4, VT_FLOAT32},   //float4 Pos     : ATTRIB0;
//			{1, 0, 4, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
//			{2, 0, 4, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
//			{3, 0, 4, VT_UINT32,False},   //float4 Joint0  : ATTRIB4;
//			{4, 0, 4, VT_FLOAT32},    //float4 Weight0 : ATTRIB5;
//			//{5, 1, 1, VT_UINT32, False, INPUT_ELEMENT_FREQUENCY_PER_INSTANCE}
//		};
//		std::vector<LayoutElement> Elements;
//		auto& InputLayout = PSODesc.GraphicsPipeline.InputLayout;
//		InputLayout.LayoutElements = inputDesc;
//		InputLayout.NumElements = _countof(inputDesc);
//		//  Try to find PSO with the same layout
//		Uint32 pso;
//		for (pso = 0; pso < m_RenderMeshPSO.size(); ++pso)
//		{
//			const auto& PSOLayout = m_RenderMeshPSO[pso]->GetDesc().GraphicsPipeline.InputLayout;
//
//			bool IsSameLayout =
//				PSOLayout.NumElements == InputLayout.NumElements &&
//				memcmp(PSOLayout.LayoutElements, InputLayout.LayoutElements, sizeof(LayoutElement) * InputLayout.NumElements) == 0;
//
//			if (IsSameLayout)
//				break;
//		}
//
//		m_PSOIndex[vb] = pso;
//		if (pso < static_cast<Uint32>(m_RenderMeshPSO.size()))
//			continue;
//		ShaderResourceVariableDesc Vars[] =
//		{
//			{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
//			{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
//			{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
//			{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
//		};
//		// clang-format off
//		StaticSamplerDesc StaticSamplers[] =
//		{
//			{SHADER_TYPE_PIXEL, "g_ColorMap", Sam_Aniso4xWrap}
//		};
//		// clang-format on
//		PSODesc.ResourceLayout.StaticSamplers = StaticSamplers;
//		PSODesc.ResourceLayout.NumStaticSamplers = _countof(StaticSamplers);
//
//
//		// clang-format on
//		PSODesc.ResourceLayout.Variables = Vars;
//		PSODesc.ResourceLayout.NumVariables = _countof(Vars);
//
//		PSODesc.Name = "Mesh PSO";
//		PSODesc.GraphicsPipeline.pVS = pVS;
//		PSODesc.GraphicsPipeline.pPS = pPS;
//
//		PSODesc.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
//		PSODesc.GraphicsPipeline.NumRenderTargets = 1;
//		PSODesc.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
//		PSODesc.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//		PSODesc.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL; // Cull back faces
//		PSODesc.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
//		// Enable depth testing
//		PSODesc.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
//		RefCntAutoPtr<IPipelineState> pRenderMeshPSO;
//		m_pDevice->CreatePipelineState(PSOCreateInfo, &pRenderMeshPSO);
//		// clang-format off
//		pRenderMeshPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(m_CameraAttribsCB);
//		// clang-format on
//		m_RenderMeshPSO.emplace_back(std::move(pRenderMeshPSO));
//	}
//	createbuffer(m_Mesh._boneanim);
//	RefCntAutoPtr<IShaderResourceBinding> pSRB;
//	m_RenderMeshPSO[0]->CreateShaderResourceBinding(&pSRB, true);
//	auto shv = pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
//	auto mtxs = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "animreco");
//	auto inpro = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas");
//	auto teins = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
//	if (shv)
//		shv->Set(m_Mesh._materials[0].pDiffuseRV);
//	if (teins)
//		teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
//	mtxs->Set(m_paniinsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
//	inpro->Set(m_pinspropertysBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
//	m_SRBs.emplace_back(std::move(pSRB));
//}
//
//void DrawMesh(IDeviceContext* pCtx, bool bIsShadowPass)
//{
//	// Note that Vulkan requires shadow map to be transitioned to DEPTH_READ state, not SHADER_RESOURCE
//	pCtx->TransitionShaderResources((bIsShadowPass ? m_RenderMeshShadowPSO : m_RenderMeshPSO)[0], (bIsShadowPass ? m_ShadowSRBs : m_SRBs)[0]);
//
//	for (Uint32 meshIdx = 0; meshIdx < m_Mesh._subMesh.size(); ++meshIdx)
//	{
//		//const auto& SubMesh = m_Mesh.GetMesh(meshIdx);
//		//BoundBox    BB;
//		//BB.Min = SubMesh.BoundingBoxCenter - SubMesh.BoundingBoxExtents * 0.5f;
//		//BB.Max = SubMesh.BoundingBoxCenter + SubMesh.BoundingBoxExtents * 0.5f;
//		//// Notice that for shadow pass we test against frustum with open near plane
//		//if (GetBoxVisibility(Frustum, BB, bIsShadowPass ? FRUSTUM_PLANE_FLAG_OPEN_NEAR : FRUSTUM_PLANE_FLAG_FULL_FRUSTUM) == BoxVisibility::Invisible)
//		//	continue;
//
//		IBuffer* pVBs[] = { m_Mesh.pVertexBuffer };
//		math::ui64   Offsets[] = { 0 };
//		pCtx->SetVertexBuffers(0, 1, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
//
//		auto pIB = m_Mesh.pIndexBuffer;
//
//		pCtx->SetIndexBuffer(pIB, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
//
//		auto& pPSO = m_RenderMeshPSO[meshIdx];
//		pCtx->SetPipelineState(pPSO);
//
//		pCtx->CommitShaderResources(m_SRBs[0], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
//
//		DrawIndexedAttribs drawAttrs(static_cast<Uint32>(m_Mesh._subMesh[meshIdx].NumIndices), VT_UINT32, DRAW_FLAG_VERIFY_ALL);
//		drawAttrs.FirstIndexLocation = static_cast<Uint32>(m_Mesh._subMesh[meshIdx].BaseIndex);
//		pCtx->DrawIndexed(drawAttrs);
//	}
//}