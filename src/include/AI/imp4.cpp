#pragma once
#include"Common/mathtable.h"
#include"imp4.h"
#include"Render/DrawTool.h"
#include"physics/physics.hpp"
#include"Gameprops0.h"
#include"GpropExt.h"
#pragma optimize( "", off )
#undef DEBUG 
namespace pj2::AI {
	using v4f=math::v4f;
	using mtransform = math::mtransform;
	mtransform::v3 AdjectV(mtransform::v4 const& newv_, mtransform::v3 const& cv_, float speed, float maxv, float maxav) {
		const auto&& newv = newv_.head<3>();
		const auto&& cv = cv_.head<3>();
		const float maxAdjustedSpeed = maxv * 0.3;
		auto cosdis = math::dot(newv, cv);
		if ((speed > maxAdjustedSpeed) || newv.isApproxToConstant(0, math::Epsilon))
		{
			return newv;
		}
		else
		{
			const float range = speed / maxAdjustedSpeed;
			const float cosine = math::interpolate(pow(range, 20), 1.0f, -1.0f);
			//return limitMaxDeviationAngle(force, cosine, forward());
			const auto& basis = cv;
			const auto& cosineOfConeAngle = cosine;
			const auto& source = newv;
			// immediately return zero length input vectors
			float sourceLength = source.norm();
			// measure the angular diviation of "source" from "basis"
			const auto direction = source / sourceLength;
			float cosineOfSourceAngle = math::dot(direction, basis);

			// Simply return "source" if it already meets the angle criteria.
			// (note: we hope this top "if" gets compiled out since the flag
			// is a constant when the function is inlined into its caller)
			if (true)
			{
				// source vector is already inside the cone, just return it
				if (cosineOfSourceAngle >= cosineOfConeAngle) return source;
			}
			else
			{
				// source vector is already outside the cone, just return it
				if (cosineOfSourceAngle <= cosineOfConeAngle) return source;
			}

			// find the portion of "source" that is perpendicular to "basis"
			mtransform::v3 perp;
			perp.setConstant(0);
			(math::rotate90(basis, perp));// math::rotate source.perpendicularComponent(basis);
			// normalize that perpendicular
			auto unitPerp = math::dot(perp, source) > 0 ? perp.normalized() : -perp.normalized();

			// construct a new vector whose length equals the source vector,
			// and lies on the intersection of a plane (formed the source and
			// basis vectors) and a cone (whose axis is "basis" and whose
			// angle corresponds to cosineOfConeAngle)
			float perpDist = math::sqrt(1 - (cosineOfConeAngle * cosineOfConeAngle));
			auto c0 = basis * cosineOfConeAngle;
			auto c1 = (unitPerp)*perpDist;
			auto vh = (c0 + c1) * sourceLength;
			if (vh.squaredNorm() > (maxv * maxv)) {
				return vh.normalized() * maxv;
			}
			return vh;
		}
	}
	inline auto ProjectClamp(const auto& targetpoint, const auto& b, const auto& c, float& percent)
	{
		var cb = b - c;
		auto ca = targetpoint - c;
		var dot = math::dot(cb, ca);
		//var projleg = dot / cb.magnitude;
		percent = dot / cb.squaredNorm();
		return  c + std::clamp<float>(percent, 0, 1) * cb;
	}

