#pragma once

#include	"Asset_Factory.h"

#include	"DirectX\DirectX_Manager.h"
#include	"Model\Model_Manager.h"

namespace NAsset {

	//!	@brief	モデルアセットを管理するクラス
	class CModelFactory : public CFactory<NModel::CManager, CModelFactory> {
		friend class NUtil::CSingleton<CModelFactory>;

	protected:
		//!	@brief	モデルを読み込む
		virtual bool LoadAsset(
			SPAsset&			_dest,
			const Key&			_key) override;

	protected:
		CModelFactory();
		virtual ~CModelFactory();
	};
}
