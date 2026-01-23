#pragma once

#include	<array>
#include	"Object\Actor\Player\Object_Player.h"
#include	"Object\UI\Object_PlayerLifeUI.h"
#include	"Object\UI\Object_ScreenFade.h"
#include	"Game\Manager\Game_StageManager.h"
#include	"Collision\CollisionProc\Collision_HitTest.h"
#include	"Util\Util_StateManager.h"

namespace NGame {

	//!	@brief		ゲームのメインシステムを管理するマネージャ
	//!	@note		主な処理
	//!				- キャラクター
	//!				- 複数のステージ(ステージマネージャ)
	//!				- 当たり判定マネージャ
	//!				- その他、オブジェクト間の処理の仲介
	class CMainManager {
	public:

		//!	@brief		ステージの階層数の最大値
		static const uint	STAGE_LEVEL_MAX = 16;

		//!	@brief		ゲームの進行状態を示す定数
		enum class EState {
			INIT,
			INIT_NEXT_STATE,
			FADE_IN,
			FADE_OUT_STAGE_CLEAR,
			FADE_OUT_GAME_CLEAR,
			FADE_OUT_GAME_OVER,
			UPDATE,
			MAX
		};


		//!	@brief		使用可能状態にする
		void Setup();

		//!	@brief		初期化
		void Init();

		//!	@brief		終了処理
		void Destory();

		//!	@brief		更新処理
		void Update();

		//!	@brief		描画処理
		void Render();


	private:

		// ステージの管理
		uint												m_stageLevelCount;	// ステージ数
		uint												m_curStageLevel;	// 現在のステージレベル
		std::array<NGame::CStageManager, STAGE_LEVEL_MAX>	m_stageManagers;	// 全てのステージレベルを管理する変数

		// プレイヤー
		NObject::CPlayer				m_player;

		// 当たり判定を処理するクラス
		NCollision::CProcHitTest		m_colProcPlayerAttackAndEnemyDamage;
		NCollision::CProcHitTest		m_colProcPlayerDamageAndEnemyAttack;

		// UI関係
		NObject::CPlayerLifeUI			m_playerLifeUI;
		NObject::CScreenFade			m_screenFade;

		//----------------------------------------------------------
		// 状態管理
		//----------------------------------------------------------
		using StateParam = NUtil::CStateManager<CMainManager, EState>::Param;
		NUtil::CStateManager<CMainManager, EState>	m_state;

		void UpdateStateInit(StateParam _param);
		void UpdateStateInitNextStage(StateParam _param);
		void UpdateStateFadeIn(StateParam _param);
		void UpdateStateFadeOutStageClear(StateParam _param);
		void UpdateStateFadeOutGameClear(StateParam _param);
		void UpdateStateFadeOutGameOver(StateParam _param);
		void UpdateStateUpdate(StateParam _param);

		//----------------------------------------------------------
		// 初期化関係処理
		//----------------------------------------------------------

		//!	@brief	ステージ情報の読み込み
		void SetupAllStageLevel();

		//!	@note	ステージ開始時の初期化(m_curStageLvelのステージを初期化し、その他のオブジェクトを初期化する)
		void IniteCurStageLevel();

		//!	@brief	グローバルマネージャに登録
		void RegisterToGlobalManager();

		//----------------------------------------------------------
		// 汎用関数
		//----------------------------------------------------------
		 
		//!	@brief	ゲーム全体の更新処理
		void UpdateMain();

		//!	@brief	ゲームのクリア状態を判断
		void UpdateGameState();

	public:
		CMainManager();
		~CMainManager();
	};
}
