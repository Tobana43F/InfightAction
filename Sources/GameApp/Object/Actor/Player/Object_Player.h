#pragma once

#include	"Object\Actor\Object_Actor.h"

#include	"Model\Model_Object.h"
#include	"Util\Util_StateManager.h"
#include	"Asset\Factory\Asset_ShaderFactory.h"
#include	"Camera\Operator\Camera_OperatorPlayer.h"

#include	"Object\Object_Effect.h"

namespace NObject {

	//!	@brief	テスト用のキャラクター
	//!	@todo	安定したら実際にプレイヤークラスにする
	class CPlayer : public CActor {
	public:

		enum EBoneIndex {
			BONE_SWORD_HOLDER = 64,
			BONE_RIGHT_HAND_WEAPON = 42,
		};

		//!	@brief	このクラスが管理する状態
		enum class EBigState {
			NORMAL,
			DAMAGE,
			ATTACK
		};

		//!	@brief	モデルのアニメーションインデックス
		enum EAnmIndex {
			ANM_ATTACK_AIR1,
			ANM_ATTACK_AIR2,
			ANM_ATTACK_AIR3,
			ANM_ATTACK_AIR_FINISH_START,
			ANM_ATTACK_AIR_FINISH_FALL,
			ANM_ATTACK_AIR_FINISH_END,
			ANM_ATTACK_PUNCH,
			ANM_ATTACK_SWORD_DOUBLE_UPPER,
			ANM_ATTACK_SWORD_ENTRY1,
			ANM_ATTACK_SWORD_GROUND_SMASH,
			ANM_ATTACK_SWORD_HORIZONTAL1,

			ANM_DAMAGE_DIE,
			ANM_DAMAGE_HEAVY,
			ANM_DAMAGE_LIGHT,

			ANM_NORMAL_DASH,
			ANM_NORMAL_GRAB_SWORD_SET,
			ANM_NORMAL_GRAB_SWORD_START,
			ANM_NORMAL_GROUNDED,
			ANM_NORMAL_IDLE,
			ANM_NORMAL_JUMP_DOWN,
			ANM_NORMAL_JUMP_START,
			ANM_NORMAL_JUMP_UP,

			ANM_NORMAL_SWORD_DASH,
			ANM_NORMAL_SWORD_SET,
		};



		// Init時にライフをリセットするフラグを設定
		void SetIsResetLife(const bool _state) { m_isResetLife = _state; }

	protected:

		//----------------------------------------------------------
		// 内部クラス
		//----------------------------------------------------------

		//!	@brief	通常状態を管理するクラス
		class CNormalState {
		public:
			//!	@brief	管理する状態
			enum class EState {
				IDLE,
				DASH,
				JUMP_START,
				JUMP_UP,
				JUMP_DOWN,
				GROUNDED,

				SWORD_SWITCH,			// 背中の武器に手をのばす
				SWORD_GRAB_TO_SET,		// 握ってか前に移る
				SWORD_GRAB_TO_ATTACK,	// 握ってすぐに攻撃に移る
				SWORD_SET,
				SWORD_DASH,

				MAX,
			};

			//!	@brief	ロード
			void Load();

			//!	@todo	初期化の実装
			void Init() { 
				m_state.Reset();
				m_moveSpeedJumpStart = 0;
				m_isIdleAttackReady = false;
				m_cntSwordSet = 0;
			}

			//!	@brief	管理外の状態から遷移した際に実行される関数
			void Entry(const EState _state) { 
				m_state.ChangeState(_state);
				m_bindObject.m_stateBig.ChangeState(EBigState::NORMAL);

				m_colDamageHolder.Reset();
			}

			//!	@brief	更新
			void Update() { 
				m_state.Update();
			}

			//!	@brief	当たり判定の更新
			void UpdateLate()
			{
				m_colDamageHolder.Update(m_state.GetStateNumber());
			}

			//!	@brief	当たり判定の描画
			void RenderDebug()
			{
				m_colDamageHolder.RenderDebug();
			}

			//!	@brief	ステートの有効状態を設定
			void SetIsEnable(bool _state)
			{
				m_colDamageHolder.SetIsEnable(_state);
			}

		private:
			using StateParam = NUtil::CStateManager<CNormalState, EState>::Param;	// 引数省略
			NUtil::CStateManager<CNormalState, EState>	m_state;	// 状態遷移の管理

			CPlayer&										m_bindObject;

			// あたり判定
			NCollision::CHolder		m_colDamageHolder;

			// 制御用
			float	m_moveSpeedJumpStart;	// ジャンプ時の移動速度
			bool	m_isIdleAttackReady;	// 通常状態から攻撃状態に移る際に使用するフラグ
			int		m_cntSwordSet;			// SwordSetステートでいた時間

