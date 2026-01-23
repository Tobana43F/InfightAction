#pragma once

#include	"Object\Object_GameObject.h"

#include	<array>
#include	"Collision\Collision_Mesh.h"
#include	"Collision\Holder\Collision_Holder.h"
#include	"Collision\Manager\Collision_Manager.h"
#include	"Collision\Def\Collision_ReceiveAttackInfo.h"
#include	"Game\Def\Game_ActorMove.h"
#include	"Game\Def\Game_ActorParam.h"

namespace NObject {

	//!	@brief	ゲーム内のキャラクターや敵の基底クラス
	//!	@note	継承後のクラスで必要なこと
	//!			- 当たり判定クラス(Holder)の処理
	class CActor : public CGameObject {
	public:
		// 定数値
		static const uint	RECEIVE_ATTACK_INFO_BUF_SIZE = 16;	// バッファサイズ(nフレーム前までの当たり判定情報を保持する)
		static const uint	RECEIVE_ATTACK_INFO_MAX = 16;	// 保持する被攻撃情報を保持する数(1フレームで同時に受けられる攻撃数)

		// 型名省略
		//!	@warning	外部では使用しないでください。
		using ReceiveAttackInfos = std::array<NCollision::CReceiveAttackInfo, RECEIVE_ATTACK_INFO_MAX>;

		//----------------------------------------------------------
		// Actorの基本的な情報のゲッター
		//----------------------------------------------------------

		//!	@brief	残りライフを取得する
		int		GetLife() const { return m_actorParam.life; }

		//!	@brief	最大ライフを取得する
		int		GetLifeMax() const { return m_actorParam.maxLife; }

		//!	@brief	Actorの生存状態を取得
		bool	GetIsAlive() const { return m_actorParam.GetIsAlive(); }

		//!	@brief	Actorの消滅状態を取得(主にEnemy)
		bool	GetIsEliminated() const { return m_actorParam.isEliminated; }

		//----------------------------------------------------------
		// あたり判定関係
		//----------------------------------------------------------

		//!	@brief		受けた当たり判定情報を追加する
		//!	@details	RECEIVE_ATTACK_INFO_MAX以上は無視される。
		void AddReceiveAttackInfo(NCollision::CReceiveAttackInfo& _rSrc);

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

		//!	@brief	移動前の座標を取得する
		virtual void	UpdateFastProc() override;

		//!	@brief	更新処理を記述する
		virtual void	UpdateProc() override;

		//!	@brief	当たり判定の更新を行う
		virtual void	UpdateLateProc() override;

		//!	@brief	描画処理を記述する
		virtual void	RenderProc() override;

		//!	@brief	当たり判定のデバッグ描画
		virtual void	RenderLateProc() override;


		//----------------------------------------------------------
		// 基本的な情報
		//----------------------------------------------------------
		NGame::SActorParam			m_actorParam;	// 体力などの基本的なデータ

		//!	@brief	ActorParamで読み込むファイル名を取得する
		virtual NAsset::CJsonFactory::Key			GetActorParamAssetKey() PURE_FUNC;

		//!	@brief	敵の削除状態を設定
		void SetIsEliminated() { m_actorParam.isEliminated = true; }

		//----------------------------------------------------------
		// 動きに関わるもの
		//----------------------------------------------------------
		NGame::CActorMove			m_actorMove;

		//!	@brief	ActorMoveで使用する定数データのファイル名を変えす
		virtual NAsset::CJsonFactory::Key			GetActorMoveAssetKey() PURE_FUNC;

		//!	@brief	地面として使用するステージ用あたり判定マネージャのインデックスを返す
		virtual NCollision::EStageCollisionIndex	GetStageGroundIndex() { return NCollision::EStageCollisionIndex::GROUND_MAIN; }

		//!	@brief	壁として使用するステージ用あたり判定マネージャのインデックスを返す
		virtual NCollision::EStageCollisionIndex	GetStageWallIndex() { return NCollision::EStageCollisionIndex::WALL_MAIN; }

