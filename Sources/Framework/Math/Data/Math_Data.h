#pragma once

//	他の数学関係の構造体はDirectXMathを利用しSIMDを内包するため、
//	ベクトルのsizeof() が float * 4 の値になってしまうため
//	データ保持用としての数学型
//	シェーダーのパラメータとして使用する型などに使用する
//	operator等は必要最低限のみ実装する

//!	@note	Int型は使用予定がないので未実装

#include	<array>

namespace NMath {

	struct SVector2;
	struct SVector3;
	struct SVector4;

	//!	@brief	float * 4型
	struct SFloat4Data {
	public:
		union {
			//!	@brief	要素アクセス用
			struct {
				float x, y, z, w;
			};

			//!	@brief	配列アクセス用
			std::array<float, 4>	array;
		};

		SFloat4Data();
		SFloat4Data(const SFloat4Data& _rhs);
		SFloat4Data(const SVector4& _rhs);
		SFloat4Data(
			const float _x,
			const float _y,
			const float _z,
			const float _w);

		//!	@brief	計算用構造体にコピーする(operatorの代わり) 
		void Copy(SVector4& _dest);
	};



	//!	@brief	float * 3型
	struct SFloat3Data {
	public:
		union {
			//!	@brief	要素アクセス用
			struct {
				float x, y, z;
			};

			//!	@brief	配列アクセス用
			std::array<float, 3>	array;
		};

		SFloat3Data();
		SFloat3Data(const SFloat3Data& _rhs);
		SFloat3Data(const SVector3& _rhs);
		SFloat3Data(
			const float _x,
			const float _y,
			const float _z);

		//!	@brief	計算用構造体にコピーする(operatorの代わり) 
		void Copy(SVector3& _dest);
	};



	//!	@brief	float * 2型
	struct SFloat2Data {
	public:
		union {
			//!	@brief	要素アクセス用
			struct {
				float x, y;
			};

			//!	@brief	配列アクセス用
			std::array<float, 2>	array;
		};

		SFloat2Data();
		SFloat2Data(const SFloat2Data& _rhs);
		SFloat2Data(const SVector2& _rhs);
		SFloat2Data(
			const float _x,
			const float _y);

		//!	@brief	計算用構造体にコピーする(operatorの代わり) 
		void Copy(SVector2& _dest);
	};
}