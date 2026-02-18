#include	"Asset_FactoryManager.h"

// 全てのファクトリのヘッダ
#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Asset\Factory\Asset_ModelFactory.h"
#include	"Asset\Factory\Asset_ShaderFactory.h"
#include	"Asset\Factory\Asset_TextureFactory.h"

namespace NAsset {

	void TearDown()
	{
		NAsset::CJsonFactory::GetInst().Optimize();
		NAsset::CModelFactory::GetInst().Optimize();
		NAsset::CShaderFactory::GetInst().Optimize();
		NAsset::CTextureFactory::GetInst().Optimize();
	}
}
