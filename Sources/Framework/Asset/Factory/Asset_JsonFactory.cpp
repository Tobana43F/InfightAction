#include	"Asset_JsonFactory.h"

namespace {

}

namespace NAsset {

	CJsonFactory::CJsonFactory()
	{
		// 汎用的に使用するため、浅い階層を設定しておく
		m_bindDirectory.assign("Asset/");
		m_dumpFileExtension.assign("JsonFactoryDump");
	}

	CJsonFactory::~CJsonFactory()
	{	


	}

	bool CJsonFactory::LoadAsset(
		SPAsset&		_dest,
		const Key&		_key)
	{
		// ディレクトリを変更
		const int BUF_SIZE = 256;
		char prevDirectory[BUF_SIZE];
		GetCurrentDirectory(BUF_SIZE, prevDirectory);
		SetCurrentDirectory(m_bindDirectory.data());

		// ファイルをオープンしファイルの内容をすべて展開
		std::fstream file(_key.data());
		if (file.fail()) {
			return false;
		}
		std::istreambuf_iterator<char>	begin(file);
		std::istreambuf_iterator<char>	end;
		std::string	fileContent(begin, end);

		// jsonとしてパース
		bool sts = true;
		std::string	err;
		*_dest = json11::Json::parse(fileContent, err);
		if (!err.empty()) {
			sts = false;
			OutWarningMessageEx(err.data());
		}

		// ファイルをクローズ
		file.close();

		// ディレクトリをもとに戻す
		SetCurrentDirectory(prevDirectory);
		return sts;
	}


}