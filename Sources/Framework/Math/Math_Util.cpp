#include	"Math_Util.h"
#include	"Math_Common.h"
#include	"Debug\Debug_Message.h"

using namespace DirectX;

namespace NMath {

	bool IsEqual(const SVector2& _rVec1, const SVector2& _rVec2)
	{
		SVector2	ans;
		ans.simd = XMVectorEqual(_rVec1.simd, _rVec2.simd);

		bool isEqual = (ans.x != 0) && (ans.y != 0);
		return isEqual;
	}

	bool IsEqual(const SVector3& _rVec1, const SVector3& _rVec2)
	{
		SVector3	ans;
		ans.simd = XMVectorEqual(_rVec1.simd, _rVec2.simd);

		bool isEqual = (ans.x != 0) && (ans.y != 0) && (ans.z != 0);
		return isEqual;
	}

	bool IsEqual(const SVector4& _rVec1, const SVector4& _rVec2)
	{
		SVector4	ans;
		ans.simd = XMVectorEqual(_rVec1.simd, _rVec2.simd);

		bool isEqual = (ans.x != 0) && (ans.y != 0) && (ans.z != 0) && (ans.w != 0);
		return isEqual;
	}



	void CalcNormal(
		float3&			_dest,
		const float3&	_p0,
		const float3&	_p1,
		const float3&	_p2)
	{
		NMath::SVector3		p0p1;
		NMath::SVector3		p1p2;
		NMath::SVector3		normal;

		p0p1 = _p1 - _p0;
		p1p2 = _p2 - _p1;

		NMath::Cross(_dest, p0p1, p1p2);
		_dest.Normalize();
	}




	float	CalcPointAndLineLength(
		const float3&	_rPoint,
		const float3&	_rStartLine,
		const float3&	_rEndLine,
		float3*			_pDestNearPoint,
		bool			_isGetSquare)
	{
		// 点と線分の最短距離となる線分上の点を取得
		float3	nearLengthPoint;
		CalcPointAndLineNearestPoint(
			nearLengthPoint,
			_rPoint,
			_rStartLine,
			_rEndLine);

		if (_pDestNearPoint != nullptr) {
			*_pDestNearPoint = nearLengthPoint;
		}

		// 最短点から点へのベクトルを取得
		NMath::SVector3	simd = nearLengthPoint - _rPoint;
		if (_isGetSquare) {
			return simd.GetLengthSquare();
		}
		else {
			return simd.GetLength();
		}
	}

	void	CalcPointAndLineNearestPoint(
		float3&			_rDest,
		const float3&	_rPoint,
		const float3&	_rStartLine,
		const float3&	_rEndLine)
	{
		// 始点から終点へ向かうベクトルを取得
		NMath::SVector3	vecLine = _rEndLine - _rStartLine;
		// 視点から点へのベクトルを取得
		NMath::SVector3	vecPoint = _rPoint - _rStartLine;

		// 正規化
		NMath::SVector3	vecLineNorm;
		vecLine.GetNormal(vecLineNorm);

		float dot = NMath::Dot(vecLineNorm, vecPoint);
		float lenSq = dot * dot;

		// 線分開始位置より後ろ側の場合は線分の開始位置が最短点
		if (dot <= 0) {
			_rDest = _rStartLine;
		}
		// 線分の終了位置を超えた場合
		else if (lenSq >= vecLine.GetLengthSquare()) {
			_rDest = _rEndLine;
		}
		// 線分上の場合
		else {
			_rDest = _rStartLine + (vecLineNorm * dot);
		}
	}

	float	CalcPointAndPlaneLength(
		const float3&					_rPoint,
		const NCollision::SPlaneInfo&	_rPlane)
	{
		// 面の中心点から点へのベクトル(正規化しない)
		NMath::SVector3	vecPlanePoint = _rPoint - _rPlane.center;
		float length = NMath::Dot(vecPlanePoint, _rPlane.normal);
		length = abs(length);
		return length;
	}

	bool CalcLineAndPlaneCross(
		const NCollision::SPlane&	_rPlane,
		const float3&				_rLineOrig,
		const float3&				_rLineDir,
		float3&						_rDestCrossPos)
	{
		float t;

		// 平行チェック(内積を計算する)
		float dot =
			_rPlane.a * _rLineDir.x +
			_rPlane.b * _rLineDir.y +
			_rPlane.c * _rLineDir.z;

		if (fabsf(dot) < FLT_EPSILON) {
			// 無効な値を代入
			_rDestCrossPos = float3(0, 0, 0);
			NDebug::OutLogMessage("Warning : 線と面のあたり判定の結果、並行でした。");
			return false;
		}

		t = -(
			_rPlane.a * _rLineOrig.x +
			_rPlane.b * _rLineOrig.y +
			_rPlane.c * _rLineOrig.z +
			_rPlane.d) / dot;

		_rDestCrossPos.x = _rLineOrig.x + _rLineDir.x * t;
		_rDestCrossPos.y = _rLineOrig.y + _rLineDir.y * t;
		_rDestCrossPos.z = _rLineOrig.z + _rLineDir.z * t;
		return true;
	}

}
