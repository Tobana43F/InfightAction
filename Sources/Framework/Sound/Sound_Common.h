//=============================================================================
// DirectX専用、音声処理関係
#ifndef _INC_DX_SOUND_H
#define _INC_DX_SOUND_H

#include	"Sound\Sound_AssetInfo.h"
#include	<xaudio2.h>

//==========================================================
// プロトタイプ宣言

namespace NSound {
	HRESULT	InitSound(void);
	void	DestroySound(void);

	// サウンドの再生状態を管理する為に更新を行う
	//!	@note	同フレームでサウンドが2回以上再生されないようにする
	void	UpdateSound(void);

	// 再生
	void	Play(NSound::ELabel _label);

	// 停止
	void	Stop(NSound::ELabel _label);

	// 一時停止
	// 未実装
	void	Pause(NSound::ELabel _label);
}

#endif
