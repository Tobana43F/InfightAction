#include	"Object_Actor.h"

#include	"Debug\Debug_Assert.h"
#include	"Math\Math_Common.h"

namespace {
	//!	@brief	被攻撃判定リストのうち、重複している情報を排除する
	//!	@note	IDが一致した場合重複と判断する
	//!	@note	_rDestには
	//!			有効状態の現在のフレームから
	//!			有効状態の過去フレームの
	//!			重複が排除された情報が取得される
	void CreateExcludeReceiveAttackInfo(
		NObject::CActor::ReceiveAttackInfos&	_rDest,
		const NObject::CActor::ReceiveAttackInfos&	_rCur,
		const NObject::CActor::ReceiveAttackInfos&	_rPrev)
	{
		// _rDestに追加されたデータ量
		uint	destDataCount = 0;

		// 総当たりを行い同じIDを探索
		uint count = _rCur.size();		// 全ての引数のサイズは等しい
		for (uint iCur = 0; iCur < count; ++iCur) {
			// 有効でない場合は無視
			if (!_rCur[iCur].GetIsEnable()) {
				continue;
			}

			bool isSameExist = false;	// 同じ判定が過去に存在するフラグ

			for (uint iPrev = 0; iPrev < count; ++iPrev) {
				// 有効状態でない場合は無視
				if (!_rPrev[iPrev].GetIsEnable()) {
					continue;
				}

				// IDが等しいかをチェック
				isSameExist = _rCur[iCur].CheckIsSameID(_rPrev[iPrev]);

				// 同じものを発見したら終了
				if (isSameExist) {
					break;
				}
			}

			// 等しいものが存在しない場合はトリガーなので、データを取得
			if (!isSameExist) {
				// 現在の値を新しい情報として取得
				_rDest[destDataCount] = _rCur[iCur];

				// 次の要素へアクセスするため添え字を加算
				destDataCount++;
			}
		}
	}

}

namespace NObject {

	CActor::CActor() : 
		m_curReceiveAttackInfoIndex(0),
		m_actorMove(*this)
	{

	}

	CActor::~CActor()
	{

	}

	//----------------------------------------------------------
	void CActor::AddReceiveAttackInfo(NCollision::CReceiveAttackInfo& _rSrc)
	{
		if (m_curReceiveAttackInfoIndex >= RECEIVE_ATTACK_INFO_MAX) {
			OutWarningMessageEx("1フレームに受けた攻撃数ががバッファの上限を超えました。\n配列のサイズを大きくするなどの対策をしてください。");
			return;
		}
		
		// 現在のフレーム情報として追加
		m_receiveAttackInfoBufs[0][m_curReceiveAttackInfoIndex] = _rSrc;

		// 次の添え字へ進む
		m_curReceiveAttackInfoIndex++;
	}

	//----------------------------------------------------------
	void CActor::LoadProc()
	{
		CGameObject::LoadProc();

		// ActorParamのデータを読み込む
		m_actorParam.Setup(GetActorParamAssetKey());

		// ActorMoveのデータを読み込む
		m_actorMove.Setup(
			GetActorMoveAssetKey(),
			GetStageGroundIndex(),
			GetStageWallIndex());
	}

	void CActor::ReleaseProc()
	{
		CGameObject::ReleaseProc();
	}

	void CActor::InitProc()
	{
		CGameObject::InitProc();

		m_actorParam.Init();
		m_actorMove.Init();

		// あたり判定情報を初期化
		for (uint i = 0; i < RECEIVE_ATTACK_INFO_BUF_SIZE; ++i) {
			for (uint j = 0; j < RECEIVE_ATTACK_INFO_MAX; ++j) {
				m_receiveAttackInfoBufs[i][j].Init();
			}
		}
		for (uint j = 0; j < RECEIVE_ATTACK_INFO_MAX; ++j) {
			m_resultReceiveAttackInfos[j].Init();
		}
		m_resultDamage.Init();

	}

	void CActor::DestroyProc()
	{
		CGameObject::DestroyProc();
	}

