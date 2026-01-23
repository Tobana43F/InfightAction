#pragma once

#include	"Math\Vector\Math_Vector4.h"

namespace NMath {

	struct SMatrix44;

	//!	@brief	クオータニオン
	//!	@todo	関数の実装が必要
	struct SQuaternion {
		union {
			//!	@brief	各要素アクセス用
			struct {
				float x, y, z, w;
			};
			//!	@brief	ベクトルとしてのアクセス用
			NMath::SVector4	vec4;	// 変換処理を不要にするため
		};

		SQuaternion();
		SQuaternion(const SQuaternion& _rhs);
		SQuaternion& operator =  (const SQuaternion& _rhs);
		SQuaternion& operator *= (const SQuaternion& _rhs);


		//!	@brief	全ての値を0にする
		void SetZero();

		//!	@brief	単位クオータニオンを設定する
		void SetIdentity();

		//!	@brief	クオータニオンを正規化する
		void Normalize();

		//!	@brief	逆クオータニオンにする
		void Inverse();

		//!	@brief	正規化したクオータニオンを取得する
		void GetNormalize(SQuaternion& _rDest);

		//!	@brief	逆クオータニオンを取得する
		void GetInverse(SQuaternion& _rDest);

	};
}

const NMath::SQuaternion operator * (const NMath::SQuaternion& _lhs, const NMath::SQuaternion& _rhs);
