#include	"Collision_TypeDataAttack.h"

#include	"System\System_Def.h"
#include	"System\System_Error.h"
#include	"Util\Util_Common.h"

namespace {

	//!	@brief	ノックバックタイプ名からノックバックタイプに変換する
	NCollision::CAttackTypeData::EKnockBackType GetKnockBackTypeFromName(const char* _pName)
	{
		using Type = NCollision::CAttackTypeData::EKnockBackType;

		if (NUtil::CompareString(_pName, "Radiate")) {
			return Type::RADIATE;
		}
		else if (NUtil::CompareString(_pName, "Directional")) {
			return Type::DIRECTIONAL;
		}
		else if (NUtil::CompareString(_pName, "AbsoluteDir")) {
			return Type::ABSOLUTE_DIR;
		}
		else {
			std::string	err = "不明なノックバックタイプが指定されました。\n";
			err += "指定された名前 : ";
			err += _pName;
			throw(err.data());
		}
	}

	//!	@brief	強さ名から強さを取得する
	NCollision::CAttackTypeData::EStrengthType GetStrengthFromName(const char* _pName)
	{
		using Type = NCollision::CAttackTypeData::EStrengthType;

		if (NUtil::CompareString(_pName, "Low")) {
			return Type::LOW;
		}
		else if (NUtil::CompareString(_pName, "Middle")) {
			return Type::MIDDLE;
		}
		else if (NUtil::CompareString(_pName, "High")) {
			return Type::HIGH;
		}
		else if (NUtil::CompareString(_pName, "Blow")) {
			return Type::BLOW;
		}
		else {
			std::string	err = "不明なストレングスタイプ指定されました。\n";
			err += "指定された名前 : ";
			err += _pName;
			throw(err.data());
		}
	}
}

namespace NCollision {

	CAttackTypeData::CAttackTypeData() :
		m_baseDamage(0),
		m_curSerialID(0),		// 無効な値で初期化
		m_serialFrameCount(0),
		m_cnt(0)
	{

	}

	CAttackTypeData::~CAttackTypeData()
	{

	}

	void CAttackTypeData::Load(json11::Json _groupData, uint _enableFrame)
	{
		// シリアルIDが変更されるフレームの数を取得する
		m_serialFrameCount = _groupData["SerialFrames"].array_items().size();
		m_serialFrames.resize(m_serialFrameCount);
		// 変更されるフレームをすべて取得する
		for (uint i = 0; i < m_serialFrameCount; ++i) {
			int intVal = _groupData["SerialFrames"].array_items()[i].int_value();
			m_serialFrames[i] = StaticCast<uint>(intVal);
		}

		// 配列が空の場合は無視
		if (m_serialFrameCount > 0) {
			// 値が小さい順にソートされていることをチェック
			uint prevVal = m_serialFrames[0];
			for (uint i = 1; i < m_serialFrameCount; ++i) {
				// 次の値が前の値と等しい、または小さい場合はソートされていないのでエラーとする
				if (prevVal >= m_serialFrames[i]) {
					throw("シリアルフレームが昇順にソートされていません。");
				}
				prevVal = m_serialFrames[i];
			}

			// 最大値がEnableFrameを超えていないことをチェック
			if (m_serialFrames[m_serialFrameCount - 1] >= _enableFrame) {
				throw("シリアルフレームが有効時間より大きい値が設定されています。");
			}
		}

		// 攻撃優先度を取得
		m_priority = _groupData["Priority"].int_value();

		// ダメージ量を取得
		m_baseDamage = _groupData["BaseDamage"].int_value();

		// ノックバック属性
		m_knockBackType = GetKnockBackTypeFromName(_groupData["KnockBackTypeName"].string_value().data());

		// 強さを設定
		m_strengthType = GetStrengthFromName(_groupData["StrengthTypeName"].string_value().data());

		// ノックバック方向
		for (uint i = 0; i < _groupData["KnockBackDir"].array_items().size(); ++i) {
			m_knockBackDir.array[i] = 
				StaticCast<float>(_groupData["KnockBackDir"].array_items()[i].number_value());
		}
		m_knockBackDir.Normalize();	// 正規化
		if (m_knockBackType == EKnockBackType::RADIATE) {
			if (m_knockBackDir.GetLengthSquare() > 0) {
				OutWarningMessageEx("ノックバックタイプにRadiateを指定したときは、KnockBackDirは無視されます。");
			}
		}

		// ノックバックの強さ
		m_knockBackPow = StaticCast<float>(_groupData["KnockBackPow"].number_value() * NSystem::DELTA_TIME);

	}

	void CAttackTypeData::Init()
	{
		m_curSerialID = 0;
		m_cnt = 0;
	}

	void CAttackTypeData::Update()
	{
		// シリアルフレームが存在しない場合は何もしない
		if (m_serialFrameCount <= 0) {
			return;
		}
		// IDが最大数を超えたら無視する
		if (m_curSerialID >= m_serialFrameCount) {
			return;
		}

		// m_curSerialIDが示すシリアルフレームを
		if (m_cnt >= m_serialFrames[m_curSerialID]) {
			m_curSerialID++;
		}

		// カウンタ更新
		m_cnt++;
	}
}