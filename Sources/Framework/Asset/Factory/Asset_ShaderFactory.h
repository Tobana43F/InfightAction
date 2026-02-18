#pragma once

#include	<map>
#include	"Asset_Factory.h"
#include	"Shader\Shader_Manager.h"

namespace NAsset {

	//!	@brief	シェーダ―アセットを管理するクラス
	class CShaderFactory : public CFactory<NShader::CManager, CShaderFactory> {
		friend class NUtil::CSingleton<CShaderFactory>;

	public:
		//!	@brief		読み込む際の付加情報を設定する
		//!	@note		実際にロードする際に、既にそのキーに対して
		//!				ロード情報が設定されていれば、この関数を呼ばなくてもよい。
		void		SetLoadInfo(
			const Key&	_key,
			const char*	_pVSEntryName,
			const char* _pPSEntryName);

	protected:
		//!	@brief	シェーダ―を読み込む
		virtual bool LoadAsset(
			SPAsset&			_dest,
			const Key&			_key) override;

		//!	@brief	ロード情報を開放する
		//!	@todo	未実装(実装しなくてもよい)
		virtual void Release(
			SPAsset&	_src,
			const Key&	_key) override;

	private:

		//!	@brief	シェーダ―の読み込みに使用する情報
		struct SLoadInfo {
			std::string		vsEntryName;
			std::string		psEntryName;
		};

		std::map<Key, SLoadInfo>	m_loadInfoArray;	// キーごとに読み込み情報を保持する

	protected:
		CShaderFactory();
		virtual ~CShaderFactory();
	};
}
