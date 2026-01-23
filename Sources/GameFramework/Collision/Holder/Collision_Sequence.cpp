#include	"Collision_Sequence.h"

#include	"Debug\Debug_Assert.h"
#include	"System\System_Error.h"

#include	<string>
#include	<fstream>
#include	"_External\json11\json11.hpp"
#include	"Debug\Debug_Message.h"

namespace {
	// テスト
	json11::Json g_data;
}

namespace NCollision {

	CSequence::CSequence() : 
		m_cnt(0),
		m_groupCount(0)
	{
		
	}

	CSequence::~CSequence()
	{

	}

	void CSequence::Load(
		NObject::CGameObject*	_pRefOwner,
		const uint				_sequenceID,
		const char*				_pCollisionInfoFilePath,
		const ModelData*		_pRefModel)
	{
		// シーケンスIDを取得
		m_sequenceID = _sequenceID;

		try {
			// ファイルオープン
			std::fstream file(_pCollisionInfoFilePath);
			if (file.fail()) {
				throw("ファイル読み込み失敗");
			}
			std::istreambuf_iterator<char>	begin(file);
			std::istreambuf_iterator<char>	end;
			std::string	fileContent(begin, end);

			std::string	err;
			g_data = json11::Json::parse(fileContent, err);
			if (!err.empty()) {
				throw(err.data());
			}

			// グループ数読み込み
			m_groupCount = g_data["Group"].array_items().size();

			// グループ配列確保
			m_colGroups.resize(m_groupCount);
		}
		catch (const char* _pErr) {
			std::string err = "";
			err += "当たり判定の読み込みでエラーが発生しました。\n\n";
			err += _pErr;
			err += "\n";
			err += "FileName : ";
			err += _pCollisionInfoFilePath;
			ErrorEx(err.data());
		}

		// 全てのグループを読み込む
		for (uint i = 0; i < m_groupCount; ++i) {
			const json11::Json&	jsonGroup = g_data["Group"].array_items()[i];

			// 1グループ情報を読み込む
			m_colGroups[i].Load(
				_pRefOwner,
				i,
				_pCollisionInfoFilePath,
				_pRefModel);
		}
	}

	void CSequence::Init()
	{
		m_cnt = 0;
		for (uint i = 0; i < m_groupCount; ++i) {
			m_colGroups[i].Init();
		}
	}

	void CSequence::Update()
	{
		// 全てのグループの更新を行う
		for (uint i = 0; i < m_groupCount; ++i) {
			// グループの有効状態になる時間を検知
			if (m_cnt >= m_colGroups[i].GetEventFrame()) {
				// イベントフレームが経過したら有効にする
				m_colGroups[i].SetIsEnable();
			}

			// グループを更新(EnableFrameが過ぎたら無効にする)
			m_colGroups[i].Update();
		}

		m_cnt++;
	}

	void CSequence::RenderDebug()
	{
		for (uint i = 0; i < m_groupCount; ++i) {
			m_colGroups[i].RenderDebug();
		}
	}

}