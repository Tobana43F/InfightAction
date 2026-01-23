#include	"Scene_Game.h"

#include	"Asset\Factory\Asset_ModelFactory.h"
#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Camera\Camera_GlobalManager.h"
#include	"Collision\Manager\Collision_Manager.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_DebugFont.h"
#include	"DirectX\DirectX_Util.h"
#include	"Input\Input_Common.h"
#include	"Input\Input_Game.h"
#include	"Math\Math_Common.h"
#include	"Math\Math_Type.h"
#include	"System\System_Def.h"

// ÉeÉXÉg
#include	"Collision\Collision_Common.h"

#include	"Polygon\Polygon_Object3D.h"
#include	"Polygon\Polygon_Object2D.h"
#include	"Asset\Factory\Asset_TextureFactory.h"

namespace {

}

namespace NScene {

	CGame::CGame()
	{

	}

	CGame::~CGame()
	{
	

	}

	void CGame::Load()
	{
		CBase::Load();

		m_mainManager.Setup();
	}

	void CGame::Init()
	{
		CBase::Init();

		m_mainManager.Init();
	}	

	void CGame::Update()
	{
		CBase::Update();

		// èâä˙âª
		if (NInput::GetKeyDown(DIK_Z)) {
			Init();
		}

		if (NInput::GetReset()) {
			NScene::ChangeScene("CTitle");
		}


		m_mainManager.Update();
	}
		
	void CGame::Render()
	{
		CBase::Render();

		m_mainManager.Render();
	}

	void CGame::Destroy()
	{
		CBase::Destroy();

		m_mainManager.Destory();
	}

	void CGame::Release()
	{
		CBase::Release();
	}
}