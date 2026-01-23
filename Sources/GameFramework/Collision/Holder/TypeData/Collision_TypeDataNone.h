#pragma once

#include	"_External\json11\json11.hpp"

namespace NCollision {

	//!	@brief		Noneタイプのあたり判定データ
	//!	@details	当たり判定Groupが保持するデータ
	//!	@note		情報を保持しないが、他のタイプのクラスと合わせるため
	//!				形だけ作っている
	class CNoneTypeData {
	public:
		void Load(json11::Json _groupData) { /*何もしない*/ }

	private:
		// 何も持たない

	public:
		CNoneTypeData() {}
		~CNoneTypeData() {}
	};
}