			// それぞれの状態で実行する関数
			void UpdateStateIdle(StateParam _param);
			void UpdateStateDash(StateParam _param);
			void UpdateStateJumpStart(StateParam _param);
			void UpdateStateJumpUp(StateParam _param);
			void UpdateStateJumpDown(StateParam _param);
			void UpdateStateGrounded(StateParam _param);
			void UpdateStateSwordSwitch(StateParam _param);
			void UpdateStateSwordGrabToSet(StateParam _param);
			void UpdateStateSwordGrabToAttack(StateParam _param);
			void UpdateStateSwordSet(StateParam _param);
			void UpdateStateSwordDash(StateParam _param);

		public:
			CNormalState(CPlayer& _bindObject);
			~CNormalState();
		};

		//!	@brief	ダメージ状態を管理するクラス
		class CDamageState {
		public:
			//!	@brief	管理する状態
			enum class EState {
				GROUND_LOW,
				GROUND_MIDDLE,
				GROUND_HIGH,
				GROUND_RECOVER,
				
				AIR,			// 空中ダメージ
				AIR_UP,			// 空中ダメージ上昇
				AIR_FALL,		// 空中ダメージ下降
				GROUNDED,		// 落下後の着地(叩きつけられる)
				LAY_RECOVER,	// 空中落下後の立ち上がり

				DIE,

				MAX,
			};

			//!	@brief	ロード
			void Load();

			//!	@brief	初期化
			void Init()
			{
				m_state.Reset();
			}

			//!	@brief	管理外の状態から遷移した際に実行される関数
			void Entry(const EState _state) {
				m_state.ChangeState(_state); 
				m_bindObject.m_stateBig.ChangeState(EBigState::DAMAGE);
			}

			//!	@brief	更新
			void Update() { m_state.Update(); }

			//!	@brief	当たり判定の更新
			void UpdateLate()
			{
				m_colDamageHolder.Update(m_state.GetStateNumber());
			}

			//!	@brief	当たり判定描画
			void RenderDebug()
			{
				m_colDamageHolder.RenderDebug();
			}

			//!	@brief	ステートの有効状態を設定
			void SetIsEnable(bool _state)
			{
				m_colDamageHolder.SetIsEnable(_state);
			}

		private:

			// 当たり判定関係
			NCollision::CHolder		m_colDamageHolder;

			// 状態管理
			using StateParam = NUtil::CStateManager<CDamageState, EState>::Param;	// 引数省略
			NUtil::CStateManager<CDamageState, EState>	m_state;	// 状態遷移の管理

			CPlayer&										m_bindObject;

			void UpdateStateGroundLow(StateParam _state);
			void UpdateStateGroundMiddle(StateParam _state);
			void UpdateStateGroundHigh(StateParam _state);
			void UpdateStateGroundRecover(StateParam _state);
			void UpdateStateAir(StateParam _state);
			void UpdateStateAirUp(StateParam _state);
			void UpdateStateAirFall(StateParam _state);
			void UpdateStateGrounded(StateParam _state);
			void UpdateStateLayRecover(StateParam _state);
			void UpdateStateDie(StateParam _state);


		public:
			CDamageState(CPlayer& _bindObject);
			~CDamageState();
		};

		//!	@brief	攻撃状態を管理するクラス
		class CAttackState {
		public:
			//!	@brief	管理する状態
			enum class EState { 
				INFIGHT_PUNCH,
				SWORD_ENTRY1,
				SWORD_HORIZONTAL1,
				SWORD_DOUBLE_UPPER,
				SWORD_GROUND_SMASH,

				SWORD_AIR1,
				SWORD_AIR2,
				SWORD_AIR3,
				SWORD_AIR_FINISH_START,
				SWORD_AIR_FINISH_END,
				SWORD_AIR_FINISH_FALL,

				MAX
			};

			//!	@brief	ロード
			void Load();

			//!	@brief	初期化
			void Init()
			{
				m_state.Reset();
			}

			//!	@brief	管理外の状態から遷移した際に実行される関数
			void Entry(const EState _state) {
				m_state.ChangeState(_state); 
				m_bindObject.m_stateBig.ChangeState(EBigState::ATTACK); 
				
				m_colAttackHolder.Reset();
				m_colDamageHolder.Reset();
			}

			//!	@brief	更新
			void Update() {
				m_state.Update(); 
			}

			//!	@brief	当たり判定の更新
			void UpdateLate()
			{
				// あたり判定更新
				m_colDamageHolder.Update(m_state.GetStateNumber());
				m_colAttackHolder.Update(m_state.GetStateNumber());
			}

			//!	@brief	当たり判定の描画
			void RenderDebug() {
				m_colDamageHolder.RenderDebug(); 
				m_colAttackHolder.RenderDebug();
			}

			//!	@brief	ステートの有効状態を設定
			void SetIsEnable(bool _state)
			{
				m_colDamageHolder.SetIsEnable(_state);
				m_colAttackHolder.SetIsEnable(_state);
			}

