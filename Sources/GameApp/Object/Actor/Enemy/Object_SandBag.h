#pragma once

#include	"Object_Enemy.h"

#include	"Model\Model_Object.h"
#include	"Util\Util_StateManager.h"

namespace NObject {

	//!	@brief	テスト用の敵
	class CSandBag : public CEnemy {
	public:
		enum EState {
			ST_NORMAL_IDLE,
			ST_NORMAL_DIE,
			ST_DAMAGE_L_HEAD,
			ST_DAMAGE_L_BODY,
			ST_DAMAGE_L_FOOT,
			ST_DAMAGE_H_BLOW_BACK,	// 後ろ吹っ飛び
			ST_MAX
		};

		//!	@brief	モデルのアニメーションを示すインデックス
		enum EAnmIndex {
			ANM_DAMAGE_L_BODY,
			ANM_DAMAGE_H_BLOW_BACK,
			ANM_DAMAGE_L_HEAD,
			ANM_DAMAGE_L_FOOT,
			ANM_NORMAL_IDLE
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


		//!	@brief	ActorParamデータのパス
		virtual NAsset::CJsonFactory::Key	GetActorParamAssetKey() { return "Data/Actor/Enemy/SandBag/ActorParam.json"; }

		//!	@brief	ActorMoveデータを取得する
		virtual NAsset::CJsonFactory::Key	GetActorMoveAssetKey() { return "Data/Actor/Enemy/SandBag/ActorMove.json"; }

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

		//----------------------------------------------------------
		// 状態遷移関係
		//----------------------------------------------------------
		using StateParam = NUtil::CStateManager<CSandBag, EState>::Param;	// 引数省略
		NUtil::CStateManager<CSandBag, EState>	m_state;

		void UpdateIdleState(StateParam _param);			// たち状態を更新
		void UpdateDamageLight(StateParam _param);			// ダメージ軽
		void UpdateDamageHeavy(StateParam _param);			// ダメージ重い
		void UpdateDie(StateParam _param);					// ダメージ重い

		//----------------------------------------------------------
		//　共通処理関数
		//----------------------------------------------------------

	public:
		CSandBag();
		virtual ~CSandBag();
	};

}