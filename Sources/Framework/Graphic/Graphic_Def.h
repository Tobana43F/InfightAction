#pragma once

namespace NGraphic {

	//!	@brief	合成方法を指定する定数
	enum EBlendState {
		BLEND_STATE_REPLACE_NOALPHA,	//!< 上書き(アルファ値無効)
		BLEND_STATE_REPLACE,			//!< 上書き
		BLEND_STATE_ADD,				//!< 加算合成
		BLEND_STATE_SUB,				//!< 減算合成

		BLEND_STATE_MAX		
	};		

	//!	@brief	標準的な頂点フォーマット
	struct SDefaultVertex {
		float3 pos;
		float3 normal;
		float2 texel;
		float4 col;

		SDefaultVertex()
		{
			ZeroMemory(this, sizeof(SDefaultVertex));
		}
	};
}
