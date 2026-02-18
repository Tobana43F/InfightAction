#pragma	once
#include	<memory>
#include	<string>
#include	<iomanip>
#include	<fstream>
#include	<map>
#include	"Windows\Windows_Include.h"
#include	"Util\Util_SingletonTemplate.h"

#include	"Debug\Debug_Assert.h"
#include	"System\System_Error.h"

namespace NAsset {

	//!	@brief		アセットの生成と解放を管理する基底クラス
	//!	@details	アセットを管理する1つのディレクトリを指定し、
	//!				そのディレクトリからの相対パスをKeyとする。
	//!				※ 相対パスは [BindDirectory/]AssetDirectory/Asset.xxx の形式で記述
	//!	@note		使用するときは必ずアセットへのスマートポインタで参照カウントの整合性をとること
	template<typename TAsset, class TInherit>
	class CFactory : public NUtil::CSingleton<TInherit> {
		friend class NUtil::CSingleton<CFactory<TAsset, TInherit>>;

	public:

		//!	@brief	キーの型
		using Key		= std::string;

		//!	@brief	アセットのシェアードポインタ型
		using SPAsset	= std::shared_ptr<TAsset>;


		//!	@brief		アセットを取得する
		//!	@details	読み込まれていない場合は読み込む
		//!				そのディレクトリからの相対パスをKeyとする。
		//!				※ 相対パスは [BindDirectory/]AssetDirectory/Asset.xxx の形式で記述
		SPAsset	Load(Key _key)
		{
			// ディレクトリにバインドされていることを確認
			Assert(m_bindDirectory.compare("") != 0);

			//	アセットが読み込まれていない場合は読み込む
			if (m_assetArray.count(_key) <= 0) {
				SPAsset pAsset = std::make_shared<TAsset>();

				bool sts = false;
				sts = LoadAsset(pAsset, _key);

				// 読み込み失敗時はエラーメッセージを表示し、例外処理。
				if (!sts) {
					std::string	str("");
					str += "アセットの読み込みに失敗しました。\n";
					str += "BindDirectory : " + m_bindDirectory + "\n";
					str += "Key(FilePath) : " + _key + "\n";
					ErrorEx(str.data());
				}

				m_assetArray.emplace(_key, pAsset);
			}
			return m_assetArray[_key];
		}

		//!	@brief		使用されていないアセットを開放する
		void Optimize()
		{
			// ディレクトリにバインドされていることを確認
			Assert(m_bindDirectory.compare("") != 0);

			//	コンテナ内をすべて走査
			auto itr = m_assetArray.begin();
			while (itr != m_assetArray.end()) {
				std::string	key = itr->first;

				// 存在しない場合は使用されていないので無視
				if (m_assetArray.count(key) <= 0) {
					itr++;
				}
				// 存在している場合は参照カウントが１(自のクラスのみ参照)の場合は削除する
				else if (itr->second.use_count() == 1) {
					Release(m_assetArray.at(key), key);
					itr = m_assetArray.erase(itr);	// 削除し、次のイテレータを取得
				}
				// 使用されている場合は無視する
				else {
					itr++;
				}
			}
		}

		//!	@brief		アセットのロード状況をダンプする
		//!	@param[in]	_fileName	書き込む(作成する)ファイル名
		//!							※拡張子は記述しない
		void DumpRefCount(const char* _fileName)
		{
			// ディレクトリにバインドされていることを確認
			Assert(m_bindDirectory.compare("") != 0);

			const int BUF_SIZE = 256;
			char prevDirectory[BUF_SIZE];
			GetCurrentDirectory(BUF_SIZE, prevDirectory);

			// 書き込む(作成)するファイル名を作成
			std::string	outDirectory = "";
			outDirectory += "Output/Dump/AssetFactory/";
			SetCurrentDirectory(outDirectory.data());

			std::string	fileName = "";
			fileName += _fileName;
			fileName += ".";
			fileName += m_dumpFileExtension;

			std::ofstream	file;
			file.open(fileName.data(), file.out | file.trunc);
			file << "バインドされているディレクトリ : " << m_bindDirectory << std::endl;
			file << "管理中のアセット数 : " << m_assetArray.size() << std::endl;
			file << std::endl;

			//	コンテナ内をすべて走査
			auto	 itr = m_assetArray.begin();
			while (itr != m_assetArray.end()) {
				std::string key = itr->first;
				int			refCount = 0;
				
				// 存在しない場合は使用されていないので無視
				if (m_assetArray.count(key) <= 0) {
					refCount = 0;
				}
				// 存在している場合は参照カウントが１(自のクラスのみ参照)の場合は削除する
				else {
					refCount = itr->second.use_count();
				}

				// 出力
				file << std::setw(30) << std::left << key << "[" << refCount << "]" << std::endl;

				// 次へ
				itr++;
			}

			file.close();

			SetCurrentDirectory(prevDirectory);
		}

	protected:
		// バインドするディレクトリへのパス(exeからの相対パス)
		// コンストラクタで設定する
		std::string									m_bindDirectory;
		std::string									m_dumpFileExtension;

		//!	@brief	アセットのロードを行う
		//!	@return	ロード可否状態(true : 成功, false : 失敗)	
		virtual bool LoadAsset(
			SPAsset&			_dest,
			const Key&			_key) PURE_FUNC;

		//!	@brief	アセットの解放処理
		//!	@note	明示的に解放処理が必要な場合のみ、オーバーライドする。
		virtual void Release(
			SPAsset&	_src, 
			const Key&	_key) { }

	private:
		// アセットを管理する連想配列
		std::map<Key, SPAsset>				m_assetArray;

	protected:
		CFactory() : 
			m_bindDirectory(""),
			m_dumpFileExtension("AssetFactoryDump"){ }
		virtual ~CFactory() { }
	};
}
