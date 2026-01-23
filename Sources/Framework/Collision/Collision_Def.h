#pragma once

//!	当たり判定に使用する定義

namespace NCollision {

	//!	@brief	平面の方程式の値を保持する構造体
	struct SPlane {
		float a, b, c, d;
	};

	//!	@brief	平面(三角形)の当たり判定の情報を保持する構造体
	struct SPlaneInfo {
		SPlane	plane;
		float3	p0, p1,	p2;

		float3	center;		// 重心
		float3	normal;		// 面の法線方向

		float3	vecPTo01Vert;	// 各辺への垂直ベクトル(非正規状態)
		float3	vecPTo12Vert;
		float3	vecPTo20Vert;

	};

	//!	@brief	球の当たり判定情報
	struct SSphere {
		float		r;			// 半径
		float3		centerPos;	// 球の中心点
	};

	//!	@brief	OBBoxの判定上
	struct SOBBox {
		float3				centerPos;
		float3				size;
		NMath::SMatrix44	axisDir;	// 3x3行列として使用する
	};
}
