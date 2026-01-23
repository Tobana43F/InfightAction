#include	"Scene_Manager.h"
#include	<stack>
#include	<typeinfo>

#include	"Scene_Base.h"
#include	"System\System_Error.h"
#include	"Scene\Scene_Def.h"
#include	"Debug\Debug_Assert.h"
#include	"Debug\Debug_Message.h"

namespace {
	//!	@brief	シーンの遷移方法を指定する
	enum ESceneMoveInfo{ 
		CHANGE,
		PUSH,
		POP
	};

	//!	@brief	シーン遷移時に必要な情報を保持する構造体
	struct SNextSceneInfo {
		ESceneMoveInfo	moveInfo;
		const char*		sceneName;		// POPが指定さあれている場合は空になる

		SNextSceneInfo(const char* _pName, ESceneMoveInfo	_moveInfo)
		{
			moveInfo = _moveInfo;
			sceneName = _pName;
		}
	};

	// シーン管理用変数
	std::stack<NScene::CBase*>	pSceneStack;
	std::stack<SNextSceneInfo>	nextSceneInfoStack;	// 基本は1以下
	bool						isGameExit = false;

	//!	@brief	現在のシーン名を取得する
	const char*	GetSceneName()
	{
		return typeid(*pSceneStack.top()).name();
	}

	//!	@brief	シーンを開放する
	void PopScene()
	{
		pSceneStack.top()->Destroy();
		pSceneStack.top()->Release();

		DeleteSafe(pSceneStack.top());
		pSceneStack.pop();

		// RootをPopすることを禁止する
		Assert(pSceneStack.size() > 0 && "PopSceneによってRootが解放されました。");

		NDebug::PrintDebug("[%s] Poped\n", GetSceneName());
	}

	//!	@brief	シーンを確保する
	void PushScene(const char* _sceneName)
	{
		AssertPlus(NScene::GetSceneDef().count(_sceneName));	// 指定したシーンが存在しない。

		// 遷移先のシーンを確保
		NScene::CBase*	pNext = NScene::GetSceneDef().at(_sceneName)();
		AssertNotNull(pNext);
		pSceneStack.push(pNext);

		// シーンの初期化処理
		pNext->Load();
		pNext->Init();

		NDebug::PrintDebug("[%s] Pushed\n", GetSceneName());
	}

}

namespace NScene {

	void LoadManager()
	{
		// 最初の入るシーンを確保
		::PushScene("CRoot");
	}

	void ReleaseManager()
	{
		// すべてのシーンを開放
		while (!pSceneStack.empty()) {
			pSceneStack.top()->Destroy();
			pSceneStack.top()->Release();

			DeleteSafe(pSceneStack.top());
			pSceneStack.pop();
		}
	}

	void UpdateManager()
	{
		pSceneStack.top()->Update();
	}

	void RenderManager()
	{
		pSceneStack.top()->Render();
	}

	void ExitManager()
	{
		isGameExit = true;
	}

	bool CheckExit()
	{
		return isGameExit;
	}

	void CheckNextScene()
	{
		// 設定されているかを確認
		if (nextSceneInfoStack.empty())
			return;

		// 次のシーンが2つ以上設定されている場合はエラーとして処理
		if (nextSceneInfoStack.size() >= 2) {
			ErrorEx("遷移先のシーンが複数存在します。");
		}

		// シーンを遷移
		SNextSceneInfo& info = nextSceneInfoStack.top();

		switch (info.moveInfo) {
			case CHANGE:
				::PopScene();
				::PushScene(info.sceneName);
				break;

			case PUSH:
				::PushScene(info.sceneName);
				break;

			case POP:
				::PopScene();

				// Popシーンのみを指定した場合は、Rootシーンに遷移していないかをチェック
				//!	@note	::PopScene()内で行うとStackが２の時にChangeSceneを読んだ際にエラーと判断されてしまうため、ここで行う。
				Assert(pSceneStack.size() > 1 && "PopSceneによってSceneがRootに遷移しました。Rootへの遷移は禁止しています。");

				break;
		}

		// 参照したシーン情報を破棄
		nextSceneInfoStack.pop();
	}

	void ChangeScene(const char* _pSceneName)
	{
		nextSceneInfoStack.push(SNextSceneInfo(_pSceneName, CHANGE));
	}

	void PushScene(const char* _pSceneName)
	{
		nextSceneInfoStack.push(SNextSceneInfo(_pSceneName, PUSH));
	}

	void PopScene()
	{
		nextSceneInfoStack.push(SNextSceneInfo("", POP));
	}
}