#pragma once

#include	<array>
#include	<vector>
#include	"Math\Math_Type.h"

namespace NObject {

	// 前方宣言
	class CGameObject;

	//!	@brief		オブジェクトの位置や姿勢など、基本的な情報を保持するクラス
	//!	@details	行列を管理し、親子関係を保持する。
	//!	@note		親子関係などの情報の整合性をとるため、コピーを禁止する。
	//!	@note		スケールは行列と別に管理する
	//!	@todo		行列の計算回数が多いので、速度面の改善が必要
	class CTransform final {
	public:

		//----------------------------------------------------------
		//	親子関係の設定
		//----------------------------------------------------------

		//! @brief		親を設定する
		//!	@warning	階層を深くすると、行列計算が増えるので、注意が必要
		//!				- 特に、ワールド変換行列や座標を取得する関数は呼び出し時に行列を計算するため、
		//!				  座標の取得のコストが増える
		//!	@param[in]	_pParent	親へのポインタ(nullptrを設定すると親を解除する)
		//!				子オブジェクトの動作
		//!				- 親子関係を設定した時 :
		//!				  　親の位置にあわされる(ローカル変換行列が単位行列になる)
		//!					※ できれば見た目が変わらないようにしたい。
		//!				- 親子関係を解除した時の動作 : 
		//!				  　ワールド空間での座標と姿勢を保持したまま親子関係を解除する(見た目が変わらない)
		//!	@todo		親子関係を設定した時の動作を、絶対座標を保持したまま関係を設定するようにする。
		void SetParent(CTransform* _pParent = nullptr);

		//!	@brief		親を取得する
		CTransform*	GetParent() { return m_pParent; }

		//----------------------------------------------------------
		//	セッター
		//----------------------------------------------------------

		//!	@brief	座標を設定
		void	SetPos(const float3& _rPos) { m_matLocal.SetRow(3, _rPos); }
		void	SetPos(const float _x, const float _y, const float _z) { SetPos(NMath::SVector3(_x, _y, _z)); }

		//!	@brief	スクリーン座標系での座標設定
		void	SetPos(const float2& _rPos) { SetPos(_rPos.x, _rPos.y, 0); }
		void	SetPos(const float _x, const float _y) { SetPos(_x, _y, 0); }


		//!	@brief	姿勢を設定
		//!	@note	引数の行列を3x3行列として扱う
		void	SetRot(const NMath::SMatrix44& _rMatRot);

		//!	@brief	スクリーン座標系で頻繁に使用するZ軸回りの回転の設定
		void	SetRot(const float _degZ);

		//!	@brief	スケールを設定する
		void	SetScale(const float3&	_rScale) { m_scale = _rScale; }
		void	SetScale(const float _x, const float _y, const float _z) { SetScale(float3(_x, _y, _z)); }
		void	SetScale(const float _scale) { SetScale(_scale, _scale, _scale); }

		//!	@brief	座標を移動(加算)
		void	Move(const float3& _rVel) { m_matLocal.vec[3].vec3 += _rVel; }
		void	Move(const float _x, const float _y, const float _z) { Move(float3(_x, _y, _z)); }

		//!	@brief	スクリーン座標系での移動
		void	Move(const float2& _rVel) { Move(_rVel.x, _rVel.y, 0); }
		void	Move(const float _x, const float _y) { Move(_x, _y, 0); }


		//!	@brief	回転させる
		void	Rotate(const float3& _deg);
		void	Rotate(const float _degX, const float _degY, const float _degZ) { Rotate(float3(_degX, _degY, _degZ)); }

		//!	@brief	スクリーン座標系用のZ軸のみを指定した回転
		void	Rotate(const float _degZ) { Rotate(0, 0, _degZ); }


		//!	@brief		ローカル変換行列を設定する
		//!	@warning	設定する行列次第では不具合が起こるので注意して使うこと
		void	SetLocalMatrix(const NMath::SMatrix44& _rSec) { m_matLocal = _rSec; }

		//----------------------------------------------------------
		//	ゲッター
		//----------------------------------------------------------

		//!	@brief	所有者のゲームオブジェクトを取得
		CGameObject*	GetGameObject() { return m_pOwner; }

		//!	@brief		ワールド座標を取得
		//!	@warning	親子関係の階層に比例して行列計算量が増える
		void			GetPos(float3& _rDest) const;

		//!	@brief		ローカル座標を取得
		const float3&	GetLocalPos() const { return m_matLocal.GetRow(3).vec3; }

		//!	@brief		スケールを取得
		const float3&	GetScale() const { return m_scale; }
		
		//!	@brief		右方向ベクトルを取得する
		const float3&	GetRight() const { return m_matLocal.GetRow(0).vec3; }

		//!	@brief		上方向ベクトルを取得する
		const float3&	GetUp() const { return m_matLocal.GetRow(1).vec3; }

		//!	@brief		前方向ベクトルを取得する
		const float3&	GetForward() const { return m_matLocal.GetRow(2).vec3; }

		//!	@brief		ローカル行列を取得する
		const NMath::SMatrix44&	GetLocalMatrix() const { return m_matLocal; }

		//!	@brief		スケール情報を適用したローカル変換行列を取得する
		void			GetScaledLocalMatrix(NMath::SMatrix44& _rDest) const;

		//!	@brief		ワールド行列を計算する
		//!	@note		スケールを適用したローカル変換行列で計算する
		//!	@warning	親子関係の階層に比例して行列計算量が増える
		void			CalcWorldMatrix(NMath::SMatrix44& _rDest) const;

		//!	@brief		スケールを適用しない状態のワールド変換行列を取得する
		void			CalcWorldMatrixNoScale(NMath::SMatrix44& _rDest) const;

	private:

		// 所有者情報
		CGameObject*				m_pOwner;	// 所有者

		// 位置、姿勢情報
		NMath::SMatrix44			m_matLocal;		// ローカル行列
		float3						m_scale;

		// 親子関係
		CTransform*					m_pParent;
		std::vector<CTransform*>	m_pChildArray;

		//!	@brief	子供を設定する
		void SetChild(CTransform* _pChild);

		//!	@brief	指定した子供を解除する
		void UnsetChild(CTransform* _pChild);


	public:
		//!	@brief		所有者を設定するconstructor
		CTransform(CGameObject&	_rOwner);
		~CTransform();

	protected:
		CTransform(const CTransform&) { }
		const CTransform&	operator = (const CTransform&) { }
	};
}