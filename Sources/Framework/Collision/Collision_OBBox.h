#pragma once 

#include	"Collision_Def.h"
#include	"Math\Math_Type.h"

// テスト
#include	<d3dx9.h>

namespace NCollision {

	//!	@brief	指定した行列の子として変化するOBBのあたり判定
	class CTransformOBBox {
	public:
		
		//!	@brief	それぞれの値をもとに当たり判定を設定する
		void	SetData(
			const float3&			_localCenterPos,
			const float3&			_size,
			const NMath::SMatrix44&	_matRot);

		//!	@brief	すでにあるOBB情報をもとにあたり判定を設定する
		void	SetData(const NCollision::SOBBox& _rSrc) { m_localOBBox = _rSrc; }

		//!	@brief	OBBに行列を適用する
		void	Update(const NMath::SMatrix44& matParent);

		//!	@brief	当たり判定情報を取得(ワールド座標系)
		const SOBBox&	GetOBBox() const { return m_worldOBBox; }

	private:

		SOBBox	m_localOBBox;	// ローカル座標系でのOBB情報
		SOBBox	m_worldOBBox;	// ワールド座標系でのOBB情報

	public:
		CTransformOBBox();
		~CTransformOBBox();
	};
}
