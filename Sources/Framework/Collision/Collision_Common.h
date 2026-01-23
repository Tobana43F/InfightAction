#pragma once

//!	あたり判定の判定テストを行う関数群の定義
//!	@note	量が増えてきた場合は、判定対象ごとにファイルを分ける。

//!	@note	命名規則 Check XXX And YYY [~~~]()
//!			- XXX	はYYYより小さい単位の形状
//!			- YYY	はXXXより多き単位の形状
//!			- [~~~]	は補足的な内容を書く(省略可)

#include	"Collision_Def.h"
#include	"Collision_Mesh.h"
#include	"Collision_OBBox.h"

namespace NCollision {

	//!	@brief	点と、三角形の判定
	bool CheckPointAndTriangle(
		const float3&	_rP0,
		const float3&	_rP1,
		const float3&	_rP2,
		const float3&	_rPoint);

	//!	@brief	点と、平行の四角形の判定
	//!	@note	四角形の頂点がすべて平面上にあることがわかっている場合にのみ使用できる
	bool CheckPointAndFlatRectangle(
		const float3&	_rP0,
		const float3&	_rP1,
		const float3&	_rP2,
		const float3&	_rP3,
		const float3&	_rPoint);



	//!	@brief		直線と球の交差判定
	//!	@param[in]	_rDir			向きベクトル(正規化されていること)
	//!	@param[out]	_pDestCrossNear	線と球の始点に近い側の交点
	//!	@param[out]	_pDestCrossFar	線と球の始点から遠いい側の交点
	bool	CheckLineAndSphere(
		const SSphere&					_rSphere,
		const float3&					_rLineStart,
		const NMath::SVector3&			_rDir,
		float3*							_pDestCrossNear = nullptr,
		float3*							_pDestCrossFar = nullptr);



	//!	@brief		球と面のあたり判定
	//!	@param[out]	_pIsCollideEdge	面の範囲外の角で衝突したフラグ
	bool	CheckPlaneAndSphere(
		const SPlaneInfo&	_rPlane,
		const SSphere&					_rSphere,
		NMath::SVector3*				_pVecPushBack = nullptr,
		NMath::SVector3*				_pNearPos = nullptr,
		bool*							_pIsCollideEdge = nullptr);

	//!	@brief		球と壁メッシュの当たり判定
	//!	@param[in]	_rSphere			判定する球
	//!	@param[in]	_rMeshWall			壁として使用する当たり判定用メッシュ
	//!	@param[out]	_rDestPushBack		押し戻しベクトル
	bool	CheckSphereAndMeshWall(
		const SSphere&					_rSphere,
		const CMesh&					_rMeshWall,
		NMath::SVector3&				_rDestPushBack);



	//!	@brief		OBBox同士の当たり判定
	bool	CheckOBBAndOBB(
		const SOBBox&	_rOBB1,
		const SOBBox&	_rOBB2);
}
