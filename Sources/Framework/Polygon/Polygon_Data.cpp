#include	"Polygon_Data.h"

namespace NPolygon {

	SData::SData() :
		matWorld(),
		size(0, 0),
		pRefTexture(nullptr),
		anmStackNo(0),
		anmFrame(0),
		isAnmEnd(false),
		isIgnoreAnm(false),
		isReverseX(false),
		isReverseY(false),
		blendState(NGraphic::EBlendState::BLEND_STATE_REPLACE),
		cullModel(NShader::ECullMode::BOTH),
		alignX(EAlignX::CENTER),
		alignY(EAlignY::CENTER)
	{
		colors.fill(float4(1, 1, 1, 1));
	}

}
