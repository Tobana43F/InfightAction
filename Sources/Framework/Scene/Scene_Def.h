#pragma once

#include	<map>
#include	"Scene_Base.h"

namespace NScene {

	// 最初にRootシーンから遷移するシーン名
	extern const char* ENTRY_SCENE_NAME;


	// 関数ポインタ名を定義
	typedef NScene::CBase*	(*FuncPtrCreateScene)();

	//!	@brief		シーンの定義情報を取得する
	std::map<const char*, FuncPtrCreateScene>&	GetSceneDef();
}