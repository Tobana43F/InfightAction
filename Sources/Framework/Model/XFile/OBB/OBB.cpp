#include	"OBB.h"

#include	"Math/Math_Type.h"

const double CBBoxGenerater::TOL = 1.0e-10;
const double PI = 3.14159265358979323846;

bool COBB::CompareLength(
	const OBBBBOX&		pboxA,
	const OBBBBOX&		pboxB,
	const D3DXVECTOR3&	pvecSeparate,		// 分離軸
	const D3DXVECTOR3&	pvecDistance)		// ２つのオブジェクトを結ぶベクトル
{
	float fDistance = fabsf(D3DXVec3Dot(&pvecDistance, &pvecSeparate));
	// 分離軸に射影した中心間を結ぶベクトル

	//分離軸上にボックスAを射影した影の長さ
	float fShadowA = 0;

	//分離軸上にボックスＢを射影した影の長さ
	float fShadowB = 0;

	//それぞれのボックスの”影”を算出
	fShadowA =
		// 指定軸へのバウンディングボックスAのX軸への射影
		fabsf(D3DXVec3Dot(&pvecSeparate, &pboxA.CurrentvecAxisX) * (pboxA.fLengthX) / 2) +
		// 指定軸へのバウンディングボックスAのY軸への射影
		fabsf(D3DXVec3Dot(&pvecSeparate, &pboxA.CurrentvecAxisY) * (pboxA.fLengthY) / 2) +
		// 指定軸へのバウンディングボックスAのZ軸への射影	
		fabsf(D3DXVec3Dot(&pvecSeparate, &pboxA.CurrentvecAxisZ) * (pboxA.fLengthZ) / 2);

	fShadowB =
		// 指定軸へのバウンディングボックスBのX軸への射影
		fabsf(D3DXVec3Dot(&pvecSeparate, &pboxB.CurrentvecAxisX) * (pboxB.fLengthX) / 2) +
		// 指定軸へのバウンディングボックスBのY軸への射影
		fabsf(D3DXVec3Dot(&pvecSeparate, &pboxB.CurrentvecAxisY) * (pboxB.fLengthY) / 2) +
		// 指定軸へのバウンディングボックスBのZ軸への射影
		fabsf(D3DXVec3Dot(&pvecSeparate, &pboxB.CurrentvecAxisZ) * (pboxB.fLengthZ) / 2);

	if (fDistance > fShadowA + fShadowB) {
		return false;
	}

	return true;
}

