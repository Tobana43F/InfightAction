#include	"Game_GlobalDataManager.h"

namespace {
	//!	@brief	î•ñŒQ
	struct SGlobalData {
		bool	isGameClear;
	};

	NGame::SGlobalData	g_globalData;
}

namespace NGame {

	SGlobalData&	GetGlobalData()
	{
		return g_globalData;
	}

}