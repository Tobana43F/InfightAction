#include	"Collision_Common.h"

#include	"Math\Math_Common.h"
#include	"Math\Math_Util.h"
#include	"Debug\Debug_Message.h"

namespace {
	//!	@brief	面内に交点がある場合の判定処理
	bool	CheckPlaneAndSphereInner(
		const NCollision::SPlaneInfo&	_rPlane,
		const NCollision::SSphere&					_rSphere,
		NMath::SVector3*				_pVecPushBack,
		NMath::SVector3*				_pNearPos);

	//!	@brief	角に交点がある場合の判定処理
	bool	CheckPlaneAndSphereEdge(
		const NCollision::SPlaneInfo&	_rPlane,
		const NCollision::SSphere&					_rSphere,
		NMath::SVector3*				_pVecPushBack,
		NMath::SVector3*				_pNearPos);



	//!	@brief		OBB同士の距離と、OBBを分離軸に投影した長さを比較する
	//!	@param[in]	_rVecSeparateNrom	分離軸ベクトル(正規化されていること)
	//!	@param[in]	_rVecDistance		OBB間のベクトル
	bool	CompareOBBDist(
		const NCollision::SOBBox&	_rOBB1,
		const NCollision::SOBBox&	_rOBB2,
		const NMath::SVector3&		_rVecSeparateNrom,
		const NMath::SVector3&		_rVecDistance);
}

namespace NCollision {

	bool CheckPointAndTriangle(
		const float3&	_rP0,
		const float3&	_rP1,
		const float3&	_rP2,
		const float3&	_rPoint)
	{
		NMath::SVector3  v01, v12, v20;		// ３辺のベクトル
		NMath::SVector3  v0p, v1p, v2p;		// 内部の点とのベクトル
		NMath::SVector3	 normal;			// ３角形の法線ベクトル
		NMath::SVector3	 n0, n1, n2;		// ３辺と内部の点との法線ベクトル

		v01 = _rP1 - _rP0;
		v12 = _rP2 - _rP1;
		v20 = _rP0 - _rP2;

		v0p = _rPoint - _rP0;
		v1p = _rPoint - _rP1;
		v2p = _rPoint - _rP2;

		// 外積を計算
		NMath::Cross(normal, v01, v12);

		NMath::Cross(n0, v01, v0p);
		NMath::Cross(n1, v12, v1p);
		NMath::Cross(n2, v20, v2p);

		if (NMath::Dot(n0, normal) < 0) return false;			// 為す角度が鈍角
		if (NMath::Dot(n1, normal) < 0) return false;			// 為す角度が鈍角
		if (NMath::Dot(n2, normal) < 0) return false;			// 為す角度が鈍角

		return true;
	}

	bool CheckPointAndFlatRectangle(
		const float3&	_rP0,
		const float3&	_rP1,
		const float3&	_rP2,
		const float3&	_rP3,
		const float3&	_rPoint)
	{
		NMath::SVector3  v01, v12, v23, v30;		// 4辺のベクトル
		NMath::SVector3  v0p, v1p, v2p, v3p;		// 内部の点とのベクトル
		NMath::SVector3	 normal;					// 法線ベクトル
		NMath::SVector3	 n0, n1, n2, n3;		// 4辺と内部の点との法線ベクトル

		v01 = _rP1 - _rP0;
		v12 = _rP2 - _rP1;
		v23 = _rP3 - _rP2;
		v30 = _rP0 - _rP3;

		v0p = _rPoint - _rP0;
		v1p = _rPoint - _rP1;
		v2p = _rPoint - _rP2;
		v2p = _rPoint - _rP3;

		// 外積を計算
		NMath::Cross(normal, v01, v12);

		NMath::Cross(n0, v01, v0p);
		NMath::Cross(n1, v12, v1p);
		NMath::Cross(n2, v23, v2p);
		NMath::Cross(n3, v30, v3p);

		if (NMath::Dot(n0, normal) < 0) return false;			// 為す角度が鈍角
		if (NMath::Dot(n1, normal) < 0) return false;			// 為す角度が鈍角
		if (NMath::Dot(n2, normal) < 0) return false;			// 為す角度が鈍角
		if (NMath::Dot(n3, normal) < 0) return false;			// 為す角度が鈍角

		return true;
	}



