#include	"Asset_ModelFactory.h"

namespace NAsset {

	CModelFactory::CModelFactory()
	{
		m_bindDirectory.assign("Asset/Model/");
		m_dumpFileExtension.assign("ModelFactoryDump");
	}

	CModelFactory::~CModelFactory()
	{	

	}

	bool CModelFactory::LoadAsset(
		SPAsset&			_dest,
		const Key&			_key)
	{
		bool sts = false;
		std::string path = m_bindDirectory + _key;
		sts = _dest->Load(path.data());
		return sts;
	}
}