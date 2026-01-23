#pragma once

#include	<array>
#include	"Polygon_Data.h"
#include	"Math\Math_Type.h"

namespace NPolygon {

	//!	@brief		ポリゴンクラスの基底クラス
	class CBase {
	public:
		//!	@brief		ポリゴンのデータのバッファ数
		static const uint	DATA_BUFFER_SIZE = 64;
		
		//----------------------------------------------------------
		// 基本的な関数
		//----------------------------------------------------------

		//!	@brief	使用できる状態にする
		void Setup();

		//!	@brief	更新
		//!	@note	バッファの内容を走査する前に更新を行う事
		void Update();

		//!	@brief	描画
		//!	@note	親クラスではアニメーションの更新のみ行っている。
		void Render(const NMath::SMatrix44& _rMatWorld);

		//!	@brief		過去バッファを用いた描画
		//!	@param[in]	_frame	_frame前のバッファを用いて描画
		void Render(uint _frame);

		//----------------------------------------------------------
		// セッター
		//----------------------------------------------------------

		//!	@brief		使用するテクスチャを設定する
		//!	@param[in]	参照するテクスチャ(nullptrを設定すると解除)
		void SetTexture(NAsset::CTextureFactory::SPAsset _pRefTexture = nullptr) { GetCurData().pRefTexture = _pRefTexture; }

		//!	@brief		アニメーションの変更
		void ChangeAnimation(const uint _anmStackNo);

		//!	@brief		サイズを設定(X,Yを同時に設定)
		void SetSize(const float _size) { GetCurData().size.x = _size; GetCurData().size.y = _size; }

		//!	@brief		サイズを設定(X,Yを個別に設定)
		void SetSize(const float2& _size) { GetCurData().size = _size; }
		void SetSize(const float _x, const float _y) { SetSize(float2(_x, _y)); }

		//!	@brief		色を設定
		//!	@brief		全ての頂点に同じ色を設定
		void SetColor(const float4& _color);

		//!	@brief		指定した頂点に色を設定
		void SetColor(const float4& _color, EVertexID _vertexID);

		//!	@brief		テクスチャをX軸反転させる
		//!	@param[in]	_state	true : 反転,		false:通常
		void SetIsReverseX(const bool _state) { GetCurData().isReverseX = _state; }

		//!	@brief		テクスチャをY軸反転
		//!	@param[in]	_state	true : 反転,		false:通常
		void SetIsReverseY(const bool _state) { GetCurData().isReverseY = _state; }

		//!	@brief		ブレンドステートを設定
		void SetBlendState(const NGraphic::EBlendState	_state) { GetCurData().blendState = _state; }

		//!	@brief		カリングモードを設定
		void SetCullMode(const NShader::ECullMode	_cullMode) { GetCurData().cullModel = _cullMode; }

		//!	@brief		X軸方向のアライメントを設定
		void SetAlignX(const EAlignX _alignX) { GetCurData().alignX = _alignX; }

		//!	@brief		Y軸方向のアライメントを設定
		void SetAlignY(const EAlignY _alignY) { GetCurData().alignY = _alignY; }


		//!	@brief		手動で連番テクスチャの位置を制定
		//!	@note		この関数を使用すると、次にChangeAnimation()が呼ばれるまで、アニメーションが無視されるようになる。
		void SetFrame(const uint _frame);


		//----------------------------------------------------------
		// 汎用ゲッター
		//----------------------------------------------------------

		//!	@brief	最新データへの参照を取得する
		SData&			GetCurData() { return m_dataBuf[0]; }
		const SData&	GetCurData() const { return m_dataBuf[0]; }

		//!	@brief		変更可能な過去データの取得する
		//!	@param[in]	_frame	_frame前の情報を取得する
		//!	@note		過去バッファを使って描画を行う場合に、
		//!				過去バッファの値を変更する場合に使用する
		SData&			GetPrevData(const uint _frame);
		const SData&	GetPrevData(const uint _frame) const;

		//!	@brief		過去データを取得する
		//!	@param[in]	_frame	_frame前の情報を取得す
		//!	@note		描画時点のデータを保持いている
		const SData&	GetConstPrevData(const uint _frame) const;


		//----------------------------------------------------------
		// 現在のデータのゲッター
		//----------------------------------------------------------

		//!	@brief		サイズを取得する
		const float2&	GetSize() const { return GetCurData().size; }

		//!	@brief		X軸の反転フラグを取得する
		bool			GetIsReverseX() const { return GetCurData().isReverseX; }

		//!	@brief		Y軸の反転フラグを取得する
		bool			GetIsReverseY() const { return GetCurData().isReverseY; }

	protected:


		//!	@brief	データを指定して描画
		//!	@note	描画のメイン関数。Render() ではこの関数で描画を行う
		virtual void RenderPolygon(const SData& _data) PURE_FUNC;

		//----------------------------------------------------------
		// ゲッター
		//----------------------------------------------------------
		
		//!	@brief	頂点フォーマットを取得
		//!	@nonte	継承後に使用するフォーマットを返す関数を定義する
		virtual DWORD	GetVertexFormat() PURE_FUNC;

		//----------------------------------------------------------
		// ユーティリティ関係
		//----------------------------------------------------------

		//!	@brief			座標にアライメントを適用する
		//!	@note			スクリーン座標系
		//!	@param[in,out]	_rInOutX	アライメントを適用するX座標
		//!	@param[in,out]	_rInOutY	アライメントを適用するY座標
		void ApplyAlign2D(
			float&			_rInOutX,
			float&			_rInOutY,
			const SData&	_data);

		//!	@brief			座標にアライメントを適用する
		//!	@brief			ワールド座標系
		//!	@param[in,out]	_rInOutX	アライメントを適用するX座標
		//!	@param[in,out]	_rInOutY	アライメントを適用するY座標
		void ApplyAlign3D(
			float&			_rInOutX,
			float&			_rInOutY,
			const SData&	_data);

		//!	@brief	指定されたデータの情報を基に、連番テクスチャの番号を返す
		//!	@note	連番をX、Y軸に分解したものを取得する
		void GetTextureNo(
			int2&			_dest,
			const SData&	_data) const;

		//!	@brief		テクスチャ反転時のオフセットを取得する
		//!	@param[out]	_dest	テクスチャの連番のオフセット(0または1が格納される)
		void GetReverseOffset(
			int2&			_dest,
			const SData&	_data) const;

		//!	@brief		分割後の1辺のUVの長さを取得する
		//!	@nte		テクスチャの存在を確認することが主な目的
		void GetDivLength(
			float2&			_dest,
			const SData&	_data) const;

	private:

		// 使用するテクスチャ

		// 描画するポリゴンの情報
		std::array<SData, DATA_BUFFER_SIZE>	m_dataBuf;		// ポリゴン情報のデータ
															// 0:最新 DATA_BUFFER_MAX-1:最古
															// 過去データの内容を変更して描画することが可能

		std::array<SData, DATA_BUFFER_SIZE>	m_constDataBuf;	// 変更する前の元データ

		//!	@brief	バッファの更新
		void UpdateBuffer();

		//!	@brief	アニメーションの更新
		void UpdateAnimation();

		//!	@brief	テクスチャをセット
		void SetTexture(const SData& _data);

	public:
		CBase();
		virtual ~CBase();
	};

}