	bool	CheckLineAndSphere(
		const SSphere&					_rSphere,
		const float3&					_rLineStart,
		const NMath::SVector3&			_rDir,
		float3*							_pDestCrossNear,
		float3*							_pDestCrossFar)
	{
		// 球の位置を原点とした座標に変換
		float3		lineStart = _rLineStart - _rSphere.centerPos;

		// 方程式を解く
		float a = NMath::Dot(_rDir, _rDir);
		float b = 2 * NMath::Dot(lineStart, _rDir);
		float c = NMath::Dot(lineStart, lineStart) - (_rSphere.r * _rSphere.r);

		// 判別式
		float d = (b * b) - (4.0f * a * c);

		if (d < 0) {		// 交点なし
			return false;
		}
		else if (d > 0) {	// 交点2つ
			float sqrtD = sqrt(d);

			float	t1 = (-b + sqrtD) / (2.0f * a);
			float	t2 = (-b - sqrtD) / (2.0f * a);

			int		colCount = 0;

			if (t1 >= 0) {
				float3 ans = lineStart + _rDir * t1 + _rSphere.centerPos;

				if (_pDestCrossNear != nullptr) {
					*_pDestCrossNear = ans;
				}
				if (_pDestCrossFar != nullptr) {
					*_pDestCrossFar = ans;
				}
				colCount++;
			}
			if (t2 >= 0) {
				float3 ans = lineStart + _rDir * t2 + _rSphere.centerPos;

				if (colCount == 0) {
					if (_pDestCrossNear != nullptr) {
						*_pDestCrossNear = ans;
					}
					if (_pDestCrossFar != nullptr) {
						*_pDestCrossFar = ans;
					}
				}
				else {
					if (_pDestCrossFar != nullptr) {
						*_pDestCrossFar = ans;
					}
				}
				colCount++;
			}

			if (colCount == 0) {
				return false;
			}
			else {
				return true;
			}
		}
		else {				// 交点1つ
			float t = -b;

			if (t < 0) {
				return false;
			}

			float3 ans = lineStart + _rDir * t + _rSphere.centerPos;
			if (_pDestCrossNear != nullptr) {
				*_pDestCrossNear = ans;
			}
			if (_pDestCrossFar != nullptr) {
				*_pDestCrossFar = ans;
			}
			return true;
		}
	}



	bool	CheckPlaneAndSphere(
		const SPlaneInfo&	_rPlane,
		const SSphere&					_rSphere,
		NMath::SVector3*				_pVecPushBack,
		NMath::SVector3*				_pNearPos,
		bool*							_pIsCollideEdge)
	{
		bool isCol = false;

		// 球の中心点が面の内側にある場合
		isCol = CheckPlaneAndSphereInner(
			_rPlane,
			_rSphere,
			_pVecPushBack,
			_pNearPos);

		// 衝突していれば終了
		if (isCol) {
			if (_pIsCollideEdge != nullptr) {
				*_pIsCollideEdge = false;
			}
			return true;
		}

		// 角の判定
		isCol = CheckPlaneAndSphereEdge(
			_rPlane,
			_rSphere,
			_pVecPushBack,
			_pNearPos);

		if (isCol) {
			if (_pIsCollideEdge != nullptr) {
				*_pIsCollideEdge = true;
			}
			return true;
		}

		return false;
	}



