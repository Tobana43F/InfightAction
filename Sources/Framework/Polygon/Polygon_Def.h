#pragma once

//!	@brief	ポリゴン(矩形ポリゴン)の定義

#include	"DirectX\DirectX_Manager.h"
#include	"Math\Math_Type.h"

namespace NPolygon {

	//!	@brief	固定パイプライン用並行投影用頂点フォーマット
	struct SVertex2D {
		NMath::SFloat3Data		pos;
		float					rhw;
		D3DCOLOR				diffuse;
		NMath::SFloat2Data		texCoord;
	};

	//!	@brief	固定パイプライン用投資投影用頂点フォーマット
	struct SVertex3D {
		NMath::SFloat3Data		pos;
		D3DCOLOR				diffuse;
		NMath::SFloat2Data		texCoord;
	};

	//!	@brief	矩形ポリゴンの頂点を特定する識別子
	//!	@note	TRIANGLE_STRIPでの描画を想定している(上から順番に描画される)
	enum EVertexID {
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
		MAX	
	};

	//!	@brief	X軸方向のアライメント位置
	enum class EAlignX {
		LEFT,		// +--
		CENTER,		// -+-
		RIGHT		// --+
	};

	//!	@brief	Y軸方向のアライメント位置
	enum class EAlignY {
		UP,			// UP +		CENTER	|	BOTTOM	|
		CENTER,		//    |				+			|
		BOTTOM		//    |				|			+
	};

}
