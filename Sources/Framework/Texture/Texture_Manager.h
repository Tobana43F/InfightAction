#pragma once

#include	<array>
#include	<string>
#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"DirectX\DirectX_Manager.h"

namespace NTexture {

	//!	@brief		テクスチャ情報を管理するクラス
	//!	@details	テクスチャアニメーションについて
	//!				- アニメーションは1つのテクスチャを分割し、それを連続して切り替えることでアニメーションを行る
	//!				- divCount(4,2)の場合
	//!					- 左上から		0, 1, 2, 3,
	//!					- 折り返して		4, 5, 6, 7
	//!					- となる。
	//!				  この番号を基に、テクスチャに張り付けるUV座標を計算し、アニメーションを行う
	//!			
	//!	@note		用語
	//!				- AnmStack		1つのアニメーションのこと
	//!				- Frame			アニメーション内の1つの連番を特定するあたい
	//!	
	//!	@note		ポリゴンクラスに張り付けるためのテクスチャとして作成した。
	//!	@todo		全てのテクスチャを管理できるように、
	//!				テクスチャの生成などもできるようにする。
	class CManager {
	public:
		//!	@brief	管理するテクスチャのエイリアス
		using TextureObj = LPDIRECT3DTEXTURE9;

		//!	@brief	テクスチャが保持するアニメーションの最大数
		static const unsigned int ANM_STACK_MAX = 32;

		//!	@brief	読み込み関数
		void Load(const char* _pFilePath);

		//----------------------------------------------------------
		// テクスチャ情報のゲッター
		//----------------------------------------------------------

		//!	@brief	テクスチャを取得する
		const TextureObj&	GetTexture() const { return m_texture; }

		//!	@brief	分割後の1辺の長さを取得する
		//!	@note	分割した1つの画像あたりのUV値のサイズになる(０～１)
		const float2&		GetDivLength() const { return m_divLength; }

		//!	@brief	分割数を取得する
		//!	@note	1つのテクスチャを分割して複数枚のテクスチャを含む場合は1以上の値になる
		const int2&			GetDivCount() const { return m_divCount; }

		//!	@brief	テクスチャ全体の分割数を取得する
		uint				GetDivCountAll() const { return m_divCount.x * m_divCount.y; }

		//!	@brief	テクスチャのサイズを取得する
		const int2&			GetSizeInt() const { return m_size; }

		//!	@brief	テクスチャサイズをfloatで取得する
		//!	@note	float型のほうが利用しやすいため、この関数を用意している
		//!	@note	float型である必要がない場合は、GetSizeInt()を利用すること。
		float2				GetSize() const { return float2(StaticCast<float>(m_size.x), StaticCast<float>(m_size.y)); }

		//!	@brief	分割後の1つ当たりのテクスチャのサイズを取得する
		const float2&		GetDivSize() const { return m_divSize; }

		//!	@brief	ファイルパスを取得する
		const char*			GetFilePath() const { return &m_filePath[0]; }

		//----------------------------------------------------------
		// アニメーション情報のゲッター
		//----------------------------------------------------------

		//!	@brief	アニメーション数を取得
		uint				GetAnmStackCount() const { return m_anmStackCount; }

		//!	@brief	アニメーション名を取得
		const char*			GetAnmStackName(const uint _anmStackNo) const;

		//!	@brief	アイメーションのループフラグを取得
		bool				GetIsLoop(const uint _anmStackNo) const;

		//!	@brief	アニメーションのフレーム長(アニメーションの長さ)を取得
		uint				GetFrameCount(const uint _anmStackNo) const;

		//!	@brief	アニメーションのフレームを取得
		uint				GetFrame(const uint _anmStackNo, const uint _anmFrameNo) const;

	private:

		//----------------------------------------------------------
		//	内部クラス
		//----------------------------------------------------------

		//!	@brief		テクスチャのスプライトアニメーションを管理するクラス
		//!	@detais		他で使用することがないので、クラスをすべて隠ぺいするためにCPPに記述
		class CAnmStack {
		public:

			//!	@brief	1つのアニメーションの最大フレーム数
			static const unsigned int	FRAME_MAX = 256;

			//!	@brief	Jsonファイルからアニメーション情報を読み込む
			void Load(const json11::Json& _json);

			//----------------------------------------------------------
			// ゲッター
			//----------------------------------------------------------

			//!	@brief	アニメーション名を取得
			const char*		GetStackName() const { return m_stackName.c_str(); }

			//!	@brief	ループフラグを取得
			bool			GetIsLoop() const { return m_isLoop; }

			//!	@brief	フレーム数を取得
			uint			GetFrameCount() const { return m_frameCount; }

			//!	@brief		アニメーションのテクスチャの連番位置を取得
			//!	@param[in]	_frameNo	アニメーションの位置を指定(再生時間(フレーム))
			uint			GetFrame(uint _frameNo) const;

		private:

			std::string					m_stackName;	// アニメーション名
			bool						m_isLoop;		// ループフラグ
			uint						m_frameCount;	// アニメーションのフレーム数
			std::array<uint, FRAME_MAX>	m_frames;		// アニメーションの連番を保持する配列

		public:
			CAnmStack();
			~CAnmStack();
		};



		// テクスチャ情報
		TextureObj		m_texture;			// テクスチャの実態
		float2			m_divLength;		// テクスチャアニメーションで使用する1辺の長さ(0 < n <=1の値が設定される)(UV値の長さ)
		int2			m_divCount;			// テクスチャアニメーションで使用する分割数

		int2			m_size;				// テクスチャのサイズ(ピクセル数)
		float2			m_divSize;			// 分割後の大きさ(ピクセル数)(m_size / m_divCount)
			
		std::string		m_filePath;			// 読み込んだファイルのパスを保持する

		// アニメーション情報
		uint									m_anmStackCount;	// 保有するアニメーション数
		std::array<CAnmStack, ANM_STACK_MAX>	m_anmStacks;		// アニメーション情報


		//!	@brief	テクスチャの生成
		void LoadTexture(const char* _pFilePath);

		//!	@brief	テクスチャの付与情報の読み込み
		//!	@note	同名のJsonファイルに付与情報を記述し、そのファイルから情報を取得
		void LoadInfo(const char* _pFilePath);

		//!	@brief	アニメーションスタックを取得
		//!	@note	引数の値が正常な値化をチェックしたうえで返す。
		const CAnmStack&	GetAnmStack(const uint _anmStackNo) const;

	public:
		CManager();
		~CManager();
	};

}