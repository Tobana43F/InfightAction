#include	"Object\_Template\Object_Template.h"
#include	"Debug\Debug_Util.h"

namespace NObject {

	CTemplate::CTemplate() : CGameObject()
	{

	}

	CTemplate::~CTemplate()
	{
	
	}

	void CTemplate::LoadProc()
	{
		CGameObject::LoadProc();
	}

	void CTemplate::ReleaseProc()
	{
		CGameObject::ReleaseProc();
	}

	void CTemplate::InitProc()
	{
		CGameObject::InitProc();
	}

	void CTemplate::DestroyProc()
	{
		CGameObject::DestroyProc();
	}

	void CTemplate::UpdateProc()
	{
		CGameObject::UpdateProc();
	}

	void CTemplate::RenderProc()
	{
		CGameObject::RenderProc();
	}
}
