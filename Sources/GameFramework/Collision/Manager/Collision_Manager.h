#pragma once

//!	@brief	使用するCollisionProcをグローバル領域でアクセスできるようにするマネージャ
//!	@note	基本的にMainManagerのみが登録することを前提としている

namespace NCollision {

	// ステージ用の当たり判定に使用するクラスの前方宣言
	class CMesh;

	// Procクラス、その子クラスの前方宣言
	class CProc;
	class CProcHitTest;

	//----------------------------------------------------------
	// 汎用
	//----------------------------------------------------------

	//!	@brief	それぞれのSetup関数をまとめて呼び出す。
	void Setup();

	//----------------------------------------------------------
	// ステージ用の当たり判定マネージャ
	//----------------------------------------------------------

	//!	@brief	ステージ用当たり判定の登録先を指定するインデックス
	enum class EStageCollisionIndex {
		GROUND_MAIN,		// 主に使用する地面の当たり判定を登録するインデックス
		WALL_MAIN,			// 〃　　　　　壁の当たり判定を 〃
		GROUND_SUB1,		// サブ(通常との当たり判定とは別の判定を使用したいときに使用する)
		WALL_SUB1,			// サブ
		MAX
	};

	//!	@brief		ステージ用当たり判定を登録
	//!	@warning	指定したインデックスに先にほかの当たり判定が登録されている場合はエラーとして処理する
	void RegisterStageCollision(CMesh* _pStageCollision, EStageCollisionIndex _index);

	//!	@brief	登録したステージ用の当たり判定を解除する
	void UnregisterStageCollision(CMesh* _pMeshCollision);

	//!	@brief	指定したインデックスに登録されているステージ用当たり判定を取得する。
	CMesh* GetStageCollision(EStageCollisionIndex _index);

	//----------------------------------------------------------
	// HitTestクラス関係
	//----------------------------------------------------------

	//!	@brief	HitTestProcの登録先を指定するインデックス
	//!	@note	今後、他のもの同士の当たり判定を行いたい場合は、ここに追加する
	enum class EHitTestProcManagerIndex {
		PLAYER_ATK_ENEMY_DEF,		// プレイヤーの攻撃と、敵の被攻撃判定処理
		PLAYER_DEF_ENEMY_ATK,		// プレイヤーの被攻撃判定と、敵の攻撃判定
		MAX							// enumの最大値(インデックスとして使用しないこと)
	};

	//!	@brief	HitTestクラスの登録
	void RegisterHitTestProc(CProcHitTest* _pProcHitTest, EHitTestProcManagerIndex _index);

	//!	@brief	登録したHitTestの解除
	void UnregisterHitTestProc(CProcHitTest* _pProcHitTest);

	//!	@brief	登録したHitTestを取得する
	CProcHitTest*	GetHitTestProc(EHitTestProcManagerIndex _index);

}