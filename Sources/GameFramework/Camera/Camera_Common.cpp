#include	"Camera_Common.h"

#include	"Camera_GlobalManager.h"
#include	"Camera_OperatorManager.h"

namespace NCamera {

	void Setup()
	{
		SetupGlobalManager();
		SetupOperatorManager();
	}

	void Update()
	{
		UpdateOperatorManager();
	}

}