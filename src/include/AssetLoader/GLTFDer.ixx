module;
#include "MapHelper.hpp"
#include "CommonlyUsedStates.h"
#include "DataBlobImpl.hpp"
#include "Image.h"
#include "FileSystem.hpp"
#include "FileWrapper.hpp"
#include "GraphicsAccessories.hpp"
#include "TextureLoader.h"
#include"RenderStr0.h"
//#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include "tinygltf/tiny_gltf.h"
//import DiliGFloader;
#include "ThirdParty/GLTFLoader.hpp";
#define EXP export
export module GFloader;
//export namespace GLTF {
//    using namespace Diligent::GLTF;
//}
namespace pj2::Model {
	//using namespace Diligent::GLTF;
	using namespace Diligent;
	struct  model;
	namespace Callbacks
	{

		namespace
		{

			struct ImageLoaderData
			{
				GLTF::Model::TextureCacheType* pTextureCache;
				std::vector<RefCntAutoPtr<ITexture>>* pTextureHold;
				std::string                           BaseDir;
			};


			bool LoadImageData(tinygltf::Image* gltf_image,
				const int            gltf_image_idx,
				std::string* error,
				std::string* warning,
				int                  req_width,
				int                  req_height,
				const unsigned char* image_data,
				int                  size,
				void* user_data)
			{
				(void)warning;

				auto* pLoaderData = reinterpret_cast<ImageLoaderData*>(user_data);
				if (pLoaderData != nullptr && pLoaderData->pTextureCache != nullptr)
				{
					auto it = pLoaderData->pTextureCache->find(pLoaderData->BaseDir + gltf_image->uri);
					if (it != pLoaderData->pTextureCache->end())
					{
						if (auto pTexture = it->second.Lock())
						{
							const auto& TexDesc = pTexture->GetDesc();
							const auto& FmtAttribs = GetTextureFormatAttribs(TexDesc.Format);

							gltf_image->width = TexDesc.Width;
							gltf_image->height = TexDesc.Height;
							gltf_image->component = FmtAttribs.NumComponents;
							gltf_image->bits = FmtAttribs.ComponentSize * 8;
							gltf_image->pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;

							// Keep strong reference to ensure the texture is alive.
							pLoaderData->pTextureHold->emplace_back(std::move(pTexture));

							return true;
						}
						else
						{
							// Texture is stale - remove it from the cache
							pLoaderData->pTextureCache->erase(it);
						}
					}
				}

				ImageLoadInfo LoadInfo;
				LoadInfo.Format = Image::GetFileFormat(image_data, size);
				if (LoadInfo.Format == IMAGE_FILE_FORMAT_UNKNOWN)
				{
					if (error != nullptr)
					{
						*error += FormatString("Unknown format for image[", gltf_image_idx, "] name = '", gltf_image->name, "'");
					}
					return false;
				}

				if (LoadInfo.Format == IMAGE_FILE_FORMAT_DDS || LoadInfo.Format == IMAGE_FILE_FORMAT_KTX)
				{
					// Store binary data directly
					gltf_image->image.resize(size);
					memcpy(gltf_image->image.data(), image_data, size);
					// Use pixel_type field to indicate the file format
					gltf_image->pixel_type = LoadInfo.Format;
				}
				else
				{
					RefCntAutoPtr<DataBlobImpl> pImageData(MakeNewRCObj<DataBlobImpl>()(size));
					memcpy(pImageData->GetDataPtr(), image_data, size);
					RefCntAutoPtr<Image> pImage;
					Image::CreateFromDataBlob(pImageData, LoadInfo, &pImage);
					if (!pImage)
					{
						if (error != nullptr)
						{
							*error += FormatString("Failed to load image[", gltf_image_idx, "] name = '", gltf_image->name, "'");
						}
						return false;
					}
					const auto& ImgDesc = pImage->GetDesc();

					if (req_width > 0)
					{
						if (static_cast<Uint32>(req_width) != ImgDesc.Width)
						{
							if (error != nullptr)
							{
								(*error) += FormatString("Image width mismatch for image[",
									gltf_image_idx, "] name = '", gltf_image->name,
									"': requested width: ",
									req_width, ", actual width: ",
									ImgDesc.Width);
							}
							return false;
						}
					}

					if (req_height > 0)
					{
						if (static_cast<Uint32>(req_height) != ImgDesc.Height)
						{
							if (error != nullptr)
							{
								(*error) += FormatString("Image height mismatch for image[",
									gltf_image_idx, "] name = '", gltf_image->name,
									"': requested height: ",
									req_height, ", actual height: ",
									ImgDesc.Height);
							}
							return false;
						}
					}

					gltf_image->width = ImgDesc.Width;
					gltf_image->height = ImgDesc.Height;
					gltf_image->component = 4;
					gltf_image->bits = GetValueSize(ImgDesc.ComponentType) * 8;
					gltf_image->pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
					auto DstRowSize = gltf_image->width * gltf_image->component * (gltf_image->bits / 8);
					gltf_image->image.resize(static_cast<size_t>(gltf_image->height * DstRowSize));
					auto* pPixelsBlob = pImage->GetData();
					const Uint8* pSrcPixels = reinterpret_cast<const Uint8*>(pPixelsBlob->GetDataPtr());
					if (ImgDesc.NumComponents == 3)
					{
						for (Uint32 row = 0; row < ImgDesc.Height; ++row)
						{
							for (Uint32 col = 0; col < ImgDesc.Width; ++col)
							{
								Uint8* DstPixel = gltf_image->image.data() + DstRowSize * row + col * gltf_image->component;
								const Uint8* SrcPixel = pSrcPixels + ImgDesc.RowStride * row + col * ImgDesc.NumComponents;

								DstPixel[0] = SrcPixel[0];
								DstPixel[1] = SrcPixel[1];
								DstPixel[2] = SrcPixel[2];
								DstPixel[3] = 1;
							}
						}
					}
					else if (gltf_image->component == 4)
					{
						for (Uint32 row = 0; row < ImgDesc.Height; ++row)
						{
							memcpy(gltf_image->image.data() + DstRowSize * row, pSrcPixels + ImgDesc.RowStride * row, DstRowSize);
						}
					}
					else
					{
						*error += FormatString("Unexpected number of image comonents (", ImgDesc.NumComponents, ")");
						return false;
					}
				}

				return true;
			}

