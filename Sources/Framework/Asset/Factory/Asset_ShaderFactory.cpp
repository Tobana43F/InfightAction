#include	"Asset_ShaderFactory.h"
#include	"Debug\Debug_Assert.h"

namespace NAsset {

	CShaderFactory::CShaderFactory()
	{
		m_bindDirectory.assign("Asset/Shader/");
		m_dumpFileExtension.assign("ShaderFactoryDump");
	}

	CShaderFactory::~CShaderFactory()
	{	

	}

	bool CShaderFactory::LoadAsset(
		SPAsset&			_dest,
		const Key&			_key)
	{
		bool sts = false;
		std::string path = m_bindDirectory + _key;

		Assert(m_loadInfoArray.count(_key) > 0);	// 読み込み情報が設定されていることを確認
		const SLoadInfo& loadInfo = m_loadInfoArray[_key];

		sts = _dest->Load(
			path.data(),
			loadInfo.vsEntryName.data(),
			path.data(),
			loadInfo.psEntryName.data());

		return true;
	}

	void CShaderFactory::Release(
		SPAsset&	_src,
		const Key&	_key)
	{
		// 未実装
	}

	//----------------------------------------------------------
	void CShaderFactory::SetLoadInfo(
		const Key&	_key,
		const char*	_pVSEntryName,
		const char* _pPSEntryName)
	{
		// 既に値が設定されている場合は整合性をチェックする
		if (m_loadInfoArray.count(_key) > 0) {
			const SLoadInfo& loadInfo = m_loadInfoArray[_key];
			Assert(loadInfo.vsEntryName.compare(_pVSEntryName) == 0);
			Assert(loadInfo.psEntryName.compare(_pPSEntryName) == 0);
		}
		// 値が設定されていない場合は設定する
		else {
			SLoadInfo	loadInfo;
			loadInfo.vsEntryName.assign(_pVSEntryName);
			loadInfo.psEntryName.assign(_pPSEntryName);
			m_loadInfoArray.emplace(_key, loadInfo);
		}
	}
}