	auto steerForSeparationlamd = [](
		auto& flock, auto& cmd, auto& dtstep, auto&itrs ,
		float bufdis_ = 0.01f)
	{
		using State = GameProps::state;
		// steering accumulator and count of neighbors, both initially zero
		mtransform::v4 steering, virtualpos;
		steering.setConstant(0);
		int neighbors = 0;
		if (cmd.cState() != State::idle)
			for (size_t i = 0; i < flock.size(); i++)
			{
				auto& flk = flock[i];
				virtualpos = flk.relativePosition + flk.relativeVelocity * dtstep;
				//flk.relativeVelocity.head<3>() *= cmd.cState() == State::idle ? 0 : 1;
				{
					// add in steering contribution
					// (opposite of the offset direction, divided once by distance
					// to normalize, divided another time to get 1/d falloff)
					const auto dis = (virtualpos.head<3>().squaredNorm() - (flk.combinedRadiussq()));
					if (dis < 0)
						steering += -(flk.relativePosition);// *math::exp(math::min((dis * -32), 10)));

					// count neighbors
					++neighbors;
				}
			}
		//return steering;
		else {

			for (size_t i = 0; i < flock.size(); i++)
			{
				auto& flk = flock[i];
				virtualpos = flk.relativePosition + flk.desiv * dtstep;
				//flk.relativeVelocity.head<3>() *= cmd.cState() == State::idle ? 0 : 1;
				{
					// add in steering contribution
					// (opposite of the offset direction, divided once by distance
					// to normalize, divided another time to get 1/d falloff)

					const auto dis = (virtualpos.head<3>().squaredNorm() - (flk.combinedRadiussq() + bufdis_));
					//const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
					if (dis < 0)
						steering += -(flk.relativePosition.normalized() * 3);// *math::exp(math::min((dis * -32), 10)));

					// count neighbors
					++neighbors;
				}
			}
		}
		return steering;

		for (size_t i = 0; i < flock.size(); i++)
		{
			auto& flk = flock[i];
			flk.relativeVelocity.head<3>() *= cmd.cState() == State::idle ? 0 : 1;
			if (flk.st != State::idle)
			{
				// add in steering contribution
				// (opposite of the offset direction, divided once by distance
				// to normalize, divided another time to get 1/d falloff)
				const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
				if (dis < 0)
					steering += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));

				// count neighbors
				++neighbors;
			}
		}

		return steering;
	};

	auto drawline = [](const v4f& a, const v4f& b, v4f color = { 0,1,0,1 }, v4f color1 = { 0,1,0,1 }) {
		std::array<math::v4f, 4> linepara;
		linepara[0] = a;
		linepara[1] = b;
		linepara[2] = color;
		pj2::n_Render::execmd<0>(linepara);
	};
	auto steerForSeparationf = [](
		auto& flock, auto& cmd, auto& dtstep, auto& itrs,auto&&func,
		float bufdis_ = 0.01f)
	{
		using State = GameProps::state;
		// steering accumulator and count of neighbors, both initially zero
		mtransform::v4 steering, virtualpos;
		steering.setConstant(0);
		int neighbors = 0;
		auto idelsep = [](auto&& dis, auto&& normal) {
		return  -(normal * math::exp(math::min((dis * -10.0), 15)));
		};
		auto sep = [](auto&& dis, auto&& normal) {

			return  -(normal * math::exp(math::min((dis * -20), 16)));
		};
		if (cmd.cState() != GameProps::state::idle)
			for (size_t i = 0; i < flock.size(); i++)
			{
				auto& flk = flock[i];
				func(flk);
				virtualpos = flk.relativePosition + flk.relativeVelocity * dtstep;
				//flk.relativeVelocity.head<3>() *= cmd.cState() == State::idle ? 0 : 1;
				{
					// add in steering contribution
					// (opposite of the offset direction, divided once by distance
					// to normalize, divided another time to get 1/d falloff)
					const auto dis = (virtualpos.head<3>().squaredNorm() - (flk.combinedRadiussq()));
					if (dis < 0)
						steering += sep(dis,flk.relativePosition.normalized());

					// count neighbors
					++neighbors;
				}
			}
		//return steering;
		else {

			for (size_t i = 0; i < flock.size(); i++)
			{
				auto& flk = flock[i];
				func(flk);
				static float stepfactor = 5;
				virtualpos = flk.relativePosition + flk.desiv * dtstep* stepfactor;
				//flk.relativeVelocity.head<3>() *= cmd.cState() == State::idle ? 0 : 1;
				{
					// add in steering contribution
					// (opposite of the offset direction, divided once by distance
					// to normalize, divided another time to get 1/d falloff)

					const auto dis = (virtualpos.head<3>().squaredNorm() - (flk.combinedRadiussq() + bufdis_));
					//const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
					if (dis < 0)
						steering += sep(dis, flk.relativePosition.normalized());// flk.st != State::idle ? sep(dis, flk.relativePosition.normalized()) : idelsep(dis, flk.relativePosition.normalized());

					// count neighbors
					++neighbors;
				}
			}
		}
		return steering;

		for (size_t i = 0; i < flock.size(); i++)
		{
			auto& flk = flock[i];
			flk.relativeVelocity.head<3>() *= cmd.cState() == State::idle ? 0 : 1;
			if (flk.st != State::idle)
			{
				// add in steering contribution
				// (opposite of the offset direction, divided once by distance
				// to normalize, divided another time to get 1/d falloff)
				const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
				if (dis < 0)
					steering += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));

				// count neighbors
				++neighbors;
			}
		}

		return steering;
	};
	using i2 = math::int2;
	float dbgLineWith = 0.01;
	void GMgr1::FindSpeed(GameProps::Cmd0& cmd,
		mtransform& transform,
		Collid& collider,
		GameProps::BasicProp& bp,
		Physics::Lavnes& neb) {
		using GameProps::PropID;
		using State = State::Actions;
		//calculate group Center
		constexpr auto GetCpid = [](auto&& nid, auto&&) {};
		constexpr auto checknan = [](auto& a) {
			return !(a[0] == a[0]);
		};
		
		const float dtstep = Physics::dt;
		const auto maxvel = Fix::toFloat(bp.get<(int)PropID::speed>());
		auto ProjectClampn = [](const auto& ca, const auto& cb, const auto& c, float& percent)
		{
			var dot = cb.dot(ca);
			//var projleg = dot / cb.magnitude;
			percent = dot / cb.squaredNorm();

			return c + std::clamp<float>(percent, 0, 1) * cb;
			// cb = c + cb.normalized * projleg;
		};
		auto dot2 = [](const auto& a, const auto& b) {
			return a.head<2>().dot(b.head<2>());
		};
		auto dotself = [](const auto& a) {
			return a.dot(a);
		};
		const float cohesionAngle = 0.75 * math::PI;
		const auto forward = transform.froward();
		auto&& dt = Physics::dt;
		float& sidesign = cmd.tpos[3];
		static v4f temppos;
		temppos = sidesign == 0 ? cmd.tpos : temppos;
		const auto& fdot = Fix::toFix(dot2(forward, (temppos - transform.pos4)));
		mtransform::v4 targetvel = sidesign == 0 || fdot < 0
			? cmd.tpos - transform.pos4 : collider.vel4;
		//targetvel[3] = math::dot(targetvel.head<3>(), targetvel.head<3>());
		mtransform::v4 ddisp, unitgoalvec={0,0,0,0}, virtualpos;//displacement after 1 second
		unitgoalvec.head<2>() = (cmd.tpos - transform.pos4).head<2>().normalized();
		//targetvel = cmd.tpos - transform.pos4;
		math::truncate(targetvel.head<3>(), maxvel);
		/*if (targetvel != collider.vel4) {
		}
		else {
			ddisp = (temppos - transform.pos4);
		}*/
		const auto& fw2 = forward.head<2>();
		const auto max_dp = maxvel * dt;
		auto ACR = math::square(collider.ConvexR + max_dp);
		const auto maxvelsq = maxvel * maxvel;
		constexpr auto cosMaxAngle = 0.0f;
		ddisp = targetvel;
		//targetvel.head<3>() *= 0.5;
		auto separation = steerForSeparationlamd(neb, cmd, dtstep, transform);;

		//forward.squaredNorm()
		auto& ut_speed = collider.Velocity;
		/*auto normsq = separation.head<3>().squaredNorm();
		if (normsq > maxvelsq) {
			separation *= maxvel / math::sqrt(normsq);
		}*/
		/*auto cohesion = Boids::steerForCohesion(forward, transform.pos4,
			cohesionAngle,
			neb);
		normsq = cohesion.head<3>().squaredNorm();
		if (normsq > maxvelsq) {
			cohesion *= maxvel / math::sqrt(normsq);
		}*/
		v4f poscor[] = {transform.pos4 ,v4f(0,1,dbgLineWith,1)};

		poscor[0][3] = collider.ConvexR;
		auto rotate90 = [](const auto& a) {
			return f2(a[1], -a[0]);
		};
		auto fowp4 = transform.pos4;
		static float fowmag = 40;
		fowp4.head<2>() += fw2 * fowmag;
		static v4f fowlinecolor = { 0.3,0.6,0,1 };
		//drawline(fowp4, transform.pos4, fowlinecolor);
		using Fix::toFix;
		auto fixdot = [](f2& a, f2& b) {return Fix::toFix(a.dot(b)); };
		constexpr float perf = 1.0 / Fix::One();
		static CollideID attached;
		attached.id = sidesign == 0 ? -1 : attached.id;
		float crsq;
		static float tstep = 0.1;
		int idismin = -1, mindis = std::numeric_limits<int>::max();
		f2 aimpos = { 1,2 }, tanre;
		const auto& tv2 = targetvel.head<2>();
		poscor[1] = separation.any() ? v4f{1, 0, 0, 1} : poscor[1];
		//targetvel.head<3>().squaredNorm() > ACR && 
		if (cmd.cState() != State::idle) {
			int u = 0, ut = 0, maxdft = -Fix::One() * 100;
			v4f potentialp[4];
			for (size_t i3 = 0; attached.id>=0&&i3 < neb.size(); i3++)
			{
				auto& other = neb[i3];
				if (other.cd == attached){
					idismin = i3;
					break;
				}
			}
			targetvel = unitgoalvec * maxvel;
			if (idismin >= 0) {
				auto& other = neb[idismin];
				auto orn2 = -other.relativePosition.head<2>();
				math::rotate90(orn2.normalized(), tanre);
				auto det2 = f2(math::Determinant2D(orn2, tanre), math::Determinant2D(orn2, -tanre));
				targetvel.head<2>() = tanre * (det2[0]*sidesign<0 ? 1 : -1);
				const auto dirdot = math::dot(unitgoalvec.head<2>(), tv2);
				if (dirdot > mathtable::costab<45>()) {
					attached.id = -1;
				}
				targetvel.head<2>() *= maxvel;
				//LOGINFO("det2={0} tanre={1} dis={2}",det2.transpose(),tanre.transpose(),orn2.norm());
				auto othpios = (transform.pos4 + other.relativePosition).eval();
				auto tancent = (othpios + transform.pos4).head<2>() * 0.5;
				v4f tan0 = { 0,0,0,0 }, tan1 = tan0;
				tan0.head<2>() = tancent.head<2>() + tanre;
				tan1.head<2>() = tancent.head<2>() - tanre;
			}
				virtualpos = (targetvel * tstep + transform.pos4).eval();
#ifdef DEBUG
			if (attached.id < 0) {
				LOGINFO("lost track id {0}", attached.id);
			}
#endif // DEBUG

			//v4f avf={0,0,0,0};
			for (size_t i3 = 0; i3 < neb.size(); i3++)
			{
				auto& other = neb[i3];
				crsq = other.combinedRadiussq();
				const auto& dcm = Fix::toFix(other.distSq() - crsq);
				//Utility::doIntersect
				bool sm1 = dcm < Fix::One();
				float percentout;
				//same commandid
				//if (other.gid == cmd.gid)
				{

					auto othpios = (transform.pos4 + other.relativePosition).eval();
					v4f pmdis = { 0,0,0,0 };
					auto npv = -(transform.pos4.head<2>() - virtualpos.head<2>());
					pmdis.head<2>() = ProjectClamp(
						othpios.head<2>(),
						virtualpos.head<2>(),
						transform.pos4.head<2>(), percentout);
					float fudis = (othpios - virtualpos).head<2>().squaredNorm();
					if (fudis - crsq < 0) {
						fudis = 0;
					}
					auto dismin = Fix::toFix(
						(pmdis - othpios).head<2>()
						.squaredNorm() - crsq);
					mindis = math::min(dismin, mindis);
					auto isobstacle = [&]() {
						return dismin < 0 && (other.cmdid != cmd.cmdid && other.st != State::idle
							&& other.relativeVelocity.head<3>().squaredNorm() >(maxvelsq * 0.5)) && (mindis == dismin);
					};
					//if (dismin < 0&&other.st!=State::idle&& other.st != State::move && (mindis == dismin)&& attached.hash() != other.cd.hash()) {
					if (isobstacle()) {
						auto orn2 = -other.relativePosition.head<2>();
						math::rotate90(orn2.normalized(), tanre);
						if (sidesign == 0) {
							int aimposi = math::dot(tanre, fw2) ? 0 : 1;
							sidesign = toFix(math::Determinant2D(fw2, math::dot(tanre, fw2)>0 ? tanre : -tanre));
						}
						idismin = i3;
						//auto det2 = f2(math::Determinant2D(fw2, tanre), math::Determinant2D(fw2, -tanre));
						auto det = math::Determinant2D(fw2, tanre);
						auto det2 = f2(math::Determinant2D(orn2, tanre), math::Determinant2D(orn2, -tanre));

						targetvel.head<2>() = tanre * (det2[0] * sidesign < 0 ? maxvel : -maxvel);
#ifdef DEBUG

						LOGINFO("chang rail from {0} to cd={1} tanre={2} det2 {3}",
							attached.id,other.cd.id, tanre.transpose(), det2.transpose());
						drawline(pmdis, transform.pos4, v4f(1, 0, 0.2, 1));
						pmdis.head<2>() = transform.pos4.head<2>() + npv * percentout;
						drawline(pmdis, othpios, v4f(1, 0, 0.2, 1));
						drawline(pmdis, transform.pos4, v4f(1, 0, 0.2, 1));
						auto tancent = (othpios + transform.pos4).head<2>() * 0.5;
						v4f tan0={0,0,0,0}, tan1=tan0;
						tan0.head<2>() = tancent.head<2>() + tanre;
						tan1.head<2>() = tancent.head<2>();
						drawline(tan0, tan1, v4f(1, 0,0, 1), v4f(0, 0, 1, 1));
						auto qroot = [](float a, float b, float sqrterm) {return (-b + sqrterm) / 2 * a; };
#endif
						attached = other.cd;
						//(x-x1)(x-x2)+(y-y1)(y-y2)=0 =>
						// x^2-(x1+x2)x+x1x2+~=0
						// (x-x2)(x1-x2)+(y-y2)(y1-y2)=b
						//(x-x1)(x2-x1)+(y-y1)(y2-y1)=dotcb  x-x1=(dotcb-(y-y1)(y2-y1))/(x2-x1)
						//(x-x2)(x1-x2)+(y-y1)(y1-y2)=dotab x-x2= (dotab-(y-y1)(y1-y2))/(x1-x2)
						// (x1-x2)(y-y1)(y-y2)-(dotcb-(y-y1)(y2-y1))*(dotab+(y-y1)(y2-y1))=0
						//  (x1-x2)(y-y1)(y-y2)+((y-y1)^2(y2-y1)^2))=dotcb^2 dotcb=dissq bc
						//  (x1-x2)(y-y2)+((y-y1)(y2-y1)^2))=dotcb^2/(y-y1)
						//  (y*x12-y2*x12)+y*y21sq-y1*y21sq=dotcb^2/(y-y1)
						// ax=y21sq+x12 m=-y1*y21sq-y2*x12
						// (yax+m)*(y-y1)-dotcb^2=0;
						// y*y*ax-yax*y1+my-my1-dotcb^2=0
						//  dotcb =bsq y2-y1+y2=y21
						//(x2-x1)(x-x2-x+x1)=dotab+dotcb
					}
					//meet a member reach  to goal
					u += other.st == State::idle && sm1 ? 1 : 0;
				}
			}
			static int iatt = -1;
			iatt = sidesign == 0 ? -1 : iatt;
			if (0&& mindis <= 0) {
				auto& other = neb[idismin];
				auto othpios = (transform.pos4 + other.relativePosition).eval();
				const auto& v21 = -other.relativePosition * 0.5;
				const auto& y1 = transform.pos[1];
				const auto& y2 = othpios[1];
				static float dt = 1;
				float diam = math::sqrt(other.distSq()), asq = (crsq)+0.01, bsq = other.distSq(),
					a = math::sqrt(asq), cbr = math::sqrt(crsq) + 0.1,
					cosbad = maxvel * dt / (cbr), sinbad = math::sqrt(1 - cosbad * cosbad);
				/*	 ax = axv[0] - axv[1], m = math::dot(axv, f2(-y1, -y2)),
					dotcbsq=math::square(other.distSq()),
					y21 =m*y1, b =-ax * transform.pos[1]+m,c=-m*y1-dotcbsq,
					sqr4ac= math::sqrt(b * b - 4 * (ax * c)),
					y =(-(b) + sqr4ac) /(2*ax);*/
				for (size_t ig = 0; ig < 4; ig++)
				{
					potentialp[ig].setZero();
				}
				math::Matrix2f rm; rm << cosbad, -sinbad, sinbad, cosbad;
				f2 cc = (othpios).head<2>();// *0.5;

				potentialp[0].head<2>() = cc + rm * (v21.head<2>());
				potentialp[1].head<2>() = cc + rm.transpose() * v21.head<2>();
				const auto& aim0 = potentialp[0].head<2>();
				const auto& aim1 = potentialp[1].head<2>();
				float aimdis = math::DotSelf(aim0 - othpios.head<2>());
				int xpi = 0;
				auto yroot = [&](float y) {
					float cy = (y - othpios[1]) * (y - transform.pos[1])
						, c = transform.pos[0] * othpios[0] + cy, b = -(transform.pos[0] + othpios[0]),
						srqt = math::sqrt(b * b - 4 * c),
						x0 = (-b - srqt) * 0.5, x1 = (-b + srqt) * 0.5;

					potentialp[xpi++] = { -100,y,0,0 };
					potentialp[3] = { 100,y,0,0 };
					//potentialp[xpi++] = { x1,y,0,0 };
					//for (size_t ifg = 0; ifg < 2; ifg++)
				};
				auto& rp0 = reinterpret_cast<f2&>(potentialp[0]);
				auto& rp1 = reinterpret_cast<f2&>(potentialp[1]);
				drawline(potentialp[0], transform.pos4);
				drawline(potentialp[1], transform.pos4);
				drawline(potentialp[0], othpios);
				drawline(potentialp[1], othpios);
				f2 uvas[2];
				auto orn2 = -other.relativePosition.head<2>();
				math::rotate90(orn2.normalized(), tanre);

				uvas[0] = tanre;
				uvas[1] = -tanre;
				float dl1 = (rp0 - transform.pos4.head<2>()).squaredNorm(),
					dl2 = (rp1 - transform.pos4.head<2>()).squaredNorm();
				//math::rotate90(other.relativePosition.head<2>().normalized(), aimpos);
				auto toFixv = [](const auto& s) {
					return (Fix::percisioni * s).cast<int>();
				};
				math::int2 i2d = toFixv(f2(math::dot(uvas[0], forward.head<2>()),
					math::dot(uvas[1], forward.head<2>())));
				if (sidesign == 0) {
					//auto i2d=(Fix::percisioni * f2(math::dot(aim0, forward.head<2>()), math::dot(aim1, forward.head<2>()))).cast<int>();
					maxdft = math::max(i2d[0], i2d[1]);
					int aimposi = i2d[0] > i2d[1] ? 0 : 1;
					sidesign = toFix(math::Determinant2D(fw2, uvas[aimposi].head<2>()));
					temppos.head<2>() = cc + (potentialp[aimposi].head<2>() - cc).normalized() * cbr;
				}
				else {
					//const auto dft = (i2d[0] > i2d[1] ? 0 : 1);
					const math::int2 dfr = i2d - i2::Ones() * 2 * Fix::One();
					auto cpv2 = toFixv(f2(math::Determinant2D(fw2, uvas[0]), math::Determinant2D(fw2, uvas[1])) * sidesign).array() > 0;// .eval();

					auto maxldft = math::max(maxdft, cpv2.select(i2d, dfr).maxCoeff());
					if (maxdft <= maxldft) {
						aimpos = i2d[0] > i2d[1] ? aim0 : aim1;//get same side of forward vector
						maxdft = maxldft;
						temppos.head<2>() = (cc + (aimpos - cc).normalized() * cbr);
					}
				}
				targetvel = temppos - transform.pos4;
				virtualpos = targetvel + transform.pos4;
			}
			const auto& dissqfix = Fix::toFix
			(dotself((transform.pos4-cmd.tpos).head<2>()));
			if (dissqfix <= Fix::percisioni||
				u > _nebth && neb.size() > _nebsz) {
				targetvel.setConstant(0);
				collider.Velocity.setConstant(0);
				cmd.SetState<State::change>();
			}
			collider.V_desire = targetvel;
			//const int& cst = ndrep.states;
			//if (nnn.eleState[cst] < nnn.eleState[(int)Ai::State::move]) [[unlikely]]
			{
				// apply weights to components (save in variables for annotation)
				const auto separationW = separation;// *separationWeight;
				//const auto alignmentW = alignment * alignmentWeight;
				//const auto cohesionW = cohesion.head<3>() * cohesionWeight;
				//targetvel.head<3>().normalize();
				auto vels = separationW + targetvel;
				targetvel = vels;
				math::truncate(targetvel.head<3>(), maxvel);
				collider.AngularVelocity = -math::cross(targetvel.head<3>(), forward.head<3>());
				constexpr float agvperstep = math::PI2 * 0.02;
				float stepcos = math::cos(agvperstep);
				//math::truncate(collider.AngularVelocity, math::PI2);
				auto radcos = (math::dot(targetvel.head<2>().normalized(), forward.head<2>()));
				collider.AngularVelocity =  collider.AngularVelocity.normalized() * math::PI2;
				
				collider.vel4 = targetvel = (radcos) > mathtable::costab<45>() ?
					targetvel : targetvel*0.5;
				if ((radcos) > stepcos)
				{
					auto delqt = math::quaternion::FromTwoVectors(forward, collider.Velocity);
					transform.rotation *= delqt;
					collider.AngularVelocity.setZero();
				}
				collider.vel4.head<3>() = AdjectV(targetvel, forward, ut_speed.norm(), maxvel, 0);
				assert(math::ckftvalid(targetvel.head<2>()[0]));
				/*const auto& AVOVel =
					AI::RVO::computeNewVelocity(collider.Velocity.head<2>(),
						neb,
						maxvel, targetvel.head<2>());
				auto delqt = math::quaternion::FromTwoVectors(forward, collider.Velocity);
				auto ea = delqt.toRotationMatrix().eulerAngles(0, 1, 2);
				auto et = transform.rotation.toRotationMatrix().eulerAngles(0, 1, 2);
				//transform.rotation *= delqt;
				auto ef = transform.rotation.toRotationMatrix().eulerAngles(0, 1, 2);
				assert((ea[0] + ea[1]) == 0);*/
			}
		}
		else// if (nnn.eleState[i2] < nnn.eleState[(int)Ai::State::attack])
		{
			cmd.tpos[3] = 0;
			//temppos[3] = 0;
			if(cmd.cState() != State::idle)
			cmd.SetState<State::idle>();
			//targetvel.setConstant(0); 
			//const auto alignmentW = alignment * alignmentWeight;
			//const auto cohesionW = cohesion.head<3>() * 0;
			//targetvel.head<3>().normalize();
			targetvel = separation;
			collider.V_desire = targetvel;
			collider.vel4 = targetvel;// AdjectV(targetvel, forward, ut_speed.norm(), maxvel, 0);
			math::truncate(collider.Velocity, maxvel);
			/*if (!collider.Velocity.isApproxToConstant(0, math::Epsilon))
				transform.rotation *= math::quaternion::FromTwoVectors(forward, collider.Velocity);
			transform.rotation.normalize();*/
		}
	}
