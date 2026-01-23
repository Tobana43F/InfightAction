#include	"Polygon_Base.h"

#include	"Camera\Camera_GlobalManager.h"
#include	"DirectX\DirectX_Manager.h"
#include	"DirectX\DirectX_Util.h"
#include	"Debug\Debug_Assert.h"

namespace NPolygon {
	
	CBase::CBase()
	{
		// 必ず何かしらの描画が行われるようにしておく
		SetSize(1);
	}

	CBase::~CBase()
	{

	}

	void CBase::Setup()
	{

	}

	void CBase::Update()
	{
		UpdateBuffer();

		// アニメーションの更新を描画直前に行う
		//!	@todo	先に更新すると、描画と1フレームずれる可能性があるので要検討
		UpdateAnimation();
	}

	void CBase::Render(const NMath::SMatrix44& _rMatWorld)
	{
		// フォーマットをセット
		NDirectX::GetD3DDevice()->SetFVF(GetVertexFormat());

		// レンダーステートをセット
		NDirectX::SetBlendState(GetCurData().blendState);

		// カリングモードを設定
		NDirectX::SetCullMode(GetCurData().cullModel);

		// データへのアクセス省略
		auto& data = GetCurData();

		// データを引数で渡された値で更新
		data.matWorld = _rMatWorld;

		// 描画時点の値を取得
		m_constDataBuf[0] = data;

		// テクスチャをセット
		//!	@todo	マルチテクスチャに対応したい(使う場面が出たら実装する)
		SetTexture(data);

		// 最新のデータで描画
		RenderPolygon(data);
	}

	void CBase::Render(uint _frame)
	{
		Assert(_frame < DATA_BUFFER_SIZE && "引数が不正です。");

		// フォーマットをセット
		NDirectX::GetD3DDevice()->SetFVF(GetVertexFormat());

		// レンダーステートをセット
		NDirectX::SetBlendState(GetCurData().blendState);

		// カリングモードを設定
		NDirectX::SetCullMode(GetCurData().cullModel);

		auto& data = GetPrevData(_frame);

		// テクスチャをセット
		//!	@todo	マルチテクスチャに対応したい(使う場面が出たら実装する)
		SetTexture(data);

		// 指定された情報を描画
		RenderPolygon(data);
	}

	void CBase::ChangeAnimation(const uint _anmStackNo)
	{
		auto& data = GetCurData();

		Assert(data.pRefTexture != nullptr && "テクスチャが設定されていません。");
		Assert(data.pRefTexture->GetAnmStackCount() > 0 && "テクスチャアニメーションが存在しません。");
		Assert(_anmStackNo < GetCurData().pRefTexture->GetAnmStackCount() && "不正なAnmStackNoが指定されました。");

		data.anmStackNo = _anmStackNo;

		// アニメーションを無視しない
		GetCurData().isIgnoreAnm = false;

		// フレーム初期化
		GetCurData().anmFrame = 0;
	}

	void CBase::SetColor(const float4& _color)
	{
		float4 col = _color;
		for (int i = 0; i < 4; ++i) {
			if (col.array[i] > 1) { col.array[i] = 1; }
			else if (col.array[i] < 0) { col.array[i] = 0; }
		}

		for (int i = 0; i < EVertexID::MAX; ++i) {
			GetCurData().colors[i] = col;
		}
	}

	//!	@brief		指定した頂点に色を設定
	void CBase::SetColor(const float4& _color, EVertexID _vertexID)
	{
		float4 col = _color;
		for (int i = 0; i < 4; ++i) {
			if (col.array[i] > 1) { col.array[i] = 1; }
			else if (col.array[i] < 0) { col.array[i] = 0; }
		}

		GetCurData().colors[_vertexID] = col;
	}

	void CBase::SetFrame(const uint _frame)
	{
		auto& data = GetCurData();
		Assert(data.pRefTexture != nullptr && "テクスチャが設定されていません。");

		// 直接テクスチャ番号を指定する方法
		uint	divNum = data.pRefTexture->GetDivCountAll();

		Assert(_frame < divNum && "指定したFrameが連番テクスチャの最大数を超えています。");

		// アニメーションを無視する
		data.isIgnoreAnm = true;
		data.anmStackNo = _frame;
	}


	//----------------------------------------------------------

	SData&	CBase::GetPrevData(const uint _frame)
	{
		Assert(_frame < DATA_BUFFER_SIZE && "引数が不正です。");
		return m_dataBuf[_frame];
	}

	const SData&	CBase::GetPrevData(const uint _frame) const
	{
		Assert(_frame < DATA_BUFFER_SIZE && "引数が不正です。");
		return m_dataBuf[_frame];
	}

	const SData& CBase::GetConstPrevData(const uint _frame) const
	{
		Assert(_frame < DATA_BUFFER_SIZE && "引数が不正です。");
		return m_constDataBuf[_frame];
	}

	//----------------------------------------------------------

