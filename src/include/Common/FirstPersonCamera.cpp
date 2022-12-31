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

#include"PCH.h"
#include "FirstPersonCamera.hpp"
#include <algorithm>
#include"io/commandin.h"
namespace pj2
{
	auto Camera::GetReferenceRotiation()const
	{

		// clang-format off
		m3f mtx;
		mtx <<
			m_ReferenceRightAxis[0], m_ReferenceUpAxis[0], m_ReferenceAheadAxis[0],// 0,
			m_ReferenceRightAxis[1], m_ReferenceUpAxis[1], m_ReferenceAheadAxis[1],// 0,
			m_ReferenceRightAxis[2], m_ReferenceUpAxis[2], m_ReferenceAheadAxis[2];// 0,
		//0, 0, 0, 1;

	/*<<
	 m_ReferenceRightAxis[0], m_ReferenceUpAxis[0], m_ReferenceAheadAxis[0], 0,
	 m_ReferenceRightAxis[1], m_ReferenceUpAxis[1], m_ReferenceAheadAxis[1], 0,
	 m_ReferenceRightAxis[2], m_ReferenceUpAxis[2], m_ReferenceAheadAxis[2], 0,
						  0,                   0,                      0, 1;*/

		return math::AngleAxisf(mtx.transpose());
		// clang-format on
	}
	auto AngleAxis(const Diligent::float3 ax, const float angle) {
		return math::AngleAxisf(angle, reinterpret_cast<const Eigen::Vector3f&>(ax).normalized());
	}
	auto AngleAxis(const math::Efloat3 ax, const float angle) {
		return math::AngleAxisf(angle, ax.normalized());
	}

#define M4t3(m4) m4.block<3, 3>(0,0)
	//Eigen::Ref<math::Matrix3f> m423(Matrix4f& mi) { return mi.block<0, 0>(3, 3); }
	//Camera* Camera::mainCam(NULL);
	void Camera::Update(Diligent::InputController& Controllerx, float ElapsedTime)
	{
		using namespace IOin;
		using MST = mousestate;
		inputmanager& Controller = inputmanager::main();
		math::Vector4f MoveDirection(0, 0, 0, 0);
		{
			auto& x = MoveDirection[0];
			auto& y = MoveDirection[1];
			auto& z = MoveDirection[2];
			// Update acceleration vector based on keyboard state
			if (Controller.IsKeyDown(Controller.kmap.MoveForward))
				z += 1.0f;
			if (Controller.IsKeyDown(Controller.kmap.MoveBackward))
				z -= 1.0f;

			if (Controller.IsKeyDown(Controller.kmap.MoveRight))
				x += 1.0f;
			if (Controller.IsKeyDown(Controller.kmap.MoveLeft))
				x -= 1.0f;

			if (Controller.IsKeyDown(Controller.kmap.MoveUp))
				y += 1.0f;
			if (Controller.IsKeyDown(Controller.kmap.MoveDown))
				y -= 1.0f;
		}

		// Normalize vector so if moving in 2 dirs (left & forward),
		// the camera doesn't move faster than if moving in 1 dir
		auto len = (MoveDirection).squaredNorm();
		if (len != 0.0)
			MoveDirection /= len;
		bool IsSpeedUpScale = Controller.IsKeyDown<Key::KEY_LEFT_SHIFT, Key::KEY_RIGHT_SHIFT>();
		bool IsSuperSpeedUpScale = Controller.IsKeyDown<Key::KEY_LEFT_CONTROL, Key::KEY_RIGHT_CONTROL>();
		MoveDirection *= m_fMoveSpeed;
		if (IsSpeedUpScale) MoveDirection *= m_fSpeedUpScale;
		if (IsSuperSpeedUpScale) MoveDirection *= m_fSuperSpeedUpScale;

		m_fCurrentSpeed = (MoveDirection).squaredNorm();

		auto PosDelta = MoveDirection * ElapsedTime;
		const auto& mouseState = Controller.GetMouseState();
		new (&m_LastMouseState)MST(mouseState);
		if (IsSpeedUpScale)
		{
			f2 mousedelta;
			float MouseDeltaX = 0;
			float MouseDeltaY = 0;
			/* if ((m_LastMouseState.CursorPos.array()>=0).any() &&
				 m_LastMouseState.key != 0)
			 {
				 mousedelta = mouseState.CursorPos - m_LastMouseState.CursorPos;
				 MouseDeltaX = mouseState.PosX - m_LastMouseState.PosX;
				 MouseDeltaY = mouseState.PosY - m_LastMouseState.PosY;
			 }*/

			if (mouseState.key & (int)mousestate::BUTTON_FLAG_LEFT)
			{
				mousedelta = mouseState.CursorPos - m_LastMouseState.CursorPos;
				mousedelta *= m_fRotationSpeed;
				const float& fYawDelta = mousedelta[0];
				const float& fPitchDelta = mousedelta[1];
				m_fYawAngle += fYawDelta * -m_fHandness;
				m_fPitchAngle += fPitchDelta * -m_fHandness;
				m_fPitchAngle = std::max(m_fPitchAngle, -PI_F / 2.f);
				m_fPitchAngle = std::min(m_fPitchAngle, +PI_F / 2.f);
			}
		}

		auto ReferenceRotation = GetReferenceRotiation();

		///m4f CameraRotation=m4f::Identity();
		  /*  auto CameraRotation = (AngleAxis(m_ReferenceUpAxis, m_fYawAngle) *
			AngleAxis(m_ReferenceRightAxis, m_fPitchAngle) *
			math::AngleAxisf(ReferenceRotation)).matrix();*/

		auto CameraRotation =
			((ReferenceRotation))//.matrix()
			* AngleAxis(m_ReferenceRightAxis, m_fPitchAngle) *
			AngleAxis(m_ReferenceUpAxis, m_fYawAngle);
		//M4t3(CameraRotation) = m3r;// .transpose();
		auto WorldRotation = CameraRotation.inverse();// .matrix().transpose();
		auto PosDeltaWorld = WorldRotation * PosDelta.head<3>();
		m_Pos += (PosDeltaWorld);
		auto tmx = math::Translation3f(-m_Pos);
		m_ViewMatrix = (Eigen::Affine3f(CameraRotation) * tmx).matrix() * SrfPreTransform;
		m_WorldMatrix = (math::Affine3f(WorldRotation) * math::Translation3f(m_Pos)).matrix();
		CameraViewProj = (m_ProjMatrix * m_ViewMatrix.matrix());// .transpose();
	}


