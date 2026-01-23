#pragma once

#include	<string>

//!	@brief		使用するサウンドを定義する

namespace NSound {

	// サウンドリソースを指定する定数。
	enum ELabel {
		BGM_TEST,
		SE_TEST,

		SE_LANDING,
		SE_WARP,
		SE_WARP_ENABLE,

		SE_ATTACK_SWORD_LOW,
		SE_ATTACK_SWORD_MIDDLE,
		SE_ATTACK_SWORD_HIGH,

		BGM_TITLE,
		BGM_BATTLE1,
		BGM_BATTLE2,

		JINGLE_RESULT,

		SOUND_LABEL_MAX	// 注意) ラベルではない。
	};

	//!	@brief		サウンドのリソースの情報を保持するクラス
	class CAssetInfo {
	public:
		const char*	GetFilePath() { return mFilePath.data(); }
		bool		GetIsLoop() { return mIsLoop; }

	private:
		std::string		mFilePath;	// サウンドリソースのみファイルパスを保持する。
		bool			mIsLoop;			// サウンドのループフラグ(Wiiでは無視される。)

	public:
		CAssetInfo(char *_fileName, bool _isLoop);
		~CAssetInfo() { };
	};

	CAssetInfo GetAssetInfo(ELabel _label);	// 指定した サウンドラベルのサウンド情報を取得する関数。
}
