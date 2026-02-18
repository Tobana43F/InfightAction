#include	"Debug_Def.h"

#ifdef _DEBUG
#define	DEBUG_MODE	true;
#else 
#define	DEBUG_MODE	true;
#endif

namespace NDebug {

	bool	IS_DEBUG_MODE = DEBUG_MODE;
	bool	IS_PAUSE = false;
	bool	IS_PAUSE_STEP = false;
	bool	IS_SLOW = false;
	float	SLOW_RATIO = 0.5f;
	bool	IS_FBX_LOAD_INFO = DEBUG_MODE;
}