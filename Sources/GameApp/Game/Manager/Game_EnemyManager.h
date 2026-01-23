#pragma once

#include	"Object\Actor\Enemy\Object_Enemy.h"

#include	<array>
#include	<vector>
#include	"Asset\Factory\Asset_JsonFactory.h"

#undef ABSOLUTE	

namespace NGame {

	//!	@brief	ステージ上に存在する全ての敵を管理するクラス
	class CEnemyManager {
	public:
		
		//----------------------------------------------------------
		// 内部クラス
		//----------------------------------------------------------

		//!	@brief	ステージに出現する敵の管理情報を保持するクラス
		//!	@todo	- 管理下にあるステージ情報のポインタを保持してい置く
		//!				- ステージステップを監視して、敵をスポーン
		class CStageEnemy {
		public:

			//!	@brief	初期姿勢の計算モード
			enum EInitDirMode {
				ABSOLUTE,	// 絶対座標上のベクトル方向を向く
				FOCUS		// 注視点として、その方向を見る
			};

			//!	@brief		使用可能状態にする
			//!	@param[in]	_json		敵情報を保持したJSON
			//!	@param[in]	_stepMax	管理下のステージの最大ステップ数(ファイルのエラー処理で使用する)
			void Setup(
				const json11::Json& _json,
				const uint			_stepMax);

			//!	@brief	初期状態にする
			void Init();

			//!	@brief	更新関数
			void Update();

			//!	@brief	描画処理
			void Render();


			//!	@brief	敵をスポーンさせる
			void Spawn() { m_isSpawned = true; }


			//----------------------------------------------------------
			// ゲッター
			//----------------------------------------------------------

			//!	@brief	敵情報を取得する
			NObject::CEnemy*	GetEnemy() { return m_pEnemy; }

			//!	@brief	出現状態を取得
			bool	GetIsSpawned() const { return m_isSpawned; }

			//!	@brief	出現するステップを取得
			uint	GetSpawnStep() const { return m_spawnStageStep; }

		private:

			// 管理する敵オブジェクト
			NObject::CEnemy*	m_pEnemy;			// 敵を管理する親のポインタ
			
			// プログラム非依存データ
			std::string			m_createClassName;	// 生成する際に使用したクラス名
													//!	@note もし敵オブジェクトをクラス名によって特定する場合は、HASHなどで、簡単に求められるようにすること
			uint				m_spawnStageStep;	// 出現するステージのステップ数
			float3				m_initPos;			// 初期座標
			EInitDirMode		m_initDirMode;		// 初期姿勢決定モード
			float3				m_initForwardXZ;	// 初期のXZ平面での方向
			
			// プログラム上で設定するデータ
			uint				m_aliveCnt;			// 出現してからの経過時間(フレーム)

			// 状態
			bool				m_isSpawned;		// ステージ上に出現したフラグ

		public:
			CStageEnemy();
			~CStageEnemy();
		};

		//----------------------------------------------------------
		// 定数値
		//----------------------------------------------------------

		//!	@brief	敵の最大数
		static const int ENEMY_MAX = 128;

		//----------------------------------------------------------
		// 基本的な関数
		//----------------------------------------------------------
		
		//!	@brief	設定する敵情報を保持したJSONファイルをもとにセットアップする
		void Setup(
			NAsset::CJsonFactory::Key	_dataKey, 
			const uint					_stageID,
			const uint					_stepMax);

		//!	@brief	初期化
		void Init();

		//!	@brief	更新
		void Update(const uint _curStep);

		//!	@brief	描画
		void Render();

		//----------------------------------------------------------
		// ゲッター
		//----------------------------------------------------------

		//!	@brief	指定したステップの敵をすべて倒したかどうか
		bool	GetIsStepClear(const uint _curStep) const;

	private:

		uint								m_enemyCount;	// 敵の数
		std::array<CStageEnemy, ENEMY_MAX>	m_stageEnemys;	// 全ての敵のポインタを保持するコンテナ

		uint								m_stageID;		// 管理下にあるステージID
		uint								m_stepCount;
		std::vector<bool>					m_isStepClearInfos;

		uint								m_prevStep;			// 1フレーム前のステップを保持し、変化を監視する

		//!	@brief	JSONの情報から敵を生成する
		void SetupStageEnemys(
			json11::Json&	_json,
			const uint		_stepMax);

		//!	@brief	グローバルマネージャに敵を登録
		void RegisterToGlobalManager();


		//!	@brief	敵の生存状態からステップのクリア状態を監視する
		void CheckIsStepClear();

		//!	@brief	指定したステップに属する敵をすべて出現させる
		void SpawnEnemys(uint _curStep);

	public:
		CEnemyManager();
		~CEnemyManager();
	};

}