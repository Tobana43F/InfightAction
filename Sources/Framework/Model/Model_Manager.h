#pragma once

#include	"Fbx\Model_FbxLoader.h"

namespace NModel {

	//!	@brief		アセットとして保持するモデルクラス
	//!	@details	ファイルから読み取るデータのみ管理する。
	class CManager {
	public:
		using Model = CFbxLoader;

		//!	@brief	読み込み
		bool Load(const char* _pFilePath);

		//!	@brief	モデル情報を取得する
		Model&			GetModel() { return m_model; }
		const Model&	GetModel() const { return m_model; }

	private:

		Model	m_model;

	public:
		CManager(){ }
		~CManager() { }
	};

}