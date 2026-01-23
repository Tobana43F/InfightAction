#include	<io.h>						// コンソール画面作成に必要
#include	<Fcntl.h>					// 〃
#include	"Windows_Include.h"
#include	"System/System_Def.h"
#include	"Game/Game_Main.h"

#define		WINDOW_STYLE		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU

// global
namespace NWindows {
	HWND		gHWnd;
	HINSTANCE	gHInst;						// 現在のインターフェイス
}

// prototype
ATOM                MyRegisterClass(HINSTANCE _hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

using namespace NWindows;

//!	@brief	エントリポイント
int APIENTRY wWinMain(
	HINSTANCE _hInstance,
	HINSTANCE _hPrevInstance,
	LPWSTR    _lpCmdLine,
	int       _nCmdShow)
{
	UNREFERENCED_PARAMETER(_hPrevInstance);
	UNREFERENCED_PARAMETER(_lpCmdLine);

#ifdef _DEBUG
	::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// メモリリーク検出用
	//::_CrtSetBreakAlloc(149);	// 検出したメモリリークの場所を検知したいときに使う。(出力ウィンドウの[]中の数字がそれ)
#endif

	// デバッグ用コンソール画面の初期化
	if (AllocConsole()) {
		FILE *fp;
		freopen_s(&fp, "CON", "w", stdout);    // 標準出力の割り当て
	}


	// 登録
	MyRegisterClass(_hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance(_hInstance, _nCmdShow)) {
		MessageBox(0, "Error", "初期化失敗", MB_OK);
		return FALSE;
	}

	// ゲームの処理
	if (!NGame::CMain::GetInst().Setup(NWindows::gHInst, NWindows::gHWnd)) {
		MessageBox(0, "Error", "初期化失敗", MB_OK);
		return FALSE;
	}

	// メイン メッセージ ループ:
	MSG msg;
	while (true) {
		BOOL ret = GetMessage(&msg, nullptr, 0, 0);
		// メッセージを処理
		if (ret > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// 終了判断
		else if (ret == 0) {
			break;
		}
		// エラー処理
		else {
			MessageBox(gHWnd, "メッセージの取得に失敗しました。", "Fatal Error", MB_OK | MB_ICONSTOP);
			break;
		}
	}

	// 終了処理
	NGame::CMain::GetInst().TearDown();

	FreeConsole();				// コンソールを開放

	return (int)msg.wParam;
}

//!	@brief	ウィンドウクラスを登録
ATOM MyRegisterClass(HINSTANCE _hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = _hInstance;
	wcex.hIcon = LoadIcon(_hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = NSystem::WINDOW_CLASS_NAME;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_WINLOGO);

	return RegisterClassEx(&wcex);
}

//!	@brief		ウィンドウの作成、初期化
BOOL InitInstance(HINSTANCE _hInstance, int _nCmdShow)
{
	using namespace NSystem;

	gHInst = _hInstance; // グローバル変数にインスタンス処理を格納します。

	// ウィンドウの作成
	HWND hWnd = CreateWindow(
		NSystem::WINDOW_CLASS_NAME,
		NSystem::WINDOW_TITLE,
		WINDOW_STYLE,
		0, 0,														// 座標
		(int)WINDOW_WIDTH, (int)WINDOW_HEIGHT,	// サイズ
		HWND_DESKTOP,
		nullptr,
		_hInstance,
		nullptr);

	if (!hWnd) {
		return FALSE;
	}

	gHWnd = hWnd;		// グローバルに格納

	// ウィンドウ位置を再設定
	RECT	window;
	RECT	client;

	GetWindowRect(hWnd, &window);
	GetClientRect(hWnd, &client);

	float width = (window.right - window.left) - (client.right - client.left) + WINDOW_WIDTH;
	float height = (window.bottom - window.top) - (client.bottom - client.top) + WINDOW_HEIGHT;
	SetWindowPos(
		hWnd,
		nullptr,
		(int)(GetSystemMetrics(SM_CXSCREEN) * 0.5f - width * 0.5f),
		(int)(GetSystemMetrics(SM_CYSCREEN) * 0.5f - height * 0.5f),
		(int)width - 1,
		(int)height - 1,
		SWP_NOZORDER);

	// ウィンドウの表示
	ShowWindow(hWnd, _nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//!	@brief		ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND _hWnd, UINT _message, WPARAM _wParam, LPARAM _lParam)
{
	switch (_message) {
		case WM_KEYDOWN:
			switch (_wParam) {
				case VK_ESCAPE:
					DestroyWindow(_hWnd);
					break;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(_hWnd, _message, _wParam, _lParam);
	}
	return 0;
}

