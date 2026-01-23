#include	"Graphic_Util.h"

namespace NGraphic {

	uint To8Color(float _col)
	{
		if (_col < 0)
			_col = 0;
		if (_col > 1)
			_col = 1;

		return (uint)floorf(255 * _col);
	}

	float	ToNormalColor(int _col)
	{
		if (_col < 0)
			_col = 0;
		if (_col > 255)
			_col = 255;

		return (float)(_col / 255.0f);
	}
}
