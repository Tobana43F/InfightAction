#pragma once

#include	<DirectXMath.h>
#include	<array>
#include	"Math\Vector\Math_Vector4.h"

namespace NMath {

	// 前方宣言
	struct SQuaternion;

	//!	@brief	4x4行列構造体
	struct SMatrix44 {
		union {
			//!	@brief	SIMDアクセス用
			DirectX::XMMATRIX	mat;

			//!	@brief	各要素アクセス用
			struct {
				float e11, e12, e13, e14;
				float e21, e22, e23, e24;
				float e31, e32, e33, e34;
				float e41, e42, e43, e44;
			};

			//!	@brief	配列アクセス用
			struct {
				std::array<std::array<float, 4>, 4>	flt;
			};

			//!	@brief	ベクトルアクセス用
			struct {
				std::array<SVector4, 4> vec;
			};
		};


		// コンストラクタ
		SMatrix44();
		SMatrix44(const SMatrix44& _rhs);
		SMatrix44(
			const SVector4& _vec1, 
			const SVector4& _vec2,
			const SVector4& _vec3,
			const SVector4& _vec4);
		SMatrix44(
			const float _e11, const float _e12, const float _e13, const float _e14,
			const float _e21, const float _e22, const float _e23, const float _e24,
			const float _e31, const float _e32, const float _e33, const float _e34,
			const float _e41, const float _e42, const float _e43, const float _e44);


		// オペレータ
		SMatrix44& operator =  (const SMatrix44& _rhs);
		SMatrix44& operator += (const SMatrix44& _rhs);
		SMatrix44& operator -= (const SMatrix44& _rhs);
		SMatrix44& operator *= (const SMatrix44& _rhs);
		SMatrix44& operator *= (const float& _rhs);
		SMatrix44& operator /= (const float& _rhs);


		// ゲッター
		//!	@brief	添え字を指定して値を取得
		float			Get(const unsigned int _y, const unsigned int _x) const;

		//!	@brief	行を取得
		const SVector4&	GetRow(const unsigned int _row) const;


		// セッター
		//!	@brief	行に値を設定
		void		SetRow(
			const unsigned int		_row,
			const NMath::SVector4&	_src);

		//!	@brief	行に値を設定(Vector3版)
		void		SetRow(
			const unsigned int		_row,
			const NMath::SVector3&	_src);


		// ユーティリティ群
		//!	@brief	ゼロを設定する
		void	SetZero();

		//!	@brief	単位行列を設定する
		void	SetIdentity();

		//!	@brief	要素を逆行列にする
		void	Inverse();

		//!	@brief	要素を転置行列にする
		void	Transpose();

		//!	@brief	要素を逆行列にする
		void	GetInverse(SMatrix44& _dest) const;

		//!	@brief	要素を転置行列にする
		void	GetTranspose(SMatrix44& _dest) const;

		//!	@brief	行列成分を分解する
		//!	@note	引数には、nullptrを設定可能
		//!			- 取得したい値にのみ変数のポインタを渡す。
		void	Decompose(
			SVector3*			_destScale,
			SQuaternion*		_destRotation,
			SVector3*			_destTrans) const;
	};
}

const NMath::SMatrix44 operator + (const NMath::SMatrix44& _lhs, const NMath::SMatrix44& _rhs);
const NMath::SMatrix44 operator - (const NMath::SMatrix44& _lhs, const NMath::SMatrix44& _rhs);

const NMath::SMatrix44 operator * (const NMath::SMatrix44& _lhs, const NMath::SMatrix44& _rhs);
const NMath::SMatrix44 operator * (const NMath::SMatrix44& _lhs, const float& _rhs);
const NMath::SMatrix44 operator * (const float& _lhs, const NMath::SMatrix44& _rhs);

const NMath::SMatrix44 operator / (const NMath::SMatrix44& _lhs, const float& _rhs);
const NMath::SMatrix44 operator / (const float& _lhs, const NMath::SMatrix44& _rhs);
