#include	"Scene_Base.h"

#include	<typeinfo>
#include	"Asset\Asset_FactoryManager.h"
#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Asset\Factory\Asset_ShaderFactory.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"

namespace NScene {
	CBase::CBase()
	{

	}
		
	CBase::~CBase()
	{

	}

	void CBase::Load()
	{ 
		// 軽いデータは毎回読み込む
		NAsset::CJsonFactory::GetInst().Optimize();
		NAsset::CShaderFactory::GetInst().Optimize();
	}

	void CBase::Init()
	{ 
		// レンダーステートを初期化
		NDirectX::SetRenderStateDefault(NDirectX::GetD3DDevice());
	}

	void CBase::Update()
	{ 
	
	}

	void CBase::Render()
	{ 
	
	}
}
