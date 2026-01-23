#pragma once
#include	<d3dx9.h>
#include	<d3d9.h>

// OBB用構造体
struct OBBBBOX {
	D3DXVECTOR3	vecAxisX;		// X軸（ローカル座標系）
	D3DXVECTOR3	vecAxisY;		// Y軸（ローカル座標系）
	D3DXVECTOR3	vecAxisZ;		// Z軸（ローカル座標系）
	D3DXVECTOR3 center;			// 中心座標（ローカル座標系）

	float		fLengthX;		// ＢＢＯＸのＸサイズ	
	float		fLengthY;		// ＢＢＯＸのＹサイズ
	float		fLengthZ;		// ＢＢＯＸのＺサイズ  

	D3DXVECTOR3	CurrentvecAxisX;// X軸(ワールド座標系)
	D3DXVECTOR3	CurrentvecAxisY;// Y軸(ワールド座標系)
	D3DXVECTOR3	CurrentvecAxisZ;// Z軸(ワールド座標系)
	D3DXVECTOR3 Currentcenter;	// BBOXの現在位置(ワールド座標系)

	D3DXVECTOR3 vMax;			// 最大値
	D3DXVECTOR3 vMin;			// 最小値
};

class CBBoxGenerater
{
public:
	static const int N = 4;
	static const int MAX = 100;
	static const double TOL;
public:
	CBBoxGenerater(void) {}
	~CBBoxGenerater(void) {}

	OBBBBOX CalcBBox(
		D3DXVECTOR3*	vertex,
		DWORD*			index,
		int				numVetices,
		D3DXMATRIX		eigenmat) {

		D3DXVECTOR3 rvec, svec, tvec, vertexvec, center;
		float rmin = 0.0, rmax = 0.0, smin = 0.0, smax = 0.0, tmin = 0.0, tmax = 0.0;
		float dot;
		OBBBBOX result;

		//固有ベクトルが格納された行列を各ベクトルへ代入し正規化
		rvec.x = eigenmat._11; rvec.y = eigenmat._21; rvec.z = eigenmat._31;
		svec.x = eigenmat._12; svec.y = eigenmat._22; svec.z = eigenmat._32;
		tvec.x = eigenmat._13; tvec.y = eigenmat._23; tvec.z = eigenmat._33;

		D3DXVec3Normalize(&rvec, &rvec);
		D3DXVec3Normalize(&svec, &svec);
		D3DXVec3Normalize(&tvec, &tvec);

		//各ベクトルとすべての頂点の内積から最小値と最大値を求める
		for (int i = 0; i<numVetices; i++) {
			vertexvec.x = vertex[index[i]].x; vertexvec.y = vertex[index[i]].y; vertexvec.z = vertex[index[i]].z;
			dot = D3DXVec3Dot(&rvec, &vertexvec);				// Rベクトルに射影
			if (i == 0 || dot < rmin) rmin = dot;					// 最小値を求める
			if (i == 0 || dot > rmax) rmax = dot;					// 最大値を求める

			dot = D3DXVec3Dot(&svec, &vertexvec);				// Sベクトルに射影
			if (i == 0 || dot < smin) smin = dot;					// 最小値を求める
			if (i == 0 || dot > smax) smax = dot;					// 最大値を求める

			dot = D3DXVec3Dot(&tvec, &vertexvec);				// Tベクトルに射影
			if (i == 0 || dot < tmin) tmin = dot;					// 最小値を求める
			if (i == 0 || dot > tmax) tmax = dot;					// 最大値を求める
		}

		//BBOX構造体に値を格納
		// 中心座標を求める
		center.x = (rmax + rmin) / 2;	center.y = (smax + smin) / 2;	center.z = (tmax + tmin) / 2;			// 原点からの距離
		result.center.x = rvec.x * center.x + svec.x * center.y + tvec.x * center.z;							
		result.center.y = rvec.y * center.x + svec.y * center.y + tvec.y * center.z;
		result.center.z = rvec.z * center.x + svec.z * center.y + tvec.z * center.z;

		result.fLengthX = (rmax - rmin); result.fLengthY = (smax - smin); result.fLengthZ = (tmax - tmin);		// BBOXの幅、高さ、奥行
		
		result.vMax.x = rmax; result.vMax.y = smax; result.vMax.z = tmax;										// 最大値の組み合わせ
		result.vMin.x = rmin; result.vMin.y = smin; result.vMin.z = tmin;										// 最小値の組み合わせ
		
		result.vecAxisX = rvec; result.vecAxisY = svec; result.vecAxisZ = tvec;									// BBOXの軸

		return result;
	}

