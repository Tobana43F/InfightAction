#pragma once

#include	"DirectX\DirectX_Manager.h"

namespace NShader {

	//!	@brief	シェーダをコンパイルする
	bool CompileShader(
		const char*				_pFilePath,
		const char*				_pEntryName,
		const char*				_pVersion,
		LPD3DXBUFFER*			_pCode,
		LPD3DXCONSTANTTABLE*	_pConstTable);

	//!	@brief	頂点シェーダ―をコンパイルする
	bool CompileVertexShader(
		LPDIRECT3DDEVICE9			_pD3DDev,		// デバイスオブジェクト
		const char*					_pFilePath,		// シェーダーファイル名
		const char*					_pEntryName,	// エントリー関数名
		const char*					_pVersion,		// バージョン
		LPD3DXCONSTANTTABLE*		_ppConstTable,	// 定数テーブル
		LPDIRECT3DVERTEXSHADER9*	_ppShader);		// 頂点シェーダーオブジェクト

	//!	@brief	ピクセルシェーダ―をコンパイルする
	bool CompilePixelShader(
		LPDIRECT3DDEVICE9			_pD3DDev,		// デバイスオブジェクト
		const char*					_pFilePath,		// シェーダーファイル名
		const char*					_pEntryName,	// エントリー関数名
		const char*					_pVersion,		// バージョン
		LPD3DXCONSTANTTABLE*		_ppConstTable,	// 定数テーブル
		LPDIRECT3DPIXELSHADER9*		_ppShader);		// 頂点シェーダーオブジェクト
}