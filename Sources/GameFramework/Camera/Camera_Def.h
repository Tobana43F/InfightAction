#pragma once

#include	"Math\Math_Type.h"

namespace NCamera {

	//!	@brief	ビュー行列作成に必要な上方
	struct SViewData {
		float3 pos;
		float3 foc;
		float3 up;
	};

	//!	@brief	プロジェクション行列に必要な情報
	struct SProjData {
		float viewAngleRad;
		float aspectRatio;	// w / h
		float nearClip;
		float farClip;
	};

	//!	@brief	ビューポート情報を持つ構造体
	struct SViewport {
		float2	topLeftPos;		// スクリーン上の左上座標
		float2	size;
		float	maxDepth;
		float	minDepth;
	};



	//!	@brief	カメラ情報をマネージャーに登録するインデックス
	enum class EManagerIndex {
		MAIN,
		SUB_1,
		SUB_2,
		SUB_3,
		DEBUG,
		MAX
	};
}
