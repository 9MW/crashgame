/*
 *  Copyright 2019-2020 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */
//#pragma comment(lib, "windowscodecs.lib")

#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef PLATFORM_WIN32
#define PLATFORM_WIN32 1
#endif

#ifndef ENGINE_DLL
#define ENGINE_DLL 0
#endif
 /*
 #ifndef D3D11_SUPPORTED
 #define D3D11_SUPPORTED 1
 #endif

 #ifndef D3D12_SUPPORTED
 #define D3D12_SUPPORTED 1
 #endif

 #ifndef GL_SUPPORTED
 #define GL_SUPPORTED 1
 #endif
 */
 //#include"Common/Math1.h"
  //#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
  //#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
  //#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "PCH.h"

#ifndef VULKAN_SUPPORTED
#define VULKAN_SUPPORTED 1
#define D3D12_SUPPORTED 0
#endif
//#include"entt/entt.hpp"
#include "PCH.h"
#include"os/input.h"
//put here solve compile error
#include"Render/geomatrys.h"
//import  physics;
//#include"physics/physics.hpp"
//#include"Common/syss.hpp"
#include"Timer.hpp"

#include "Common/interface/RefCntAutoPtr.hpp"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include <Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h>

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "CommonlyUsedStates.h"

#include <ImGuiImplDiligent.hpp>
#include <imGuiimpGLFW.h>
//#include <ImGuiImplWin32.hpp>

#include <crtdbg.h>
#include<Log/log.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <type_traits>

#include <fcntl.h>
#include <cstdarg>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include"assimp/Importer.hpp"
//#include"Render/Mesh.h"


#include"Image.h"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"
#include"BasicMath.hpp"
#include <Graphics\GraphicsTools\interface\ShaderMacroHelper.hpp>
#include"Common/FirstPersonCamera.hpp"
//#include"shaders/BasicStructures.fxh"

#include<imgui.h>
#include<imgui_internal.h>
#include"ImGuizmo/ImGuizmo.h"
//#include"Render/drawer.h"
//import drawerm;
#include"io/commandin.h"
#include"shpso.h"
#include"Render/drawer.h"
#if PLATFORM_WIN32
#    undef GetObject
#    undef CreateWindow
#endif
#if PLATFORM_WIN32
#    define GLFW_EXPOSE_NATIVE_WIN32 1
#endif

#if PLATFORM_LINUX
#    define GLFW_EXPOSE_NATIVE_X11 1
#endif

#if PLATFORM_MACOS
#    define GLFW_EXPOSE_NATIVE_COCOA 1
#endif

#include <GLFW/glfw3.h>
#include<GLFW/glfw3native.h>
#include"App0.h"
template<std::size_t>
struct comp {
	int x;
};
constexpr bool debug = 0;
struct timer final {
	timer() : start{ std::chrono::system_clock::now() } {}

	void elapsed() {
		auto now = std::chrono::system_clock::now();
		LOGINFO("cost {0} sec", std::chrono::duration<double>(now - start).count());
	}

private:
	std::chrono::time_point<std::chrono::system_clock> start;
};
namespace fs = std::filesystem;
std::unique_ptr<Diligent::ImGuiImplDiligent> m_pImGui;
namespace cage {
	using namespace pj2;
	void EditTransform(const float* cameraView, const float* cameraProjection, float* matrix, bool editTransformDecomposition)
	{
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
		static bool useSnap = false;
		static float snap[3] = { 1.f, 1.f, 1.f };
		static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
		static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
		static bool boundSizing = false;
		static bool boundSizingSnap = false;

		if (editTransformDecomposition)
		{
			if (ImGui::IsKeyPressed(90))
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			if (ImGui::IsKeyPressed(69))
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			if (ImGui::IsKeyPressed(82)) // r Key
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
				mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
				mCurrentGizmoOperation = ImGuizmo::ROTATE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
				mCurrentGizmoOperation = ImGuizmo::SCALE;
			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
			ImGui::InputFloat3("Tr", matrixTranslation);
			ImGui::InputFloat3("Rt", matrixRotation);
			ImGui::InputFloat3("Sc", matrixScale);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

			if (mCurrentGizmoOperation != ImGuizmo::SCALE)
			{
				if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
					mCurrentGizmoMode = ImGuizmo::LOCAL;
				ImGui::SameLine();
				if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
					mCurrentGizmoMode = ImGuizmo::WORLD;
			}
			if (ImGui::IsKeyPressed(83))
				useSnap = !useSnap;
			ImGui::Checkbox("", &useSnap);
			ImGui::SameLine();

			switch (mCurrentGizmoOperation)
			{
			case ImGuizmo::TRANSLATE:
				ImGui::InputFloat3("Snap", &snap[0]);
				break;
			case ImGuizmo::ROTATE:
				ImGui::InputFloat("Angle Snap", &snap[0]);
				break;
			case ImGuizmo::SCALE:
				ImGui::InputFloat("Scale Snap", &snap[0]);
				break;
			}
			ImGui::Checkbox("Bound Sizing", &boundSizing);
			if (boundSizing)
			{
				ImGui::PushID(3);
				ImGui::Checkbox("", &boundSizingSnap);
				ImGui::SameLine();
				ImGui::InputFloat3("Snap", boundsSnap);
				ImGui::PopID();
			}
		}
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
	}

