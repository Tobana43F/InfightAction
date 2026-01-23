#include	"Collision_TypeDataDamage.h"

#include	<string>
#include	"Util\Util_Common.h"

namespace {

	//!	@brief	jsonファイル内で使用する文字列での識別子を定義
	const char*	PART_TYPE_NAME_BODY = "Body";
	const char* PART_TYPE_NAME_HEAD = "Head";
	const char* PART_TYPE_NAME_FOOT = "Foot";

	//!	@brief	パーツタイプ名から、パーツタイプIDを取得する
	void SetPartTypeID(
		const std::string& _typeName, 
		NCollision::CDamageTypeData::EPartType& _rDest)
	{
		// 省略
		using PartType = NCollision::CDamageTypeData::EPartType;

		if (NUtil::CompareString(_typeName, PART_TYPE_NAME_BODY)) {
			_rDest = PartType::BODY;
		}
		else if (NUtil::CompareString(_typeName, PART_TYPE_NAME_HEAD)) {
			_rDest = PartType::HEAD;
		}
		else if (NUtil::CompareString(_typeName, PART_TYPE_NAME_FOOT)) {
			_rDest = PartType::FOOT;
		}
		else {
			std::string	err = "無効なPartTypeが指定されました。\n";
			err += "入力データ : ";
			err += _typeName;
			throw(err.data());
		}
	}
}


namespace NCollision {

	CDamageTypeData::CDamageTypeData()
	{

	}

	CDamageTypeData::~CDamageTypeData()
	{

	}

	void CDamageTypeData::Load(json11::Json _groupData)
	{
		// パーツタイプを取得する
		std::string partTypeName = _groupData["PartTypeName"].string_value();
		SetPartTypeID(partTypeName, m_partType);
	}
}