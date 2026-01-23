#pragma once

#include	"Scene\Scene_Base.h"

namespace NScene {

	//!	@brief	常に保持されるルートとなるシーン
	class CRoot : public CBase {
	public:

		virtual void Load() override;
		virtual void Init() override;
		virtual void Update() override;
		virtual void Render() override;
		virtual void Destroy() override;
		virtual void Release() override;

	private:

	public:
		CRoot();
		virtual ~CRoot();
	};

}