		private:

			// 当たり判定関係
			NCollision::CHolder		m_colDamageHolder;
			NCollision::CHolder		m_colAttackHolder;


			// 状態遷移関係
			using StateParam = NUtil::CStateManager<CAttackState, EState>::Param;	// 引数省略
			NUtil::CStateManager<CAttackState, EState>	m_state;	// 状態遷移の管理

			CPlayer&										m_bindObject;

			void UpdateStateInFightPunch(StateParam _param);
			void UpdateStateSwordEntry1(StateParam _param);
			void UpdateStateSwordHorizontal1(StateParam _param);
			void UpdateStateSwordDoubleUpper(StateParam _param);
			void UpdateStateSwordGroundSmash(StateParam _param);

			void UpdateStateSwordAir1(StateParam _param);
			void UpdateStateSwordAir2(StateParam _param);
			void UpdateStateSwordAir3(StateParam _param);
			void UpdateStateSwordAirFinishStart(StateParam _param);
			void UpdateStateSwordAirFinishEnd(StateParam _param);
			void UpdateStateSwordAirFinishFall(StateParam _param);


		public:
			CAttackState(CPlayer& _bindObject);
			~CAttackState();
		};

		//----------------------------------------------------------
		// 関数
		//----------------------------------------------------------

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

		//!	@brief	当たり判定を更新する
		virtual void	UpdateLateProc() override;

		//!	@brief	描画処理を記述する
		virtual void	RenderProc() override;


		//!	@brief	ActorParamのデータへのパス
		virtual NAsset::CJsonFactory::Key GetActorParamAssetKey() { return "Data/Actor/Player/ActorParam.json"; }

		//!	@brief	ActorMoveのファイルを読み込む
		virtual NAsset::CJsonFactory::Key GetActorMoveAssetKey() { return "Data/Actor/Player/ActorMove.json"; }

		//!	@brief	操作関数
		virtual void Operate();

		//!	@brief	ダメージ処理
		virtual void ProcDamage(const NGame::SDamageParam& _rDamage);

	private:

		// 継承元省略
		using Base = CActor;

		 
		NModel::CObject					m_model;	// 描画用モデル
		NAsset::CShaderFactory::SPAsset	m_shader;

		// 制御用
		bool							m_isResetLife;	// 初期化時にライフをリセットするフラグ

		// 剣の管理
		NModel::CObject					m_swordModel;
		NObject::CGameObject			m_swordTransform;
		bool							m_isSwordGrab;

		// 使用するエフェクト(テスト)
		NObject::CEffect m_effect;

		//----------------------------------------------------------
		// カメラ操作関係
		//----------------------------------------------------------

		//!	@todo	仮実装
		//!			- メンバに持つのはいいが,マネージャへの登録は、
		//!			  ほかの場所で行う
		NCamera::COperatorPlayer		m_cameraOperator;	// カメラ操作クラス
		float							m_moveSpeedAlias;	// 移動速度のエイリアス

		//!	@brief	カメラオペレータに渡すパラメータを更新する関数
		void	UpdateOperatorParam();

		//----------------------------------------------------------
		// 状態遷移関係
		//----------------------------------------------------------

		//!	@brief	排他制御が必要(当たり判定Holderの排他制御)
		CNormalState		m_normalState;
		CDamageState		m_damageState;
		CAttackState		m_attackState;

		using BigStateParam = NUtil::CStateManager<CPlayer, EBigState>::Param;	// 引数省略
		NUtil::CStateManager<CPlayer, EBigState>	m_stateBig;

		void UpdateStateNormal(BigStateParam _param) { m_normalState.Update(); }
		void UpdateStateDamage(BigStateParam _param) { m_damageState.Update(); }
		void UpdateStateAttack(BigStateParam _param) { m_attackState.Update(); }

		//!	@brief	ステートの排他制御を行う
		//!	@note	主にあたり判定の有効状態の制御
		void UpdateExcludeState();

		//----------------------------------------------------------
		// 汎用操作関数
		//----------------------------------------------------------

		//!	@brief	移動を実行
		void UpdateUtilMove();

		//!	@brief	落下状態へ遷移
		void UpdateUtilFall();

		//----------------------------------------------------------
		// 状態を取得
		//----------------------------------------------------------

		//!	@brief	移動状態
		bool	GetIsStop() const;

		//!	@brief	着地状態
		//!	@note	基本はActorMoveの値を使用するが、
		//!			それ以外の判定が必要になった時ように関数化
		bool	GetIsGrounded() const;

		//!	@brief	落下状態
		bool	GetIsFalling() const;

		//!	@brief	移動速度の割合を取得
		float	GetMoveSpeedXZRatio() const;

	public:
		CPlayer();
		virtual ~CPlayer();

	};

}
