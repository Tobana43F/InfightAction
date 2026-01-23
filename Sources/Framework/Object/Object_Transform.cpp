#include	"Object_Transform.h"

#include	"Debug\Debug_Assert.h"
#include	"Math\Math_Common.h"

namespace NObject {

	CTransform::CTransform(CGameObject&	_rOwner) :
		m_pOwner(&_rOwner),
		m_pParent(nullptr),
		m_scale(1, 1, 1)
	{

	}

	CTransform::~CTransform()
	{

	}

	void CTransform::SetParent(CTransform* _pParent)
	{
		// 登録操作
		if (_pParent != nullptr) {
			// すでに同じ親が登録されている場合は何もしない
			if (_pParent == GetParent()) {
				return;
			}

			// 親を登録
			m_pParent = _pParent;

			// 親に登録
			_pParent->SetChild(this);

			//!	@todo	ここで、親子関係を設定した時の処理を行う
			// 親から見た相対位置に変換
			NMath::SMatrix44 matWorld;
			_pParent->CalcWorldMatrix(matWorld);

			NMath::SMatrix44 matInv;
			matWorld.GetTranspose(matInv);
			NMath::SMatrix44 matIdentity;
			m_matLocal = matInv * matIdentity;
		}
		// 解除操作
		else {
			// もともと登録されていなければ何もしない
			if (m_pParent == nullptr) {
				return;
			}

			// 親からの相対座標をワールド座標に変換
			NMath::SMatrix44 mat;
			m_pParent->CalcWorldMatrix(mat);
			m_matLocal *= mat;

			// 親から登録を解除
			m_pParent->UnsetChild(this);

			// 親にnullptrを設定
			m_pParent = nullptr;
		}
	}

	void CTransform::SetRot(const NMath::SMatrix44& _rMatRot)
	{
		for (unsigned int i = 0; i < 3; ++i) {
			m_matLocal.vec[i].vec3 = _rMatRot.vec[i].vec3;
		}
	}

	void CTransform::SetRot(const float _degZ)
	{
		NMath::SMatrix44	matRot;
		NMath::MakeMatrixRotationZ(matRot, _degZ);
		SetRot(matRot);
	}

	void CTransform::Rotate(const float3& _deg)
	{
		NMath::SMatrix44	mat;
		NMath::MakeMatrixRotation(mat, _deg.x, _deg.y, _deg.z);
		m_matLocal = mat * m_matLocal;
	}

	void CTransform::GetPos(float3& _rDest) const
	{
		NMath::SMatrix44	mat;
		CalcWorldMatrix(mat);
		_rDest = mat.GetRow(3).vec3;
	}

	void CTransform::GetScaledLocalMatrix(NMath::SMatrix44& _rDest) const
	{
		NMath::SMatrix44	matScale;
		NMath::MakeMatrixScaling(matScale, m_scale);

		_rDest = matScale * m_matLocal;
	}

	void CTransform::CalcWorldMatrix(NMath::SMatrix44& _rDest) const
	{
		NMath::SMatrix44	matWorld;
		GetScaledLocalMatrix(matWorld);
		CTransform*			pParent = m_pParent;

		while (pParent != nullptr) {
			NMath::SMatrix44	matParentWorld;
			pParent->GetScaledLocalMatrix(matParentWorld);

			// 親の行列を合成
			matWorld = matWorld * matParentWorld;

			// 親をたどる
			pParent = pParent->m_pParent;
		}
		_rDest = matWorld;
	}

	void CTransform::CalcWorldMatrixNoScale(NMath::SMatrix44& _rDest) const
	{
		NMath::SMatrix44	matWorld = m_matLocal;
		CTransform*			pParent = m_pParent;

		while (pParent != nullptr) {
			// 親の行列を合成
			matWorld = matWorld * pParent->m_matLocal;

			// 親をたどる
			pParent = pParent->m_pParent;
		}
		_rDest = matWorld;
	}

	//----------------------------------------------------------

	void CTransform::SetChild(CTransform* _pChild)
	{
		AssertNotNull(_pChild);
		m_pChildArray.push_back(_pChild);
	}

	void CTransform::UnsetChild(CTransform* _pChild)
	{
		bool isFind = false;
		for (uint i = 0; i < m_pChildArray.size(); ++i) {
			CTransform*	pTransform = m_pChildArray[i];

			// 探索成功時
			if (pTransform == _pChild) {
				// コンテナから削除
				m_pChildArray.erase(m_pChildArray.begin() + i);
				isFind = true;
				break;
			}
		}
		Assert(isFind);

		// 親を解除する
		_pChild->m_pParent = nullptr;
	}

}