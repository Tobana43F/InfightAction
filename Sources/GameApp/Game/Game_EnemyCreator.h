#pragma once

#include	"Object\Actor\Enemy\Object_Enemy.h"

namespace NGame {

	//!	@brief		敵のクラス名を指定し、敵クラスを生成する。
	//!	@param[in]	_pEnemyClassName	敵クラス名
	NObject::CEnemy*	CreateEnemy(const char* _pEnemyClassName);
}