	void CActor::UpdateFastProc()
	{
		CGameObject::UpdateFastProc();

		m_actorMove.UpdateFast();
	}

	void CActor::UpdateProc()
	{
		CGameObject::UpdateProc();

		// 現在受けている攻撃情報をもとに処理を行う
		ProcReceiveAttackInfo();

		// 受けた当たり判定情報を初期化
		InitReceiveAttackInfo();

		// 操作関数
		Operate();

		// 移動量を適用
		m_actorMove.Update();
	}

	void CActor::UpdateLateProc()
	{
		CGameObject::UpdateLateProc();
	}

	void CActor::RenderProc()
	{
		CGameObject::RenderProc();
	}

	void CActor::RenderLateProc()
	{
		CGameObject::RenderLateProc();
		m_actorMove.RenderDebug();
	}

	//----------------------------------------------------------

	const NCollision::CReceiveAttackInfo&	CActor::GetCurReceiveAttackInfo(
		uint _idx) const
	{
		Assert(_idx < RECEIVE_ATTACK_INFO_MAX);
		return m_receiveAttackInfoBufs[0][_idx];
	}

	const NCollision::CReceiveAttackInfo&	CActor::GetPrevReceiveAttackInfo(
		uint	_prevFrame,
		uint	_infoIdx) const
	{
		Assert(_prevFrame < RECEIVE_ATTACK_INFO_BUF_SIZE);
		Assert(_infoIdx < RECEIVE_ATTACK_INFO_MAX);
		return m_receiveAttackInfoBufs[_prevFrame][_infoIdx];
	}

	void CActor::ProcDamage(const NGame::SDamageParam& _rDamage)
	{
	
	}


	//!	@brief	攻撃を受けた際のActorMoveの設定を適用
	void CActor::SetDamageActorMoveParam()
	{
		// ノックバック処理を適用するための設定
		m_actorMove.SetIsApplyRotateMoveDir(false);
		m_actorMove.SetApplyType(NGame::CActorMove::EApplyType::DIRECT);
	}

	//!	@brief	通常時のActorMoveの設定を適用
	void CActor::SetNormalActorMoveParam()
	{
		// 通常の移動をする設定
		m_actorMove.SetIsApplyRotateMoveDir(true);
		m_actorMove.SetApplyType(NGame::CActorMove::EApplyType:: FORWARDING);
	}

	//!	@brief	ダメージを受けた際のノックバックをActorMoveに適用する
	void CActor::ApplyKnockBack(const NGame::SDamageParam& _rDamage)
	{
		SetDamageActorMoveParam();

		m_actorMove.SetMove(_rDamage.knockBackDir, _rDamage.knockBackPow);
	}

	//!	@brief	ダメージを受けた際に攻撃者の方向を向く(補完なし)
	void CActor::LookAtAttacker(const NGame::SDamageParam& _rDamage)
	{
		float3 myPos;
		GetTransform().GetPos(myPos);
		float3 otherPos = _rDamage.ownerPos;

		const float3 mask(1, 0, 1);
		float3 myPosXZ = myPos * mask;
		float3 otherPosXZ = otherPos * mask;
		float3 vecMyXZToOtherXZ = otherPosXZ - myPosXZ;

		if (vecMyXZToOtherXZ.GetLengthSquare() > FLT_EPSILON) {
			NMath::SMatrix44 posture;
			NMath::MakeMatrixLookAt(
				posture,
				myPosXZ,
				otherPosXZ,
				float3(0, 1, 0));

			// 姿勢部分のみ使用する(座標は元の値を設定)
			posture.vec[3].vec3 = myPos;
			GetTransform().SetLocalMatrix(posture);
		}
	}

	//----------------------------------------------------------
	void CActor::InitReceiveAttackInfo()
	{
		// バッファを更新
		for (uint iBuf = RECEIVE_ATTACK_INFO_BUF_SIZE - 1; iBuf > 0; --iBuf) {
			m_receiveAttackInfoBufs[iBuf] = m_receiveAttackInfoBufs[iBuf - 1];
		}

		// 現在の当たり判定情報配列をすべて初期化
		for (uint iInfo = 0; iInfo < RECEIVE_ATTACK_INFO_MAX; ++iInfo) {
			m_receiveAttackInfoBufs[0][iInfo].Init();
		}

		// 現在受けた攻撃情報の数を初期化
		m_curReceiveAttackInfoIndex = 0;
	}


