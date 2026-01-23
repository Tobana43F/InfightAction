#include	"Scene_Root.h"

#include	"Scene\Scene_Def.h"

namespace NScene {

	CRoot::CRoot() 
	{

	}

	CRoot::~CRoot()
	{

	}

	void CRoot::Load()
	{
		CBase::Load();
	}

	void CRoot::Init()
	{
		CBase::Init();
	}

	void CRoot::Update()
	{
		CBase::Update();

		// ‘JˆÚæ‚ÌƒV[ƒ“‚ğw’è
		NScene::PushScene(ENTRY_SCENE_NAME);
	}

	void CRoot::Render()
	{
		CBase::Render();
	}

	void CRoot::Destroy()
	{
		CBase::Destroy();
	}

	void CRoot::Release()
	{
		CBase::Release();

	}
}