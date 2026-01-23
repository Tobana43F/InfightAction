#include	"Game_StageManager.h"

#include	"System\System_Error.h"

namespace NGame {

	CStageManager::CStageManager() : 
		m_stepCount(0),
		m_isClear(false),
		m_curStep(0),
		m_stageCnt(0),
		m_stageID(UINT_MAX)	// 無効な値で初期化
	{

	}

	CStageManager::~CStageManager()
	{

	}

	void CStageManager::Setup(const uint _stageNo, NAsset::CJsonFactory::Key _dataKey)
	{
		Assert(_stageNo != UINT_MAX);
		m_stageID = _stageNo;

		try {
			// アセット取得
			NAsset::CJsonFactory::SPAsset pJson = NAsset::CJsonFactory::GetInst().Load(_dataKey);

			// ステージ情報の読み込み
			SetupStageInfo(*pJson);

			// 敵の読み込み
			m_enemyManager.Setup(
				_dataKey, 
				m_stageID,
				m_stepCount);
		}
		catch (const char* _err) {
			std::string	err = "ステージ情報の読み込みで例外が発生しました。\n\n";
			err += "Message : ";
			err += _err;
			err += "\n\n指定したファイル : ";
			err += _dataKey;
			ErrorEx(err.data());
		}
	}

	void CStageManager::Init()
	{
		m_isClear = false;
		m_curStep = 0;
		m_stageCnt = 0;

		m_stage.SetStageCollision();	// 明示的にステージのあたり判定を設定する
		m_stage.Init();

		m_enemyManager.Init();
	}

	void CStageManager::Destory()
	{ 
		m_stage.Destroy();
	}

	void CStageManager::Update()
	{
		// ステージ更新
		m_stage.Update();

		// 敵更新
		m_enemyManager.Update(m_curStep);

		// クリアしていない間実行
		if (!m_isClear) {
			// 時間更新
			m_stageCnt++;

			// 敵の生存状態を確認しステップを進行
			if (m_enemyManager.GetIsStepClear(m_curStep)) {
				m_curStep++;
				
				// ステップの最大を超えたらステージクリア
				if (m_curStep >= m_stepCount) {
					m_isClear = true;
				}
			}
		}
	}

	void CStageManager::Render()
	{
		m_stage.Render();
		m_enemyManager.Render();
	}

	//----------------------------------------------------------

	void CStageManager::SetupStageInfo(json11::Json& _json)
	{
		m_stepCount = StaticCast<uint>(_json["StepCount"].int_value());
		if (m_stepCount > STEP_MAX) {
			std::string	err = "ステップの数上限値を超えています。\n";
			err += "最大値 : ";
			err += std::to_string(STEP_MAX);
			err += "\n指定した値 : ";
			err += std::to_string(m_stepCount);
			throw(err.data());
		}
	}

}