#pragma once

#include"Common/Math1.h"
#include"Common/Config.h"
#include"physics/physics.hpp"
#include "common/span.hpp"
//#include"AllType.h"
namespace pj2 {
	namespace AI {

		struct unitprop
		{
			//15 deg
			math::Fix::fix_s attackangulardis=math::PI/12;
		};
	}
	namespace Ai {
		using namespace nonstd;
		using namespace Eigen;
		using float3 = math::Efloat3;
		struct Boid
		{
			float CellRadius;
			float SeparationWeight;
			float AlignmentWeight;
			float TargetWeight;
			float ObstacleAversionDistance;
			float MoveSpeed;
		};
		struct BoidBehaviour
		{
			// Reference to the controller.
			//public BoidController controller;

			// Options for animation playback.
		  //  public float animationSpeedVariation = PsConst::dt;

			// Random seed.
			float SeparationWeight;

			// Caluculates the separation vector with a target.
			auto GetSeparationVector(math::Efloat3 const diff, const float neighborDist)
			{

				var diffLen = diff.norm();

				var scaler = math::Clamp01(1.0f - diffLen / neighborDist);
				return diff * (scaler / diffLen);
			}
			inline auto agdis(mtransform& trans, mtransform::v4& tgp ) {
				return math::acos(trans.froward().normalized().dot(tgp.head<3>().normalized()));
			}
			inline void LookAtDirection(mtransform::v4& direction, mtransform& trans,Collid& cd)
			{
				constexpr bool fixz = true;
				using Scalar = mtransform::v4::Scalar;
				using Vector3 = mtransform::v3;
				assert(direction.norm() > 0.001f);
				{
					const auto cos = math::acos(direction.head<3>().normalized().dot(trans.froward()));
					auto adis =std::abs(cos);
					//attention £º do not swap cross direction
					if(adis > (cd.MaxAngularVeocity * Physics::dt))
					cd.AngularVelocity =((trans.froward().cross(direction.head<3>())).normalized() * cd.MaxAngularVeocity);
					else
					{
						cd.AngularVelocity.setZero();
					}
				}
				//math::quaternion qdel;
				//mtransform::v3 v0 = direction.head<3>().normalized();
				//mtransform::v3 v1= trans.froward().head<3>().normalized();
				//Scalar c = v1.dot(v0);

				//// if dot == -1, vectors are nearly opposites
				//// => accurately compute the rotation axis by computing the
				////    intersection of the two planes. This is done by solving:
				////       x^T v0 = 0
				////       x^T v1 = 0
				////    under the constraint:
				////       ||x|| = 1
				////    which yields a singular value problem
				//if (c < Scalar(-1) + NumTraits<Scalar>::dummy_precision())
				//{
				//	c = numext::maxi(c, Scalar(-1));
				//	Matrix<Scalar, 2, 3> m; m << v0.transpose(), v1.transpose();
				//	JacobiSVD<Matrix<Scalar, 2, 3> > svd(m, ComputeFullV);
				//	Vector3 axis = svd.matrixV().col(2);

				//	Scalar w2 = (Scalar(1) + c) * Scalar(0.5);
				//	qdel.w() = sqrt(w2);
				//	qdel.vec() = axis * sqrt(Scalar(1) - w2);

				//}
				//Vector3 axis = v0.cross(v1);
				//Scalar s = sqrt((Scalar(1) + c) * Scalar(2));
				//Scalar invs = Scalar(1) / s;
				//qdel.vec() = axis * invs;
				//qdel.w() = s * Scalar(0.5);

			}
			/* mtransfrom* transform;
			 Collid* controller;*/
			BoidBehaviour(float SeparationWeight = 1.2) : SeparationWeight(SeparationWeight) {}
			template<typename Around>
			auto Update(mtransfrom& transform, Collid& controller, Around& around, Controller& gettc, const float3 target)
			{
				var& currentPosition = transform.pos;
				var& currentRotation = transform.rotation;

				// Current velocity randomized with noise.
			  //  var noise = Mathf.PerlinNoise(Time.time, noiseOffset) * 2.0f - 1.0f;
				var& velocity = controller.Velocity;// *(1.0f + noise * controller.velocityVariation);

				// Initializes the vectors.
				decltype(transform.pos) separation(0, 0, 0);
				decltype(controller.Velocity) alignment(0, 0, 0);
				var cohesion = alignment;
				const var avg = around.size() == 0 ? 0 : 1.0f / around.size();
				// Accumulates the vectors.
				for (size_t i = 0; i < around.size(); i++)
				{
					// auto& boid = around[i];
					const auto& [voxx, PhyObj_ptr] = around[i];
					auto& collider = PhyObj_ptr->collider;
					auto& atransfoem = *reinterpret_cast<Physics::PhysicsObject<>*>(PhyObj_ptr);

					auto spvec = transform.pos - atransfoem.pos;
					const auto dissq = (spvec).squaredNorm();
					if (dissq == 0)
						continue;
					const auto spdis = math::square(controller.ConvexR + collider.ConvexR) * SeparationWeight;

					separation += spvec * std::max((spdis / dissq), 0.0f);
					/*if (math::isNan(separation[0]))
						std::abort();*/
					alignment += collider.Velocity;
					cohesion += atransfoem.pos;
				}
				alignment *= avg;
				cohesion *= avg;
				(cohesion -= currentPosition).normalize();
				alignment = alignment - velocity;
				// Calculates a rotation from the vectors.
				velocity += separation + alignment + cohesion + (target - transform.pos).normalized();
				velocity = velocity.squaredNorm() > controller.MaxVelocitySq ? velocity.normalized() * math::sqrt(controller.MaxVelocitySq) : velocity;
				const auto targetrotation = currentRotation.FromTwoVectors(transform.froward(), velocity) * currentRotation;
				// var rotation = Quaternion.FromToRotation(Vector3.forward, direction.normalized);
				const auto adis = currentRotation.angularDistance(targetrotation);
				// Applys the rotation with interpolation
				const auto da = controller.MaxAngularVeocity * PhysicsStep;
				if (da < adis)
					transform.rotation.slerp(math::exp((adis - controller.MaxAngularVeocity * PhysicsStep) / adis), targetrotation);
				else
				{
					transform.rotation = targetrotation;
				}
			}
		};
	}
}