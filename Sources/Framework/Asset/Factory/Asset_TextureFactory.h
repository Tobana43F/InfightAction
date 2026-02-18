#pragma once

#include	"Asset_Factory.h"

#include	"Texture\Texture_Manager.h"

namespace NAsset {

	//!	@brief	モデルアセットを管理するクラス
	class CTextureFactory : public CFactory<NTexture::CManager, CTextureFactory> {
		friend class NUtil::CSingleton<CTextureFactory>;

	protected:
		//!	@brief	モデルを読み込む
		virtual bool LoadAsset(
			SPAsset&			_dest,
			const Key&			_key) override;

	protected:
		CTextureFactory();
		virtual ~CTextureFactory();
	};
}
