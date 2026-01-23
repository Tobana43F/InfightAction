#include	"Scene_Def.h"

// 使用するシーンのヘッダ
#include	"Scene\Scene_Root.h"
#include	"Scene\Scene_TestAssetOptimize.h"
#include	"Scene\Scene_Game.h"
#include	"Scene\Scene_Title.h"
#include	"Scene\Scene_Result.h"

//!	@brief	シーン登録用マクロ
//!	@note	クラス名のCを抜いた名前を指定する
#define RegisterScene(_name)	 #_name, CreateScene<NScene::##_name>

namespace {

	//!	@brief	シーンを生成する関数
	template<class T>
	NScene::CBase*	CreateScene()
	{
		return New T;
	}

	// VisualStudioの予測変換を利用して入力できるようにするため
	using namespace NScene;

	//!	@brief		使用するシーンをすべて定義したグローバル変数
	//!	@brief		ゲーム中で必要なシーンをすべてここに登録する
	std::map<const char*, NScene::FuncPtrCreateScene> gSceneDef = {
		{ RegisterScene(CRoot) },
		{ RegisterScene(CTitle) },
		{ RegisterScene(CGame) },
		{ RegisterScene(CResult) },
		{ RegisterScene(CTestAssetOptimize) },
	};
}

namespace NScene {

	const char* ENTRY_SCENE_NAME = "CTitle";

	std::map<const char*, FuncPtrCreateScene>&	GetSceneDef()
	{
		return gSceneDef;
	}
}