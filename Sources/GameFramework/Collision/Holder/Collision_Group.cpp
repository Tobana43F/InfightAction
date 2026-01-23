#include	"Collision_Group.h"

#include	"Debug\Debug_Assert.h"

#include	<fstream>
#include	<string>
#include	"_External\json11\json11.hpp"
#include	"System\System_Error.h"	
#include	"Util\Util_Common.h"

namespace {
	// テスト
	json11::Json g_data;

	// タイプの識別名
	const char*	TYPE_NAME_NONE = "None";
	const char*	TYPE_NAME_ATTACK = "Attack";
	const char*	TYPE_NAME_DAMAGE = "Damage";

	//!	@brief	タイプに設定された名前からETypeに変換する関数
	void SetTypeID(NCollision::CGroup::EType& _rDest, const std::string& _typeName)
	{
		using Type = NCollision::CGroup::EType;

		// None 
		if (NUtil::CompareString(_typeName, TYPE_NAME_NONE)) {
			_rDest = Type::NONE;
		}
		// 攻撃
		else if (NUtil::CompareString(_typeName, TYPE_NAME_ATTACK)) {
			_rDest = Type::ATTACK;
		}
		// ダメージ
		else if (NUtil::CompareString(_typeName, TYPE_NAME_DAMAGE)) {
			_rDest = Type::DAMAGE;
		}
		else {
			std::string	err = "無効なTypeが指定されました。\n";
			err += "入力データ : ";
			err += _typeName;
			throw(err.data());
		}
	}

}

namespace NCollision {

	CGroup::CGroup() :
		m_isEnable(false),
		m_isHit(false),
		m_groupID(UINT_MAX),			// 無効な値で初期化
		m_isPermanent(false),
		m_curEnableFrame(UINT_MAX),		// 無効な値で初期化
		m_eventFrame(0),
		m_enableFrame(0),
		m_colType(EType::NONE),
		m_colGameCount(0)
	{

	}

	CGroup::~CGroup()
	{
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].Release();
		}
	}


	void CGroup::Load(
		NObject::CGameObject*	_pRefOwner,
		const uint				_groupID,
		const char*				_pCollisionInfoFilePath,
		const ModelData*		_pRefModelData)
	{
		// ファイルオープン
		try {
			std::fstream file(_pCollisionInfoFilePath);
			if (file.fail()) {
				throw("ファイル読み込み失敗。");
			}
			std::istreambuf_iterator<char>	begin(file);
			std::istreambuf_iterator<char>	end;
			std::string	fileContent(begin, end);

			std::string	err;
			g_data = json11::Json::parse(fileContent, err);
			if (!err.empty()) {
				throw(err.data());
			}

			// Group配列の自分のIDのものを取得
			json11::Json jsonGroup = g_data["Group"].array_items()[_groupID];

			// 基本的な情報を読み込む
			LoadBaseData(jsonGroup, _groupID);

			// タイプ別の情報を読み込む
			LoadAddData(jsonGroup);
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

		// 全ての当たり判定を読み込み
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].Load(
				_pRefOwner,
				_groupID,
				i,
				_pCollisionInfoFilePath,
				_pRefModelData);
		}
	}

	void CGroup::Init()
	{
		// 有効状態、衝突状態を初期化
		m_isEnable = false;
		m_isHit = false;

		// 現在の有効フレームを初期化
		m_curEnableFrame = m_enableFrame;

		// タイプごとのあたり判定情報を初期化
		m_typeDataAttack.Init();

		// あたり判定情報を初期化
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].Init();
		}
	}

	void CGroup::Update()
	{
		// 当たり判定を初期化
		m_isHit = false;

		// タイプごとのデータの更新
		// 有効時のみ、攻撃判定のシリアルIDなどを更新する
		if (m_isEnable) {
			m_typeDataAttack.Update();
		}

		// 当たり判定を更新
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].Update();
		}

		// グループの有効状態を更新
		if (!m_isPermanent && m_isEnable) {
			if (m_curEnableFrame <= 0) {
				m_isEnable = false;
				m_curEnableFrame = 0;
				SetCollisionGameEnable(false);
			}
			else {
				m_curEnableFrame--;
			}
		}
	}

	void CGroup::RenderDebug()
	{
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].RenderDebug();
		}
	}

	const CAttackTypeData&	CGroup::GetAttackTypeData() const
	{
		Assert(m_colType == ATTACK && "タイプに一致しないデータを取得しました。");
		return m_typeDataAttack;
	}

	const CDamageTypeData&	CGroup::GetDamageTypeData() const
	{
		Assert(m_colType == DAMAGE && "タイプに一致しないデータを取得しました。");
		return m_typeDataDamage;
	}

	void CGroup::SetIsEnable()
	{
		m_isEnable = true;
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].SetIsEnable(true);
		}
	}

	//----------------------------------------------------------

	void CGroup::LoadBaseData(
		json11::Json&	_groupData,
		uint			_groupID)
	{
		// グループIDを設定
		m_groupID = _groupID;

		// (自分のグループに属する)当たり判定の数を数える
		json11::Json jsonGroup = _groupData;

		// タイプを読み込み
		std::string strTypeName = jsonGroup["TypeName"].string_value();
		SetTypeID(m_colType, strTypeName);

		// EventFrameを取得
		int intEventFrame = jsonGroup["EventFrame"].int_value();
		if (intEventFrame < 0) {
			throw("EventFrameに負の値が指定されました。");
		}
		m_eventFrame = StaticCast<uint>(intEventFrame);

		// EnableFrameを取得
		if (jsonGroup["EnableFrame"].is_null()) {		// null が設定されている場合永続的に有効
			m_isPermanent = true;
		}
		else {
			int enableFrame = jsonGroup["EnableFrame"].int_value();
			// 負の値の場合はエラー
			if (enableFrame < 0) {
				throw("EnableFrameに負の値が設定されています。");
			}

			// 数値が入っている場合は、その値を使用する
			m_enableFrame = StaticCast<uint>(enableFrame);
		}

		// 当たり判定の数を取得し確保する
		m_colGameCount = jsonGroup["Collision"].array_items().size();
		m_colGames.resize(m_colGameCount);
	}

	void CGroup::LoadAddData(json11::Json&	_groupData)
	{
		// 判定タイプにより、読み込む情報を変える
		switch (m_colType) {	
			case EType::NONE:
				m_typeDataNone.Load(_groupData);
				break;

			case EType::ATTACK:
				m_typeDataAttack.Load(_groupData, m_enableFrame);
				break;

			case EType::DAMAGE:
				m_typeDataDamage.Load(_groupData);
				break;

			default:
				throw("無効なタイプです。");
				break;
		}
	}

	void CGroup::SetCollisionGameEnable(bool _state)
	{
		for (uint i = 0; i < m_colGameCount; ++i) {
			m_colGames[i].SetIsEnable(_state);
		}
	}
}
