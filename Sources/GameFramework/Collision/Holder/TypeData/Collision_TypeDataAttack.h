#pragma once

#include	"_External\json11\json11.hpp"

namespace NCollision {

	//!	@brief	Attackタイプのあたり判定データ
	//!	@details	当たり判定Groupが保持するデータ
	class CAttackTypeData {
	public:

		//----------------------------------------------------------
		// 攻撃情報用の定義
		//----------------------------------------------------------

		//!	@brief	XZ平面でのノックバック方向のタイプ(Y軸は固定で0,1,0方向)
		enum class EKnockBackType {
			RADIATE,		// 攻撃者から放射的にノックバックする
			DIRECTIONAL,	// プレイヤーから見た指定方向にノックバックする
			ABSOLUTE_DIR	// 絶対座標での指定した方向にノックバックする。
		};

		//!	@brief	攻撃の強さ(吹っ飛ばしなどの設定もこの値を使用する)
		enum class EStrengthType {
			LOW,		// 弱
			MIDDLE,		// 中
			HIGH,		// 強
			BLOW		// 吹き飛ばし
		};

		//----------------------------------------------------------
		// 基本的な関数群
		//----------------------------------------------------------

		//!	@brief		jsonからのデータ読み込み
		//!	@param[in]	_enableFrame	エラーチェック用に必要
		void Load(json11::Json _groupData, uint _enableFrame);

		//!	@brief	初期化
		void Init();

		//!	@brief	更新
		void Update();


		//----------------------------------------------------------
		// アクセッサ
		//----------------------------------------------------------

		// ゲッター
		uint		GetSerialID() const { return m_curSerialID; }

		// 攻撃情報ゲッター
		int						GetPriority() const { return m_priority; }
		int						GetBaseDamage() const { return m_baseDamage; }
		EKnockBackType			GetKnockBackType() const { return m_knockBackType; }
		EStrengthType			GetStrengthType() const { return m_strengthType; }
		const NMath::SVector3&	GetKnockBackDir() const { return m_knockBackDir; }
		float					GetKnockBackPow() const { return m_knockBackPow; }
				
	private:
		// プログラム依存データ
		uint					m_curSerialID;		// シリアルID(0から1づつ増加する)
		uint					m_cnt;				// シーケンス開始時のカウンタ

		//----------------------------------------------------------
		// プログラム非依存データ(外部データ)
		//----------------------------------------------------------
		uint					m_serialFrameCount;	// シリアルフレームの数(0なら1度のみヒットする)
		std::vector<uint>		m_serialFrames;		// シリアルID(連続ヒットに使用するID)が変更されるフレーム
													// 小さい値の順に並んでいること。
													// GroupのEnableFrameより小さい値の中で設定する

		//----------------------------------------------------------
		// 攻撃情報(外部データ)
		//----------------------------------------------------------

		int						m_priority;			// 優先度(値が大きいほど優先される。基本は0)
		int						m_baseDamage;		// 基本のダメージ量

		EKnockBackType			m_knockBackType;	// ノックバックタイプ
		EStrengthType			m_strengthType;		// 強さ(タイプ)
		NMath::SVector3			m_knockBackDir;		// ノックバック方向(正規化済みであること)
		float					m_knockBackPow;		// ノックバックの強さ(衝撃)

	public:
		CAttackTypeData();
		~CAttackTypeData();
	};
}