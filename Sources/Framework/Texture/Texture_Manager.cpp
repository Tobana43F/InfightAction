#include	"Texture_Manager.h"

#include	"DirectX\DirectX_Util.h"

namespace NTexture {


	//----------------------------------------------------------
	// CManager::CAnimation
	//----------------------------------------------------------

	CManager::CAnmStack::CAnmStack() : 
		m_stackName(""),
		m_isLoop(false),
		m_frameCount(0)
	{

	}

	CManager::CAnmStack::~CAnmStack()
	{

	}

	void CManager::CAnmStack::Load(const json11::Json& _json)
	{
		// アニメーション情報を取得
		m_stackName		= _json["Name"].string_value();
		m_isLoop		= _json["IsLoop"].bool_value();

		// アニメーション読み込み
		m_frameCount	= _json["Frame"].array_items().size();

		// 最大数を超えていないかをチェック
		if (m_frameCount >= FRAME_MAX) {
			throw("アニメーションのフレームが最大数を超えています。\n最大数を増やすか、アニメーションを短くしてください。");
		}

		// 全てのフレームを読み込み
		for (uint i = 0; i < m_frameCount; ++i) {
			m_frames[i] = _json["Frame"].array_items()[i].int_value();
		}
	}

	uint CManager::CAnmStack::GetFrame(uint _frameNo) const
	{
		Assert(_frameNo < m_frameCount && "指定したアニメーションフレームが不正です。");
		return m_frames[_frameNo];
	}



	//----------------------------------------------------------
	// CManager
	//----------------------------------------------------------

	CManager::CManager() : 
		m_texture(nullptr),
		m_divLength(1,1),
		m_divCount(1,1),
		m_size(0,0),
		m_divSize(0,0),
		m_filePath(""),
		m_anmStackCount(0)
	{

	}

	CManager::~CManager()
	{
		// テクスチャの解放
		ReleaseSafe(m_texture);
	}


	void CManager::Load(const char* _pFilePath)
	{
		try {
			LoadTexture(_pFilePath);
			LoadInfo(_pFilePath);
		}
		catch (const char* _err) {
			std::string	err = "テクスチャの読み込みで例外が発生しました。\n";
			err += "Message : ";
			err += _err;
			err += "\n\n";
			err += "指定されたパス : ";
			err += _pFilePath;
			ErrorEx(err.data());
		}
	}

	const char*	CManager::GetAnmStackName(const uint _anmStackNo) const
	{
		return GetAnmStack(_anmStackNo).GetStackName();
	}

	bool CManager::GetIsLoop(const uint _anmStackNo) const
	{
		return GetAnmStack(_anmStackNo).GetIsLoop();
	}

	uint CManager::GetFrameCount(const uint _anmStackNo) const
	{
		return GetAnmStack(_anmStackNo).GetFrameCount();
	}

	uint CManager::GetFrame(const uint _anmStackNo, const uint _anmFrameNo) const
	{
		return GetAnmStack(_anmStackNo).GetFrame(_anmFrameNo);
	}

	//----------------------------------------------------------

	void CManager::LoadTexture(const char* _pFilePath)
	{
		// テクスチャの読み込み
		HRESULT hr;
		hr = D3DXCreateTextureFromFile(
			NDirectX::GetD3DDevice(),
			_pFilePath,
			&m_texture);

		// 読み込み失敗時
		if (FAILED(hr)) {
			LPVOID string = nullptr;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				hr,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&string,
				0,
				NULL);

			std::string	str = "";
			str += "HRESULT Message : ";
			if (string != nullptr) {
				str += (LPCSTR)string;
				LocalFree(string);
			}
			throw(str.data());
		}
	}

	void CManager::LoadInfo(const char* _pFilePath)
	{
		// ファイルパスを保持
		m_filePath = _pFilePath;

		//----------------------------------------------------------
		// 付与情報を保持するJsonファイルの読み込み
		//----------------------------------------------------------

		// 読み込んだファイルのディレクトリを取得
		std::string	jsonBindDir = "Asset/";
		size_t		dirAssetOffset = m_filePath.find_first_of(jsonBindDir);	// Asset/を無視する
		if (dirAssetOffset == std::string::npos) {
			dirAssetOffset = 0;	// Asset/がなければ最初から
		}
		else {
			dirAssetOffset = std::strlen(jsonBindDir.data());
		}

		const size_t		dirOffset	= m_filePath.find_last_of(".");
		const std::string	dir			= m_filePath.substr(dirAssetOffset, dirOffset - dirAssetOffset);
		const std::string	extension	= ".json";
		const std::string	infoFilePath = dir + extension;	// 付与情報を記述したJsonファイルのパスを作成

		// Jsonファイルを読み込む
		NAsset::CJsonFactory::SPAsset pJson = NAsset::CJsonFactory::GetInst().Load(infoFilePath);

		// アクセスしやすいようにする
		auto& json = *pJson;

		//----------------------------------------------------------
		// Jsonファイルから情報を読み込む
		//----------------------------------------------------------

		// 分割数を読み込む
		for (uint i = 0; i < 2; ++i) {
			m_divCount.array[i] = json["DivCount"].array_items()[i].int_value();
		}

		// 分割数が0いかになることは認めない
		if (m_divCount.x <= 0 || m_divCount.y <= 0) {
			throw("DivCountには1以上の値を設定してください。");
		}

		// 読み込んだ分割数から分割後の一辺の長さを取得
		m_divLength.x = 1.0f / m_divCount.x;
		m_divLength.y = 1.0f / m_divCount.y;

		// テクスチャサイズ
		for (uint i = 0; i < 2; ++i) {
			m_size.array[i] = json["Size"].array_items()[i].int_value();
		}

		// 分割後のサイズ
		m_divSize.x = StaticCast<float>(m_size.x) / StaticCast<float>(m_divCount.x);
		m_divSize.y = StaticCast<float>(m_size.y) / StaticCast<float>(m_divCount.y);

		// アニメーション情報
		m_anmStackCount = json["AnmStack"].array_items().size();

		if (m_anmStackCount > ANM_STACK_MAX) {
			throw("AnmStackが管理可能最大数を超えています。\n管理可能数を増やすか、AnmStackを減らしてください。");
		}

		// 全てのアニメーション情報を取得
		for (uint i = 0; i < m_anmStackCount; ++i) {
			auto& jsonAnmStack = json["AnmStack"].array_items()[i];
			m_anmStacks[i].Load(jsonAnmStack);
		}
	}

	const CManager::CAnmStack& CManager::GetAnmStack(const uint _anmStackNo) const
	{
		Assert(_anmStackNo < m_anmStackCount && "不正なアニメーションスタックが指定されました。");
		return m_anmStacks[_anmStackNo];
	}
}