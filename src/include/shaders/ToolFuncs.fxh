#include "BasicStructures.fxh"

inline float4 transformf(const mtransfrom t, const float3 v3)
{
	float4 v = float4(v3, 1);
	v.xyz *= t.Scale;
	v = QuaternionMul(v, t.rotation);
	v.xyz += t.pos;
	return v;
}
inline bool inrect(float4 v,in Selects isele) {
	//v = mul(g_CameraAttribs.mViewProj, v);
	v.xyz /= v.w;
	if (all(v.xy < isele.maxin.xy) && all(v.xy > isele.maxin.zw)) {
		//int pos;
		/*InterlockedAdd(sele_out[0].itid, 1, ops);
		sele_out[ops].itid = obj.insd.tid << 16 | obj.insd.id;*/
		return true;
	}
	return false;
}
inline bool raysele(in mtransfrom transform,in Selects usele,in tinfo tf){

	const float4 he = tf.halfext;
	const float4 boxof=tf.offset;
float4 pos = float4(transform.pos.xyz, 1);
	const float4 qinv=QuaternionInv(transform.rotation),
	idir=QuaternionMul(usele.dir,qinv), 
	iorg=QuaternionMul(float4(usele.org.xyz-transform.pos,1),qinv);

	float4 dirfrac = 1 / idir,
		lb = (-he - iorg) * dirfrac, rt = (he -iorg) * dirfrac;
	const float t1 = lb.x;
	const float t2 = rt.x;
	const float t3 = lb.y;
	const float t4 = rt.y;
	const float t5 = lb.z;
	const float t6 = rt.z;

	float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
	float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));
	return tmax>=0.0 &&tmin<=tmax;
}

inline bool rectsele(in mtransfrom transform,in Selects usele,in tinfo tf,
in CameraAttribs g_CameraAttribs){

	const float4 he = tf.halfext;
	const float4 boxof=tf.offset;
float4 pos = float4(transform.pos.xyz, 1);
	for (int x = 0; x < 2; x++)
	{
		[unroll]
		for (int y = 0; y < 2; y++)
		{

			for (int z = 0; z < 2; z++)
			{
				float4 v = transformf(transform, -he + 2 * he * float3(x, y, z));
				int oi;
				return inrect(mul(g_CameraAttribs.mViewProj, v),usele);
			}
		}
	}
}