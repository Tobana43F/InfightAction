#include	"Model_Manager.h"

#include	"DirectX\DirectX_Manager.h"

namespace NModel {

	bool CManager::Load(const char* _pFilePath)
	{
		return m_model.Load(_pFilePath);
	}
}