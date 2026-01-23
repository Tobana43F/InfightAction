#include	"Game_Main.h"

#include	<mmsystem.h>				// タイマー関係の関数に必要

#include	"Asset\Asset_FactoryManager.h"
#include	"Camera\Camera_Common.h"
#include	"Collision\Manager\Collision_Manager.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_DebugFont.h"
#include	"Debug\Debug_Message.h"
#include	"Debug\Debug_Def.h"
#include	"Debug\Debug_Debugger.h"
#include	"Game\Manager\Game_GlobalAccessManager.h"
#include	"Input\Input_Common.h"
#include	"Sound\Sound_Common.h"
#include	"Scene\Scene_Manager.h"
#include	"System\System_Common.h"
#include	"System\System_Def.h"
#include	"System\System_Exception.h"
#include	"System\System_Error.h"
#include	"Windows\Windows_Global.h"

#pragma		comment(lib, "winmm.lib")	// mmsystem

namespace {

	float4 BACKGROUND_COLOR(0.5f, 0.5f, 0.5f, 1.0f);	// デフォルトの背景色

	//!	@brief		タイマーのコールバック
	void CALLBACK OnTimer(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR)
	{
		NGame::CMain::GetInst().SetEvent();
	}

	//!	@brief		経過時間をウィンドウタイトルに設定
	void SetWindowTitleExecTime(int _startTime, int _endTime)
	{
		int time = _endTime - _startTime;
		char buf[256];
		sprintf_s<sizeof(buf)>(buf, "[%3d(ms)]", time);
		NSystem::SetWindowTitle(buf);
	}
}

namespace NGame {

	CMain::CMain()
	{

	}

	CMain::~CMain()
	{

	}

	bool CMain::Setup(HINSTANCE _hInstance, HWND _hWnd)
	{
		srand((unsigned int)time(nullptr));	// システム関係の初期化

		// イベントハンドルの作成
		mHEvent = CreateEvent(nullptr, false, false, nullptr);
		if (mHEvent == nullptr) {
			return false;
		}

		// タイマーの設定
		timeBeginPeriod(1);
		mTimerID = timeSetEvent(
			(int)NSystem::MILL_SECOND_PER_FRAME, 
			1,
			OnTimer, 
			1, 
			TIME_PERIODIC);

		// DirectXの初期化
		NDirectX::Setup(
			_hInstance, 
			_hWnd, 
			(UINT)NSystem::WINDOW_WIDTH, 
			(UINT)NSystem::WINDOW_HEIGHT);

		NDirectX::SetupDebugFont(NDirectX::GetD3DDevice());
		NDebug::InitDebugger();

		NInput::Setup(NWindows::gHInst, NWindows::gHWnd);	// 入力
		NSound::InitSound();	// サウンド
		NScene::LoadManager();	// シーン生成

		// ゲームの初期化
		NCamera::Setup();
		NCollision::Setup();
		NGame::SetupGlobalAccessManager();

		// スレッド生成
		mIsThreadEnd = false;
		mThread = std::thread(&CMain::MainLoop, this);

		return true;
	}

	void CMain::TearDown()
	{
		mIsThreadEnd = true;		// スレッド終了フラグを設定
		mThread.join();				// スレッドの同期待ち

		// ゲームの解放処理
		NGame::TearDownGlobalAccessManager();

		NScene::ReleaseManager();	// シーン解放

		NAsset::TearDown();			// Factoryで生成したリソースを全て解放

		NInput::TearDown();			// 入力
		NSound::DestroySound();		// サウンド

		NDirectX::TearDownDebugFont();
		NDirectX::TearDown();		// DirectX終了

		if (mTimerID) {
			timeKillEvent(mTimerID);// タイマー
		}
		timeEndPeriod(1);			// 分解能初期化
	}

	void CMain::SetEvent()
	{
		if (!mIsThreadEnd) {
			::SetEvent(mHEvent);
		}
	}

	void CMain::MainLoop()
	{
		// 初期化処理
		Init();

		// メインループ
		try {
			while (!mIsThreadEnd) {
				DWORD sts = WaitForSingleObject(mHEvent, 1000);	// イベントフラグがセットされるのを待つ（1000msでタイムアウト）
				if (sts == WAIT_FAILED) { break; }
				else if (sts == WAIT_TIMEOUT) { continue; }
				
				Run();

				if (NScene::CheckExit()) {
					mIsThreadEnd = true;
				}
			}
		}
		catch (NSystem::CException _exception) {
			_exception.OutputMessage();
		}
		catch (...) {
			MessageBox(NWindows::gHWnd, "予期せぬ例外が発生しました。", "Exception", MB_OK | MB_ICONSTOP);
			throw;
		}

		// 終了処理
		Destroy();
	}

	void CMain::Init()
	{

	}

	void CMain::Destroy()
	{
		SendMessage(NWindows::gHWnd, WM_CLOSE, 0, 0);
	}

	void CMain::Run()
	{
		HRESULT	hr = E_FAIL;

		NDirectX::ClearBackBuffer(BACKGROUND_COLOR);
		NDirectX::BeginScene();
		int s, e;
		s = timeGetTime();

		NSound::UpdateSound();
		NInput::Update();
		NDebug::UpdateDebugger();

		if (NDebug::IS_PAUSE_STEP) {	
			//!	@todo ゲーム用の入力の更新はここで行うべき
			NScene::UpdateManager();
			NCamera::Update();
		}

		NScene::RenderManager();

		NScene::CheckNextScene();

		e = timeGetTime();
		SetWindowTitleExecTime(s, e);

		NDirectX::EndScene();
		NDirectX::Present();
	}
}