#pragma once

//!	@brief		プレイヤーへのアクセス、敵へのアクセス、ステージ情報へのアクセスなど、
//!				様々な場所で参照される情報にアクセスするための仲介を行う関数群

#include	"Game_MainManager.h"

namespace NGame {

	//!	@brief	初期化
	void SetupGlobalAccessManager();

	//!	@brief	完全終了処理
	void TearDownGlobalAccessManager();

	//----------------------------------------------------------
	// アクセス対象の登録関数群
	//----------------------------------------------------------

	//!	@brief	プレイヤーを登録
	void RegisterPlayer(NObject::CPlayer& _rPlayer);

	//!	@brief	ステージ上の敵を登録
	//!	@note	登録時にステージと対応付けを行う
	void RegisterEnemy(const uint _stageID, NGame::CEnemyManager::CStageEnemy& _rEnemy);

	//----------------------------------------------------------
	// 登録解除関数
	//----------------------------------------------------------
	
	//!	@brief	登録した全ての情報を解除する
	void UnregisterAllGlobalAccessManager();

	//----------------------------------------------------------
	// アクセス関数
	//----------------------------------------------------------

	//!	@brief	プレイヤーを取得
	NObject::CPlayer*	GetPlayer();

	//!	@brief	敵の数を取得
	uint				GetEnemyCount(uint _stageID);

	//!	@brief		敵を取得
	//!	@param[in]	_stageID	取得する敵が管理されているステージID
	//!	@param[in]	_no			敵配列のインデックス(_no > 0, _no < enemyCount)
	NGame::CEnemyManager::CStageEnemy*	GetEnemy(
		uint	_stageID,
		uint	_no);


}