	void EditTransform(const Camera& camera, Diligent::float4x4& matrix)
	{
		EditTransform(camera.GetViewMatrix().data(), camera.m_ProjMatrix.data(), matrix.Data(), true);
	}
	void EditTransform(const Camera& camera,float* matrix)
	{
		EditTransform(camera.GetViewMatrix().data(), camera.m_ProjMatrix.data(), matrix, true);
	}
	class App
	{
		ImGuiIO* guiio;
		inline math::Matrix4f f4m4derf(Diligent::float4x4 d4) {
			return  math::Map<math::Matrix4f>(d4.Data());
		}
		GLFWwindow* m_Window=NULL;

	public:
		App()
		{


		}
		~App()
		{
			pTabTex->Release();
			glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
			//m_pImmediateContext->Flush();
		}
		Diligent::float4x4 GetSurfacePretransformMatrix(const Diligent::float3& f3CameraViewAxis) const
		{
			const auto& SCDesc = m_pSwapChain->GetDesc();
			constexpr auto PI_F = math::PI_F;
			switch (SCDesc.PreTransform)
			{
			case SURFACE_TRANSFORM_ROTATE_90:
				// The image content is rotated 90 degrees clockwise.
				return Diligent::float4x4::RotationArbitrary(f3CameraViewAxis, -PI_F / 2.f);

			case SURFACE_TRANSFORM_ROTATE_180:
				// The image content is rotated 180 degrees clockwise.
				return Diligent::float4x4::RotationArbitrary(f3CameraViewAxis, -PI_F);

			case SURFACE_TRANSFORM_ROTATE_270:
				// The image content is rotated 270 degrees clockwise.
				return Diligent::float4x4::RotationArbitrary(f3CameraViewAxis, -PI_F * 3.f / 2.f);

			case SURFACE_TRANSFORM_OPTIMAL:
				UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
				return Diligent::float4x4::Identity();

			case SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
			case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
			case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
			case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
				UNEXPECTED("Mirror transforms are not supported");
				return Diligent::float4x4::Identity();

			default:
				return Diligent::float4x4::Identity();
			}
		}
		bool InitializeDiligentEngine(void* hWnd)
		{
			m_Window =(GLFWwindow*) hWnd;
#if PLATFORM_WIN32
			Win32NativeWindow Window{ glfwGetWin32Window(m_Window) };
#endif
#if PLATFORM_LINUX
			LinuxNativeWindow Window;
			Window.WindowId = glfwGetX11Window(m_Window);
			Window.pDisplay = glfwGetX11Display();
			if (DevType == RENDER_DEVICE_TYPE_GL)
				glfwMakeContextCurrent(m_Window);
#endif
#if PLATFORM_MACOS
			MacOSNativeWindow Window;
			if (DevType == RENDER_DEVICE_TYPE_GL)
				glfwMakeContextCurrent(m_Window);
			else
				Window.pNSView = GetNSWindowView(m_Window);
#endif
			SwapChainDesc SCDesc;
			SCDesc.DepthBufferFormat = TEX_FORMAT_D24_UNORM_S8_UINT;
			auto swusage = SWAP_CHAIN_USAGE_RENDER_TARGET | SWAP_CHAIN_USAGE_SHADER_INPUT | SWAP_CHAIN_USAGE_COPY_SOURCE;
			SCDesc.Usage = swusage;
			auto rdv = RENDER_DEVICE_TYPE_D3D12;

#ifdef VULKAN_SUPPORTED
			rdv = RENDER_DEVICE_TYPE_VULKAN;
#endif
			switch (rdv)
			{
#if D3D11_SUPPORTED
			case RENDER_DEVICE_TYPE_D3D11:
			{
				EngineD3D11CreateInfo EngineCI;
#ifdef DILIGENT_DEBUG
				EngineCI.DebugFlags |=
					D3D11_DEBUG_FLAG_CREATE_DEBUG_DEVICE |
					D3D11_DEBUG_FLAG_VERIFY_COMMITTED_SHADER_RESOURCES;
#endif
#if ENGINE_DLL
				// Load the dll and import GetEngineFactoryD3D11() function
				auto GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
#endif
				auto* pFactoryD3D11 = GetEngineFactoryD3D11();
				pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &m_pDevice, &m_pImmediateContext);
				Win32NativeWindow Window{ hWnd };
				pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
			}
			break;
#endif

#if D3D12_SUPPORTED
			case RENDER_DEVICE_TYPE_D3D12:
			{
#if ENGINE_DLL
				// Load the dll and import GetEngineFactoryD3D12() function
				auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#endif
				EngineD3D12CreateInfo EngineCI;
				EngineCI.EnableValidation = 1;
				EngineCI.DynamicHeapPageSize = 8 << 22;
				EngineCI.NumDeferredContexts = std::max(std::thread::hardware_concurrency() - 1, 2u);
				EngineCI.Features = DeviceFeatures{ DEVICE_FEATURE_STATE_OPTIONAL };
				ppContexts.resize(1 + EngineCI.NumDeferredContexts);
#ifdef DILIGENT_DEBUG
				// There is a bug in D3D12 debug layer that causes memory leaks in this tutorial
				//EngineCI.EnableDebugLayer = true;
#endif
				auto* pFactoryD3D12 = GetEngineFactoryD3D12();
				IDeviceContext** dc =m_pImmediateContext.RawDblPtr();
				pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &m_pDevice, dc);

				m_pEngineFactory = pFactoryD3D12;
				//Win32NativeWindow Window{ hWnd };
				pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, Window, &m_pSwapChain);
			}
			break;
#endif

#if GL_SUPPORTED
			case RENDER_DEVICE_TYPE_GL:
			{

#if EXPLICITLY_LOAD_ENGINE_GL_DLL
				// Load the dll and import GetEngineFactoryOpenGL() function
				auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
#endif
				auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

				EngineGLCreateInfo EngineCI;
				EngineCI.Window.hWnd = hWnd;
#ifdef DILIGENT_DEBUG
				EngineCI.CreateDebugContext = true;
#endif
				pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);
			}
			break;