	D3DXMATRIX GetCovarianceMatrix(D3DXVECTOR3* vertex, DWORD* index, int numVertices);
	bool GetEigenVector(D3DXMATRIX& a, D3DXMATRIX& x);						// 固有ベクトルを求める
};

class COBB
{
private:
	OBBBBOX			m_obbboxinfo;		// OBB BBOX情報
	D3DXMATRIX		m_matworld;			// OBB 描画用ワールド変換行列
	LPD3DXMESH		m_boxmesh;			// BBOX mesh
	D3DMATERIAL9	m_material;			// 半透明マテリアル
	D3DMATERIAL9	m_materialR;		// 半透明マテリアル
	D3DMATERIAL9	m_materialG;		// 無効時のマテリアル

	bool			m_hitflag;			// 当たり：true はずれ:false
	bool			m_isActive;			// あたり判定が有効かどうか

	//bool CompareLength(OBBBBOX* pboxA,OBBBBOX* pboxB,D3DXVECTOR3* pvecSeparate,D3DXVECTOR3* pvecDistance)
	//「距離」が「２つの影の合計」より大きい場合（非衝突）はFALSE  等しいか小さい場合（衝突）はTRUE
	// この関数は、OBBCollisionDetection関数のみに呼び出されるサブルーチン
	bool CompareLength(
		const OBBBBOX& pboxA,
		const OBBBBOX& pboxB,
		const D3DXVECTOR3& pvecSeparate,		// 分離軸
		const D3DXVECTOR3& pvecDistance);		// ２つのオブジェクトを結ぶベクトル

	void TraneparentMaterial(float alpha) {
		// 半透明なマテリアルを生成
		m_material.Diffuse.r = 0.0f;
		m_material.Diffuse.g = 0.0f;
		m_material.Diffuse.b = 1.0f;
		m_material.Diffuse.a = alpha;

		m_material.Ambient.r = 0.0f;
		m_material.Ambient.g = 0.0f;
		m_material.Ambient.b = 1.0f;
		m_material.Ambient.a = 1.0f;

		m_material.Specular.r = 0.0f;
		m_material.Specular.g = 0.0f;
		m_material.Specular.b = 1.0f;
		m_material.Specular.a = 1.0f;

		m_material.Emissive.r = 0.0f;
		m_material.Emissive.g = 0.0f;
		m_material.Emissive.b = 0.0f;
		m_material.Emissive.a = 0.0f;
	}

	void TraneparentMaterialRed(float alpha) {
		// 半透明なマテリアルを生成
		m_materialR.Diffuse.r = 1.0f;
		m_materialR.Diffuse.g = 0.0f;
		m_materialR.Diffuse.b = 0.0f;
		m_materialR.Diffuse.a = alpha;

		m_materialR.Ambient.r = 1.0f;
		m_materialR.Ambient.g = 0.0f;
		m_materialR.Ambient.b = 0.0f;
		m_materialR.Ambient.a = 1.0f;

		m_materialR.Specular.r = 1.0f;
		m_materialR.Specular.g = 0.0f;
		m_materialR.Specular.b = 0.0f;
		m_materialR.Specular.a = 1.0f;

		m_materialR.Emissive.r = 0.0f;
		m_materialR.Emissive.g = 0.0f;
		m_materialR.Emissive.b = 0.0f;
		m_materialR.Emissive.a = 0.0f;
	}