	bool	CheckSphereAndMeshWall(
		const SSphere&					_rSphere,
		const CMesh&					_rMeshWall,
		NMath::SVector3&				_rDestPushBack)
	{
		NMath::SVector3	nearPosFinal;
		NMath::SVector3	pushBackFinal;
		NMath::SVector3	vecSphereToNearPos;
		float			sphereToNearLengthSq = 0;

		int					colCount = 0;
		std::array<int, 32>	hitWallIndex;		// 衝突の可能性がある壁のインデックスを取得する
		bool				isColAny = false;	// どれかにあたったフラグ

												// デバッグ用
		int					pushBackCount = 0;	// 押し戻した回数



		// 衝突の可能性がある壁をすべて取得する
		for (uint i = 0; i < _rMeshWall.GetPlaneInfo().size(); ++i) {
			const NCollision::SPlaneInfo& plane = _rMeshWall.GetPlaneInfo()[i];

			// 一度目の衝突を解消するための値を取得
			float3	pushBack;
			float3	nearPos;

			bool isCol = false;

			isCol = NCollision::CheckPlaneAndSphere(
				plane,
				_rSphere,
				&pushBack,
				&nearPos);

			if (isCol && !isColAny) {
				// 初期値を代入
				nearPosFinal = nearPos;
				pushBackFinal = pushBack;
				vecSphereToNearPos = nearPos - _rSphere.centerPos;
				sphereToNearLengthSq = vecSphereToNearPos.GetLengthSquare();

				isColAny = true;

				hitWallIndex[colCount] = i;
				colCount++;
			}
			if (isCol) {
				NMath::SVector3	vec = nearPos - _rSphere.centerPos;
				float			lenSq = vec.GetLengthSquare();

				// より球に近いほうの壁を最初に解決する
				if (lenSq < sphereToNearLengthSq) {
					nearPosFinal = nearPos;
					pushBackFinal = pushBack;
					vecSphereToNearPos = vec;
					sphereToNearLengthSq = lenSq;

					// 近いインデックスが先に来るようにソート
					int tmp = hitWallIndex[colCount - 1];
					hitWallIndex[colCount - 1] = i;
					hitWallIndex[colCount] = tmp;

					colCount++;
				}
				else {
					hitWallIndex[colCount] = i;
					colCount++;
				}
			}
		}

		// 衝突しない場合は無視
		if (!isColAny) {
			return false;
		}

		// 最短点の衝突を解決する
		// 押し戻す処理
		float3 pos = _rSphere.centerPos;
		pos += pushBackFinal;

		pushBackCount++;

		// その他の解決を行う(0は解決されている状態)
		// ループごとに解決処理を行う
		for (int i = 1; i < colCount; ++i) {
			int		index = hitWallIndex[i];
			const NCollision::SPlaneInfo& plane = _rMeshWall.GetPlaneInfo()[index];

			float3	pushBack;
			float3	nearPos;

			bool isCol = false;
			isCol = NCollision::CheckPlaneAndSphere(
				plane,
				_rSphere,
				&pushBack,
				&nearPos);

			// 衝突しなければ無視
			if (isCol) {
				// 押し戻す処理
				pos += pushBack;
				pushBackCount++;
			}
		}

		// 出力に代入
		_rDestPushBack = pushBackFinal;

		return true;
	}



	bool	CheckOBBAndOBB(
		const SOBBox&	_rOBB1,
		const SOBBox&	_rOBB2)
	{
		// 汎用変数
		bool	sts = false;

		// 2つのオブジェクトを結んだベクトルを計算
		NMath::SVector3	vecDist = _rOBB1.centerPos - _rOBB2.centerPos;

		// OBB1の3軸を分離軸にして衝突確認
		for (int i = 0; i < 3; ++i) {
			sts = CompareOBBDist(
				_rOBB1,
				_rOBB2,
				_rOBB1.axisDir.GetRow(i).vec3,
				vecDist);

			if (!sts) {
				return false;
			}
		}

		// OBB2の3軸を分離軸にして衝突確認
		for (int i = 0; i < 3; ++i) {
			sts = CompareOBBDist(
				_rOBB1,
				_rOBB2,
				_rOBB2.axisDir.GetRow(i).vec3,
				vecDist);

			if (!sts) {
				return false;
			}
		}

		// 外積を分離軸にして計算
		for (int idxOBB1 = 0; idxOBB1 < 3; ++idxOBB1) {
			for (int idxOBB2 = 0; idxOBB2 < 3; ++idxOBB2) {

				// 外積を計算
				NMath::SVector3	vecCross;
				NMath::Cross(
					vecCross,
					_rOBB1.axisDir.GetRow(idxOBB1).vec3,
					_rOBB2.axisDir.GetRow(idxOBB2).vec3);

				sts = CompareOBBDist(
					_rOBB1,
					_rOBB2,
					vecCross,
					vecDist);

				if (!sts) {
					return false;
				}
			}
		}

		return true;
	}
}