#endif

#if VULKAN_SUPPORTED
			case RENDER_DEVICE_TYPE_VULKAN:
			{
#if EXPLICITLY_LOAD_ENGINE_VK_DLL
				// Load the dll and import GetEngineFactoryVk() function
				auto GetEngineFactoryVk = LoadGraphicsEngineVk();
#endif
				EngineVkCreateInfo EngineCI;
				EngineCI.DynamicHeapSize = 1<<27;//128 MB memory
				EngineCI.NumDeferredContexts = std::max(std::thread::hardware_concurrency() - 1, 2u);
				EngineCI.Features = DeviceFeatures{ DEVICE_FEATURE_STATE_OPTIONAL };
#ifdef DILIGENT_DEBUG
				EngineCI.EnableValidation = true;
#endif
				auto* pFactoryVk = GetEngineFactoryVk();
				ppContexts.resize(1 + EngineCI.NumDeferredContexts);
				pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice, ppContexts.data());
				m_pImmediateContext.Attach(ppContexts[0]);
				m_pEngineFactory = pFactoryVk;
				if (!m_pSwapChain)
				{
					pFactoryVk->CreateSwapChainVk(m_pDevice, ppContexts[0], SCDesc, Window, &m_pSwapChain);
				}
			}
			break;
#endif

			default:
				//// Load the dll and import GetEngineFactoryVk() function
				//auto GetEngineFactoryVk = LoadGraphicsEngineVk();
				//EngineVkCreateInfo EngineCI;
				//auto* pFactoryVk = GetEngineFactoryVk();
				//pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice, &m_pImmediateContext);
				//m_pEngineFactory = pFactoryVk;
				//if (!m_pSwapChain && hWnd != nullptr)
				//{
				//	Win32NativeWindow Window{ hWnd };
				//	pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, Window, &m_pSwapChain);
				//}

				break;
			}

			EngineVkCreateInfo EngineCI;
			EngineCI.DynamicHeapSize = 8 << 22;
			glfwSetWindowUserPointer(m_Window, this);
			glfwSetFramebufferSizeCallback(m_Window, &GLFW_ResizeCallback);
			glfwSetKeyCallback(m_Window, &GLFW_KeyCallback);
			glfwSetMouseButtonCallback(m_Window, &GLFW_MouseButtonCallback);
			glfwSetCursorPosCallback(m_Window, &GLFW_CursorPosCallback);
			//glfwSetScrollCallback(m_Window, &GLFW_MouseWheelCallback);

			glfwSetWindowSizeLimits(m_Window, 320, 240, GLFW_DONT_CARE, GLFW_DONT_CARE);
			
			const auto PI_F = math::PI_F;
			m_pImGui.reset(new ImGuiimpGLFW(m_Window, m_pDevice, m_pSwapChain->GetDesc().ColorBufferFormat, m_pSwapChain->GetDesc().DepthBufferFormat));
			//m_pDevice->CreatePipelineState(m_pImGui->)
			auto dm4 = GetSurfacePretransformMatrix(Diligent::float3(0, 1, 2));
			auto axis = Diligent::float3(0, 1, 2);
			auto dr90 = Diligent::float4x4::RotationArbitrary(axis, -PI_F / 2.f);
			auto er90 = (Eigen::AngleAxisf(-PI_F / 2.f, Efloat3(0, 1, 2).normalized())).toRotationMatrix();
			{
				auto shaderpath = loader::Get(shader);
				m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(shaderpath.string().c_str(), &pShaderSourceFactory);
				//m_Camera.SetPos(Diligent::float3(5, 5, 0.f));
				//entetTest::enter(m_pDevice, m_pImmediateContext, pShaderSourceFactory);
			}
			Camera::SetMain(m_Camera);
			m_Camera.SrfPreTransform = f4m4derf(GetSurfacePretransformMatrix(Diligent::float3{ 0, 0, 1 }));
			tst.setIdentity();
			WindowResize(m_pSwapChain->GetDesc().Width, m_pSwapChain->GetDesc().Height);
			CreateUniformBuffer(m_pDevice, 256 * 2, "Camera attribs buffer", &m_CameraAttribsCB);

			//{
			m_Camera.SetPos(Diligent::float3(15, 0, 20));
			m_Camera.m_fYawAngle = 4;
			m_Camera.m_fPitchAngle = -1;
			//m_Camera.SetRotation(PI_F / 2.f, 0);
			m_Camera.SetRotationSpeed(0.005f);
			m_Camera.SetMoveSpeed(5.f);
			m_Camera.SetSpeedUpScales(5.f, 10.f);
			m_Camera.SetLookAt({ 0,0,0 });
			// Create window-size offscreen render target
			TextureDesc RTColorDesc;
			RTColorDesc.Name = "Offscreen render target";
			RTColorDesc.Type = RESOURCE_DIM_TEX_2D;
			RTColorDesc.Width = m_pSwapChain->GetDesc().Width;
			RTColorDesc.Height = m_pSwapChain->GetDesc().Height;
			RTColorDesc.MipLevels = 1;
			RTColorDesc.Format =m_pSwapChain->GetDesc().ColorBufferFormat;

			// The render target can be bound as a shader resource and as a render target
			RTColorDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;

			RTColorDesc.ClearValue.Format = RTColorDesc.Format;
			RTColorDesc.ClearValue.Color[0] = 0.350f;
			RTColorDesc.ClearValue.Color[1] = 0.350f;
			RTColorDesc.ClearValue.Color[2] = 0.350f;
			RTColorDesc.ClearValue.Color[3] = 1.f;
			RefCntAutoPtr<ITexture> pRTColor;
			m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColor);
			/*TextureViewDesc TVD(TEXTURE_VIEW_SHADER_RESOURCE, RESOURCE_DIM_TEX_2D);
			TVD.Format = */RTColorDesc.Format;
			m_RendeRT = pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			m_RendeSRV = pRTColor->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
			// Create window-size depth buffer
			TextureDesc RTDepthDesc = RTColorDesc;
			RTDepthDesc.Name = "Offscreen depth buffer";
			RTDepthDesc.Format = m_pSwapChain->GetDesc().DepthBufferFormat;
			RTDepthDesc.BindFlags = BIND_DEPTH_STENCIL;
			// Define optimal clear value
			RTDepthDesc.ClearValue.Format = RTDepthDesc.Format;
			RTDepthDesc.ClearValue.DepthStencil.Depth = 1;
			RTDepthDesc.ClearValue.DepthStencil.Stencil = 0;
			RefCntAutoPtr<ITexture> pRTDepth;
			m_pDevice->CreateTexture(RTDepthDesc, nullptr, &pRTDepth);
			// Store the depth-stencil view
			m_pDepthDSV = pRTDepth->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL);

			return true;
		}
		Eigen::Matrix4f efpj, efca;
		void loadshader(const char* shadername, RefCntAutoPtr<IShader>& pVS, RefCntAutoPtr<IShader>& pPS) {
			ShaderCreateInfo                               ShaderCI;
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
			auto shaderpath = loader::Get(shader);
			m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(shaderpath.string().c_str(), &pShaderSourceFactory);
			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			ShaderCI.UseCombinedTextureSamplers = true;

			ShaderMacroHelper Macros;
			// clang-format on
			ShaderCI.Macros = Macros;

			ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
			ShaderCI.Desc.Name = "Mesh VS";
			ShaderCI.EntryPoint = "vert";
			ShaderCI.FilePath = shadername;

			m_pDevice->CreateShader(ShaderCI, &pVS);

			ShaderCI.Desc.Name = "Mesh PS";
			ShaderCI.EntryPoint = "frag";
			ShaderCI.FilePath = shadername;
			ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;

			m_pDevice->CreateShader(ShaderCI, &pPS);
		}
		RefCntAutoPtr<ITextureView>           m_RendeRT;
		RefCntAutoPtr<ITextureView>           m_RendeSRV;
		RefCntAutoPtr<ITextureView> m_pDepthDSV;
		RefCntAutoPtr<IBuffer>                m_pTransAttribsBuffer;
		RefCntAutoPtr<IBuffer>                m_paniinsBuffer;
		RefCntAutoPtr<IBuffer>                m_pinspropertysBuffer;
		RefCntAutoPtr<IEngineFactory>              m_pEngineFactory;
		RefCntAutoPtr<IBuffer>                             m_CameraAttribsCB;
		RefCntAutoPtr<IBuffer>                             m_LightAttribsCB;

		InputController m_InputController;
		Camera m_Camera;
		bool ProcessCommandLine(const char* CmdLine)
		{
			const auto* Key = "-mode ";
			const auto* pos = strstr(CmdLine, Key);
			if (pos != nullptr)
			{
				pos += strlen(Key);
				if (_stricmp(pos, "D3D11") == 0)
				{
#if D3D11_SUPPORTED
					m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
#else
					std::cerr << "Direct3D11 is not supported. Please select another device type";
					return false;
#endif
				}
				else if (_stricmp(pos, "D3D12") == 0)
				{
#if D3D12_SUPPORTED
					m_DeviceType = RENDER_DEVICE_TYPE_D3D12;
#else
					std::cerr << "Direct3D12 is not supported. Please select another device type";
					return false;
#endif
				}
				else if (_stricmp(pos, "GL") == 0)
				{
#if GL_SUPPORTED
					m_DeviceType = RENDER_DEVICE_TYPE_GL;
#else
					std::cerr << "OpenGL is not supported. Please select another device type";
					return false;
#endif
				}
				else if (_stricmp(pos, "VK") == 0)
				{
#if VULKAN_SUPPORTED
					m_DeviceType = RENDER_DEVICE_TYPE_VULKAN;
#else
					std::cerr << "Vulkan is not supported. Please select another device type";
					return false;
#endif
				}
				else
				{
					std::cerr << "Unknown device type. Only the following types are supported: D3D11, D3D12, GL, VK";
					return false;
				}
				}
			else
			{
#if D3D12_SUPPORTED
				m_DeviceType = RENDER_DEVICE_TYPE_D3D12;
#elif VULKAN_SUPPORTED
				m_DeviceType = RENDER_DEVICE_TYPE_VULKAN;
#elif D3D11_SUPPORTED
				m_DeviceType = RENDER_DEVICE_TYPE_D3D11;
#elif GL_SUPPORTED
				m_DeviceType = RENDER_DEVICE_TYPE_GL;
#endif
			}
			return true;
			}

		struct CameraAttribs
		{
			using float4 = math::Vector4f;
			using float2 = math::Vector2f;
			using float4x4 = math::Matrix4f;
			float4 f4Position;     // Camera world position
			float4 f4ViewportSize; // (width, height, 1/width, 1/height)

			float2 f2ViewportOrigin; // (min x, min y)
			float fNearPlaneZ;
			float fFarPlaneZ; // fNearPlaneZ < fFarPlaneZ

#ifdef __cplusplus
			float4x4 mViewT;
			float4x4 mProjT;
			float4x4 mViewProjT;
			float4x4 mViewInvT;
			float4x4 mProjInvT;
			float4x4 mViewProjInvT;
#else
			matrix mView;
			matrix mProj;
			matrix mViewProj;
			matrix mViewInv;
			matrix mProjInv;
			matrix mViewProjInv;
#endif

			float4 f4ExtraData[5]; // Any appliation-specific data
			// Sizeof(CameraAttribs) == 256*2
	};
		struct instData
		{
			using float4 = Diligent::float4;
			uint bonelength;
			uint frameIndex;
			uint transformid;
			uint base;
			// float transitionProgress;
			float4 color;
		};
		//InstanceRender* irr = nullptr;
		using DbgRender = n_Render::debug;
		std::unique_ptr<DbgRender> debugDraw;
		App0 app0;
		std::vector<instData> squareinst;

		shpso::Visible_f _vcull;
		Audio::AudioManager amanager;
		void CreateResources()
		{
			const auto RenderTargetFormat = m_pSwapChain->GetDesc().ColorBufferFormat;
			const auto DepthBufferFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
			RenderContext rt;
			rt.m_pDevice = m_pDevice;
			rt.m_pImmediateContext = m_pImmediateContext;
			rt.pShaderSourceFactory = pShaderSourceFactory;
			rt.pCameraAttribs = m_CameraAttribsCB;
			rt.m_pSwapChain = m_pSwapChain;
			{
				geomatrys::Init(rt);
			}
			{
				ResManager& rm_ = ResManager::Get();
				loader::Build(rm_, rt);//dispose all resource held
				auto pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
				auto pDTV = m_pSwapChain->GetDepthBufferDSV();
				rt.RenderTargetFormat = RenderTargetFormat;
				rt.DepthBufferFormat = DepthBufferFormat;
				rt.m_RendeSRV = this->m_RendeSRV;
				rt.m_pDepthDSV = this->m_pDepthDSV;

				Audio::AudioManager& amanager = rm_.am;
				amanager.cb_camera = rt.pCameraAttribs;
				amanager.init(rt, 12288);
				app0.SetData(&tst);
				amanager.LoadAudios(loader::Get(Paths::audioclip).string());
				app0.init(rt);
			}
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
			auto shaderpath = loader::Get(shader);
			m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(shaderpath.string().c_str(), &pShaderSourceFactory);
			
			debugDraw.reset(new DbgRender(m_pDevice, m_pImmediateContext,
				RenderTargetFormat, m_pSwapChain->GetDesc().DepthBufferFormat,
				m_CameraAttribsCB, pShaderSourceFactory, pTabTex));
			std::vector< sStruct::VFoutArgs> vfargs;
			return;
		}
		void fixupdate() {
		}
		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		RefCntAutoPtr<IBuffer>                m_CubeVertexBuffer;
		RefCntAutoPtr<IBuffer>                m_CubeIndexBuffer;
		std::vector<sStruct::objrep> lamp;
		using t_clock = std::chrono::high_resolution_clock;
		double m_CurrentTime, frame_accumulator = 0;
		t_clock::time_point prev_frame_time;
		float  m_fSmoothFPS = 0;
		double m_LastFPSTime = 0;
		Uint32 m_NumFramesRendered = 0;
		Uint32 m_CurrentFrameNumber = 0;
		bool m_ShowDemoWindow = true;
		int maxIns = 1000;
		double lastclick = -1;
		math::Efloat3 v2 = { NAN,NAN ,NAN };
		//std::vector<int> _cmdres;
		phmap::parallel_flat_hash_map<int, int> _cmddeltmap;
		void Update(double CurrTime = 0, double ElapsedTime = 0) {
			if (false)
			{
				const auto now = t_clock::now();
				const auto delta_time = std::chrono::duration<double, std::milli>(now - prev_frame_time).count();
				const auto consumedDeltaTime = delta_time;
				frame_accumulator += delta_time;
				//current_frame_time = std::chrono::duration<double>(now);
				prev_frame_time = now;
				const auto desired_frametime = GameProps::desired_frametime;
				ui64 maxiter = 2;
				while (frame_accumulator >= desired_frametime && --maxiter > 0) {
					fixupdate();
					frame_accumulator -= desired_frametime;
				}
			}
			const auto& SCDesc = m_pSwapChain->GetDesc();
			const auto& pattr = m_Camera.GetProjAttribs();
			m_Camera.Update(m_InputController, static_cast<float>(ElapsedTime));
			{
				MapHelper<CameraAttribs> CamAttribs(m_pImmediateContext, m_CameraAttribsCB, MAP_WRITE, MAP_FLAG_DISCARD);
				(CamAttribs->mProjT) = (m_Camera.GetProjMatrix().transpose());
				CamAttribs->mViewProjT = m_Camera.CameraViewProj;// .transpose();
				CamAttribs->mViewProjInvT = m_Camera.CameraViewProj.inverse();
				CamAttribs->f4Position = m_Camera.GetWorldMatrix().col(3);// Diligent::float4(Diligent::float3::MakeVector(cam.GetWorldMatrix()[3]), 1);
				CamAttribs->f4Position[3] = 1;
				f2 wh = { (float)pattr.Width,(float)pattr.Height };
				CamAttribs->f4ViewportSize.head<2>() = wh;
				CamAttribs->f4ViewportSize.tail<2>() = 1/wh.array();
			}
			m_CurrentTime = CurrTime;
			static int posmapsize = 0;
			const auto& mouseState = m_InputController.GetMouseState();
			static double ClickCounter = -9;
			using f3 = math::Efloat3;
			using f4 = math::Vector4f;
			const auto& m_ProjAttribs = m_Camera.m_ProjAttribs;
			const float w = m_ProjAttribs.Width, h = m_ProjAttribs.Height;
			const Diligent::float2 screen_pos(400, 100);
			const float nx = 2 * (screen_pos.x / w) - 1;
			const float ny = 1 - 2 * ((screen_pos.y) / h);
			auto inverted = (m_Camera.CameraViewProj).inverse();

			if (m_pImGui)
			{
				const auto& SCDesc = m_pSwapChain->GetDesc();
				m_pImGui->NewFrame(SCDesc.Width, SCDesc.Height, SCDesc.PreTransform);
			}
			app0.Update();
			if (m_pDevice)
			{
				++m_NumFramesRendered;
				++m_CurrentFrameNumber;
				static const double dFPSInterval = 0.5;
				if (CurrTime - m_LastFPSTime > dFPSInterval)
				{
					m_fSmoothFPS = static_cast<float>(m_NumFramesRendered / (CurrTime - m_LastFPSTime));
					m_NumFramesRendered = 0;
					m_LastFPSTime = CurrTime;
				}
				if(debug)
				{

					static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
					static float snap[3] = { 1.f, 1.f, 1.f };
					debugDraw->DrawUI();
					(ImGui::Begin("-(=|=)"));
					if (ImGui::Button("reset")) {
						//tst = tst.Identity();
						m_Camera.SetPos(Diligent::float3(0, 0, 0));
					}
					static math::int4 bkpara = { 100,100,0,0 };
					static math::int4 bkpara0 = { 100,100,0,100 };
					ImGui::DragInt4("Block para", bkpara.data(), 1);
					ImGui::DragInt4("Block para0", bkpara0.data(), 1);
					ImGui::InputFloat3("cam pos", (m_Camera.m_Pos.data()));
					ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
					ImGui::Checkbox("Gizemo", &m_ShowDemoWindow); // Edit bools storing our window open/close state
					//ImGuizmo::SetOrthographic(true);
					ImGuizmo::BeginFrame();
					//ImGuizmo::DrawGrid(m_Camera.GetViewMatrix().Data(), m_Camera.GetProjMatrix().Data(), Diligent::float4x4::RotationX(PI / 2).Data(), 100.f);
					ImGui::DragFloat3("m_ReferenceRightAxis", m_Camera.m_ReferenceRightAxis.data());
					ImGui::DragFloat3("m_ReferenceUpAxis", m_Camera.m_ReferenceUpAxis.data());
					ImGui::DragFloat3("m_ReferenceAheadAxis", m_Camera.m_ReferenceAheadAxis.data());
					ImGui::DragFloat("m_fYawAngle", &m_Camera.m_fYawAngle, .5);
					ImGui::DragFloat("m_fPitchAngle", &m_Camera.m_fPitchAngle, .5);
					for (size_t i = 0; i < 4; i++)
					{
						ImGui::DragFloat4(("m" + std::to_string(i)).c_str(), tst.col(i).data());
					}
					if(m_ShowDemoWindow)
					EditTransform(m_Camera, tst.data());
					ImGui::End();
		}
}
		}
		ITexture* pTabTex = NULL;
		bool m_PlayAnimation = false;
		int frame = 0, m_AnimationIndex = 0;
		math::Matrix4f tst;

		int Currinstancenum = 0;
		int time = 0;
		void Render()
		{

			// Note that Present() unbinds the back buffer if it is set as render target.
			const float ClearColor[] = { 0.350f, 0.350f, 0.350f, 1.0f };
			//auto& pRTV = m_RendeRT; //m_pSwapChain->GetCurrentBackBufferRTV();
			auto pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
			//auto pRTV0 = m_pSwapChain->GetCurrentBackBufferRTV();
			auto pDSV = m_pSwapChain->GetDepthBufferDSV();
			auto df = pDSV->GetTexture()->GetDesc();
			// Set render targets before issuing any draw command.
			m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

			// Clear the back buffer
			m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

			time++;
			debugDraw->Render();
			app0.render();
			{
				(*m_pImGui).Render(m_pImmediateContext); 
			}
		}

		void Present()
		{
			m_pSwapChain->Present();
		}
		void WindowResize(Uint32 Width, Uint32 Height)
		{
			if (!m_pSwapChain)
				return;
			m_pSwapChain->Resize(Width, Height);

			auto SCDesc = m_pSwapChain->GetDesc();

			const auto PI_F = math::PI_F;
			float NearPlane = 0.1f;
			float FarPlane = 1000.f;
			//float AspectRatio = static_cast<float>(SCDesc.Width) / static_cast<float>(SCDesc.Height);
			m_Camera.SetProjAttribs(NearPlane, FarPlane, SCDesc.Width, SCDesc.Height, PI_F / 4.f,
				m_pSwapChain->GetDesc().PreTransform, false);

		}

		RENDER_DEVICE_TYPE GetDeviceType() const {
			return m_DeviceType;
		}

		static void GLFW_ResizeCallback(GLFWwindow* wnd, int w, int h)
		{
			auto* pSelf = static_cast<App*>(glfwGetWindowUserPointer(wnd));
			if (pSelf->m_pSwapChain != nullptr)
				pSelf->m_pSwapChain->Resize(static_cast<Uint32>(w), static_cast<Uint32>(h));
		}

		static void GLFW_KeyCallback(GLFWwindow* wnd, int key, int scan, int state, int modf)
		{
			auto* pSelf = static_cast<App*>(glfwGetWindowUserPointer(wnd));
			pSelf->_imgr.KeyCallback(key,scan,state,modf);
		}

		static void GLFW_MouseButtonCallback(GLFWwindow* wnd, int button, int state, int mdf)
		{
			auto* pSelf = static_cast<App*>(glfwGetWindowUserPointer(wnd));
			pSelf->_imgr.MouseButtonCallback(button,state,mdf);
		}

		static void GLFW_CursorPosCallback(GLFWwindow* wnd, double xpos, double ypos)
		{
			float xscale = 1;
			float yscale = 1;
			glfwGetWindowContentScale(wnd, &xscale, &yscale);
			auto* pSelf = static_cast<App*>(glfwGetWindowUserPointer(wnd));
			pSelf->_imgr.CursorPosCallback(math::f2(static_cast<float>(xpos * xscale), static_cast<float>(ypos * yscale)));
		}
	private:
		inputmanager _imgr;
		MapHelper<sStruct::VfProp> instancedata;
		std::atomic_size_t insCounter = 0;
		RefCntAutoPtr<IRenderDevice> m_pDevice;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
		RefCntAutoPtr<ISwapChain> m_pSwapChain;
		RefCntAutoPtr<IPipelineState> m_pPSO;
		RefCntAutoPtr<IPipelineState> m_uiPSO;
		std::vector<IDeviceContext*> ppContexts;
		RENDER_DEVICE_TYPE m_DeviceType = RENDER_DEVICE_TYPE_VULKAN;
	};
}
using cage::App;
std::unique_ptr<App> g_pTheApp;
LRESULT CALLBACK MessageProc(HWND, UINT, WPARAM, LPARAM);
// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}
int   CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int cmdShow)
{
	spdlog::shutdown();
	spdlog::set_default_logger(spdlog::stdout_color_mt("console"));
#if defined(_DEBUG) || defined(DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Register our window class
	g_pTheApp.reset(new App);

	const auto* cmdLine = GetCommandLineA();
	if (!g_pTheApp->ProcessCommandLine(cmdLine))
		return -1;
	/*uint r = -1;
	auto m1 = aim4 * aim4;
	Matrix4f m4 = Map<Matrix4f>(r1);
	Matrix4f m2 = m4 * m4;*/
	using namespace Diligent;
	std::wstring Title(L"pj22");
	switch (g_pTheApp->GetDeviceType())
	{
	case RENDER_DEVICE_TYPE_D3D11:
		Title.append(L" (D3D11)");
		break;
	case RENDER_DEVICE_TYPE_D3D12:
		Title.append(L" (D3D12)");
		break;
	case RENDER_DEVICE_TYPE_GL:
		Title.append(L" (GL)");
		break;
	case RENDER_DEVICE_TYPE_VULKAN:
		Title.append(L" (VK)");
		break;
	}

	GLFWwindow* m_Window = nullptr;
	if (glfwInit() != GLFW_TRUE)
		return false;
	int GlfwApiHint = 0;
	glfwWindowHint(GLFW_CLIENT_API, GlfwApiHint);

	// Register our window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, NULL,//MessageProc,
					   0L, 0L, instance, NULL, NULL, NULL, NULL, L"SampleApp", NULL };
	RegisterClassEx(&wcex);

	// Create a window
	LONG WindowWidth = 1280;
	LONG WindowHeight = 1024;
	m_Window = glfwCreateWindow(WindowWidth, WindowHeight, utf8_encode(Title).c_str(), nullptr, nullptr);
	/*RECT rc = { 0, 0, WindowWidth, WindowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND wnd = CreateWindow(L"SampleApp", Title.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);
	if (!wnd)
	{
		MessageBox(NULL, L"Cannot create window", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(wnd, cmdShow);


	UpdateWindow(wnd);*/

	if (!g_pTheApp->InitializeDiligentEngine(m_Window))
		return -1;

	g_pTheApp->CreateResources();

	// Main message loop
	MSG msg = { 0 };
	Diligent::Timer Timer;

	auto   PrevTime = Timer.GetElapsedTime();
	double filteredFrameTime = 0.0;
	ImGuiContext& g = (*ImGui::GetCurrentContext());
	ImGuiIO& io = g.IO;
	io.WantCaptureMouse = true;
	int fk = 0;
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			auto CurrTime = Timer.GetElapsedTime();
			auto ElapsedTime = CurrTime - PrevTime;
			PrevTime = CurrTime;

			glfwPollEvents();
			g_pTheApp->Update(CurrTime, ElapsedTime);
			g_pTheApp->Render();
			g_pTheApp->Present();
			fk++;
		}
	}

	g_pTheApp.reset();

	return (int)msg.wParam;
}

using namespace Diligent;
// Called every time the NativeNativeAppBase receives a message
LRESULT CALLBACK MessageProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Key::Enum key;
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(wnd, &ps);
		EndPaint(wnd, &ps);
		return 0;
	}
	//case WM_SIZE: // Window size has been changed
	//	if (g_pTheApp)
	//	{
	//		g_pTheApp->WindowResize(LOWORD(lParam), HIWORD(lParam));
	//	}
	//	return 0;

	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		key = translateKey(wParam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

		lpMMI->ptMinTrackSize.x = 320;
		lpMMI->ptMinTrackSize.y = 240;
		return 0;
	}
	case WM_KEYDOWN: {

		key = translateKey(wParam);
		return 0;
	}
	case WM_SYSKEYUP:
	{
		key = translateKey(wParam);

		return 0;
	}

	default:
		return DefWindowProc(wnd, message, wParam, lParam);
	}
}