void COBB::CalculateBoundingBox(
	const LPD3DXMESH		lpmesh,
	const LPDIRECT3DDEVICE9 lpdevice)
{

	LPD3DXMESH		clonemesh;						// クローンメッシュ

	LPDIRECT3DVERTEXBUFFER9		lpVertexBuffer;		// 頂点バッファ
	int							numvertices;		// 頂点数

	float			*x;						// 頂点座標データ
	float			*y;						// 頂点座標データ
	float			*z;						// 頂点座標データ

	float			minx, miny, minz;		// 最小値（Ｘ，Ｙ，Ｚ）
	float			maxx, maxy, maxz;		// 最大値（Ｘ，Ｙ、Ｚ）
	float			cx, cy, cz;				// 中心座標

	HRESULT			hr;						// 戻り値

	BYTE*	data;							// 頂点バッファ内の頂点データの先頭アドレス
	float*	pfloat;							// ワーク用のポインタ

	void* pVertices = NULL;

	// 当たり判定用データ作成のためにクローンメッシュを作成（頂点座標だけの）
	hr = lpmesh->CloneMeshFVF(D3DXMESH_MANAGED, (D3DFVF_XYZ), lpdevice, &clonemesh);
	if (FAILED(hr)) {
		MessageBox(0, "Clone mesh create error ", "Clone mesh create error ", MB_OK);
	}
	else {
		numvertices = clonemesh->GetNumVertices();		// 頂点数をゲット
		clonemesh->GetVertexBuffer(&lpVertexBuffer);	// 頂点バッファオブジェクトへのポインタをゲット
														// 頂点バッファロック
		hr = lpVertexBuffer->Lock(0,					// ０バイト目からロック
			0,
			(void**)&data,				// 頂点座標の先頭アドレス格納場所
			D3DLOCK_READONLY);			// 読み込み専用で取得

		if (hr == D3D_OK) {
			// 頂点数分の座標格納エリア確保
			x = new float[numvertices];
			y = new float[numvertices];
			z = new float[numvertices];

			// 頂点をすべて取り出す
			pfloat = (float *)data;
			for (int i = 0; i < numvertices; i++) {
				x[i] = *pfloat++;			// Ｘ座標取得
				y[i] = *pfloat++;			// Ｙ座標取得
				z[i] = *pfloat++;			// Ｚ座標取得
			}

			// ＸＹＺそれぞれについて、最大値、最小値を求める
			for (int i = 0; i < numvertices; i++) {
				if (i == 0) {
					minx = maxx = x[i];
					miny = maxy = y[i];
					minz = maxz = z[i];
				}
				if (minx >= x[i]) minx = x[i];
				if (miny >= y[i]) miny = y[i];
				if (minz >= z[i]) minz = z[i];
				if (maxx <= x[i]) maxx = x[i];
				if (maxy <= y[i]) maxy = y[i];
				if (maxz <= z[i]) maxz = z[i];
			}

			cx = (maxx + minx) / 2;		// 球の中心座標を求める
			cy = (maxy + miny) / 2;		// 球の中心座標を求める
			cz = (maxz + minz) / 2;		// 球の中心座標を求める

			delete[] z;
			delete[] y;
			delete[] x;
			lpVertexBuffer->Unlock();

			// 中心座標をセット
			m_obbboxinfo.center.x = cx;
			m_obbboxinfo.center.y = cy;
			m_obbboxinfo.center.z = cz;

			m_obbboxinfo.Currentcenter = m_obbboxinfo.center;

			// 大きさをセット
			m_obbboxinfo.fLengthX = fabsf(maxx - minx);
			m_obbboxinfo.fLengthY = fabsf(maxy - miny);
			m_obbboxinfo.fLengthZ = fabsf(maxz - minz);

			// ３軸をセット
			m_obbboxinfo.vecAxisX = D3DXVECTOR3(1, 0, 0);
			m_obbboxinfo.vecAxisY = D3DXVECTOR3(0, 1, 0);
			m_obbboxinfo.vecAxisZ = D3DXVECTOR3(0, 0, 1);
		}
	}
	clonemesh->Release();
}

void COBB::Update(D3DXMATRIX& matworld)
{
	D3DXMATRIX  mat = matworld;

	// OBBの中心座標を更新
	D3DXVec3TransformCoord(&m_obbboxinfo.Currentcenter, &m_obbboxinfo.center, &matworld);

	//行列を回転成分のみにする
	mat._41 = 0; mat._42 = 0; mat._43 = 0; mat._44 = 1;
	mat._14 = 0; mat._24 = 0; mat._34 = 0;

	// 共分散行列で求めた固有ベクトル（Ｘ、Ｙ、Ｚ軸）をボーンを描画する行列で座標変換する
	D3DXVec3TransformCoord(&m_obbboxinfo.CurrentvecAxisX, &m_obbboxinfo.vecAxisX, &mat);
	D3DXVec3TransformCoord(&m_obbboxinfo.CurrentvecAxisY, &m_obbboxinfo.vecAxisY, &mat);
	D3DXVec3TransformCoord(&m_obbboxinfo.CurrentvecAxisZ, &m_obbboxinfo.vecAxisZ, &mat);

	// 求めたベクトルでＢＢＯＸの行列を求める
	m_matworld._11 = m_obbboxinfo.CurrentvecAxisX.x;
	m_matworld._12 = m_obbboxinfo.CurrentvecAxisX.y;
	m_matworld._13 = m_obbboxinfo.CurrentvecAxisX.z;

	m_matworld._21 = m_obbboxinfo.CurrentvecAxisY.x;
	m_matworld._22 = m_obbboxinfo.CurrentvecAxisY.y;
	m_matworld._23 = m_obbboxinfo.CurrentvecAxisY.z;

	m_matworld._31 = m_obbboxinfo.CurrentvecAxisZ.x;
	m_matworld._32 = m_obbboxinfo.CurrentvecAxisZ.y;
	m_matworld._33 = m_obbboxinfo.CurrentvecAxisZ.z;

	m_matworld._41 = m_obbboxinfo.Currentcenter.x;
	m_matworld._42 = m_obbboxinfo.Currentcenter.y;
	m_matworld._43 = m_obbboxinfo.Currentcenter.z;

	m_hitflag = false;
}

