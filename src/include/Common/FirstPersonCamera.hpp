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

#pragma once

#include"Common/InputUtil.h"
#include "BasicMath.hpp"
#include "common/Math1.h"
#include "InputController.hpp"
#include "GraphicsTypes.h"

namespace pj2
{
	//using namespace Diligent;
	using namespace math;
	using Float32 = float;
	class Camera
	{
		using mapf3 = math::Map<const math::Vector3f>;
		using m4f = math::Matrix4f;
		using m3f = math::Matrix3f;
	public:
		static inline Camera* mainCam=NULL;
		static Camera& Main() {
			return *mainCam;
		}
		static inline void SetMain(Camera& ci) {
			mainCam=&ci;
		}
		Camera() { //Camera::mainCam ? Camera::mainCam: this;
		}
		void Update(Diligent::InputController& Controller, float ElapsedTime);
		void SetRotation(float Yaw, float Pitch);
		void SetLookAt(const math::Efloat3& LookAt);
		void SetMoveSpeed(float MoveSpeed) { m_fMoveSpeed = MoveSpeed; }
		void SetRotationSpeed(float RotationSpeed) { m_fRotationSpeed = RotationSpeed; }
		void SetPos(const math::Efloat3& Pos) { m_Pos = Pos; }
		void SetPos(const Diligent::float3& Pos) { m_Pos =mapf3(Pos.Data()); }
		void SetProjAttribs(Float32 NearClipPlane, Float32 FarClipPlane, math::uint Width, math::uint Height, Float32 FOV, Diligent::SURFACE_TRANSFORM SrfPreTransform, bool IsGL);
		// AspectRatio = width / height accounting for surface pretransform
		// (i.e. logical widht / logical height)
		void m_Camera(Float32           NearClipPlane,
			Float32           FarClipPlane,
			math::uint            Width,
			math::uint            Height,
			Float32           FOV,
			Diligent::SURFACE_TRANSFORM SrfPreTransform,
			bool              IsGL);
		void SetSpeedUpScales(Float32 SpeedUpScale, Float32 SuperSpeedUpScale);


		// clang-format off
		const math::Matrix4f& GetViewMatrix()  const { return m_ViewMatrix.matrix(); }
		const math::Matrix4f& GetWorldMatrix() const { return m_WorldMatrix; }
		const math::Matrix4f& GetProjMatrix()  const { return m_ProjMatrix; }

		math::Efloat3 GetWorldRight() const { return GetViewMatrix().row(0).head<3>(); }// { return math::Efloat3(m_ViewMatrix._11, m_ViewMatrix._21, m_ViewMatrix._31); }
		math::Efloat3 GetWorldUp()    const { return GetViewMatrix().row(1).head<3>(); }//{ return math::Efloat3(m_ViewMatrix._12, m_ViewMatrix._22, m_ViewMatrix._32); }
		math::Efloat3 GetWorldAhead() const { return GetViewMatrix().row(2).head<3>(); }//{ return math::Efloat3(m_ViewMatrix._13, m_ViewMatrix._23, m_ViewMatrix._33); }
		// clang-format on
		inline void getRay(const Diligent::float2& screen_pos, math::Efloat3& origin, math::Efloat3& dir) const
		{
			origin = (m_Pos);
			const float w = m_ProjAttribs.Width, h = m_ProjAttribs.Height;
			if (m_ProjAttribs.AspectRatio <= 0) {
				std::abort();
				//dir = rot.rotate(Vec3(0, 0, 1));
				return;
			}

			const float nx = 2 * (screen_pos.x / w) - 1;
			const float ny = 1-2 * ((screen_pos.y) / h);

			/*if (is_ortho) {
				const Vec3 x = rot * Vec3(1, 0, 0);
				const Vec3 y = rot * Vec3(0, 1, 0);
				float ratio = h > 0 ? w / (float)h : 1;
				origin += x * nx * ortho_size * ratio
					+ y * ny * ortho_size;
			}*/

			//const auto view_matrix = m_ViewMatrix;
			auto inverted = (CameraViewProj).inverse();

			auto p0 = (inverted * Efloat4(nx, ny,0,1));
			auto p1 = inverted *  Efloat4(nx, ny,1, 1);
			
			auto sc= 1/p0[3];
			//p0*= sc;
			/*p1 *= 1 / p1.w;*/
			//origin = p0;
			dir=((p0*sc).head<3>()-origin).normalized();
			//if (is_ortho) dir *= -1.f;
		}

