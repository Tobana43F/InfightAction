#pragma once

#include	"DirectX\DirectX_Manager.h"
#include	"Shader_Def.h"

namespace NShader {

	//!	@brief	シェーダ―ファイルをコンパイルし管理するクラス
	//!	@note	処理を統一するため、頻繁に行う、DirectXが請け負う描画処理の設定などもここで行う
	class CManager {
	public:

		//----------------------------------------------------------
		// Static関数
		//----------------------------------------------------------

		//!	@brief	カリングモードを設定
		static void SetCullMode(const ECullMode _cullMode);

		//!	@brief	頂点フォーマットを設定
		static void SetVertexFormat(const DWORD _fvf);

		//!	@brief	テクスチャをシェーダーに設定
		static void SetTexture(const uint _index, const LPDIRECT3DTEXTURE9 _pTexture);

		//----------------------------------------------------------
		// メンバ関数
		//----------------------------------------------------------

		//!	@brief	シェーダ―ファイルを読み込む
		bool Load(
			const char* _pVSFilePath,
			const char*	_pVSEntryName,
			const char*	_pPSFilePath,
			const char* _pPSEntryName);

		//!	@brief	シェーダ―をセット
		void SetShaderToPipeline();

		//!	@brief	シェーダ―をリセット(固定パイプラインに戻す)
		void UnsetShaderFromPipeline();

		//!	@brief	頂点シェーダ―のコンスタントテーブルを取得
		LPD3DXCONSTANTTABLE	GetVSCT() { return m_pVSCT; }

		//!	@brief	ピクセルシェーダ―のコンスタントテーブルを取得
		LPD3DXCONSTANTTABLE	GetPSCT() { return m_pPSCT; }


	private:
		// シェーダ―オブジェクト
		LPDIRECT3DVERTEXSHADER9 m_pVS;
		LPD3DXCONSTANTTABLE		m_pVSCT;

		LPDIRECT3DPIXELSHADER9	m_pPS;
		LPD3DXCONSTANTTABLE		m_pPSCT;

	public:
		CManager();
		~CManager();
	};
}