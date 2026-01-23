#include	"Game_GlobalAccessManager.h"

#include	<array>
#include	"Debug\Debug_Assert.h"

namespace {

	//----------------------------------------------------------
	// 定義
	//----------------------------------------------------------

	const uint		MANAGEABLE_STAGE_MAX		= 128;		// 管理できるステージ数
	const uint		MANAGEABLE_STAGE_ENEMY_MAX	= 128;		// 管理できる１ステージあたりの敵数


	//!	@brief	全てのステージの全ての敵を管理する構造体
	struct SAllStageEnemys {
		//!	@brief	ステージに対応する全敵を格納する型
		using StageEnemys = std::array<NGame::CEnemyManager::CStageEnemy*, MANAGEABLE_STAGE_ENEMY_MAX>;

		//!	@brief	全ステージの全敵を管理する2次元配列
		//!	@note	[StageID][No] = pEnemy
		//!	@note	敵は登録した順番で管理される
		std::array<StageEnemys, MANAGEABLE_STAGE_MAX>	stageEnemys;

		//!	@brief	全ステージの登録された敵の数を管理する配列
		//!	@note	[StageID] = EnemyCount
		std::array<uint, MANAGEABLE_STAGE_MAX>			stageEnemyCount;


		//!	@brief	初期化
		void Setup()
		{
			UnregisterAll();
		}
		
		//!	@brief	敵の登録を行う
		void Register(
			const uint							_stageID,
			NGame::CEnemyManager::CStageEnemy&	_rEnemy)
		{
			Assert(_stageID < MANAGEABLE_STAGE_MAX && "管理できるステージ数を超えたステージIDが指定されました。");

			// 現在のカウンタを取得
			int curCount = stageEnemyCount[_stageID];
			Assert(curCount < MANAGEABLE_STAGE_ENEMY_MAX && "1ステージに登録できる敵の数の上限を超えました。");

			// 敵を登録
			stageEnemys[_stageID][curCount] = &_rEnemy;
			Assert(stageEnemys[_stageID][curCount] != nullptr && "nullptrが設定されました。");

			// カウンタを加算
			stageEnemyCount[_stageID]++;
		}

		//!	@brief	全ての登録を解除する
		void UnregisterAll()
		{
			// 全てnullptrで初期化
			for (int iStage = 0; iStage < MANAGEABLE_STAGE_MAX; ++iStage) {
				for (int iEnemy = 0; iEnemy < MANAGEABLE_STAGE_ENEMY_MAX; ++iEnemy) {
					stageEnemys[iStage][iEnemy] = nullptr;
				}
			}

			// 敵の数を0で初期化
			for (int iStage = 0; iStage < MANAGEABLE_STAGE_MAX; ++iStage) {
				stageEnemyCount[iStage] = 0;
			}
		}
	};


	//----------------------------------------------------------
	// 変数
	//----------------------------------------------------------

	// プレイヤーへのアクセス
	NObject::CPlayer*	g_pPlayer;

	// 全てのステージの敵の管理を行う
	SAllStageEnemys		g_allStageEnemys;
}

namespace NGame {

	void SetupGlobalAccessManager()
	{
		// 管理するプレイヤーを初期化
		g_pPlayer = nullptr;

		// 敵管理変数を初期化
		g_allStageEnemys.Setup();
	}

	void TearDownGlobalAccessManager()
	{
		UnregisterAllGlobalAccessManager();
	}

	//----------------------------------------------------------

	void RegisterPlayer(NObject::CPlayer& _rPlayer)
	{
		if (g_pPlayer != nullptr) {
			OutWarningMessageEx("グローバルマネージャのプレイヤーに上書きして登録しようとしています。");
		}
		g_pPlayer = &_rPlayer;
	}

	void RegisterEnemy(const uint _stageID, NGame::CEnemyManager::CStageEnemy& _rEnemy)
	{
		int curCount = g_allStageEnemys.stageEnemyCount[_stageID];
		if (g_allStageEnemys.stageEnemys[_stageID][curCount] != nullptr) {
			OutWarningMessageEx("グローバルマネージャの敵に上書きして登録しようとしています。");
		}

		g_allStageEnemys.Register(_stageID, _rEnemy);
	}

	//----------------------------------------------------------

	void UnregisterAllGlobalAccessManager()
	{
		// プレイヤーの参照を解除
		g_pPlayer = nullptr;

		// 敵管理変数の解放
		g_allStageEnemys.UnregisterAll();
	}

	//----------------------------------------------------------

	NObject::CPlayer*	GetPlayer()
	{
		Assert(g_pPlayer != nullptr && "プレイヤーが登録されていません。");
		return g_pPlayer;
	}

	uint	GetEnemyCount(uint _stageID)
	{
		Assert(_stageID < MANAGEABLE_STAGE_MAX && "引数が不正です。");
		return g_allStageEnemys.stageEnemyCount[_stageID];
	}

	NGame::CEnemyManager::CStageEnemy*	GetEnemy(
		uint	_stageID,
		uint	_no)
	{
		Assert(_no < GetEnemyCount(_stageID) && "引数が不正です。");
		
		Assert(g_allStageEnemys.stageEnemys[_stageID][_no] != nullptr && "取得しようとした値がnullptrでした。プログラムが間違えている可能性が高いです。");
		return g_allStageEnemys.stageEnemys[_stageID][_no];
	}
}
