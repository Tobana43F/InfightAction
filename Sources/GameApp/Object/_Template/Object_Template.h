#pragma once

#include	"Object\Object_GameObject.h"

namespace NObject {

	//!	@brief	新規オブジェクトのファイルを作成するときのテンプレート
	//!			ファイルをコピペして名前を変更
	class CTemplate : public CGameObject {
	public:
	
	protected:
		//!	@brief	生成時に1度だけ行う処理を記述する
		virtual void	LoadProc() override;

		//!	@brief	終了時に1度だけ行う処理を記述する
		virtual void	ReleaseProc() override;

		//!	@brief	初期化処理を記述する
		virtual void	InitProc() override;

		//!	@brief	終了処理を記述する
		//!	@note	あまり使うことはない
		virtual void	DestroyProc() override;

		//!	@brief	更新処理を記述する
		virtual void	UpdateProc() override;

		//!	@brief	描画処理を記述する
		virtual void	RenderProc() override;

	public:
		CTemplate();
		virtual ~CTemplate();
	};
}
