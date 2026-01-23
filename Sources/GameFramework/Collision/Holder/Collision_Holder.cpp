#include	"Collision_Holder.h"

#include	"Debug\Debug_Assert.h"
#include	"System\System_Error.h"

namespace NCollision {

	CHolder::CHolder() :
		m_pRefOwner(nullptr),
		m_pRefModel(nullptr),
		m_curStateIndex(0),
		m_seqCount(0),
		m_isEnable(true),
		m_isLoadOK(false),
		m_prevStateIndex(0),
		m_isReset(false)
	{

	}

	CHolder::~CHolder()
	{
		m_pRefOwner	= nullptr;
		m_seqCount	= 0;

		m_seqArray.clear();
	}

	void CHolder::Load(
		NObject::CGameObject*	_pRefOwner,
		const uint				_seqCount,
		const char**			_ppCollisionInfoFilePath,
		const ModelData*		_pRefModelData)
	{
		AssertNotNull(_pRefOwner);
		Assert(!m_isLoadOK);
		AssertNotNull(_ppCollisionInfoFilePath);

		m_isLoadOK = true;
		m_pRefOwner = _pRefOwner;
		m_pRefModel = _pRefModelData;

		// 確保
		m_seqCount = _seqCount;
		m_seqArray.resize(_seqCount);

		// 情報読み込み
		LoadData(
			_pRefOwner,
			_ppCollisionInfoFilePath,
			_pRefModelData);
	}

	void CHolder::Init()
	{
		m_seqArray[m_curStateIndex].Init();
		m_isEnable = true;
	}

	void CHolder::Update(const uint _stateIndex)
	{
		Assert(m_isLoadOK);

		if (!m_isEnable) { return; }
		
		if (_stateIndex >= m_seqCount) {
			ErrorEx("不正なインデックスが指定されました。");
		}

		m_curStateIndex = _stateIndex;

		// 状態の変化を監視
		CheckStateChange(_stateIndex);

		// リセットを判断
		CheckReset();

		// 更新
		m_seqArray[_stateIndex].Update();

		// 前情報を更新
		m_prevStateIndex = _stateIndex;
	}

	void CHolder::RenderDebug()
	{
	#ifdef _GAME_DEBUG
		if (!m_isEnable) {
			return;
		}
		m_seqArray[m_curStateIndex].RenderDebug();
	#endif
	}
	void CHolder::LoadData(
		NObject::CGameObject*	_pRefOwner,
		const char**			_ppCollisionInfoFilePath,
		const ModelData*		_pRefModelData)
	{
		// 全データ(シーケンス)を読み込み
		for (uint i = 0; i < m_seqCount; ++i) {
			const char* pColInfoPath = _ppCollisionInfoFilePath[i];

			// 1状態分のデータを読み込む
			m_seqArray[i].Load(
				_pRefOwner,
				i,
				pColInfoPath, 
				_pRefModelData);
		}
	}

	void CHolder::CheckStateChange(const uint _curStateIndex)
	{
		if (_curStateIndex == m_prevStateIndex) {
			return;
		}

		m_seqArray[m_prevStateIndex].Init();	// デバッグ情報初期化用
		m_seqArray[_curStateIndex].Init();
	}

	void CHolder::CheckReset()
	{
		if (!m_isReset) {
			return;
		}

		// 全ての判定を初期化する
		for (uint i = 0; i < m_seqCount; ++i) {
			m_seqArray[i].Init();
		}

		// フラグを初期化
		m_isReset = false;
	}
}
