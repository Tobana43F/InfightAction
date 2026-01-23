#include	"Game_EnemyManager.h"

#include	<string>
#include	"Game\Game_EnemyCreator.h"
#include	"Game\Manager\Game_GlobalAccessManager.h"
#include	"Math\Math_Common.h"
#include	"Util\Util_Common.h"

namespace {
	const char* INIT_DIR_MODE_ABSOLUTE = "Absolute";
	const char* INIT_DIR_MODE_FOCUS = "Focus";
}

namespace NGame {

	//==========================================================
	// CEnemyManager::CStageEnemy
	//==========================================================
	CEnemyManager::CStageEnemy::CStageEnemy() :
		m_pEnemy(nullptr),
		m_initDirMode(ABSOLUTE),
		m_aliveCnt(0),
		m_isSpawned(false)	//!	@todo falseにし、ほかの場所からtrueに変更させる。
	{

	}

	CEnemyManager::CStageEnemy::~CStageEnemy()
	{
		DeleteSafe(m_pEnemy);
	}

	void CEnemyManager::CStageEnemy::Setup(
		const json11::Json& _json, 
		const uint			_stepMax)
	{
		// 生成する敵クラスの名前を取得
		m_createClassName = _json["EnemyClassName"].string_value();

		// 敵クラス名から敵を生成する
		m_pEnemy = NGame::CreateEnemy(m_createClassName.data());
		Assert(m_pEnemy != nullptr && "敵クラスの生成に失敗しています。");

		// 生成されるステージのステップを取得
		m_spawnStageStep = StaticCast<uint>(_json["StageStep"].int_value());
		if (m_spawnStageStep >= _stepMax) {
			std::string err = "指定したStageStepがステージの最大ステップ数に等しいか超えています。\n";
			err += "最大ステップ数 : ";
			err += std::to_string(_stepMax);
			err += "\n";
			err += "指定したステップ数 : ";
			err += std::to_string(m_spawnStageStep);
			err += "\n\n";
			err += "0 <= StageStep < 最大ステップ 　の範囲で設定してください。";
			throw(err.data());
		}

		// 初期位置
		for (uint i = 0; i < 3; i++) {
			m_initPos.array[i] = StaticCast<float>(_json["InitPos"][i].number_value());
		}

		// 初期姿勢決定モードを設定
		std::string initDirModeName = _json["InitDirModeName"].string_value();
		if (NUtil::CompareString(initDirModeName, INIT_DIR_MODE_ABSOLUTE)) {
			m_initDirMode = EInitDirMode::ABSOLUTE;
		}
		else if (NUtil::CompareString(initDirModeName, INIT_DIR_MODE_FOCUS)) {
			m_initDirMode = EInitDirMode::FOCUS;
		}	
		else {
			std::string err = "不明な初期姿勢決定モードが指定されまいした。\n";
			err += "指定された値 : " + initDirModeName;
			throw(err.data());
		}

		// XZ平面での方向
		m_initForwardXZ.x = StaticCast<float>(_json["InitDirXZ"][0].number_value());
		m_initForwardXZ.y = 0;
		m_initForwardXZ.z = StaticCast<float>(_json["InitDirXZ"][1].number_value());
	}

	void CEnemyManager::CStageEnemy::Init()
	{
		m_aliveCnt = 0;
		m_isSpawned = false;

		// 初期状態としてオブジェクトを無効化しておく
		m_pEnemy->SetIsActive(false);

		// 敵クラスの初期化
		m_pEnemy->Init();

		// オブジェクトに初期位置を設定
		m_pEnemy->GetTransform().SetPos(m_initPos);

		// 初期姿勢を設定
		NMath::SMatrix44	matRot;

		if (m_initDirMode == ABSOLUTE) {
			if (m_initForwardXZ.GetLengthSquare() < FLT_EPSILON) {
				throw("InitDirModeNameがAbsoluteの時にInitDirXZに(0,0)が指定されました。値を設定してください。");
			}

			// 指定した方向を向くような行列を作成
			NMath::MakeMatrixLookAt(
				matRot,
				m_initPos,
				m_initPos + m_initForwardXZ,
				float3(0, 1, 0));
			m_pEnemy->GetTransform().SetRot(matRot);
		}
		else if (m_initDirMode == FOCUS) {
			// 初期位置と初期注視場所のベクトルを求める
			NMath::SVector3 vec = m_initPos - m_initForwardXZ;
			// 同じ場所を中止すると行列が崩壊するためエラー処理
			if (vec.GetLengthSquare() < FLT_EPSILON) {
				throw("InitDirModeNameがFocusの時に初期位置と同じ場所を注視点に設定しました。同じ値は設定しないでください。");
			}

			float3	mask(1, 0, 1);
			float3	initPosXZ = m_initPos * mask;
			float3	initDirXZ = m_initForwardXZ * mask;

			// 注視点を向くような行列を作成
			NMath::MakeMatrixLookAt(
				matRot,
				initPosXZ,
				initDirXZ,	// 注視点の座標地と指定使用する
				float3(0, 1, 0));
			m_pEnemy->GetTransform().SetRot(matRot);
		}
	}

	void CEnemyManager::CStageEnemy::Update()
	{
		// 敵クラスを更新
		m_pEnemy->Update();

		// 出現後に生きている間更新する
		if (m_isSpawned && !m_pEnemy->GetIsEliminated()) {
			// アクティブに切り替える
			//!	@todo	1度だけ行えばいいので、ほかに切り替える場所がある場合はそちらに処理を移す
			m_pEnemy->SetIsActive(true);

			// 生存時間更新
			m_aliveCnt++;
		}
		else {
			m_pEnemy->SetIsActive(false);
		}

	}

