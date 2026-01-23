#pragma once

#include	<d3dx9.h>
#include	<d3d9.h>
#include	<vector>
#include	"Collision_Def.h"
#include	"Asset\Factory\Asset_ModelFactory.h"

namespace NCollision {

	//!	@brief	メッシュの当たり判定を管理するクラス
	class CMesh {
	public:

		//!	@brief		初期処理(メッシュ情報の生成)
		void Load(NAsset::CModelFactory::Key _key);

		//!	@brief		解放処理
		void Release();

		//!	@brief		法線を描画
		void RenderNormal();

		//!	@brief		メッシュにレイを飛ばし、衝突位置を取得する
		//!	@note		複数検知した場合は最短点を取得する
		bool SearchMesh(
			float3&			_dest,
			const float3&	_pos,
			const float3&	_dir, 
			SPlaneInfo&		_rDestPlaneInfo);

		//!	@brief		メッシュと線分の衝突を検知し、衝突位置を取得する
		//!	@note		複数検知した場合は最短点を取得する
		bool SearchMesh(
			float3&			_destCross,
			const float3&	_posStart,
			const float3&	_posEnd,
			SPlaneInfo*		_pDestPlaneInfo = nullptr);

		//!	@brief		メッシュと真下方向の半直線の衝突検知、最短交差点を取得する
		bool SerchGroundOrigToDirection(
			float3&			_destCross,
			const float3&	_orig,
			SPlaneInfo*		_pDestPlaneInfo = nullptr);

		//!	@brief		メッシュと真下方向の半直線の衝突検知、最短交差点を取得する
		//!	@note		SerchGroundOrigToDirection()の関数に長さによる制限を追加したバージョン
		//!	param[in]	_lenSq	判定する真下方向への長さの２乗(平方根計算省略のため)
		bool SerchGroundOrigToDirection(
			float3&			_destCross,
			const float3&	_orig,
			const float		_lenSq,
			SPlaneInfo*		_pDestPlaneInfo = nullptr);
		
		//!	@brief		指定した座標の地面の位置を探索する
		//!	@details	上下問わず地面を取得する
		bool SearchGround(
			const float3&	_rCurPos,
			float3&			_rDestCrossPos,
			SPlaneInfo*		_pDestPlaneInfo);

		//!	@brief		面情報を取得
		const std::vector<SPlaneInfo>&	GetPlaneInfo() const { return m_planeInfo; }

	private:

		std::vector<SPlaneInfo>		m_planeInfo;	// 面情報の管理
		uint						m_faceCount;	// 面の数

		// 当たり判定に使用するモデルデータ
		NAsset::CModelFactory::SPAsset	m_model;

	public:
		CMesh();
		~CMesh();
	};
}