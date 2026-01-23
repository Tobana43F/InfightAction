#pragma once

//! 数学系のユーティリティ

#include	"Math\Math_Type.h"
#include	"Collision\Collision_Def.h"

namespace NMath {

	//!	@brief	2つのベクトルが等しいかどうかを判定する
	bool IsEqual(const SVector2& _rVec1, const SVector2& _rVec2);
	bool IsEqual(const SVector3& _rVec1, const SVector3& _rVec2);
	bool IsEqual(const SVector4& _rVec1, const SVector4& _rVec2);


	//!	@brief	三角形の法線を計算する
	void CalcNormal(
		float3&			_dest,
		const float3&	_p0,
		const float3&	_p1,
		const float3&	_p2);



	//!	@brief		点と線分の最短距離を求める
	//!	@param[in]	_rPoint				点
	//!	@param[in]	_rStartLine			線分の開始点
	//!	@param[in]	_rEndLine			線分の終了点
	//!	@param[out]	_pDestNearPoint		最短距離となる線分上の点を取得する先
	//!	@param[in]	_isGetSquare		2乗の長さを取得するフラグ
	float	CalcPointAndLineLength(
		const float3&	_rPoint,
		const float3&	_rStartLine,
		const float3&	_rEndLine,
		float3*			_pDestNearPoint = nullptr,
		bool			_isGetSquare	= false);

	//!	@brief	点と線分の最短距離となる線分上の点を計算する
	void	CalcPointAndLineNearestPoint(
		float3&			_rDest,
		const float3&	_rPoint,
		const float3&	_rStartLine,
		const float3&	_rEndLine);

	//!	@brief	点と面の最短距離を求める
	float	CalcPointAndPlaneLength(
		const float3&					_rPoint,
		const NCollision::SPlaneInfo&	_rPlane);

	//!	@brief	線と、無限遠の面の交点を計算し取得する
	//!	@retval 平行な場合false
	bool CalcLineAndPlaneCross(
		const NCollision::SPlane&	_rPlane,
		const float3&				_rLineOrig,
		const float3&				_rLineDir,
		float3&						_rDestCrossPos);
}
