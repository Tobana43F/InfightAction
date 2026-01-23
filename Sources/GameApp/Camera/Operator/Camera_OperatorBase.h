#pragma once

#include	"Camera\Camera_Def.h"

namespace NCamera {

	//!	@brief		カメラの操作を行うクラスの基底クラス
	//!	@details	継承して、個別の操作を行うクラスを作成し
	//!				使用するときは、動的に操作を切り替えられるようにする
	class COperatorBase {
	public:
		//!	@brief	カメラ操作の更新を行う
		//!	@note	マネージャによって呼び出されるので、
		//!			他の場所で呼び出さないこと。
		void Update();

	protected:

		//!	@brief	実際に操作を行う関数
		virtual void Operate() PURE_FUNC;

		//!	@brief	クラス内のカメラ情報を取得
		const float3&	GetCameraPosData() const { return m_cameraViewData.pos; }
		const float3&	GetCameraFocData() const { return m_cameraViewData.foc; }
		const float3&	GetCameraUpData() const { return m_cameraViewData.up; }

		//!	@brief	クラス内のカメラ情報に設定
		void			SetCameraPosData(const float3& _rPos) { m_cameraViewData.pos = _rPos; }
		void			SetCameraFocData(const float3& _rFoc) { m_cameraViewData.foc = _rFoc; }
		void			SetCameraUpData(const float3& _rUp) { m_cameraViewData.up = _rUp; }
		
		//!	@brief	操作対象とするカメラの添え字を設定する
		void			SetCameraIndex(EManagerIndex _index) { m_cameraIndex = _index; }

	private:
		SViewData		m_cameraViewData;	// クラス内で操作する際に使用するカメラ情報
		EManagerIndex	m_cameraIndex;		// 操作対象にするカメラの添え字


		//!	@brief	クラス内のカメラ情報をグローバル管理領域に登録
		void Apply();

	public:
		COperatorBase();
		virtual ~COperatorBase();

	private:
		// 不要な操作をできないようにする
		COperatorBase(const COperatorBase&) { }
		COperatorBase& operator= (const COperatorBase&) { }
	};

}