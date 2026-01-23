#pragma once

#include	"Object_Enemy.h"

#include	"Model\Model_Object.h"
#include	"Util\Util_StateManager.h"

namespace NObject {

	//!	@brief	雑魚敵(バビー)
	//!	@note	概要
	//!			- プレイヤーを常にプレイヤーを付け回し、
	//!			  弱い攻撃を素早く行う
	class CBaby : public CEnemy {
	public:

		//!	@brief	状態遷移
		enum class EState {
			IDLE,
			FOLLOW,
			ATTACK,
			
			DAMAGE_LOW,		
			DAMAGE_HIGH,	

			DAMAGE_RECOVER,	// ダメージ後の復帰処理
			DAMAGE_DIE,		// 死亡

			MAX
		};

		//!	@brief	アニメーション
		enum EAnmIndex {
			ATTACK_HEAD_BAT,
			DAMAGE_L_BODY,
			DAMAGE_H_BLOW_BACK,
			DAMAGE_L_HEAD,
			DAMAGE_L_FOOT,
			NORMAL_IDLE,
			MAX
		};

	protected:
		//!	@brief	生成時に1度だけ行う処理を記述する
		virtual void	LoadProc() override;

		//!	@brief	終了時に1度だけ行う処理を記述する
		virtual void	ReleaseProc() override;

		//!	@brief	初期化処理を記述する
		virtual void	InitProc() override;

		//!	@brief	終了処理を記述する
		//!	@note	あまり使うことはない
		virtual void	DestroyProc() override;

		//!	@brief	更新処理を記述する
		virtual void	UpdateProc() override;

		//!	@brief	当たり判定の更新
		virtual void	UpdateLateProc() override;

		//!	@brief	描画処理を記述する
		virtual void	RenderProc() override;


		//!	@brief	ActorParamのデータのパス
		virtual NAsset::CJsonFactory::Key			GetActorParamAssetKey() { return "Data/Actor/Enemy/Baby/ActorParam.json"; }

		//!	@brief	ActorMoveデータを取得する
		virtual NAsset::CJsonFactory::Key GetActorMoveAssetKey() { return "Data/Actor/Enemy/Baby/ActorMove.json"; }

		//!	@brief	操作関数
		virtual void	Operate() override;

		//!	@brief	ダメージを受けた際の処理
		virtual void	ProcDamage(const NGame::SDamageParam& _rDamage) override;

	private:

		// 親クラスのエイリアス
		using Base = CEnemy;

		NModel::CObject				m_model;	// 描画用モデル

		// あたり判定
		NCollision::CHolder			m_damageHolder;
		NCollision::CHolder			m_attackHolder;

		// 制御関係
		CGameObject*				m_pFollowObject;	// このオブジェクトを追従する
		int							m_idleFrame;

		//----------------------------------------------------------
		// 状態遷移関係
		//----------------------------------------------------------
		using StateParam = NUtil::CStateManager<CBaby, EState>::Param;	// 引数省略
		NUtil::CStateManager<CBaby, EState>	m_state;

		void UpdateStateIdle(StateParam _param);			// たち状態を更新
		void UpdateStateFollow(StateParam _param);
		void UpdateStateAttack(StateParam _param);

		void UpdateStateDamageLow(StateParam _param);
		void UpdateStateDamageHigh(StateParam _param);

		void UpdateStateDamageRecover(StateParam _param);
		void UpdateStateDamageDie(StateParam _param);


		//----------------------------------------------------------
		// 汎用操作関数
		//----------------------------------------------------------
	
		void UpdateUtilFollowPlayer();

		//----------------------------------------------------------
		// 状態取得
		//----------------------------------------------------------
		bool CheckIsAttackRange();

	public:
		CBaby();
		virtual ~CBaby();
	};
}