void COBB::Draw(LPDIRECT3DDEVICE9 lpdev)
{
	if (!m_isActive) {
		lpdev->SetMaterial(&m_materialG);
		lpdev->SetTexture(0, nullptr);
		lpdev->SetTransform(D3DTS_WORLD, &m_matworld);
		m_boxmesh->DrawSubset(0);
	}
	else {
		if (m_hitflag == false) {
			lpdev->SetMaterial(&m_material);
			lpdev->SetTexture(0, nullptr);
			lpdev->SetTransform(D3DTS_WORLD, &m_matworld);
			m_boxmesh->DrawSubset(0);
		}
		else {
			DrawRed(lpdev);
		}
	}
}

void COBB::DrawRed(LPDIRECT3DDEVICE9 lpdev)
{
	lpdev->SetMaterial(&m_materialR);
	lpdev->SetTexture(0, nullptr);
	lpdev->SetTransform(D3DTS_WORLD, &m_matworld);
	m_boxmesh->DrawSubset(0);
}

bool COBB::Collision(COBB& obb)
{
	// 無効時は当たらない
	if (!m_isActive) {
		return false;
	}
	if (!obb.m_isActive) {
		return false;
	}

	//分離軸
	D3DXVECTOR3 vecSeparate;

	// 2つのオブジェクトを結んだベクトルを計算
	D3DXVECTOR3 vecDistance;
	D3DXVECTOR3 obbpos = obb.GetPosition();
	vecDistance = m_obbboxinfo.Currentcenter - obbpos;

	OBBBBOX boxB, boxA;
	boxA = this->GetOBB();
	boxB = obb.GetOBB();

	bool sts;			// 戻り値

	// OBB-A軸リスト
	D3DXVECTOR3* OBB_A_Axis[3] = {
		&boxA.CurrentvecAxisX,
		&boxA.CurrentvecAxisY,
		&boxA.CurrentvecAxisZ,
	};

	// OBB-B軸リスト
	D3DXVECTOR3* OBB_B_Axis[3] = {
		&boxB.CurrentvecAxisX,
		&boxB.CurrentvecAxisY,
		&boxB.CurrentvecAxisZ,
	};

	// OBB-Aの３軸を分離軸にしてチェック
	for (int i = 0; i < 3; i++) {
		// 分離軸（boxAのX軸)
		sts = CompareLength(boxA,			// OBB-A
			boxB,							// OBB-B
			*OBB_A_Axis[i],					// 分離軸
			vecDistance);					// ２つのオブジェクトを結ぶベクトル
		if (sts == false) {
			return false;
		}
	}

	// OBB-Bの３軸を分離軸にしてチェック
	for (int i = 0; i < 3; i++) {
		// 分離軸（boxAのX軸)
		sts = CompareLength(boxA,			// OBB-A
			boxB,							// OBB-B
			*OBB_B_Axis[i],					// 分離軸
			vecDistance);					// ２つのオブジェクトを結ぶベクトル
		if (sts == false) {
			return false;
		}
	}

	// 外積を分離軸として計算
	for (int p1 = 0; p1 < 3; p1++) {
		for (int p2 = 0; p2 < 3; p2++) {
			D3DXVECTOR3 crossseparate;					// 外積ベクトル
														// 外積を計算
			D3DXVec3Cross(&crossseparate, OBB_A_Axis[p1], OBB_B_Axis[p2]);

			sts = CompareLength(
				boxA,				// OBB-A
				boxB,				// OBB-B
				crossseparate,		// 分離軸
				vecDistance);		// ２つのオブジェクトを結ぶベクトル
			if (sts == false) {
				return false;
			}
		}
	}
	return true;
}

