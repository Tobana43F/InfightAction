#include	"Debug_Util.h"
#include	"Math\Math_Common.h"
#include	"Mesh\Mesh_Line.h"

namespace NDebug {

	void	RenderLine(
		const float3& _rStart,
		const float3& _rEnd,
		const float4& _rColor)
	{
	#ifdef _GAME_DEBUG
		NMesh::SetLineColor(_rColor);
		NMesh::RenderLine(_rStart, _rEnd);
	#endif
	}

	void	RenderPoint(
		const float3&	_rPos,
		const float		_len,
		const float		_alpha)
	{
	#ifdef _GAME_DEBUG
		float3	s, e;
		float	hLen = _len * 0.5f;

		float alpha = _alpha;

		NMesh::SetLineColor(float4(1, 0, 0, alpha));
		s = e = _rPos;
		s.x -= hLen;
		e.x += hLen;
		NMesh::RenderLine(s, e);

		NMesh::SetLineColor(float4(0, 1, 0, alpha));
		s = e = _rPos;
		s.y -= hLen;
		e.y += hLen;
		NMesh::RenderLine(s, e);

		NMesh::SetLineColor(float4(0, 0, 1, alpha));
		s = e = _rPos;
		s.z -= hLen;
		e.z += hLen;
		NMesh::RenderLine(s, e);
	#endif
	}

	void	RenderAxisDir(
		const NMath::SMatrix44&	_rMat,
		const float				_length,
		const float				_alpha)
	{
	#ifdef _GAME_DEBUG
		float3 orig(0, 0, 0);
		float3 dirX(1, 0, 0);
		float3 dirY(0, 1, 0);
		float3 dirZ(0, 0, 1);

		dirX *= _length;
		dirY *= _length;
		dirZ *= _length;

		NMath::TransformVector(orig, _rMat);
		NMath::TransformVector(dirX, _rMat);
		NMath::TransformVector(dirY, _rMat);
		NMath::TransformVector(dirZ, _rMat);

		NMesh::SetLineColor(float4(1, 0, 0, _alpha));
		NMesh::RenderLine(orig, dirX);
		NMesh::SetLineColor(float4(0, 1, 0, _alpha));
		NMesh::RenderLine(orig, dirY);
		NMesh::SetLineColor(float4(0, 0, 1, _alpha));
		NMesh::RenderLine(orig, dirZ);
	#endif
	}

	void	RenderWireFrameSphere(
		const float3&		_centerPos,
		const float			_radius,
		const float			_alpha)
	{
	#ifdef _GAME_DEBUG
		float	div = 32;
		float	addDeg = 360.0f / div;

		float3	pos = _centerPos;
		float	radius = _radius;

		// X Ž²Žü‚è
		for (int i = 0; i < div; ++i) {
			float radFrom = NMath::ToRad(addDeg * i);
			float3 localPosFrom;
			localPosFrom.z = cos(radFrom) * radius;
			localPosFrom.y = sin(radFrom) * radius;

			float radTo = NMath::ToRad(addDeg * (i + 1));
			float3 localPosTo;
			localPosTo.z = cos(radTo) * radius;
			localPosTo.y = sin(radTo) * radius;

			float3 worldPosFrom = localPosFrom + pos;
			float3 worldPosTo = localPosTo + pos;

			NMesh::SetLineColor(float4(1, 0, 0, _alpha));
			NMesh::RenderLine(worldPosFrom, worldPosTo);
		}

		// Y Ž²Žü‚è
		for (int i = 0; i < div; ++i) {
			float radFrom = NMath::ToRad(addDeg * i);
			float3 localPosFrom;
			localPosFrom.x = cos(radFrom) * radius;
			localPosFrom.z = sin(radFrom) * radius;

			float radTo = NMath::ToRad(addDeg * (i + 1));
			float3 localPosTo;
			localPosTo.x = cos(radTo) * radius;
			localPosTo.z = sin(radTo) * radius;

			float3 worldPosFrom = localPosFrom + pos;
			float3 worldPosTo = localPosTo + pos;

			NMesh::SetLineColor(float4(0, 1, 0, _alpha));
			NMesh::RenderLine(worldPosFrom, worldPosTo);
		}

		// Z Ž²Žü‚è
		for (int i = 0; i < div; ++i) {
			float radFrom = NMath::ToRad(addDeg * i);
			float3 localPosFrom;
			localPosFrom.x = cos(radFrom) * radius;
			localPosFrom.y = sin(radFrom) * radius;

			float radTo = NMath::ToRad(addDeg * (i + 1));
			float3 localPosTo;
			localPosTo.x = cos(radTo) * radius;
			localPosTo.y = sin(radTo) * radius;

			float3 worldPosFrom = localPosFrom + pos;
			float3 worldPosTo = localPosTo + pos;

			NMesh::SetLineColor(float4(0, 0, 1, _alpha));
			NMesh::RenderLine(worldPosFrom, worldPosTo);
		}
	#endif
	}


}