	void CActor::ProcReceiveAttackInfo()
	{
		InitResultReceiveAttackInfo();
		MakeResultReceiveAttackInfo();
		ProcDamageCommon(m_resultDamage);
	}

	void CActor::InitResultReceiveAttackInfo()
	{
		// 実際に反映させる情報を初期化する
		m_resultDamage.Init();

		// 現フレームの重複を排除した当たり判定情報を全て初期化
		for (uint i = 0; i < RECEIVE_ATTACK_INFO_MAX; ++i) {
			m_resultReceiveAttackInfos[i].Init();
		}
	}

	void CActor::MakeResultReceiveAttackInfo()
	{
		// 現在フレームから1フレーム前の情報を排除したデータを作成する
		CreateExcludeReceiveAttackInfo(
			m_resultReceiveAttackInfos,
			m_receiveAttackInfoBufs[0],
			m_receiveAttackInfoBufs[1]);

		// 最終的なダメージ量を取得する
		int curPriority = INT_MIN;	// 最小値で初期化
		for (uint i = 0; i < RECEIVE_ATTACK_INFO_MAX; ++i) {
			// 有効状態でなければ無視
			if (!m_resultReceiveAttackInfos[i].GetIsEnable()) {
				continue;
			}

			// 攻撃を受けたことを設定
			m_resultDamage.isReciveDamage = true;

			// アクセス省略
			const auto& receiveAttackInfo = m_resultReceiveAttackInfos[i];
			const auto& receiveAttackData = m_resultReceiveAttackInfos[i].GetReceiveAttackData();

			//----------------------------------------------------------
			// 優先度による判断
			//----------------------------------------------------------
			auto priority = receiveAttackData.GetPriority();
			// 優先度が高い場合はそのデータを使用する
			if (priority > curPriority) {
				m_resultDamage.ownerPos = receiveAttackInfo.GetAttackOwnerPos();
				m_resultDamage.collidePos = receiveAttackInfo.GetCollidePos();

				m_resultDamage.damage = receiveAttackData.GetBaseDamage();
				m_resultDamage.knockBackDir = receiveAttackInfo.GetKnockBackDir();
				m_resultDamage.knockBackPow = receiveAttackData.GetKnockBackPow();
				m_resultDamage.receiveAttackStrengthType = receiveAttackData.GetStrengthType();

				// 優先度を更新
				curPriority = priority;
			}
			// 優先度が等しい場合
			else if (curPriority == curPriority) {
				// 攻撃力の高いほうを取得
				auto damage = receiveAttackData.GetBaseDamage();
				if (damage > m_resultDamage.damage) {
					m_resultDamage.damage = damage;

					m_resultDamage.ownerPos = receiveAttackInfo.GetAttackOwnerPos();
					m_resultDamage.collidePos = receiveAttackInfo.GetCollidePos();
					m_resultDamage.knockBackDir = receiveAttackInfo.GetKnockBackDir();
					m_resultDamage.knockBackPow = receiveAttackData.GetKnockBackPow();
					m_resultDamage.receiveAttackStrengthType = receiveAttackData.GetStrengthType();

					// 優先度を更新
					curPriority = priority;
				}
			}
		}
	}

	void CActor::ProcDamageCommon(const NGame::SDamageParam& _rDamage)
	{
		// 攻撃を受けていない場合は無視
		if (!_rDamage.isReciveDamage) {
			return;
		}

		// 体力の減少処理
		m_actorParam.life -= _rDamage.damage;

		// 死亡時の状態遷移
		if (!m_actorParam.GetIsAlive()) {
			m_actorParam.life = 0;	// 値を補正
		}

		// 継承後の処理を実行
		ProcDamage(_rDamage);
	}
}
