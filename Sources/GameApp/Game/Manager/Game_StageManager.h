#pragma once

#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Game_EnemyManager.h"
#include	"Object\Stage\Object_Stage.h"

namespace NGame {

	//!	@brief	1階層分のステージ情報を保持するクラス
	//!	@note	主な処理内容
	//!			- 敵の生成制御などを行う
	//!			- ステージクリア判定
	//!	@note	説明
	//!			- ステップとは、1階層内での敵の生成などによる進行の分割
	//!			  ステップをクリアすると次のステップに行き、
	//!			　全てのステップをクリアするとステージクリア
	//!	@todo	ステージモデル、あたり判定などもここで管理する。
	class CStageManager {
	public:

		//!	@brief	ステップの最大数
		static const int STEP_MAX = 16;

		//----------------------------------------------------------
		// 基本的な関数
		//----------------------------------------------------------

		//!	@brief		ステージ情報を読み込みこのクラスを使用できる状態にする
		//!	@param[in]	_stageNo	ステージの出現順番(IDとして使用する)
		void Setup(const uint _stageNo, NAsset::CJsonFactory::Key _dataKey);

		//!	@brief	初期化
		void Init();

		//!	@brief	サウンド終了用
		void Destory();

		//!	@brief	更新処理
		void Update();

		//!	@brief	描画処理
		void Render();

		//----------------------------------------------------------
		// ゲッター
		//----------------------------------------------------------

		uint	GetStepCount() const { return m_stepCount; }
		bool	GetIsClear() const { return m_isClear; }
		uint	GetStageID() const { return m_stageID; }

	private:

		// プログラム非依存データ
		uint				m_stepCount;	// 最大ステップ数

		// 制御用変数
		bool				m_isClear;		// ステージのクリアフラグ
		uint				m_curStep;		// 現在のステップ数
		uint				m_stageCnt;		// ステージをプレイした時間(フレーム)
		uint				m_stageID;		// ステージの識別子

		// オブジェクト群
		NObject::CStage		m_stage;		
		CEnemyManager		m_enemyManager;	// 敵の管理を行うクラス

		//!	@brief	JSONの情報から必要な値を取得する
		void SetupStageInfo(json11::Json& _json);

	public:
		CStageManager();
		~CStageManager();
	};

}