#pragma once

#include	"Scene\Scene_Base.h"

namespace NScene {

	//!	@brief	プログラム中のテスト用のアセットを再読み込みするシーン
	//!	@note	アセットの解放と、元のシーンへの遷移を行う
	class CTestAssetOptimize : public CBase {
	public:

		virtual void Load() override;
		virtual void Init() override;
		virtual void Destroy() override;
		virtual void Release() override;

		virtual void Update() override;
		virtual void Render() override;

	private:

	public:
		CTestAssetOptimize();
		virtual ~CTestAssetOptimize();
	};

}