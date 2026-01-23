#pragma once

namespace NGraphic {

	//!	@brief	正規化された色を8bitの値に変換
	uint To8Color(float _col);

	//!	@brief	8ビットの色を正規化する
	float	ToNormalColor(int _col);
}