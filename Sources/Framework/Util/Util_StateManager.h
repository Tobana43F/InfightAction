#pragma	once

#include	<unordered_map>
#include	<memory>
#include	<assert.h>

namespace NUtil {

	//!	@class		CStateManager
	//!	@brief		状態遷移を管理するクラス
	//!	@tparam		TOwner		このクラスを使用するクラス
	//!	@tparam		TEnumState	ステート定数に使用するEnum型
	//!	@note		使い方
	//!				- コンストラクタでインスタンスをバインドする。
	//!				- インデックス(ステート定数)と対応する関数の関数ポインタを登録する。
	//!				- このクラスの更新関数でで指定されたインデックスに登録された関数が実行される。
	template<class TOwner, typename TEnumState>
	class CStateManager {
	private	:

		//!	@class		SStateParam
		//!	@brief		状態管理クラスに必要ならパラメータを管理するクラス
		//!	@tparam		TEnumState	ステート定数に使用するEnum型
		//!	@brief		関数ポインタとして登録する関数の引数となる
		template<typename TEnumState>
		class SStateParam {
			template<class TOwner, typename TEnumState>
			friend class CStateManager;

		public:

			//!	@brief	エントリー状態を取得し、リセット
			bool		GetIsEntry()
			{
				bool	tmpState = m_isEntry;
				m_isEntry = false;
				return	tmpState;
			}

			//!	@brief	イグジット状態を取得し、リセット
			bool		GetIsExit()
			{
				bool	tmpState = m_isExit;
				m_isExit = false;
				return	tmpState;
			}

			//!	@brief	インデックスを変更する
			void		ChangeState(const TEnumState _nextState)
			{
				m_curState = _nextState;
				m_isStateChange = true;
				m_isEntry = true;
				m_isExit = true;
			}

		private:

			// 制御変数
			bool		m_isStateChange;

			bool		m_isEntry;
			bool		m_isExit;
			TEnumState	m_curState;
			TEnumState	m_initState;




			//!	@brief	フラグのリセットを判断し更新
			void		UpdateFlagReset()
			{
				if (m_isStateChange) {
					m_isStateChange = false;
				}
				else {
					// 取得されなかったフラグをリセット
					m_isEntry = false;
				}

				// 必ず更新後はFalseになる
				m_isExit = false;
			}

			//!	@brief	現在のインデックスを取得する
			TEnumState	GetCurState()
			{
				return m_curState;
			}

			//!	@brief	状態を初期化
			void		InitState()
			{
				ChangeState(m_initState);
			}



		public:
			//!	@brief	初期状態を0とするコンストラクタ
			SStateParam() :
				SStateParam((TEnumState)0)
			{}

			//!	@brief	初期状態を指定するコンストラクタ
			SStateParam(const TEnumState _entry) :
				m_isStateChange(false),
				m_isEntry(true),
				m_isExit(false),
				m_curState(_entry),
				m_initState(_entry)
			{}

			~SStateParam()
			{}
		};

		//!	@brief	メンバ関数ポインタ型
		using FPExecMember	= void	(TOwner::*)(SStateParam<TEnumState>& _param);	// メンバ関数ポインタ用

	public:
		//!	@brief	メンバ関数の引数になるパラメータ
		using Param		= SStateParam<TEnumState>&;



		//!	@brief	メンバ関数ポインタ登録関数
		void	Register(const TEnumState _index, const FPExecMember _fpFunc);

		//!	@brief	実行関数
		void	Update();

		//!	@brief	状態を初期状態に戻す
		void	Reset();


		
		//!	@brief	外部から割り込み的に状態を変化させる
		void	ChangeState(const TEnumState _nextState) { m_stateParam.ChangeState(_nextState); }

		//!	@brief	現在の状態を取得
		TEnumState	GetState() const { return m_stateParam.m_curState; }

		//!	@brief	現在の状態をuintに変換して取得する
		//!	@note	Key値にENUMを使用していない場合は使用することができない。
		uint		GetStateNumber() const { return StaticCast<uint>(m_stateParam.m_curState); }

	private:

		// 登録先
		TOwner*											m_pBindInst;				// バインドされたインスタンス
		std::unordered_map<TEnumState, FPExecMember>	m_stateFuncArray;			// 関数ポインタ登録用変数

		// 制御用変数
		SStateParam<TEnumState>							m_stateParam;				// 状態管理変数

	public:
		//!	@brief	バインドするインスタンスのみを指定する
		CStateManager(TOwner* _pOwner);

		//!	@brief	バインドするインスタンスと、初期状態を指定する
		CStateManager(TOwner* _pOwner, const TEnumState _entry);

		~CStateManager() {}
	};
}

#include	"Util_StateManager.inl"