	//inline void Camera::getRay(const math::Efloat4& screen_pos, math::Efloat3& origin, math::Efloat3& dir) const
	//{
	//    origin = (m_Pos);
	//    const float w = m_ProjAttribs.Width, h = m_ProjAttribs.Height;
	//    if (m_ProjAttribs.AspectRatio <= 0) {
	//        std::abort();
	//        //dir = rot.rotate(Vec3(0, 0, 1));
	//        return;
	//    }
	//
	//    const float nx = 2 * (screen_pos[0] / w) - 1;
	//    const float ny = 1 - 2 * ((screen_pos[1]) / h);
	//
	//    /*if (is_ortho) {
	//    const Vec3 x = rot * Vec3(1, 0, 0);
	//    const Vec3 y = rot * Vec3(0, 1, 0);
	//    float ratio = h > 0 ? w / (float)h : 1;
	//    origin += x * nx * ortho_size * ratio
	//    + y * ny * ortho_size;
	//    }*/
	//
	//    //const auto view_matrix = m_ViewMatrix;
	//    auto inverted = (CameraViewProj).inverse();
	//
	//    auto p0 = (inverted * Efloat4(nx, ny, 0, 1));
	//    auto p1 = inverted * Efloat4(nx, ny, 1, 1);
	//
	//    auto sc = 1 / p0[3];
	//    //p0*= sc;
	//    /*p1 *= 1 / p1.w;*/
	//    //origin = p0;
	//    dir = ((p0 * sc).head<3>() - origin).normalized();
	//    //if (is_ortho) dir *= -1.f;
	//}


	//void getRay(const math::Efloat4& screen_pos, math::Efloat3& origin, math::Efloat3& dir) const;

	inline void Camera::getRay(const math::Efloat4& screen_pos, math::Efloat4& origin, math::Efloat4& dir) const
	{
		origin.head<3>() = (m_Pos);
		const float w = m_ProjAttribs.Width, h = m_ProjAttribs.Height;
		if (m_ProjAttribs.AspectRatio <= 0) {
			std::abort();
			//dir = rot.rotate(Vec3(0, 0, 1));
			return;
		}

		const float nx = 2 * (screen_pos[0] / w) - 1;
		const float ny = 1 - 2 * ((screen_pos[1]) / h);

		/*if (is_ortho) {
		const Vec3 x = rot * Vec3(1, 0, 0);
		const Vec3 y = rot * Vec3(0, 1, 0);
		float ratio = h > 0 ? w / (float)h : 1;
		origin += x * nx * ortho_size * ratio
		+ y * ny * ortho_size;
		}*/

		//const auto view_matrix = m_ViewMatrix;
		auto inverted = (CameraViewProj).inverse();

		auto p0 = (inverted * Efloat4(nx, ny, 0, 1));
		auto p1 = inverted * Efloat4(nx, ny, 1, 1);

		auto sc = 1 / p0[3];
		//p0*= sc;
		/*p1 *= 1 / p1.w;*/
		//origin = p0;
		dir.head<3>() = ((p0 * sc) - origin).head<3>().normalized();
		//if (is_ortho) dir *= -1.f;
	}