/*----------------------------
共分散行列を作る
------------------------------*/
D3DXMATRIX CBBoxGenerater::GetCovarianceMatrix(
	D3DXVECTOR3* vertex,	// 頂点データ配列 
	DWORD* index,			// ボーンに影響を受ける頂点インデックスの配列
	int numVertices)		// ボーンに影響を受ける頂点数
{
	float mx = 0, my = 0, mz = 0;
	D3DXMATRIX retmat(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	//ボーンを構成する頂点について平均値を求める
	for (int j = 0; j < numVertices; j++) {
		mx += vertex[index[j]].x;		// Ｘ値合計
		my += vertex[index[j]].y;		// Ｙ値合計
		mz += vertex[index[j]].z;		// Ｚ値合計
	}
	mx /= numVertices;		// Ｘ値平均
	my /= numVertices;		// Ｙ値平均
	mz /= numVertices;		// Ｚ値平均

							//共分散行列の各成分を求める
	for (int j = 0; j < numVertices; j++) {
		retmat._11 += (vertex[index[j]].x - mx) * (vertex[index[j]].x - mx);
		retmat._22 += (vertex[index[j]].y - my) * (vertex[index[j]].y - my);
		retmat._33 += (vertex[index[j]].z - mz) * (vertex[index[j]].z - mz);
		retmat._12 += (vertex[index[j]].x - mx) * (vertex[index[j]].y - my);
		retmat._13 += (vertex[index[j]].x - mx) * (vertex[index[j]].z - mz);
		retmat._23 += (vertex[index[j]].y - my) * (vertex[index[j]].z - mz);
	}
	retmat._11 /= numVertices;
	retmat._22 /= numVertices;
	retmat._33 /= numVertices;
	retmat._12 /= numVertices;
	retmat._13 /= numVertices;
	retmat._23 /= numVertices;
	retmat._21 = retmat._12;
	retmat._31 = retmat._13;
	retmat._32 = retmat._23;

	return retmat;
}

/*----------------------------
固有ベクトルを求める
------------------------------*/
bool CBBoxGenerater::GetEigenVector(D3DXMATRIX& a, D3DXMATRIX& x)
{

	int i, j, k, m, count;
	double amax, amax0, theta, co, si, co2, si2, cosi, pi = 4.0 * atan(1.0);
	double aii, aij, ajj, aik, ajk;
	bool result = false;

	// 単位行列にする
	D3DXMatrixIdentity(&x);

	count = 0;
	while (count <= MAX) {
		//非対角要素の最大値を探索
		amax = 0.0;
		for (k = 0; k < N - 1; k++) {
			for (m = k + 1; m < N; m++) {
				//				amax0 = fabs(*(a.m[N*k+m]));			// おかしい
				float temp = fabs(a.m[k][m]);		// ダブル型としてみていたので修正
				amax0 = temp;
				if (amax0 > amax) {
					i = k;
					j = m;
					amax = amax0;
				}
			}
		}

		//収束判定
		if (amax <= TOL) {
			result = true;
			break;
		}
		else {
			aii = a.m[i][i];
			aij = a.m[i][j];
			ajj = a.m[j][j];
			//回転角度計算
			if (fabs(aii - ajj) < TOL) {
				theta = 0.25 * pi * aij / fabs(aij);
			}
			else {
				theta = 0.5 * atan(2.0 * aij / (aii - ajj));
			}
			co = cos(theta);
			si = sin(theta);
			co2 = co * co;
			si2 = si * si;
			cosi = co * si;

			//相似変換行列
			a.m[i][i] = (float)(co2 * aii + 2.0 * cosi * aij + si2 * ajj);
			a.m[j][j] = (float)(si2 * aii - 2.0 * cosi * aij + co2 * ajj);
			a.m[i][j] = 0.0f;
			a.m[j][i] = 0.0f;

			for (k = 0; k < N; k++) {
				if (k != i && k != j) {
					aik = a.m[k][i];
					ajk = a.m[k][j];
					a.m[k][i] = (float)(co * aik + si * ajk);
					a.m[i][k] = (float)(a.m[k][i]);
					a.m[k][j] = (float)(-si * aik + co * ajk);
					a.m[j][k] = (float)(a.m[k][j]);
				}
			}

			//固有ベクトル
			for (k = 0; k < N; k++) {
				aik = x.m[k][i];
				ajk = x.m[k][j];
				x.m[k][i] = (float)(co * aik + si* ajk);
				x.m[k][j] = (float)(-si * aik + co * ajk);
			}

			count++;
		}
	}
	return result;
}
