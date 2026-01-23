#pragma once

//!	シーンを管理する関数群
//!	
//!	シーンを操作する関数はメインでのみ使用する
//!	遷移状態を簡潔にするため、シーンの遷移関数はシーンクラスのみで呼び出す。

namespace NScene {

	//!	@brief	最初に遷移するシーンをロード
	void LoadManager();

	//!	@brief	シーンの解放
	void ReleaseManager();

	//!	@brief	シーンを更新
	void UpdateManager();

	//!	@brief	シーンを描画
	void RenderManager();

	//!	@brief	シーンの遷移先が設定されているかを確認し、あれば遷移する
	void CheckNextScene();

	//!	@brief	ゲームを終了する
	void ExitManager();

	//!	@brief	ゲームの終了状態を取得する
	bool CheckExit();



	//!	@brief		シーンを遷移する
	//!	@details	現在のシーンを削除する
	void ChangeScene(const char* _pSceneName);

	//!	@brief	シーンを遷移する
	//!	@brief	現在のシーンを削除しない
	void PushScene(const char* _pSceneName);

	//!	@brief	シーンを遷移する
	//!	@brief	前のシーンに戻る
	void PopScene();
}