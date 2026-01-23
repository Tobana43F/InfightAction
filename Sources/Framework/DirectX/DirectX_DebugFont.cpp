#include	"DirectX_DebugFont.h"

#include	<cstdarg>
#include	<string>
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX_Util.h"
#include	"System\System_Error.h"

// 領域確保

namespace {
	constexpr int BUFFER_SIZE = 512;

	LPD3DXFONT	g_pFont;			// フォント管理	
	D3DCOLOR	g_fontColor;		// フォントの色
}

namespace NDirectX {

	void SetupDebugFont(LPDIRECT3DDEVICE9	_lpd3ddevice)
	{
		HRESULT hr = E_FAIL;

		hr = D3DXCreateFont(
			_lpd3ddevice,				// デバイスオブジェクト
			0,							// 文字の高さ
			10,							// 文字の幅
			FW_REGULAR,					// 文字の太さ
			0,							// ミップマップレベルの数
			FALSE,						// イタリック体か否か true:イタリック体
			SHIFTJIS_CHARSET,			// 文字セット
			OUT_DEFAULT_PRECIS,			// 常にＴＲＵＥＴＹＰＥのフォントを使う
			PROOF_QUALITY,				// 出力品質
			FIXED_PITCH | FF_MODERN,	// ピッチとファミリーインデックス
			"ＭＳＰゴシック",				// フォント名
			&g_pFont);					// 生成されたフォント

		if (FAILED(hr)) {
			ErrorEx("デバッグフォントの作成に失敗しました。");
		}

		g_fontColor = ToD3DColor(1, 0, 0, 1);
	}

	void TearDownDebugFont()
	{
		ReleaseSafe(g_pFont);
	}

	void PrintDebugFont(float _x, float _y, const char* _pStr)
	{
	#ifdef _GAME_DEBUG
		int x = (int)floorf(_x);
		int y = (int)floorf(_y);

		RECT	rect = { x, y, 0, 0 };
		// 文字列のサイズを計算
		g_pFont->DrawText(nullptr, _pStr, -1, &rect, DT_CALCRECT, ToD3DColor(0,0,0,0));

		// 文字列描画（赤色）
		g_pFont->DrawText(nullptr, _pStr, -1, &rect, DT_LEFT | DT_BOTTOM, g_fontColor);
	#endif
	}

	void PrintDebugFontF(float _x, float _y, const char* _fmt, ...)
	{
	#ifdef _GAME_DEBUG
		va_list	arg;
		char	buf[BUFFER_SIZE];

		va_start(arg, _fmt);
		vsprintf_s<sizeof(buf)>(buf, _fmt, arg);
		va_end(arg);

		PrintDebugFont(_x, _y, buf);
	#endif
	}

	void SetColorDebugFont(float _r, float _g, float _b, float _a)
	{
		g_fontColor = ToD3DColor(_r, _g, _b, _a);
	}
}
