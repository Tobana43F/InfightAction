#pragma once

#include	"Asset_Factory.h"

#include	"_External\json11\json11.hpp"

namespace NAsset {

	//!	@brief	Jsonファイルを管理するクラス
	class CJsonFactory : public CFactory<json11::Json, CJsonFactory> {
		friend class NUtil::CSingleton<CJsonFactory>;
	protected:

		//!	@brief	Jsonを読み込む
		virtual bool LoadAsset(
			SPAsset&			_dest,
			const Key&			_key) override;

	protected:
		CJsonFactory();
		virtual ~CJsonFactory();
	};
}
