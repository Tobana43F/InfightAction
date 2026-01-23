#pragma once

//!	@brief		システムにかかわる定数を定義する

namespace NSystem {

	static const float	WINDOW_SIZE_RATIO = 0.8;						//!< @brief	比率を保持して、ウィンドウサイズを決めるための倍率(外部からは参照しない)
	static const float	WINDOW_WIDTH	= 1600 * WINDOW_SIZE_RATIO;		//!< @brief	ウィンドウの幅
	static const float	WINDOW_HEIGHT	= 900 * WINDOW_SIZE_RATIO;		//!< @brief	ウィンドウの高さ

	static const char*	WINDOW_TITLE		= "DirectX Project";	//!< @brief	ウィンドウのタイトル
	static const char*	WINDOW_CLASS_NAME	= "Created by Naruse";	//!< @brief	ウィンドウの識別子

	static const float	FRAME_PER_SECOND = 60.0f;							//!< @brief	1秒間のフレーム数
	static const float	SECOND_PER_FRAME = 1.0f / FRAME_PER_SECOND;			//!< @brief	1フレーム間の秒数
	static const float	MILL_SECOND_PER_FRAME = SECOND_PER_FRAME * 1000;	//!< @brief	1フレーム間のミリ秒数
	static const float	DELTA_TIME = SECOND_PER_FRAME;						//!< @brief	1フレームの経過時間(秒)　使用するときにわかりやすいような名前にしているだけ
}