		//!	@brief	プレイヤーの操作を行う関数
		virtual void Operate() PURE_FUNC;
		
		//----------------------------------------------------------
		// あたり判定関係
		//----------------------------------------------------------

		//!	@brief	現在フレームで受けた当たり判定情報の数を取得
		uint	GetReceiveAttackInfoCount() const { return  RECEIVE_ATTACK_INFO_MAX; }

		//!	@brief	現在フレームで受けた当たり判定情報を取得
		const NCollision::CReceiveAttackInfo&	GetCurReceiveAttackInfo(
			uint	_idx) const;

		//!	@brief	過去フレームで受けた当たり判定情報を取得する
		const NCollision::CReceiveAttackInfo&	GetPrevReceiveAttackInfo(
			uint	_prevFrame,
			uint	_infoIdx) const;
		
		//!	@brief		被攻撃情報をもとにダメージ処理を実行する
		//!	@note		継承後にオーバーライドして、アニメーションの再生や、状態の遷移を行う
		//!	@note		ProcDamageCommon()で呼び出す
		virtual void ProcDamage(const NGame::SDamageParam& _rDamage);

		
		//----------------------------------------------------------
		// 被攻撃時の汎用処理
		//----------------------------------------------------------

		//!	@brief	攻撃を受けた際のActorMoveの設定を適用
		void SetDamageActorMoveParam();

		//!	@brief	通常時のActorMoveの設定を適用
		void SetNormalActorMoveParam();

		//!	@brief	ダメージを受けた際のノックバックをActorMoveに適用する
		//!	@note	SetDamageActorMoveParam()は内部で呼び出している
		void ApplyKnockBack(const NGame::SDamageParam& _rDamage);

		//!	@brief	ダメージを受けた際に攻撃者の方向を向く(補完なし)
		void LookAtAttacker(const NGame::SDamageParam& _rDamage);

	private:

		//----------------------------------------------------------
		// 当たり判定関係
		//----------------------------------------------------------
	
		//!	@brief	そのフレームで受けた攻撃判定情報をすべて保持するバッファ変数
		//!	@note	添え字の対応
		//!			- 0:最新　
		//!			- RECEIVE_ATTACK_INFO_BUF_SIZE - 1 : 最古
		std::array<ReceiveAttackInfos, RECEIVE_ATTACK_INFO_BUF_SIZE>	m_receiveAttackInfoBufs;

		//!	@brief	現フレームで受けた当たり判定のうち、過去フレーム(1フレーム前)で
		//!			受けていた当たり判定を省いた判定のみ保持する変数
		ReceiveAttackInfos												m_resultReceiveAttackInfos;

		//!	@brief	最終的に反映される攻撃情報
		NGame::SDamageParam												m_resultDamage;



		uint		m_curReceiveAttackInfoIndex;	// 被攻撃情報を追加する際に使用するインデックス()
													// 攻撃情報格納処理終了後は、現フレームに受けた攻撃の数を表す

		//!	@brief		被攻撃情報を初期化する
		//!	@note		UpdateProc()で呼び出さ
		void	InitReceiveAttackInfo();

		//!	@brief		現在受けている攻撃をもとに処理を行う
		//!				- 被攻撃判定処理をここですべて行う
		void	ProcReceiveAttackInfo();

		//!	@brief		最終的な受けた攻撃結果を初期化する
		//!	@note		ProcReceiveAttackInfo() で呼び出す
		void	InitResultReceiveAttackInfo();

		//!	@brief		現フレームで受けた攻撃のうち、実際に反映させるダメージ量などを計算し、
		//!				最終結果を作成する。
		//!	@note		ProcReceiveAttackInfo() で呼び出す
		void	MakeResultReceiveAttackInfo();


		//!	@brief		被攻撃時の共通処理処理を実行
		void	ProcDamageCommon(const NGame::SDamageParam& _rDamage);

	public:
		CActor();
		virtual ~CActor();
	};

}