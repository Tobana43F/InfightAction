#include	"Asset_TextureFactory.h"

namespace NAsset {

	CTextureFactory::CTextureFactory()
	{
		m_bindDirectory.assign("Asset/Image/");
		m_dumpFileExtension.assign("TextureFactoryDump");
	}

	CTextureFactory::~CTextureFactory()
	{	

	}

	bool CTextureFactory::LoadAsset(
		SPAsset&			_dest,
		const Key&			_key)
	{
		bool sts = false;
		std::string path = m_bindDirectory + _key;
		_dest->Load(path.data());	// 例外でエラー処理を行うため、戻り値は使用しない
		return true;
	}
}