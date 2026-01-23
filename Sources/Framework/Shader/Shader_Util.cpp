#include	"Shader_Util.h"

#include	<string>
#include	"System\System_Error.h"

namespace NShader {

	bool CompileShader(
		const char*				_pFilePath,
		const char*				_pEntryName,
		const char*				_pVersion,
		LPD3DXBUFFER*			_pCode,
		LPD3DXCONSTANTTABLE*	_pConstTable)
	{
		LPD3DXBUFFER err = nullptr;

		HRESULT hr;

		// 頂点シェーダをコンパイル
		hr = D3DXCompileShaderFromFile(
			_pFilePath,		// ファイル名
			nullptr,		// プリプロセッサ定義へのポインタ 
			nullptr,		// ID3DXInclude（#include疑似命令）
			_pEntryName,	// 頂点シェーダー関数名 
			_pVersion,		// 頂点シェーダーのバージョン 
			0,				// コンパイルオプション
			_pCode,			// コンパイル済みのシェーダーコード（OUT）
			&err,			// コンパイルエラー情報が格納される（OUT）
			_pConstTable);	// シェーダー内のコンスタントテーブル（OUT）

		// エラー発生
		if (FAILED(hr)) {
			std::string str = "";
			if (err) {
				// コンパイルエラーあり
				str += "シェーダ―コンパイルエラー\n";
				str += (LPSTR)err->GetBufferPointer();
			}
			else {
				// その他のエラー
				str += "シェーダーファイルが読み込めません";
			}
			ErrorEx(str.data());
			return false;
		}
		return true;
	}

	bool CompileVertexShader(
		LPDIRECT3DDEVICE9			_pD3DDev,
		const char*					_pFilePath,
		const char*					_pEntryName,
		const char*					_pVersion,
		LPD3DXCONSTANTTABLE*		_ppConstTable,
		LPDIRECT3DVERTEXSHADER9*	_ppShader)
	{
		bool sts;
		LPD3DXBUFFER code;

		// シェーダーコンパイル
		sts = CompileShader(
			_pFilePath,
			_pEntryName,
			_pVersion,
			&code,
			_ppConstTable);
		if (!sts) {
			return false;
		}

		// 頂点シェーダーオブジェクトを作成する
		HRESULT hr = _pD3DDev->CreateVertexShader((DWORD*)code->GetBufferPointer(), _ppShader);
		if (FAILED(hr)) {
			ErrorEx("頂点シェーダーの作成に失敗しました。");
			return false;
		}

		return true;
	}

	bool CompilePixelShader(
		LPDIRECT3DDEVICE9			_pD3DDev,
		const char*					_pFilePath,
		const char*					_pEntryName,
		const char*					_pVersion,
		LPD3DXCONSTANTTABLE*		_ppConstTable,
		LPDIRECT3DPIXELSHADER9*		_ppShader)
	{
		bool sts;
		LPD3DXBUFFER code;

		// シェーダーコンパイル
		sts = CompileShader(
			_pFilePath,
			_pEntryName,
			_pVersion,
			&code,
			_ppConstTable);

		if (!sts) {
			return false;
		}

		// ピクセルシェーダーオブジェクトを作成する
		HRESULT hr = _pD3DDev->CreatePixelShader((DWORD*)code->GetBufferPointer(), _ppShader);
		if (FAILED(hr)) {
			ErrorEx("ピクセルシェーダ―の作成に失敗しました。");
			return false;
		}

		return true;
	}

}