		inline void getRay(const math::f2& screen_pos, math::Efloat4& origin, math::Efloat4& dir) const
		{
			origin.head<3>() = (m_Pos);
			const float w = m_ProjAttribs.Width, h = m_ProjAttribs.Height;
			if (m_ProjAttribs.AspectRatio <= 0)[[unlikely]] {
				std::abort();
				//dir = rot.rotate(Vec3(0, 0, 1));
				return;
			}
			const float nx = 2 * (screen_pos.x() / w) - 1;
			const float ny = 1 - 2 * ((screen_pos.y()) / h);
			auto inverted = (CameraViewProj).inverse();

			Efloat4 p0 = (inverted * Efloat4(nx, ny, 0, 1));
			auto p1 = inverted * Efloat4(nx, ny, 1, 1);
			auto sc = 1 / p0[3];
			dir.head<3>() = ((p0 * sc) - origin).head<3>().normalized();
			//if (is_ortho) dir *= -1.f;
		}
		//void getRay(const math::Efloat4& screen_pos, math::Efloat3& origin, math::Efloat3& dir) const;
		void getRay(const math::Efloat4& screen_pos, math::Efloat4& origin, math::Efloat4& dir) const;
		math::Efloat3 GetPos() const { return m_Pos; }
		float  GetCurrentSpeed() const { return m_fCurrentSpeed; }

		struct ProjectionAttribs
		{
			math::uint            Width = 0;
			math::uint            Height = 0;
			Float32           NearClipPlane = 1.f;
			Float32           FarClipPlane = 1000.f;
			Float32           AspectRatio = 1.f;
			Float32           FOV = math::PI/ 4.f;
			Diligent::SURFACE_TRANSFORM PreTransform = Diligent::SURFACE_TRANSFORM_IDENTITY;
			bool              IsGL = false;
		};
		const ProjectionAttribs& GetProjAttribs() { return m_ProjAttribs; }

		void SetReferenceAxes(const math::Efloat3& ReferenceRightAxis, const math::Efloat3& ReferenceUpAxis, bool IsRightHanded = false);

		void SetHandness(bool IsRightHanded)
		{
			m_fHandness = IsRightHanded ? +1.f : -1.f;
		}

		Eigen::Affine3f m_ViewMatrix;
		math::Matrix4f m_WorldMatrix;
		math::Matrix4f m_ProjMatrix;
		math::Matrix4f SrfPreTransform;
		math::Matrix4f CameraViewProj;
		math::Efloat3 m_ReferenceRightAxis = math::Efloat3{ 0, 1, 0 };
		math::Efloat3 m_ReferenceUpAxis = math::Efloat3{ 0, 0, 1 };
		math::Efloat3 m_ReferenceAheadAxis = math::Efloat3{ 1, 0, 0 };
		//protected:
		auto GetReferenceRotiation() const;

		ProjectionAttribs m_ProjAttribs;

		mousestate m_LastMouseState;


		math::Efloat3 m_Pos;

		float    m_fRotationSpeed = 0.01f;
		float    m_fMoveSpeed = 1.f;
		float    m_fCurrentSpeed = 0.f;

		float m_fYawAngle = 0; // Yaw angle of camera
		float m_fPitchAngle = 0; // Pitch angle of camera
		float m_fSpeedUpScale = 1.f;
		float m_fSuperSpeedUpScale = 1.f;
		float m_fHandness = 1.f; // -1 - left handed
										  // +1 - right handed
	};
} // namespace Diligent
