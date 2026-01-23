#pragma once

#include	"Scene\Scene_Base.h"

#include	"Game\Manager\Game_MainManager.h"

namespace NScene {

	//!	@brief	ゲームを実行するシーン
	class CGame : public CBase {
	public:

		virtual void Load() override;
		virtual void Init() override;
		virtual void Update() override;
		virtual void Render() override;
		virtual void Destroy() override;
		virtual void Release() override;

	private:

		NGame::CMainManager	m_mainManager;

	public:
		CGame();
		virtual ~CGame();
	};
}