#include	"Math_Data.h"

#include	<assert.h>
#include	"../Math_Type.h"

namespace NMath {

	//----------------------------------------------------------
	SFloat4Data::SFloat4Data()
	{
		memset(this, 0, sizeof(*this));
	}

	SFloat4Data::SFloat4Data(const SFloat4Data& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SFloat4Data::SFloat4Data(const SVector4& _rhs)
	{
		for (unsigned int i = 0; i < array.size(); ++i) {
			array[i] = _rhs.array[i];
		}
	}

	SFloat4Data::SFloat4Data(
		const float _x,
		const float _y,
		const float _z,
		const float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	void SFloat4Data::Copy(SVector4& _dest)
	{
		for (unsigned int i = 0; i < array.size(); ++i) {
			_dest.array[i] = array[i];
		}
	}

	//----------------------------------------------------------
	SFloat3Data::SFloat3Data()
	{
		memset(this, 0, sizeof(*this));
	}

	SFloat3Data::SFloat3Data(const SFloat3Data& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SFloat3Data::SFloat3Data(const SVector3& _rhs)
	{
		for (unsigned int i = 0; i < array.size(); ++i) {
			array[i] = _rhs.array[i];
		}
	}

	SFloat3Data::SFloat3Data(
		const float _x,
		const float _y,
		const float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	void SFloat3Data::Copy(SVector3& _dest)
	{
		for (unsigned int i = 0; i < array.size(); ++i) {
			_dest.array[i] = array[i];
		}
	}


	//----------------------------------------------------------
	SFloat2Data::SFloat2Data()
	{
		memset(this, 0, sizeof(*this));
	}

	SFloat2Data::SFloat2Data(const SFloat2Data& _rhs)
	{
		memcpy(this, &_rhs, sizeof(*this));
	}

	SFloat2Data::SFloat2Data(const SVector2& _rhs)
	{
		for (unsigned int i = 0; i < array.size(); ++i) {
			array[i] = _rhs.array[i];
		}
	}

	SFloat2Data::SFloat2Data(
		const float _x,
		const float _y)
	{
		x = _x;
		y = _y;
	}

	void SFloat2Data::Copy(SVector2& _dest)
	{
		for (unsigned int i = 0; i < array.size(); ++i) {
			_dest.array[i] = array[i];
		}
	}
}