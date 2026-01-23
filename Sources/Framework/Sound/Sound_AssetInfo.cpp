#define		_CRT_SECURE_NO_WARNINGS
#include	"Sound_AssetInfo.h"
#include	<stdio.h>					// 文字列操作
#include	"System\System_Error.h"

#define		RegisterSound(_pFileName, _isLoop)	NSound::CAssetInfo(_pFileName, _isLoop)

namespace {
	const char	*SOUND_RESOURCE_DIRECTORY = "Asset/Sound/";
	const char	*SOUND_RESOURCE_EXTENSION = ".wav";

	// サウンド情報を持つグローバル変数
	// メモ)
	// 　リソースを追加した際は、ここに追加し、ラベルのほうも追加すること。
	static NSound::CAssetInfo gAssetInfo[NSound::SOUND_LABEL_MAX] = {
		RegisterSound("Common/_BGMTest", true),
		RegisterSound("Common/_SETest", false),

		RegisterSound("SE/Landing", false),
		RegisterSound("SE/Warp", false),
		RegisterSound("SE/WarpEnable", true),

		RegisterSound("SE/AttackSwordLow", false),
		RegisterSound("SE/AttackSwordMiddle", false),
		RegisterSound("SE/AttackSwordHigh", false),

		RegisterSound("BGM/Title", true),
		RegisterSound("BGM/Battle1", true),
		RegisterSound("BGM/Battle2", true),
		RegisterSound("BGM/ResultJingle", false),
	};
}

namespace NSound {

	// ファイルまでのパスを作成するコンストラクタ
	CAssetInfo::CAssetInfo(char *_fileName, bool _isLoop)
	{
		// ファイルパスを作成し、それを保持する
		mFilePath = "";
		mFilePath += SOUND_RESOURCE_DIRECTORY;
		mFilePath += _fileName;
		mFilePath += SOUND_RESOURCE_EXTENSION;
		mIsLoop = _isLoop;
	}

	//!	@brief	指定したサウンドラベルのサウンド情報を取得する関数
	CAssetInfo GetAssetInfo(ELabel _label)
	{
		if (_label >= SOUND_LABEL_MAX || _label < 0) {
			ErrorEx("無効なサウンドラベルが指定されました。");
			return CAssetInfo("", false);
		}

		return gAssetInfo[_label];
	}
}