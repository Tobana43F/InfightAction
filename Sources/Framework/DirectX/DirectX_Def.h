#pragma once
//!	DirectXに必要なものを定義する

#include	<d3dx9.h>

namespace NDirectX {

	//!	@brief	スクリーン座標計で描画する頂点フォーマット
	struct SVertex2D {
		float3		pos;
		float		rhw;
		D3DCOLOR	diffuse;
		float2		texel;
	};

	//!	@brief	ワールド座標系で描画する頂点フォーマット
	struct SVertex3D {
		float3		pos;
		D3DCOLOR	diffuse;
		float2		texel;
	};
}