namespace {
	bool	CheckPlaneAndSphereInner(
		const NCollision::SPlaneInfo&	_rPlane,
		const NCollision::SSphere&		_rSphere,
		NMath::SVector3*				_pVecPushBack,
		NMath::SVector3*				_pNearPos)
	{
		bool	isCross = false;		// 線の交差判断用フラグ
		float3	crossPos;

		NMath::SVector3	vecRayDir = _rPlane.normal * -1.0f;

		// 球の中心から面の-法線方向にレイを飛ばし、内外判定
		isCross = NMath::CalcLineAndPlaneCross(
			_rPlane.plane,
			_rSphere.centerPos,
			vecRayDir,
			crossPos);

		if (!isCross) {
			return false;
		}

		// 交点が平面の内側にあるかを判定
		bool isInner = NCollision::CheckPointAndTriangle(
			_rPlane.p0,
			_rPlane.p1,
			_rPlane.p2,
			crossPos);

		// 外側の場合は無視
		if (!isInner) {
			return false;
		}

		// 球の中心点と面間の最短距離を取得
		float planeSphereLen = NMath::CalcPointAndPlaneLength(
			_rSphere.centerPos,
			_rPlane);

		// 最短距離が球の半径より短い場合は衝突
		if (!(planeSphereLen < _rSphere.r)) {
			// 衝突していない場合は無視
			return false;
		}

		// 戻しベクトルを設定
		if (_pVecPushBack != nullptr) {
			NMath::SVector3 pushDir = _rPlane.normal;					// 戻し方向
			NMath::SVector3	vecMin = _rSphere.centerPos - crossPos;	// 交点から球へのベクトル

																	// めり込み量から補正の仕方を変える
			float dot = NMath::Dot(vecMin, _rPlane.normal);
			if (dot > 0) {
				*_pVecPushBack = pushDir * (_rSphere.r - vecMin.GetLength());
			}
			else if (dot < 0) {
				*_pVecPushBack = pushDir * (_rSphere.r + vecMin.GetLength());
			}
			else {
				*_pVecPushBack = pushDir * (_rSphere.r);
			}
		}

		// 最短距離の座標を代入
		if (_pNearPos != nullptr) {
			*_pNearPos = crossPos;
		}

		return true;
	}

