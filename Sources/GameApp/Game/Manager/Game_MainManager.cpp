#include	"Game_MainManager.h"

#include	<string>
#include	"Collision\Manager\Collision_Manager.h"
#include	"Camera\Camera_GlobalManager.h"
#include	"Game\Manager\Game_GlobalAccessManager.h"
#include	"Game_GlobalDataManager.h"
#include	"Scene\Scene_Manager.h"
#include	"System\System_Error.h"

namespace NGame {

	CMainManager::CMainManager() : 
		m_stageLevelCount(0),
		m_curStageLevel(0),
		m_state(this, EState::INIT)
	{
		// 当たり判定マネージャを登録
		NCollision::RegisterHitTestProc(
			&m_colProcPlayerAttackAndEnemyDamage,
			NCollision::EHitTestProcManagerIndex::PLAYER_ATK_ENEMY_DEF);

		NCollision::RegisterHitTestProc(
			&m_colProcPlayerDamageAndEnemyAttack,
			NCollision::EHitTestProcManagerIndex::PLAYER_DEF_ENEMY_ATK);

		// ステート登録
		m_state.Register(EState::INIT, &CMainManager::UpdateStateInit);
		m_state.Register(EState::INIT_NEXT_STATE, &CMainManager::UpdateStateInitNextStage);
		m_state.Register(EState::FADE_IN, &CMainManager::UpdateStateFadeIn);
		m_state.Register(EState::FADE_OUT_STAGE_CLEAR, &CMainManager::UpdateStateFadeOutStageClear);
		m_state.Register(EState::FADE_OUT_GAME_CLEAR, &CMainManager::UpdateStateFadeOutGameClear);
		m_state.Register(EState::FADE_OUT_GAME_OVER, &CMainManager::UpdateStateFadeOutGameOver);
		m_state.Register(EState::UPDATE, &CMainManager::UpdateStateUpdate);
	}

	CMainManager::~CMainManager()
	{
		// 登録したあたり判定マネージャを解除
		NCollision::UnregisterHitTestProc(&m_colProcPlayerAttackAndEnemyDamage);
		NCollision::UnregisterHitTestProc(&m_colProcPlayerDamageAndEnemyAttack);

		// グローバルマネージャに登録したポインタを解除
		NGame::UnregisterAllGlobalAccessManager();
	}

	void CMainManager::Setup()
	{
		// 当たり判定マネージャを初期化
		m_colProcPlayerAttackAndEnemyDamage.Setup();
		m_colProcPlayerDamageAndEnemyAttack.Setup();

		try {
			// ステージ情報を初期化
			SetupAllStageLevel();

			// グローバルマネージャに登録
			RegisterToGlobalManager();
		}
		catch (const char* _err) {
			std::string	err = "ゲームメインマネージャのSetup時に例外が発生しました。\n";
			err += "Message : ";
			err += _err;
			ErrorEx(err.data());
		}
	}

	void CMainManager::Init()
	{
		m_curStageLevel = 0;

		// 全てのステージを初期化する
		for (uint i = 0; i < m_stageLevelCount; ++i) {
			m_stageManagers[i].Init();
		}

		// プレイヤーを初期化する
		m_player.SetIsResetLife(true);
		m_player.Init();
		m_player.SetIsResetLife(false);

		// UI初期化
		m_playerLifeUI.Init();
		m_screenFade.Init();

		// ステート初期化
		m_state.Reset();

		// グローバルデータ
		NGame::GetGlobalData().isGameClear = false;
	}

	void CMainManager::Destory()
	{
		for (uint i = 0; i < m_stageLevelCount; ++i) {
			m_stageManagers[i].Destory();
		}
	}
		
	void CMainManager::Update()
	{
		// 状態更新
		m_state.Update();

		// 共通処理
		m_playerLifeUI.Update();
		m_screenFade.Update();
	}

	void CMainManager::Render()
	{
		NCamera::SetViewMatrix();
		NCamera::SetProjMatrix();

		m_stageManagers[m_curStageLevel].Render();
		m_player.Render();

		// 2D描画
		NCamera::SetOrthoMatrix();
		m_playerLifeUI.Render();
		m_screenFade.Render();
	}

	//----------------------------------------------------------

