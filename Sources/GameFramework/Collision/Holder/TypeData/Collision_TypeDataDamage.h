#pragma once

#include	"_External\json11\json11.hpp"

namespace NCollision {

	//!	@brief		Damageタイプのあたり判定データ
	//!	@details	当たり判定Groupが保持するデータ
	//!				被ダメージ側の当たり判定データ
	//!	@note		管理する情報
	//!				- 判定箇所
	//!				- ダメージ倍率など
	class CDamageTypeData {
	public:
		//!	@brief	jsonからのデータ読み込み
		void Load(json11::Json _groupData);
		enum EPartType {
			BODY,	// デフォルト値
			HEAD,
			FOOT
		};

	private:

		EPartType	m_partType;


	public:
		CDamageTypeData();
		~CDamageTypeData();
	};
}