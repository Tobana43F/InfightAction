#pragma once

#include	<d3dx9.h>
#include	"Mesh_Base.h"

//!	@todo	Debug‚É’u‚«Š·‚¦‚é

namespace NMesh {

	//!	@brief	ü‚Ì•`‰æF‚ğæ“¾
	float4	GetLineColor();

	//!	@brief	ü‚Ì•`‰æF‚ğİ’è
	void SetLineColor(const float4& _col);

	//!	@brief	ü‚ğ•`‰æ
	void RenderLine(
		const float3& _posStart, 
		const float3& _posEnd);
}