	void CMainManager::UpdateStateInit(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			// ステージ開始時の初期化
			IniteCurStageLevel();

			// すぐにフェードを開始する
			m_state.ChangeState(EState::FADE_IN);

			// 1フレームだけだが更新を行う
			UpdateMain();
		}
	}

	void CMainManager::UpdateStateInitNextStage(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			// 現在のステージの終了処理を行う
			m_stageManagers[m_curStageLevel].Destory();

			// ステージの初期化
			// ステージ開始時の初期化
			IniteCurStageLevel();

			// すぐにフェードを開始する
			m_state.ChangeState(EState::FADE_IN);

			// 次のステージへ
			m_curStageLevel++;

			// 1フレームだけだが更新を行う
			UpdateMain();
		}
	}

	void CMainManager::UpdateStateFadeIn(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_screenFade.SetFadeSec(2);
			m_screenFade.SetFadeColor(float4(1, 1, 1, 1));
			m_screenFade.FadeIn();
		}

		UpdateMain();

		if (m_screenFade.GetIsFadeEnd()) {
			m_state.ChangeState(EState::UPDATE);
		}
	}

	void CMainManager::UpdateStateFadeOutStageClear(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_screenFade.SetFadeSec(2);
			m_screenFade.SetFadeColor(float4(1, 1, 1, 1));
			m_screenFade.FadeOut();
		}

		UpdateMain();

		if (m_screenFade.GetIsFadeEnd()) {
			m_state.ChangeState(EState::INIT_NEXT_STATE);
		}
	}

	void CMainManager::UpdateStateFadeOutGameClear(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_screenFade.SetFadeSec(3);
			m_screenFade.SetFadeColor(float4(1, 1, 1, 1));
			m_screenFade.FadeOut();

			NGame::GetGlobalData().isGameClear = true;
		}

		UpdateMain();

		if (m_screenFade.GetIsFadeEnd()) {
			// 遷移
			NScene::ChangeScene("CResult");
		}
	}

	void CMainManager::UpdateStateFadeOutGameOver(StateParam _param)
	{
		if (_param.GetIsEntry()) {
			m_screenFade.SetFadeSec(3);
			m_screenFade.SetFadeColor(float4(0, 0, 0, 1));
			m_screenFade.FadeOut();
		}
	
		UpdateMain();

		if (m_screenFade.GetIsFadeEnd()) {
			// 遷移
			NScene::ChangeScene("CResult");
		}
	}

	void CMainManager::UpdateStateUpdate(StateParam _param)
	{
		UpdateMain();

		// 状態の判断はここだけで行う
		UpdateGameState();
	}


	//----------------------------------------------------------

	void CMainManager::SetupAllStageLevel()
	{
		//!	@todo	- ステージ情報を管理するJSONをすべて取得する
		//!			- それぞれの情報を各ステージに割り当て、Setup

		NAsset::CJsonFactory::SPAsset pJson = NAsset::CJsonFactory::GetInst().Load("Data/Stage/StageInfo.json");
		
		// アクセス省略
		auto& json = *pJson;

		m_stageLevelCount = json["StageInfo"].array_items().size();

		for (uint i = 0; i < m_stageLevelCount; ++i) {
			auto& stageJsonKey = json["StageInfo"].array_items()[i].string_value();
			m_stageManagers[i].Setup(i, stageJsonKey);
		}
	}

	void CMainManager::IniteCurStageLevel()
	{
		// 現在のレベルのステージを初期化
		//!	@note	ステージのあたり判定の設定を行うため、ほかのオブジェクトより先に初期化すること
		m_stageManagers[m_curStageLevel].Init();

		// プレイヤー初期化
		m_player.Init();
	}

	void CMainManager::RegisterToGlobalManager()
	{
		// プレイヤーを登録
		NGame::RegisterPlayer(m_player);
		
		//!	@note	敵は生成時に登録する
	}

	//----------------------------------------------------------

	void CMainManager::UpdateMain()
	{
		// プレイヤーを更新
		m_player.Update();

		// 現在のステージを更新
		m_stageManagers[m_curStageLevel].Update();

		// 当たり判定の更新処理
		m_colProcPlayerAttackAndEnemyDamage.Update();
		m_colProcPlayerDamageAndEnemyAttack.Update();
	}

	void CMainManager::UpdateGameState()
	{
		// ステージクリア後の進行処理
		//!	@todo	- 次のステージに進行
		//!			- 全てのステージクリア後はクリア処理
		if (m_stageManagers[m_curStageLevel].GetIsClear()) {
			// 次のステージを判断
			uint nextStageLvel = m_curStageLevel;
			nextStageLvel++;

			// 全てのステージをクリアした場合
			if (nextStageLvel >= m_stageLevelCount) {
				m_state.ChangeState(EState::FADE_OUT_GAME_CLEAR);
			}
			else {
				m_state.ChangeState(EState::FADE_OUT_STAGE_CLEAR);
			}
		}

		// ゲームオーバー処理
		if (m_player.GetIsEliminated()) {
			m_state.ChangeState(EState::FADE_OUT_GAME_OVER);
		}
	}
}
