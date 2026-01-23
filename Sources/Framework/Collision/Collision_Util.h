#pragma once

//!	あたり判定情報の作成等の様々な関数群

#include	"Collision_Def.h"

// テスト
#include	<vector>

// 前方宣言
namespace NModel {
	class CManager;
	class CObject;
}

namespace NCollision {
	
	//!	@brief		平面のあたりはんてい情報を作成する。
	//!	@details	平面の方程式を求める。
	void CreatePlaneInfo(
		SPlane&			_plane,
		const float3&	_p0,
		const float3&	_p1,
		const float3&	_p2);

	//!	@brief		ボーンに影響する頂点からOBBを作成する。
	//!	@param[in]	_rDestOBB		OBB情報の作成先
	//!	@param[in]	_rRefModel		参照するボーンと頂点を含むモデル情報
	//!	@param[in]	_boneIndex		基にするボーンのインデックス(FBXのDump情報をもとに設定する)
	//!	@note		const char*型の例外を投げる。(エラーメッセージ)
	void CreateBoneOBB(
		NCollision::SOBBox&			_rDestOBB,
		const NModel::CObject&		_rRefModel,
		const uint					_boneIndex);
}
