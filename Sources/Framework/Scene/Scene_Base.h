#pragma once

#include	<string>
#include	"Scene\Scene_Manager.h"

namespace NScene {

	//! @brief	シーンの基底クラス
	class CBase {
	public:

		//!	@brief	リソースなどのロード
		//!	@note	生成時に1度のみ実行される
		virtual void Load();

		//!	@brief	初期化
		//!	@brief	生成後に複数回呼び出される可能性がある
		virtual void Init();

		//!	@brief	終了処理
		//!	@brief	Initに対応する終了関数
		virtual void Destroy() { };

		//!	@brief	解放処理
		//!	@brief	Loadに対応する解放関数
		virtual void Release() { };

		//!	@brief	更新関数
		virtual void Update();

		//!	@brief	描画関数
		virtual void Render();

	protected:
	
	public:

	public:
		CBase();
		virtual ~CBase();
	};

}