	void TraneparentMaterialGray(float alpha)
	{
		// 半透明なマテリアルを生成
		m_materialG.Diffuse.r = 0.0f;
		m_materialG.Diffuse.g = 0.0f;
		m_materialG.Diffuse.b = 0.0f;
		m_materialG.Diffuse.a = alpha;

		m_materialG.Ambient.r = 0.0f;
		m_materialG.Ambient.g = 0.0f;
		m_materialG.Ambient.b = 0.0f;
		m_materialG.Ambient.a = 1.0f;

		m_materialG.Specular.r = 0.0f;
		m_materialG.Specular.g = 0.0f;
		m_materialG.Specular.b = 0.0f;
		m_materialG.Specular.a = 1.0f;

		m_materialG.Emissive.r = 0.0f;
		m_materialG.Emissive.g = 0.0f;
		m_materialG.Emissive.b = 0.0f;
		m_materialG.Emissive.a = 0.0f;
	}

	void CalculateBoundingBox(const LPD3DXMESH	lpmesh,
		const LPDIRECT3DDEVICE9 lpdevice);

public:

	COBB() {
		m_boxmesh=nullptr;						
		D3DXMatrixIdentity(&m_matworld);		// 単位行列化
		TraneparentMaterial(0.3f);				// 半透明マテリアル（ブルー）
		TraneparentMaterialRed(0.3f);			// 半透明マテリアル（赤）
		TraneparentMaterialGray(0.3f);

		m_isActive = true;
	}

	~COBB() {
		Exit();
	}

	LPD3DXMESH CreateBoxMesh(const LPDIRECT3DDEVICE9 lpdev) {
		// バウンディングボックスを表すＢＯＸメッシュを生成
		D3DXCreateBox(
			lpdev,
			m_obbboxinfo.fLengthX,				// 幅
			m_obbboxinfo.fLengthY,				// 高さ
			m_obbboxinfo.fLengthZ,				// 奥行
			&m_boxmesh,
			nullptr
		);

		return m_boxmesh;
	}

	// 主成分分析
	OBBBBOX PCA(D3DXVECTOR3* Vertices,		// 頂点座標軍
			 DWORD* vindex,					// インデックス群
			 int	numVertices) {				// 頂点数

		CBBoxGenerater	Generater;			// PCA分析でBBOXを作るクラス
		D3DXMATRIX		eigenmat;			// 固有値行列

		// 共分散行列を作成
		D3DXMATRIX mat = Generater.GetCovarianceMatrix(Vertices, 
													vindex, 
													numVertices);
		// 固有ベクトルを計算
		Generater.GetEigenVector(mat, eigenmat);

		// 固有ベクトルを基にしてＢＢＯＸを作成
		m_obbboxinfo = Generater.CalcBBox(
							Vertices,		// 頂点座標群
							vindex,			// インデックス数
							numVertices,	// 頂点数
							eigenmat);		// 固有ベクトル

		return m_obbboxinfo;
	}

	void Init(LPD3DXMESH mesh,LPDIRECT3DDEVICE9 lpdev) {
		CalculateBoundingBox(mesh,lpdev);			// OBBBOX作成
		TraneparentMaterial(0.3f);					// 半透明マテリアル
		CreateBoxMesh(lpdev);						// メッシュ作成
	}

	void Exit() {
		if(m_boxmesh){
			m_boxmesh->Release();
			m_boxmesh=nullptr;
		}
	}

	void Update(D3DXMATRIX& matworld);

	void Draw(LPDIRECT3DDEVICE9 lpdev);

	void DrawRed(LPDIRECT3DDEVICE9 lpdev);

	bool Collision(COBB& obb);

	D3DXVECTOR3 GetPosition() {
		return m_obbboxinfo.Currentcenter;
	}

	OBBBBOX GetOBB() {
		return m_obbboxinfo;
	}

	D3DXMATRIX GetMatrix() {
		return m_matworld;
	}

	bool isHit() {
		return m_hitflag;
	}

	void SetHit() {
		m_hitflag = true;
	}

	void SetNotHit() {
		m_hitflag = false;
	}

	void SetIsActive(bool _state)
	{
		m_isActive = _state;
	}

	bool GetIsActive() { return m_isActive; }
};

