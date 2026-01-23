#pragma once

#include	"Polygon_Base.h"

namespace NPolygon {

	//!	@brief		ポリゴンクラスの基底クラス
	class CObject3D : public CBase {
	public:

	protected:
		//!	@brief	頂点フォーマットの指定
		virtual DWORD	GetVertexFormat() override { return (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1); }

		//!	@brief	指定したポリゴン情報を基にポリゴンを描画する
		virtual void RenderPolygon(const SData& _data);

	private:
		//!	@brief	継承元のクラスのエイリアス
		using Base = CBase;

	public:
		CObject3D();
		virtual ~CObject3D();
	};

}