	void CBase::UpdateBuffer()
	{
		for (int i = DATA_BUFFER_SIZE - 1; i > 0; --i) {
			m_dataBuf[i] = m_dataBuf[i - 1];
			m_constDataBuf[i] = m_constDataBuf[i - 1];
		}
	}

	void CBase::UpdateAnimation()
	{
		auto& data = GetCurData();

		// アニメーション無視、テクスチャなし、アニメーションなしのいずれかの場合は無視する
		if (data.isIgnoreAnm) { return; }
		if (data.pRefTexture == nullptr) { return; }
		if (data.pRefTexture->GetAnmStackCount() <= 0) { return; }


		uint prevAnmFrame = data.anmFrame;

		// 初期化
		data.isAnmEnd = false;
		data.anmFrame++;

		//!	@todo	制御
		// ループ処理
		if (data.anmFrame >= data.pRefTexture->GetFrameCount(data.anmStackNo)) {
			if (data.pRefTexture->GetIsLoop(data.anmStackNo)) {
				// 最初に戻す
				data.anmFrame = 0;
			}
			else {
				// 基に戻す
				data.anmFrame = prevAnmFrame;
				data.isAnmEnd = true;
			}
		}
	}

	void CBase::SetTexture(const SData& _data)
	{
		if (_data.pRefTexture != nullptr) {
			NDirectX::GetD3DDevice()->SetTexture(0, _data.pRefTexture->GetTexture());
		}
		else {
			NDirectX::GetD3DDevice()->SetTexture(0, nullptr);
		}
	}

	void CBase::ApplyAlign2D(
		float&			_rInOutX, 
		float&			_rInOutY, 
		const SData&	_data)
	{
		float2 halfSize = _data.size * 0.5f;

		// X軸のアライメントを設定
		switch (_data.alignX) {
			case EAlignX::LEFT:
				_rInOutX += halfSize.x;
				break;
			case EAlignX::CENTER:
				// 何もしない
				break;
			case EAlignX::RIGHT:
				_rInOutX -= halfSize.x;
				break;
		}

		// Y軸のアライメントを設定
		switch (_data.alignY) {
			case EAlignY::UP:
				_rInOutY += halfSize.y;
				break;
			case EAlignY::CENTER:
				// 何もしない
				break;
			case EAlignY::BOTTOM:
				_rInOutY -= halfSize.y;
				break;
		}
	}

	void CBase::ApplyAlign3D(
		float&			_rInOutX,
		float&			_rInOutY,
		const SData&	_data)
	{
		float2 halfSize = _data.size * 0.5f;

		// X軸のアライメントを設定
		switch (_data.alignX) {
			case EAlignX::LEFT:
				_rInOutX += halfSize.x;
				break;
			case EAlignX::CENTER:
				// 何もしない
				break;
			case EAlignX::RIGHT:
				_rInOutX -= halfSize.x;
				break;
		}

		// Y軸のアライメントを設定
		switch (_data.alignY) {
			case EAlignY::UP:
				_rInOutY -= halfSize.y;
				break;
			case EAlignY::CENTER:
				// 何もしない
				break;
			case EAlignY::BOTTOM:
				_rInOutY += halfSize.y;
				break;
		}
	}

	void CBase::GetTextureNo(
		int2&			_dest,
		const SData&	_data) const
	{
		_dest = int2(0, 0);

		// アニメーション付きテクスチャのテクスチャ番号を取得する
		// テクスチャあり、アニメーションあり、アニメーション有効時
		if (_data.pRefTexture != nullptr && _data.pRefTexture->GetAnmStackCount() > 0 && !_data.isIgnoreAnm) {
			_dest.x = _data.pRefTexture->GetFrame(_data.anmStackNo, _data.anmFrame) % _data.pRefTexture->GetDivCount().x;		// 横の数での剰余
			_dest.y = _data.pRefTexture->GetFrame(_data.anmStackNo, _data.anmFrame) / _data.pRefTexture->GetDivCount().x;		// 横の数の商
		}
		// アニメーションがない場合は指定したAnmNoを使用して指定できるようにする。
		// テクスチャあり、アニメーション無視の時
		else if (_data.pRefTexture != nullptr && _data.isIgnoreAnm) {
			_dest.x = _data.anmStackNo % _data.pRefTexture->GetDivCount().x;
			_dest.y = _data.anmStackNo / _data.pRefTexture->GetDivCount().x;
		}
	}

	void CBase::GetReverseOffset(
		int2&			_dest,
		const SData&	_data) const
	{
		_dest = int2(0, 0);

		if (_data.isReverseX) {
			_dest.x = 1;
		}
		if (_data.isReverseY) {
			_dest.y = 1;
		}
	}

	void CBase::GetDivLength(
		float2&			_dest,
		const SData&	_data) const
	{
		_dest = float2(1, 1);

		// テクスチャがある場合、テクスチャの分割後のUVを取得
		if (_data.pRefTexture != nullptr) {
			_dest = _data.pRefTexture->GetDivLength();
		}
	}
}
