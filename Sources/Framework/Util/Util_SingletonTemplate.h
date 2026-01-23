#pragma once

namespace NUtil {

	//!	@brief		シングルトンテンプレートクラス
	template<class T>
	class CSingleton {
	public:
		//!	@brief		インスタンスの取得
		static T& GetInst()
		{
			static T inst;
			return inst;
		}

	private:

	protected:
		CSingleton() { };
		virtual ~CSingleton() { };

	private:
		CSingleton(const CSingleton&) { }
		const CSingleton& operator = (const CSingleton&) { }
	};

}