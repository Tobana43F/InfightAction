#pragma once

#include	"Camera_OperatorBase.h"

#include	"Object\Object_GameObject.h"

namespace NCamera {
	
	//!	@brief		デバッグ用のフリーカメラ
	//!	details		自由に移動、自由に向きを変えることが出来る
	class COperatorDebug : public COperatorBase {
	public: 

	protected:
		virtual void Operate() override;

	private:

		float	MOVE_SPEED_HIGH_MAX;
		float	MOVE_SPEED_LOW_MAX;
		float	ROTATE_SPEED_HIGH_MAX;
		float	ROTATE_SPEED_LOW_MAX;
		float3	FOCUS_OFFSET;
		
		bool	m_isSlow;

		NObject::CGameObject	m_posObj;

		//!	@brief	カメラ操作
		void OperateCamera();

	public:
		COperatorDebug();
		virtual ~COperatorDebug();
	};


}
