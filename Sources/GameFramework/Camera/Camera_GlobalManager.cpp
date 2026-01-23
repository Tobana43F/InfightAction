#include	"Camera_GlobalManager.h"

#include	<array>
#include	"Math\Math_Common.h"
#include	"Debug\Debug_Assert.h"
#include	"DirectX\DirectX_Manager.h"	// コテイパイプへの設定も同時に行う(DX9)
#include	"DirectX\DirectX_Util.h"
#include	"System\System_Def.h"

namespace {

	//	登録用配列のサイズ
	constexpr	size_t	ARRAY_SiZE = StaticCast<size_t>(NCamera::EManagerIndex::MAX);

	//	登録先
	std::array<NMath::SMatrix44, ARRAY_SiZE>	g_viewHolder;
	std::array<NMath::SMatrix44, ARRAY_SiZE>	g_projHolder;
	std::array<NCamera::SViewport, ARRAY_SiZE>	g_viewportHolder;

	std::array<NMath::SMatrix44, ARRAY_SiZE>	g_orthoHolder;


	//!	@brief	MAINのカメラのSetupを行う
	void SetupMainCamera();

	//!	@brief	ORTHOのカメラのSetupを行う
	void SetupOrthoCamera();
}

namespace NCamera {

	void SetupGlobalManager()
	{
		SetupMainCamera();
		SetupOrthoCamera();

		// 3Dように初期化
		SetViewMatrix();
		SetProjMatrix();
	}

	void RegisterViewMatrix(
		const NMath::SMatrix44& _view,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		g_viewHolder[index] = _view;

		// 固定パイプラインへの設定
		D3DXMATRIX	mat;
		NDirectX::ToD3DMatrix(mat, _view);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_VIEW, &mat);
	}

	void RegisterProjMatrix(
		const NMath::SMatrix44& _proj,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		g_projHolder[index] = _proj;

		// 固定パイプラインへの設定
		D3DXMATRIX	mat;
		NDirectX::ToD3DMatrix(mat, _proj);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &mat);
	}

	void RegisterViewport(
		const SViewport&		_viewport,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		g_viewportHolder[index] = _viewport;

		// 固定パイプラインへの設定
		D3DVIEWPORT9	viewport;
		viewport.X = StaticCast<DWORD>(_viewport.topLeftPos.x);
		viewport.Y = StaticCast<DWORD>(_viewport.topLeftPos.y);
		viewport.Width = StaticCast<DWORD>(_viewport.size.x);
		viewport.Height = StaticCast<DWORD>(_viewport.size.y);
		viewport.MinZ = _viewport.minDepth;
		viewport.MaxZ = _viewport.maxDepth;
		NDirectX::GetD3DDevice()->SetViewport(&viewport);
	}

	void RegisterOrthoMatrix(
		const NMath::SMatrix44& _ortho,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		g_orthoHolder[index] = _ortho;

		// 固定パイプラインへの設定
		D3DXMATRIX	mat;
		NDirectX::ToD3DMatrix(mat, _ortho);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &mat);
	}



	void SetViewMatrix(const EManagerIndex	_index)
	{
		NMath::SMatrix44 mat;
		NCamera::GetViewMatrix(mat, _index);

		// 固定パイプラインへの設定
		D3DXMATRIX	matDX;
		NDirectX::ToD3DMatrix(matDX, mat);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_VIEW, &matDX);
	}

	void SetProjMatrix(const EManagerIndex	_index)
	{
		NMath::SMatrix44 mat;
		NCamera::GetProjMatrix(mat, _index);

		// 固定パイプラインへの設定
		D3DXMATRIX	matDX;
		NDirectX::ToD3DMatrix(matDX, mat);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matDX);
	}

	void SetViewport(const EManagerIndex	_index)
	{
		SViewport	viewport;
		NCamera::GetViewport(viewport, _index);

		// 固定パイプラインへの設定
		D3DVIEWPORT9	viewportDX;
		viewportDX.X = StaticCast<DWORD>(viewport.topLeftPos.x);
		viewportDX.Y = StaticCast<DWORD>(viewport.topLeftPos.y);
		viewportDX.Width = StaticCast<DWORD>(viewport.size.x);
		viewportDX.Height = StaticCast<DWORD>(viewport.size.y);
		viewportDX.MinZ = viewport.minDepth;
		viewportDX.MaxZ = viewport.maxDepth;
		NDirectX::GetD3DDevice()->SetViewport(&viewportDX);
	}

	void SetOrthoMatrix(const EManagerIndex	_index)
	{
		NMath::SMatrix44 mat;
		NCamera::GetOrthoMatrix(mat, _index);

		// 固定パイプラインへの設定
		D3DXMATRIX	matDX;
		NDirectX::ToD3DMatrix(matDX, mat);
		NDirectX::GetD3DDevice()->SetTransform(D3DTS_PROJECTION, &matDX);
	}



	void GetViewMatrix(
		NMath::SMatrix44&		_dest,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		_dest = g_viewHolder[index];
	}

	void GetProjMatrix(
		NMath::SMatrix44&		_dest,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		_dest = g_projHolder[index];
	}

	void GetViewport(
		SViewport&				_dest,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		_dest = g_viewportHolder[index];
	}

	void GetOrthoMatrix(
		NMath::SMatrix44&		_dest,
		const EManagerIndex		_index)
	{
		int index = StaticCast<int>(_index);
		_dest = g_orthoHolder[index];
	}
}


namespace {

	void SetupMainCamera()
	{
		// ビュー変換行列の初期値
		NMath::SMatrix44	matView;
		NMath::SVector3		pos(0, 0, -1);
		NMath::SVector3		foc(0, 0, 0);
		NMath::SVector3		up(0, 1, 0);
		NMath::MakeMatrixLookAt(
			matView,
			pos, foc, up);
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			g_viewHolder[i] = matView;
		}

		// プロジェクション変換行列の初期値
		NMath::SMatrix44	matProj;
		float				fovYRad = NMath::ToRad(60);
		float				aspect = NSystem::WINDOW_WIDTH / NSystem::WINDOW_HEIGHT;	
		float				nearCull = 0.1;	// 10cm
		float				farCull = 3000;	// 3km
		NMath::MakeMatrixPerspectiveFovLH(
			matProj,
			fovYRad,
			aspect,
			nearCull,
			farCull);
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			g_projHolder[i] = matProj;
		}

		// ビューポートの初期値
		NCamera::SViewport	viewport;
		viewport.topLeftPos = float2(0, 0);
		viewport.size = float2(NSystem::WINDOW_WIDTH, NSystem::WINDOW_HEIGHT);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			g_viewportHolder[i] = viewport;
		}

		// 登録
		NCamera::RegisterViewMatrix(matView);
		NCamera::RegisterProjMatrix(matProj);
		NCamera::RegisterViewport(viewport);
	}

	void SetupOrthoCamera()
	{
		// ビュー変換行列の初期値
		NMath::SMatrix44	matOrtho;
		NMath::SVector3		pos(0, 0, -1);
		NMath::SVector3		foc(0, 0, 0);
		NMath::SVector3		up(0, 1, 0);
		NMath::MakeMatrixOrthographicLH(
			matOrtho,
			NSystem::WINDOW_WIDTH,
			NSystem::WINDOW_HEIGHT,
			0,
			1);

		for (uint i = 0; i < ARRAY_SiZE; ++i) {
			g_orthoHolder[i] = matOrtho;
		}

		// メインに登録
		NCamera::RegisterOrthoMatrix(matOrtho);
	}
}