	void Camera::SetReferenceAxes(const math::Efloat3& ReferenceRightAxis, const math::Efloat3& ReferenceUpAxis, bool IsRightHanded)
	{
		m_ReferenceRightAxis = math::normalize(ReferenceRightAxis);
		m_ReferenceUpAxis = ReferenceUpAxis - dot(ReferenceUpAxis, m_ReferenceRightAxis) * m_ReferenceRightAxis;
		auto            UpLen = (m_ReferenceUpAxis).squaredNorm();
		constexpr float Epsilon = 1e-5f;
		if (UpLen < Epsilon)
		{
			UpLen = Epsilon;
			LOG_WARNING_MESSAGE("Right and Up axes are collinear");
		}
		m_ReferenceUpAxis /= UpLen;

		m_fHandness = IsRightHanded ? +1.f : -1.f;
		m_ReferenceAheadAxis = m_fHandness * cross(m_ReferenceRightAxis, m_ReferenceUpAxis);
		auto AheadLen = (m_ReferenceAheadAxis).squaredNorm();
		if (AheadLen < Epsilon)
		{
			AheadLen = Epsilon;
			LOG_WARNING_MESSAGE("Ahead axis is not well defined");
		}
		m_ReferenceAheadAxis /= AheadLen;
	}

	void Camera::SetLookAt(const math::Efloat3& LookAt)
	{
		math::Efloat3 ViewDir = LookAt - m_Pos;

		ViewDir = GetReferenceRotiation() * ViewDir;

		m_fYawAngle = atan2f(ViewDir[0], ViewDir[2]);

		float fXZLen = sqrtf(ViewDir[2] * ViewDir[2] + ViewDir[0] * ViewDir[0]);
		m_fPitchAngle = -atan2f(ViewDir[1], fXZLen);
	}

	void Camera::SetRotation(float Yaw, float Pitch)
	{
		m_fYawAngle = Yaw;
		m_fPitchAngle = Pitch;
	}

	void Camera::SetProjAttribs(Float32 NearClipPlane, Float32 FarClipPlane, math::uint Width, math::uint Height, Float32 FOV, Diligent::SURFACE_TRANSFORM SrfPreTransform, bool IsGL)
	{
		m_ProjAttribs.NearClipPlane = NearClipPlane;
		m_ProjAttribs.FarClipPlane = FarClipPlane;
		m_ProjAttribs.AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
		m_ProjAttribs.FOV = FOV;
		m_ProjAttribs.Width = Width;
		m_ProjAttribs.Height = Height;
		m_ProjAttribs.PreTransform = SrfPreTransform;
		m_ProjAttribs.IsGL = IsGL;

		float XScale, YScale;
		if (SrfPreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_90 ||
			SrfPreTransform == Diligent::SURFACE_TRANSFORM_ROTATE_270 ||
			SrfPreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90 ||
			SrfPreTransform == Diligent::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270)
		{
			// When the screen is rotated, vertical FOV becomes horizontal FOV
			XScale = 1.f / std::tan(FOV / 2.f);
			// Aspect ratio is width/height accounting for pretransform
			YScale = XScale / m_ProjAttribs.AspectRatio;
		}
		else
		{
			YScale = 1.f / std::tan(FOV / 2.f);
			XScale = YScale / m_ProjAttribs.AspectRatio;
		}
		using m4f = Diligent::float4x4;
		m4f Proj;
		Proj._11 = XScale;
		Proj._22 = YScale;
		Proj.SetNearFarClipPlanes(NearClipPlane, FarClipPlane, IsGL);

		m_ProjMatrix = Eigen::Map<Matrix4f>(m4f::Projection(m_ProjAttribs.FOV, m_ProjAttribs.AspectRatio, m_ProjAttribs.NearClipPlane, m_ProjAttribs.FarClipPlane, IsGL).Data());

	}

	void Camera::SetSpeedUpScales(Float32 SpeedUpScale, Float32 SuperSpeedUpScale)
	{
		m_fSpeedUpScale = SpeedUpScale;
		m_fSuperSpeedUpScale = SuperSpeedUpScale;
	}

} // namespace Diligent