	bool	CheckPlaneAndSphereEdge(
		const NCollision::SPlaneInfo&	_rPlane,
		const NCollision::SSphere&		_rSphere,
		NMath::SVector3*				_pVecPushBack,
		NMath::SVector3*				_pNearPos)
	{
		bool	isCross = false;		// 線の交差判断用フラグ
		float3	crossPos;

		NMath::SVector3	vecRayDir = _rPlane.normal * -1.0f;

		// 球の中心から面の-法線方向にレイを飛ばし、内外判定
		isCross = NMath::CalcLineAndPlaneCross(
			_rPlane.plane,
			_rSphere.centerPos,
			vecRayDir,
			crossPos);

		if (!isCross) {
			return false;
		}

		// 球の中心から各辺の線分の最短距離までの長さが半径より小さい場合は衝突の可能性がある
		bool			isMayCol = false;	// 衝突の可能性があるフラグ
		float			len01;
		len01 = NMath::CalcPointAndLineLength(
			_rSphere.centerPos,
			_rPlane.p0,
			_rPlane.p1);
		if (len01 < _rSphere.r) {
			isMayCol |= true;
		}
		if (!isMayCol) {
			float			len12;
			len12 = NMath::CalcPointAndLineLength(
				_rSphere.centerPos,
				_rPlane.p1,
				_rPlane.p2);
			if (len12 < _rSphere.r) {
				isMayCol |= true;
			}
		}
		if (!isMayCol) {
			float			len20;
			len20 = NMath::CalcPointAndLineLength(
				_rSphere.centerPos,
				_rPlane.p2,
				_rPlane.p0);
			if (len20 < _rSphere.r) {
				isMayCol |= true;
			}
		}

		if (!isMayCol) {
			return false;
		}

		// 交差点が、各頂点を円にした円ないかを判定
		float	radiusSq = _rSphere.r * _rSphere.r;
		bool	isInner = false;
		float3	vecCrossToP0 = _rPlane.p0 - crossPos;
		if (vecCrossToP0.GetLengthSquare() < radiusSq) {
			isInner = true;
		}
		if (!isInner) {
			float3 vecCrossToP1 = _rPlane.p1 - crossPos;
			if (vecCrossToP1.GetLengthSquare() < radiusSq) {
				isInner = true;
			}
		}
		if (!isInner) {
			float3 vecCrossToP2 = _rPlane.p2 - crossPos;
			if (vecCrossToP2.GetLengthSquare() < radiusSq) {
				isInner = true;
			}
		}
		// 交点が、各辺を半径分拡張した長方形に内包されるかを判断
		if (!isInner) {
			// 0 1
			NMath::SVector3	dir = _rPlane.vecPTo01Vert;
			dir.Normalize();

			float3 addPos0 = _rPlane.p0 + dir * _rSphere.r;
			float3 addPos1 = _rPlane.p1 + dir * _rSphere.r;

			isInner = NCollision::CheckPointAndFlatRectangle(
				addPos0,
				addPos1,
				_rPlane.p1,
				_rPlane.p0,
				crossPos);
		}
		if (!isInner) {
			// 1 2
			NMath::SVector3	dir = _rPlane.vecPTo12Vert;
			dir.Normalize();

			float3 addPos1 = _rPlane.p1 + dir * _rSphere.r;
			float3 addPos2 = _rPlane.p2 + dir * _rSphere.r;

			isInner = NCollision::CheckPointAndFlatRectangle(
				addPos1,
				addPos2,
				_rPlane.p2,
				_rPlane.p1,
				crossPos);
		}
		if (!isInner) {
			// 2 0
			NMath::SVector3	dir = _rPlane.vecPTo20Vert;
			dir.Normalize();

			float3 addPos2 = _rPlane.p2 + dir * _rSphere.r;
			float3 addPos0 = _rPlane.p0 + dir * _rSphere.r;

			isInner = NCollision::CheckPointAndFlatRectangle(
				addPos2,
				addPos0,
				_rPlane.p0,
				_rPlane.p2,
				crossPos);
		}

		if (!isInner) {
			return false;
		}

		// 押し戻しベクトルを設定
		if (_pVecPushBack != nullptr) {
			float3	nearPoint01;
			float3	nearPoint12;
			float3	nearPoint20;

			float	lenSphereToVec01;
			float	lenSphereToVec12;
			float	lenSphereToVec20;

			// それぞれのベクトルへの最短距離を取得する
			lenSphereToVec01 = NMath::CalcPointAndLineLength(
				_rSphere.centerPos,
				_rPlane.p0,
				_rPlane.p1,
				&nearPoint01,
				true);
			//RenderPoint(nearPoint01, 0.01f, 0.25f);

			lenSphereToVec12 = NMath::CalcPointAndLineLength(
				_rSphere.centerPos,
				_rPlane.p1,
				_rPlane.p2,
				&nearPoint12,
				true);
			//RenderPoint(nearPoint12, 0.01f, 0.25f);

			lenSphereToVec20 = NMath::CalcPointAndLineLength(
				_rSphere.centerPos,
				_rPlane.p2,
				_rPlane.p0,
				&nearPoint20,
				true);
			//RenderPoint(nearPoint20, 0.01f, 0.25f);

			// 取得した最短距離のうち最も距離が短いものを取得
			float3	nearPoint = nearPoint01;
			float	nearLen = lenSphereToVec01;
			if (lenSphereToVec12 < nearLen) {
				nearLen = lenSphereToVec12;
				nearPoint = nearPoint12;
			}
			if (lenSphereToVec20 < nearLen) {
				nearLen = lenSphereToVec20;
				nearPoint = nearPoint20;
			}
			//RenderPoint(nearPoint, 0.05f, 1.0f);

			// 球の中心点から最短距離となる線分上の点へのベクトルを取得する
			NMath::SVector3	vecSphereToNear = nearPoint - _rSphere.centerPos;
			NMath::SVector3	vecSphereToNearNorm = vecSphereToNear;
			vecSphereToNearNorm.Normalize();
			NMath::SVector3	pushDir = _rPlane.normal;
			pushDir.Normalize();
			//NMesh::RenderLine(_rSphere.centerPos, nearPoint);

			// 内積でめり込み距離を求めるための値を取得
			float dotLen = NMath::Dot(vecSphereToNear, pushDir) * -1.0f;

			// 最短点から面に平行する軸に足を下した点を計算
			float3	posProjYDir = _rPlane.normal * dotLen;
			float3	posProjY = nearPoint + posProjYDir;
			//NMesh::SetLineColor(float4(1, 1, 0, 1));
			//NMesh::RenderLine(nearPoint, posProjY);

			//NMesh::SetLineColor(float4(1, 0, 1, 1));
			//NMesh::RenderLine(_rSphere.centerPos, posProjY);

			// 球の軸上の点から最短点へレイを飛ばし交点を取得する
			float3				rayStart = posProjY;
			NMath::SVector3		rayDir = _rPlane.normal;

			bool				isCol = false;
			float3				crossSphere1;	// 1つのみ交点を持つ
			float3				crossSphere2;
			isCol = CheckLineAndSphere(
				_rSphere,
				rayStart,
				rayDir,
				&crossSphere1,
				&crossSphere2);

			// 衝突時
			if (isCol) {
				//RenderPoint(crossSphere1);
				//RenderPoint(crossSphere2);

				// 押し戻しベクトルを計算
				NMath::SVector3	vecProj = posProjYDir * -1;			// 投影点から最短点へ向かうベクトル
				NMath::SVector3	vecR = crossSphere1 - rayStart;	// 投影点から球面上の点へ向かうベクトル

				NMath::SVector3	pushBack = vecR + vecProj;

				//NMesh::SetLineColor(float4(0, 1, 0, 1));
				//NMesh::RenderLine(posProjY, posProjY + pushBack);

				*_pVecPushBack = pushBack;
			}

			// 最短距離への点を設定
			if (_pNearPos != nullptr) {
				*_pNearPos = nearPoint;
			}
		}
		return true;
	}



