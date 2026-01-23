#pragma once

#include	"Util_StateManager.h"

#include	"Debug\Debug_Assert.h"

namespace NUtil {

	template<class TOwner, typename TEnumState>
	CStateManager<TOwner, TEnumState>::CStateManager(
		TOwner*				_pOwner) :
		CStateManager(_pOwner, (TEnumState)0)
	{

	}
	
	template<class TOwner, typename TEnumState>
	CStateManager<TOwner, TEnumState>::CStateManager(
		TOwner*				_pOwner, 
		const TEnumState	_entry) :
		m_pBindInst(_pOwner),
		m_stateParam(_entry)
	{

	}



	template<class TOwner, typename TEnumState>
	void CStateManager<TOwner, TEnumState>::Register(
		const TEnumState	_index, 
		const FPExecMember	_fpFunc)
	{
		AssertZero(m_stateFuncArray.count(_index));
		m_stateFuncArray.emplace(_index, _fpFunc);
	}

	template<class TOwner, typename TEnumState>
	void CStateManager<TOwner, TEnumState>::Update()
	{
		TEnumState	curState = m_stateParam.GetCurState();
		Assert(m_stateFuncArray.count(curState) == 1 && "ステートに関数が設定されていません。");
		
		// メンバ関数の場合はバインドされたインスタンスから実行
		(m_pBindInst->*m_stateFuncArray[curState])(m_stateParam);

		// 使用されなかったフラグをリセット
		m_stateParam.UpdateFlagReset();
	}

	template<class TOwner, typename TEnumState>
	void CStateManager<TOwner, TEnumState>::Reset()
	{
		m_stateParam.InitState();
	}

}