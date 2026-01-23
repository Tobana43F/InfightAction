#pragma once

#include	"Scene\Scene_Base.h"

namespace NScene {

	//!	@brief	テンプレート
	class CTemplate : public CBase {
	public:

		virtual void Load() override;
		virtual void Init() override;
		virtual void Destroy() override;
		virtual void Release() override;

		virtual void Update() override;
		virtual void Render() override;

	private:

	public:
		CTemplate();
		virtual ~CTemplate();
	};

}