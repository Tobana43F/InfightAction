// 未実装
// @note	これを実装しなくても効果音の再生ができるため実装を中止
//!			他の目的で、使用する用事が出来たら実装する

/*
#pragma once

#include	"Collision\CollisionProc\Collision_Proc.h"
#include	"Sound\Sound_Common.h"

namespace NObject {
	class CGameObject;
}

namespace NCollision {

	// 前方宣言
	class CAttackTypeData;
	class CDamageTypeData;


	//!	@brief		ゲーム内のキャラクターが攻撃を与えた時の保持するデータ
	//!	@details	用途
	//!				- 主に効果音の再生の制御を行うのに使用する。
	class CSendAttackInfo {
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
		bool	CheckIsSameID(const CSendAttackInfo& _rhs) const;

	private:

		CAttackTypeData				m_sendAttackData;

		// 攻撃したフラグ
		bool	m_isEnable;

		// 攻撃した当たり判定を特定するデータ
		const NObject::CGameObject*	m_pOwner;				// ポインタの値をIDとする
		uint						m_attackSequenceID;		// あたり判定シーケンス番号
		uint						m_attackGroupID;		// あたり判定グループ番号
		uint						m_attackSerialID;		// 連続攻撃番号

		// 再生するサウンドラベル
		NSound::ELabel				m_sndSELabel;

	public:
		CSendAttackInfo();
		~CSendAttackInfo();
	};

}
*/