	void CEnemyManager::CStageEnemy::Render()
	{
		// 出現後に生きている時のみ描画する
		if (m_isSpawned && !m_pEnemy->GetIsEliminated()) {
			m_pEnemy->Render();
		}
	}

	//==========================================================
	// CEnemyManager
	//==========================================================

	CEnemyManager::CEnemyManager() : 
		m_enemyCount(0),
		m_stepCount(0),
		m_prevStep(UINT_MAX)	// 無効な値を代入
	{

	}

	CEnemyManager::~CEnemyManager()
	{

	}

	//----------------------------------------------------------

	void CEnemyManager::Setup(
		NAsset::CJsonFactory::Key	_dataKey, 
		const uint					_stageID,
		const uint					_stepMax)
	{
		// 管理下にあるステージIDを取得
		m_stageID = _stageID;

		// ステップ数を取得する
		m_stepCount = _stepMax;
		
		// ステップ数分ステップのクリア状態を管理するフラグを確保する
		m_isStepClearInfos.resize(m_stepCount);
		// フラグ初期化
		for (uint i = 0; i < m_stepCount; ++i) {
			m_isStepClearInfos[i] = false;
		}

		// JSONファイルをオープン
		NAsset::CJsonFactory::SPAsset	pJson = NAsset::CJsonFactory::GetInst().Load(_dataKey);

		try {
			// 敵をセットアップする
			SetupStageEnemys(*pJson, _stepMax);
		}
		catch (const char* _err) {
			std::string	err = "EnemyManagerのセットアップで例外が発生しました。\n\n";
			err += "Message : ";
			err += _err;
			err += "\n\n";
			err += "AssetKey : ";
			err += _dataKey;
			ErrorEx(err.data());
		}

		// 生成した全ての敵をグローバルマネージャに登録
		RegisterToGlobalManager();
	}

	void CEnemyManager::Init()
	{
		m_prevStep = UINT_MAX;

		// 敵初期化
		for (uint i = 0; i < m_enemyCount; ++i) {
			m_stageEnemys[i].Init();
		}

		// フラグ初期化
		for (uint i = 0; i < m_stepCount; ++i) {
			m_isStepClearInfos[i] = false;
		}
	}

	void CEnemyManager::Update(const uint _curStep)
	{
		Assert((_curStep >= m_prevStep) || (m_prevStep == UINT_MAX) && "ステップが減少しました。");

		// ステップ進行時(終了時は除く) 
		if (_curStep != m_prevStep && _curStep < m_stepCount) {
			// 新たに敵をスポーンさせる
			SpawnEnemys(_curStep);
		}

		// 全ての敵の更新処理
		for (uint i = 0; i < m_enemyCount; ++i) {
			m_stageEnemys[i].Update();
		}

		// ステップのクリア状態を監視
		CheckIsStepClear();

		// バッファ更新
		m_prevStep = _curStep;
	}

	void CEnemyManager::Render()
	{
		for (uint i = 0; i < m_enemyCount; ++i) {
			m_stageEnemys[i].Render();
		}
	}


	bool CEnemyManager::GetIsStepClear(const uint _curStep) const
	{
		Assert(_curStep < m_stepCount && "不正なステップ数が指定されました。");
		return m_isStepClearInfos[_curStep];
	}

	//----------------------------------------------------------

	void CEnemyManager::SetupStageEnemys(
		json11::Json&	_json,
		const uint		_stepMax)
	{
		m_enemyCount = _json["EnemyList"].array_items().size();

		for (uint i = 0; i < m_enemyCount; ++i) {
			auto& jsonEnemy = _json["EnemyList"].array_items()[i];
			auto& enemy	= m_stageEnemys[i];

			// 敵をセットアップ
			enemy.Setup(jsonEnemy, _stepMax);
		}
	}

	void CEnemyManager::RegisterToGlobalManager()
	{
		// 全ての敵をマネージャに登録
		for (uint i = 0; i < m_enemyCount; ++i) {
			NGame::RegisterEnemy(m_stageID, m_stageEnemys[i]);
		}
	}

	void CEnemyManager::CheckIsStepClear()
	{
		// ステップ数分ループ
		for (uint iStep = 0; iStep < m_stepCount; ++iStep) {

			// そのステップをクリアしているかどうかを示すフラグ
			bool isStepClear = true;

			// 敵の数分ループ
			for (uint iEnemy = 0; iEnemy < m_enemyCount; ++iEnemy) {
				auto& enemy = m_stageEnemys[iEnemy];

				// 敵の出現するステップと現在探索中のステップと違う場合は無視
				if (enemy.GetSpawnStep() != iStep) {
					continue;
				}

				// 敵が出現していない場合は、のちに出現するため、クリア状態ではないと判断する
				if (!enemy.GetIsSpawned()) { 
					isStepClear = false;
					continue; 
				}

				// 敵が消滅していない場合は生存中と判断
				if (!enemy.GetEnemy()->GetIsEliminated()) {
					isStepClear = false;
				}

				// ステップがクリア状態ではないと分かった段階で終了する
				if (!isStepClear) {
					break;
				}
			}

			// ステップがクリア状態だった場合フラグを更新
			if (isStepClear) {
				m_isStepClearInfos[iStep] = true;
			}
		}
	}

	void CEnemyManager::SpawnEnemys(uint _curStep)
	{
		for (uint i = 0; i < m_enemyCount; ++i) {
			auto& enemy = m_stageEnemys[i];

			// 敵が指定したステップに属する場合はスポーンさせる
			if (enemy.GetSpawnStep() == _curStep) {
				enemy.Spawn();
			}
		}
	}
}