#define GidOf(gid) GameProps::getPlayerId(gid)
	math::ui64 calcultargweight(Physics::LAVNe& vin,mtransform& trsn, GameProps::Cmd0& cmd) {
		const ui64 fxdissq=(math::ui64)(vin.distSq()*100);
		constexpr ui64 kk = (ui64)1 << 63;
		ui64 tv = cmd.hostile[GidOf(vin.gid)]? kk : 0;
		return tv & (fxdissq << 32 & (vin.cd.tid << 16) & vin.cd.tid0);
		//cmd.gid
	}
	template<class T>
	math::ui64 calcultargweight_(T& vin, GameProps::Cmd0& cmd) {
		ui64 fxdissq = (math::ui64)(vin.distSq() * 100);
		constexpr ui64 kk = (ui64)1 << 63;
		fxdissq <<= 32;
		GameProps::gids& gr = GameProps::getPlayerInfo((uint&)(vin.gid));
		auto host = cmd.hostile[gr.faction];
		//ui64 tv = cmd.hostile[getPlayerId(vin.gid)] ? kk : 0;
		return  host ? (fxdissq | ((vin.cd.tid << 24) & (vin.cd.tid0 << 16) & (vin.cd.id))) : 0;
		//cmd.gid
	}
	template<>
	void GMgr1::FindSpeed<0>(GameProps::Cmd0& cmd,
		mtransform& transform,
		Collid& collider,
		GameProps::BasicProp& bp,
		Physics::Lavnes& neb, Physics::LAVNe*& tgr) {
		using GameProps::PropID;
		using State = State::Actions;
		tgr = NULL;
		//calculate group Center
		constexpr auto GetCpid = [](auto&& nid, auto&&) {};
		constexpr auto checknan = [](auto& a) {
			return !(a[0] == a[0]);
		};
		const float dtstep = Physics::dt;
		const auto maxvel = Fix::toFloat(bp.get<(int)PropID::speed>());
		auto ProjectClampn = [](const auto& ca, const auto& cb, const auto& c, float& percent)
		{
			var dot = cb.dot(ca);
			//var projleg = dot / cb.magnitude;
			percent = dot / cb.squaredNorm();

			return c + std::clamp<float>(percent, 0, 1) * cb;
			// cb = c + cb.normalized * projleg;
		};
		auto dot2 = [](const auto& a, const auto& b) {
			return a.head<2>().dot(b.head<2>());
		};
		auto dotself = [](const auto& a) {
			return a.dot(a);
		};
		const float cohesionAngle = 0.75 * math::PI;
		const auto forward = transform.froward();
		auto&& dt = Physics::dt;
		float& sidesign = cmd.tpos[3];
		static v4f temppos;
		temppos = sidesign == 0 ? cmd.tpos : temppos;
		const auto& fdot = Fix::toFix(dot2(forward, (temppos - transform.pos4)));
		mtransform::v4 targetvel = sidesign == 0 || fdot < 0
			? cmd.tpos - transform.pos4 : collider.vel4;
		//targetvel[3] = math::dot(targetvel.head<3>(), targetvel.head<3>());
		mtransform::v4 ddisp, unitgoalvec = { 0,0,0,0 }, virtualpos;//displacement after 1 second
		unitgoalvec.head<2>() = (cmd.tpos - transform.pos4).head<2>().normalized();
		//targetvel = cmd.tpos - transform.pos4;
		math::truncate(targetvel.head<3>(), maxvel);
		const auto& fw2 = forward.head<2>();
		const auto max_dp = maxvel * dt;
		auto ACR = math::square(collider.ConvexR + max_dp);
		const auto maxvelsq = maxvel * maxvel;
		constexpr auto cosMaxAngle = 0.0f;
		ddisp = targetvel;
		math::ui64 maxweight = 0;
		auto&& itarg = tgr;
		//targetvel.head<3>() *= 0.5;
		auto separation = steerForSeparationf(neb, cmd, dtstep, transform, [&](auto&& other) {
			const auto& maxvcal =  calcultargweight_(other, cmd);
			maxweight = std::max(maxweight, maxvcal);
			itarg = maxweight == maxvcal ? &other : itarg;
			});
		itarg = maxweight == 0 ? NULL : itarg;
		//forward.squaredNorm()
		auto& ut_speed = collider.Velocity;
		auto rotate90 = [](const auto& a) {
			return f2(a[1], -a[0]);
		};
		auto fowp4 = transform.pos4;
		static float fowmag = 40;
		fowp4.head<2>() += fw2 * fowmag;
		static v4f fowlinecolor = { 0.3,0.6,0,1 };
		//drawline(fowp4, transform.pos4, fowlinecolor);
		using Fix::toFix;
		auto fixdot = [](f2& a, f2& b) {return Fix::toFix(a.dot(b)); };
		constexpr float perf = 1.0 / Fix::One();
		static CollideID attached;
		attached.id = sidesign == 0 ? -1 : attached.id;
		float crsq;
		static float tstep = 0.1;
		int idismin = -1, mindis = std::numeric_limits<int>::max();
		f2 aimpos = { 1,2 }, tanre;
		const auto& tv2 = targetvel.head<2>();
		if (cmd.cState() > State::count) {
			targetvel.setConstant(0);
			collider.Velocity.setConstant(0);
			collider.AngularVelocity.setConstant(0);
		}else
		if (cmd.cState() != State::idle) {
			int u = 0, ut = 0;
			v4f potentialp[4];
			for (size_t i3 = 0; attached.id >= 0 && i3 < neb.size(); i3++)
			{
				auto& other = neb[i3];
				if (other.cd == attached) {
					idismin = i3;
					break;
				}
			}
			targetvel = unitgoalvec * maxvel;
			if (idismin >= 0) {
				auto& other = neb[idismin];
				auto orn2 = -other.relativePosition.head<2>();
				math::rotate90(orn2.normalized(), tanre);
				auto det2 = f2(math::Determinant2D(orn2, tanre), math::Determinant2D(orn2, -tanre));
				targetvel.head<2>() = tanre * (det2[0] * sidesign < 0 ? 1 : -1);
				const auto dirdot = math::dot(unitgoalvec.head<2>(), tv2);
				if (dirdot > mathtable::costab<45>()) {
					attached.id = -1;
				}
				targetvel.head<2>() *= maxvel;
				//LOGINFO("det2={0} tanre={1} dis={2}",det2.transpose(),tanre.transpose(),orn2.norm());
				auto othpios = (transform.pos4 + other.relativePosition).eval();
				auto tancent = (othpios + transform.pos4).head<2>() * 0.5;
				v4f tan0 = { 0,0,0,0 }, tan1 = tan0;
				tan0.head<2>() = tancent.head<2>() + tanre;
				tan1.head<2>() = tancent.head<2>() - tanre;
			}
			virtualpos = (targetvel * tstep + transform.pos4).eval();
#ifdef DEBUG
			if (attached.id < 0) {
				LOGINFO("lost track id {0}", attached.id);
			}
#endif // DEBUG
			//v4f avf={0,0,0,0};
			for (size_t i3 = 0; i3 < neb.size(); i3++)
			{
				auto& other = neb[i3];
				crsq = other.combinedRadiussq();
				//const auto& dcm = Fix::toFix(other.distSq() - crsq);
				//Utility::doIntersect dcm <= Fix::One()&&
				int sm1=0 ;
				bool isobstacle = other.st != State::move?!(other.cmdid == cmd.cmdid && other.st == State::idle) : 0;
				if (other.cmdid == cmd.cmdid && other.st != State::move && other.st != State::change && other.st < State::count) {
					sm1 = isobstacle? 3: 1;// other.st == State::idle ? 1 : 3;
				}
				other.st != State::move;
				float percentout;
				//same group id
				//if (other.gid == cmd.gid) 
				{

					auto othpios = (transform.pos4 + other.relativePosition).eval();
					v4f pmdis = { 0,0,0,0 };
					auto npv = -(transform.pos4.head<2>() - virtualpos.head<2>());
					pmdis.head<2>() = ProjectClamp(
						othpios.head<2>(),
						virtualpos.head<2>(),
						transform.pos4.head<2>(), percentout);
					float fudis = (othpios - virtualpos).head<2>().squaredNorm();
					if (fudis - crsq < 0) {
						fudis = 0;
					}
					auto dismin = Fix::toFix(
						(pmdis - othpios).head<2>()
						.squaredNorm() - crsq);
					mindis = math::min(dismin, mindis);
					//if (dismin < 0&&other.st!=State::idle&& other.st != State::move && (mindis == dismin)&& attached.hash() != other.cd.hash()) {
					if (((dismin < 0 && isobstacle) && (mindis == dismin))) {
						auto orn2 = -other.relativePosition.head<2>();
						math::rotate90(orn2.normalized(), tanre);
						if (sidesign == 0) {
							int aimposi = math::dot(tanre, fw2) ? 0 : 1;
							sidesign = toFix(math::Determinant2D(fw2, math::dot(tanre, fw2) > 0 ? tanre : -tanre));
						}
						idismin = i3;
						//auto det2 = f2(math::Determinant2D(fw2, tanre), math::Determinant2D(fw2, -tanre));
						auto det = math::Determinant2D(fw2, tanre);
						auto det2 = f2(math::Determinant2D(orn2, tanre), math::Determinant2D(orn2, -tanre));

						targetvel.head<2>() = tanre * (det2[0] * sidesign < 0 ? maxvel : -maxvel);
						attached = other.cd;
					}
					//meet a member reach  to goal
					u +=  sm1 ;
				}
			}
			static int iatt = -1;
			iatt = sidesign == 0 ? -1 : iatt;
			const auto& dissqfix = Fix::toFix
			(dotself((transform.pos4 - cmd.tpos).head<2>()));
			if (dissqfix <= Fix::percisioni ||
				u >= 2) {
				//targetvel.setConstant(0);
				//collider.Velocity.setConstant(0);
				cmd.SetState<State::change>();
			}
			collider.V_desire = targetvel;
			//const int& cst = ndrep.states;
			//if (nnn.eleState[cst] < nnn.eleState[(int)Ai::State::move]) [[unlikely]]
			{
				// apply weights to components (save in variables for annotation)
				const auto separationW = separation;// *separationWeight;
				//const auto alignmentW = alignment * alignmentWeight;
				//const auto cohesionW = cohesion.head<3>() * cohesionWeight;
				//targetvel.head<3>().normalize();
				auto vels = separationW + targetvel;
				targetvel = vels;
				math::truncate(targetvel.head<3>(), maxvel);
				collider.AngularVelocity = -math::cross(targetvel.head<3>(), forward.head<3>());
				constexpr float agvperstep = math::PI2 * 0.02;
				float stepcos = math::cos(agvperstep);
				//math::truncate(collider.AngularVelocity, math::PI2);
				auto radcos = (math::dot(targetvel.head<2>().normalized(), forward.head<2>()));
				collider.AngularVelocity = collider.AngularVelocity.normalized() * math::PI2;

				collider.vel4 = targetvel = (radcos) > mathtable::costab<45>() ?
					targetvel : targetvel * 0.5;
				if ((radcos) > stepcos)
				{
					auto delqt = math::quaternion::FromTwoVectors(forward, collider.Velocity);
					transform.rotation *= delqt;
					collider.AngularVelocity.setZero();
				}
				collider.vel4.head<3>() = AdjectV(targetvel, forward, ut_speed.norm(), maxvel, 0);
				assert(math::ckftvalid(targetvel.head<2>()[0]));
			}
		}
		else //if (nnn.eleState[i2] < nnn.eleState[(int)Ai::State::attack])
		{
			cmd.tpos[3] = 0;
			//temppos[3] = 0;
			//targetvel.setConstant(0); 
			//const auto alignmentW = alignment * alignmentWeight;
			//const auto cohesionW = cohesion.head<3>() * 0;
			//targetvel.head<3>().normalize();
			targetvel = separation;

			if (targetvel.norm() > 1) {
				//pj2::n_Render::
				drawline(transform.pos4, transform.pos4 + targetvel);
			}
			collider.vel4 = targetvel;// AdjectV(targetvel, forward, ut_speed.norm(), maxvel, 0);
			math::truncate(collider.Velocity, maxvel);
			collider.V_desire = collider.vel4;
			/*if (!collider.Velocity.isApproxToConstant(0, math::Epsilon))
				transform.rotation *= math::quaternion::FromTwoVectors(forward, collider.Velocity);
			transform.rotation.normalize();*/
		}

	}
}