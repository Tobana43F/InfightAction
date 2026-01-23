#include	"Scene_TestAssetOptimize.h"

#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Asset\Factory\Asset_ModelFactory.h"
#include	"DirectX\DirectX_DebugFont.h"
#include	"Input\Input_Common.h"

namespace NScene {

	CTestAssetOptimize::CTestAssetOptimize()
	{

	}

	CTestAssetOptimize::~CTestAssetOptimize()
	{

	}

	void CTestAssetOptimize::Load()
	{
		CBase::Load();

		NAsset::CJsonFactory::GetInst().DumpRefCount("BeforeOptimizeJSON");
		//NAsset::CModelFactory::GetInst().DumpRefCount("BeforeOptimizeMODEL");

		NAsset::CJsonFactory::GetInst().Optimize();
		//NAsset::CModelFactory::GetInst().Optimize();

		NAsset::CJsonFactory::GetInst().DumpRefCount("AfterOptimizeJSON");
		//NAsset::CModelFactory::GetInst().DumpRefCount("AfterOptimizeMODEL");
	}

	void CTestAssetOptimize::Init()
	{
		CBase::Init();

	}

	void CTestAssetOptimize::Destroy()
	{
		CBase::Destroy();

	}

	void CTestAssetOptimize::Release()
	{
		CBase::Release();

	}

	void CTestAssetOptimize::Update()
	{
		CBase::Update();

		if (NInput::GetKeyDown(DIK_RETURN)) {
			NScene::ChangeScene("CGame");
		}

	}

	void CTestAssetOptimize::Render()
	{
		CBase::Render();
		NDirectX::PrintDebugFont(0, 0, "TestAssetOptimizeScene");
		NDirectX::PrintDebugFont(100, 0, "Push Enter");
	}
}