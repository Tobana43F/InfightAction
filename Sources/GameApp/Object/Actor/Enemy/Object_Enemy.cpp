#include	"Object_Enemy.h"

#include	"Math\Math_Common.h"

namespace NObject {

	CEnemy::CEnemy()
	{

	}

	CEnemy::~CEnemy()
	{

	}
	
	//----------------------------------------------------------

	void CEnemy::LoadProc()
	{
		Base::LoadProc();
	}

	void CEnemy::ReleaseProc()
	{
		Base::ReleaseProc();
	}

	void CEnemy::InitProc()
	{
		Base::InitProc();
	}

	void CEnemy::DestroyProc()
	{
		Base::DestroyProc();
	}

	void CEnemy::UpdateProc()
	{
		Base::UpdateProc();
	}

	void CEnemy::RenderProc()
	{
		Base::RenderProc();
	}

	void CEnemy::RenderLateProc()
	{
		Base::RenderLateProc();
	}

	void CEnemy::ProcDamage(const NGame::SDamageParam& _rDamage)
	{
		CActor::ProcDamage(_rDamage);
	}
}