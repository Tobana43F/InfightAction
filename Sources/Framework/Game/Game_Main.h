#pragma once
#include	<thread>
#include	"Windows/Windows_Include.h"
#include	"Util/Util_SingletonTemplate.h"

namespace NGame {

	//!	@brief		ゲーム処理のメインループを実行
	class CMain : public NUtil::CSingleton<CMain> {
		friend class NUtil::CSingleton<CMain>;

	public:

		//!	@brief		初期化
		//!	@details	1度のみ実行する
		//!	@retval		true	成功
		//!	@retval		false	失敗
		bool Setup(HINSTANCE _hInstance, HWND _hWnd);

		//!	@brief		終了処理
		void TearDown();

		//!	@brief		更新イベントのセット
		void SetEvent();

		//!	@brief		終了フラグを取得
		bool GetIsThreadEnd() const { return mIsThreadEnd; }

	private:

		std::thread		mThread;
		bool			mIsThreadEnd;

		HANDLE			mHEvent;
		int				mTimerID;

		//!	@brief		メインループ
		void MainLoop();

		//!	@brief		ゲームの初期化
		void Init();

		//!	@brief		ゲームの終了処理
		void Destroy();

		//!	@brief		ゲームの実行
		void Run();

	private:
		CMain();
		virtual ~CMain();
	};


}
