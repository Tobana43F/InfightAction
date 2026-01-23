#pragma once

#include	"Collision\CollisionProc\Collision_Proc.h"

namespace NObject {
	class CGameObject;
}

namespace NCollision {

	// 前方宣言
	class CAttackTypeData;
	class CDamageTypeData;

	//!	@brief		ゲーム内のキャラクターが攻撃を受けた時に保持するデータ
	//!	@details	用途
	//!				- 当たり判定処理の状態を被攻撃側クラスが保持するデータ
	//!				  被攻撃側がこのクラスを保持して、この情報をもとに、必要な情報を取得する。
	//!				- 攻撃された情報を保持して、攻撃のトリガーの判断や、
	//!				  連続攻撃のトリガー判断に使用する。
	class CReceiveAttackInfo {
	public:

		//!	@brief	情報を初期化する
		//!	@note	毎フレーム実行する
		void Init();

		//!	@brief		衝突時のデータを設定する
		//!	@param[in]	_rAttackColData	攻撃側の当たり判定情報
		//!	@param[in]	_rAttackData	攻撃タイプのデータ
		//!	@note		元の攻撃、ダメージ上方データをもとに最終的に受けるダメージ量などを計算する。
		void SetData(
			const CProc::SCollideData&	_rAttackColData,
			const CAttackTypeData&		_rAttackData, 
			const CProc::SCollideData&	_rDamageColData,
			const CDamageTypeData&		_rDamageData);


		//!	@brief		渡されたCReceiveAttackInfoがおなじIDかどうかを判断する
		//!	@note		有効状態ではない場合は判断せずfalseを返す
		//!	@return		全てのIDが等しい場合trueを返す
		bool	CheckIsSameID(const CReceiveAttackInfo& _rhs) const;


		//----------------------------------------------------------
		// 受けた攻撃情報のアクセッサ
		//----------------------------------------------------------

		//!	@brief	攻撃上方がセットされているかを取得する
		bool	GetIsEnable() const { return m_isEnable; }

		//!	@brief	攻撃情報のゲッター群
		const CAttackTypeData&	GetReceiveAttackData() const { return m_receiveAttackData; }

		//!	@brief	攻撃してきたオブジェクトの座標を取得
		const float3&			GetAttackOwnerPos() const { return m_attackOwnerPos; }

		//!	@brief	衝突座標を取得する
		const float3&			GetCollidePos() const { return m_collidePos; }
		
		//!	@brief	計算して取得したノックバック方向を取得する
		const float3			GetKnockBackDir() const { return m_knockBackDir; }

	private:
		// 受けた攻撃情報がセットされているフラグ
		bool	m_isEnable;

		// 攻撃された当たり判定を特定するデータ
		const NObject::CGameObject*	m_pOwner;				// ポインタの値をIDとする
		uint						m_attackSequenceID;		// あたり判定シーケンス番号
		uint						m_attackGroupID;		// あたり判定グループ番号
		uint						m_attackSerialID;		// 連続攻撃番号

		//----------------------------------------------------------
		// 受けた攻撃情報
		//----------------------------------------------------------

		//!	@brief	相手の攻撃情報
		//!	@note	データのみを参照する。(アクセッサ以外の関数は呼ばない。)
		CAttackTypeData				m_receiveAttackData;

		//  受け取った情報をもとに作成するデータ
		float3						m_attackOwnerPos;		// 衝突した当たり判定の所有者(GameObject)の位置
		NMath::SMatrix44			m_attackOwnerDir;		// 向き
		float3						m_collidePos;			// 衝突位置

		float3						m_myPos;		// その時点での自分の位置
		NMath::SMatrix44			m_myDir;	

		// ノックバック方向
		NMath::SVector3				m_knockBackDir;	// 計算後のノックバック方向

	public:
		CReceiveAttackInfo();
		~CReceiveAttackInfo();
	};
}