	bool	CompareOBBDist(
		const NCollision::SOBBox&	_rOBB1,
		const NCollision::SOBBox&	_rOBB2,
		const NMath::SVector3&		_rVecSeparateNrom,
		const NMath::SVector3&		_rVecDistance)
	{
		// 分離軸に射影したベクトルの長さを取得
		float	dist = abs(NMath::Dot(_rVecSeparateNrom, _rVecDistance));

		// 分離軸上にOBBを射影した長さを取得
		float	distOBB1 = 
			abs(NMath::Dot(_rVecSeparateNrom, _rOBB1.axisDir.GetRow(0).vec3 * (_rOBB1.size.x * 0.5f))) +
			abs(NMath::Dot(_rVecSeparateNrom, _rOBB1.axisDir.GetRow(1).vec3 * (_rOBB1.size.y * 0.5f))) +
			abs(NMath::Dot(_rVecSeparateNrom, _rOBB1.axisDir.GetRow(2).vec3 * (_rOBB1.size.z * 0.5f)));

		float	distOBB2 =
			abs(NMath::Dot(_rVecSeparateNrom, _rOBB2.axisDir.GetRow(0).vec3 * (_rOBB2.size.x * 0.5f))) +
			abs(NMath::Dot(_rVecSeparateNrom, _rOBB2.axisDir.GetRow(1).vec3 * (_rOBB2.size.y * 0.5f))) +
			abs(NMath::Dot(_rVecSeparateNrom, _rOBB2.axisDir.GetRow(2).vec3 * (_rOBB2.size.z * 0.5f)));

		if (dist > distOBB1 + distOBB2) {
			return false;
		}
		return true;
	}
}