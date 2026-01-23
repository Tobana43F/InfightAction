#pragma once

#include	<vector>
#include	<d3dx9.h>
#include	<d3d9.h>
#include	"Graphic\Graphic_Def.h"

namespace NMesh {

	//!	@brief	実行中に生成するメッシュの基底クラス
	//!	@todo	複数のメッシュができた時に共通の処理を定義する。
	class CBase {
	public:
		virtual void Load()		PURE_FUNC;
		virtual void Release();
		virtual void Render();

	protected:
		std::vector<NGraphic::SDefaultVertex>	m_vertex;

	private:

	public:
		CBase();
		virtual ~CBase();
	};
}