			bool FileExists(const std::string& abs_filename, void*)
			{
				return FileSystem::FileExists(abs_filename.c_str());
			}

			bool ReadWholeFile(std::vector<unsigned char>* out,
				std::string* err,
				const std::string& filepath,
				void*)
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

				out->resize(size);
				pFile->Read(out->data(), size);

				return true;
			}

		} // namespace

	} // namespace Callbacks
	EXP struct modelinfo {
		bool hasskin;
	};
	EXP struct model : public Diligent::GLTF::Model
	{
		std::vector<Uint32>         IndexBuffer;
		std::vector<VertexAttribs0> VertexData0;
		std::vector<VertexAttribs1> VertexData1;
		std::vector<VertexAttribs0> tmp_VertexData0;
		std::vector<VertexAttribs1> tmp_VertexData1;
		std::vector<n_Render::modelidn>modeloffset;
		model() {
		
		}
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
			TextureCacheType* pTextureCache = nullptr) {
			tinygltf::Model    gltf_model;
			tinygltf::TinyGLTF gltf_context;
			//modelinfo mif;
			std::vector<RefCntAutoPtr<ITexture>> TextureHold;

			Callbacks::ImageLoaderData LoaderData //
			{
				pTextureCache,
				&TextureHold //
			};

			if (filename.find_last_of("/\\") != std::string::npos)
				LoaderData.BaseDir = filename.substr(0, filename.find_last_of("/\\"));
			LoaderData.BaseDir += '/';

			gltf_context.SetImageLoader(Callbacks::LoadImageData, &LoaderData);
			tinygltf::FsCallbacks fsCallbacks = {};
			fsCallbacks.ExpandFilePath = tinygltf::ExpandFilePath;
			fsCallbacks.FileExists = Callbacks::FileExists;
			fsCallbacks.ReadWholeFile = Callbacks::ReadWholeFile;
			fsCallbacks.WriteWholeFile = tinygltf::WriteWholeFile;
			fsCallbacks.user_data = this;
			gltf_context.SetFsCallbacks(fsCallbacks);

			bool   binary = false;
			size_t extpos = filename.rfind('.', filename.length());
			if (extpos != std::string::npos)
			{
				binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
			}

			std::string error;
			std::string warning;

			bool fileLoaded;
			if (binary)
				fileLoaded = gltf_context.LoadBinaryFromFile(&gltf_model, &error, &warning, filename.c_str());
			else
				fileLoaded = gltf_context.LoadASCIIFromFile(&gltf_model, &error, &warning, filename.c_str());
			if (!fileLoaded)
			{
				LOG_ERROR_AND_THROW("Failed to load gltf file ", filename, ": ", error);
			}
			if (!warning.empty())
			{
				LOG_WARNING_MESSAGE("Loaded gltf file ", filename, " with the following warning:", warning);
			}// = models.emplace_back();
			n_Render::modelidn& mdi = modeloffset.emplace_back();
			mdi.anims = this->Animations.size();
			mdi.Ids = this->IndexBuffer.size();
			mdi.sks = this->Skins.size();
			mdi.nds = this->Nodes.size();
			mdi.lnds = this->LinearNodes.size();
			mdi.mtls = this->Materials.size();
			mdi.texstart = this->Textures.size();
			mdi.texsps = this->TextureSamplers.size();
			mdi.Name = filename;
			Model::LoadTextureSamplers(pDevice, gltf_model);
			Model::LoadTextures(pDevice, pContext, gltf_model, LoaderData.BaseDir, pTextureCache);
			Model::LoadMaterials(gltf_model);

			// TODO: scene handling with no default scene
			const tinygltf::Scene& scene = gltf_model.scenes[gltf_model.defaultScene > -1 ? gltf_model.defaultScene : 0];
			tmp_VertexData0.clear();
			tmp_VertexData1.clear();
			for (size_t i = 0; i < scene.nodes.size(); i++)
			{
				const tinygltf::Node node = gltf_model.nodes[scene.nodes[i]];
				Model::LoadNode(pDevice, nullptr, node, scene.nodes[i], gltf_model, IndexBuffer, tmp_VertexData0, tmp_VertexData1);
			}
			this->VertexData0.insert(this->VertexData0.end(), tmp_VertexData0.begin(), tmp_VertexData0.end());
			this->VertexData1.insert(this->VertexData1.end(), tmp_VertexData1.begin(), tmp_VertexData1.end());
			if (gltf_model.animations.size() > 0)
			{
				Model::LoadAnimations(gltf_model);
			}
			Model::LoadSkins(gltf_model);

			for (auto* node : LinearNodes)
			{
				// Assign skins
				if (node->SkinIndex >= 0)
				{
					node->_Skin = Skins[node->SkinIndex].get();
				}

				// Initial pose
				if (node->_Mesh)
				{
					node->Update();
				}
			}


			Extensions = gltf_model.extensionsUsed;


			Model::GetSceneDimensions();

			mdi.skl = this->Skins.size() - mdi.sks;
			mdi.Idl = this->IndexBuffer.size() - mdi.Ids;
			mdi.animl = this->Animations.size() - mdi.anims;
			mdi.ndl = this->Nodes.size() - mdi.nds;
			mdi.lndl = this->LinearNodes.size() - mdi.lnds;
			mdi.mtlsize = this->Materials.size() - mdi.mtls;
			mdi.texsize = this->Textures.size() - mdi.texsize;
			mdi.texspl = this->TextureSamplers.size() - mdi.texsps;
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