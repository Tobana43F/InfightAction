#pragma once

#include	<array>
#include	"Asset\Factory\Asset_JsonFactory.h"
#include	"Collision\Collision_Mesh.h"
#include	"Collision\Collision_Def.h"
#include	"Object\Object_GameObject.h"
#include	"Collision\Manager\Collision_Manager.h"

namespace NGame {

	//!	@brief	Actorクラスの移動を管理するクラス
	//!	@bug	軽微なバグ
	//!			- 当たり判定の描画が1フレームずれて、初期化後に原点に描画されてしまう。
	//!			　実際にあたり判定がとられているかは要検証
	class CActorMove {
	public:

		//----------------------------------------------------------
		// 定数
		//----------------------------------------------------------

		enum class EApplyType {
			FORWARDING,	// 前方を向くように進行する(回転と移動が連動する)(移動などの動作に使用)
			DIRECT,		// MoveDir方向を向き移動方向に移動(回転と移動が別々に動作する)(ノックバックなどの動作に使用)
		};

		//----------------------------------------------------------
		// 内部クラス
		//----------------------------------------------------------

		//!	@brief	Actorの体の当たり判定
		//!	@note	壁との判定や、ほかのActorとの当たり判定に使用する
		//!	@todo	複数の当たり判定に対応させる(内部で複数管理するようにする)
		class CActorBody {
		public:
			//!	@brief	当たり判定の最大数
			static const uint BODY_COLLISION_MAX = 10;

			//!	@brief	値の設定
			void Setup(json11::Json& _json);

			//!	@brief	判定領域のデバッグ描画
			void RenderDebug(const float3& _worldPos);


			//!	@brief	当たり判定のワールド座標を取得する
			void GetWorldCol(
				NCollision::SSphere&	_dest,
				const float3&			_worldPos);

		private:

			float				m_mass;		// 重さ	
			NCollision::SSphere	m_colBodys;	// 体の当たり判定

			// デバッグ用
			LPD3DXMESH			m_sphereMesh;
			D3DMATERIAL9		m_materialEnable;

		public:
			CActorBody();
			~CActorBody();
		};

		//----------------------------------------------------------
		// 基本的な関数
		//----------------------------------------------------------

		//!	@brief	定数値をロードする
		void Setup(
			NAsset::CJsonFactory::Key			_key,
			NCollision::EStageCollisionIndex	_colGroundIndex,
			NCollision::EStageCollisionIndex	_colWallIndex);

		//!	@brief	初期化処理
		void Init();

		//!	@brief	前座標を取得
		void UpdateFast();

		//!	@brief	移動量をバインドさアれたゲームオブジェクトに適用
		void Update();

		//!	@brief	デバッグ用描画
		void RenderDebug();
		
		//----------------------------------------------------------
		// 操作用関数
		//----------------------------------------------------------

		//!	@brief		XZ方向の加速
		//!	@param[in]	_ratio		加速度(読み込み済みの定数値)の適用量
		void AccelXZ(float _ratio = 1.0f);

		//!	@brief		XZ方向の加速量を直接指定する
		void AccelXZDirect(const float _accelDelta);

		//!	@brief		Y方向の加速量を直接指定する
		//!	@param[in]	_accelDelta		加速量
		void AccelYDirect(const float _accelDelta);

		//!	@brief		ジャンプ(Y方向の加速)
		//!	@details	AccelYと違い、現在の落下速度を0にし、Y軸方向の加速度を直接設定する
		//!				ジャンプ後に空中状態になった後に着地状態を判別する
		void Jump(const float _jumpPow);

		//----------------------------------------------------------
		// セッター(直接の設定)
		//----------------------------------------------------------

		//!	@brief		移動方向と移動量で設定する
		void SetMove(const float3& _dir, const float _speed);

		//!	@brief		XZ平面での移動量を設定する
		void SetMoveSpeedXZ(const float _moveSpeed) { m_moveSpeedXZ = _moveSpeed; }

		//!	@brief		XZ平面での移動方向を設定する
		//!	@details	引数のYの値を無視し、正規化した値を設定する
		//!	@note		長さが０になるベクトルを渡すと、無視し、値を変更しない。
		void SetMoveDirXZ(const float3& _moveDir);

		//!	@brief		Y軸方向の移動量を設定する
		void SetMoveSpeedY(const float _moveSpeed) { m_moveSpeedY = _moveSpeed; }

		//!	@brief		減速をするためのフラグをセット
		void SetIsApplyDeccel(const bool _state) { m_isApplyDeccel = _state; }

		//!	@brief		移動方向を向くように回転させるフラグを設定する
		void SetIsApplyRotateMoveDir(const bool _state) { m_isApplyRotateToMoveDir = _state; }

		//!	@brief		Y軸方向のスピードを適用するフラグを設定する
		void SetIsApplySpeedY(const bool _state) { m_isApplySpeedY = _state; }

		//!	@brief		動作の適用方法を設定する
		void SetApplyType(const EApplyType _type = EApplyType::FORWARDING) { m_applyType = _type; }

		//!	@brief		XZ平面の最大速度の調整値を設定する
		void SetMoveSpeedXZRatio(const float _ratio);

		//----------------------------------------------------------
		// ゲッター
		//----------------------------------------------------------

		const float3&	GetPrevPos() const { return m_prevPos; }
		float			GetMoveSpeedXZ() const { return m_moveSpeedXZ; }
		const float3&	GetMoveDirXZ() const { return m_moveDirXZ; }
		float			GetMoveSpeedY() const { return m_moveSpeedY; }

		//!	@brief	ジャンプの制御を考慮しない生の着地状態データを取得する
		bool			GetIsGroundedRaw() const { return m_isGrounded; }

		//!	@brief	ジャンプの制御を考慮して着地状態を取得する
		bool			GetIsGrounded() const { return (m_isGrounded & m_isReleaseGround); }

		//!	@brief	XZ平面とY軸方向の速度を考慮した、3D空間上での移動量を取得
		float			GetMoveSpeed() const;

		//----------------------------------------------------------
		// 定数値を取得
		//----------------------------------------------------------

		float	GetMoveSpeedXZMax() const { return m_constMoveSpeedXZMax; }

	private:
		
		// バインドするゲームオブジェクト
		NObject::CGameObject*	m_pGameObject;	// 適用するゲームオブジェクト

		// 動作に関わる変数
		float3	m_prevPos;			// 前フレームの座標
		float	m_curAccelXZ;		// 現在フレームXZ平面での加速量
		float	m_moveSpeedXZ;		// XZ平面での移動速度
		float3	m_moveDirXZ;		// XZ平面での移動方向
		float	m_curAccelY;		// 現在フレームのY方向の移動速度
		float	m_moveSpeedY;		// Y方向の移動速度

		// 動作の制限
		float	m_moveSpeedXZMaxRatio;	// 最大速度の調整用0 ~ 1

		// 状態にかかわる変数
		bool	m_isGrounded;		// 着地フラグ
		bool	m_isCollideWall;	// 壁の衝突したフラグ
		
		bool	m_isJumpControl;	// ジャンプ制御中フラグ
		bool	m_isJumpFrame;		// ジャンプを実行したフレームフラグ(ジャンプのトリガー)
		bool	m_isReleaseGround;	// ジャンプ時に空中状態に遷移したフラグ

		// 値を適用するかどうかのフラグ
		bool		m_isApplyDeccel;				// 減速率を適用するフラグ
		bool		m_isApplyRotateToMoveDir;		// 移動方向に向くような回転を適用するフラグ
		bool		m_isApplySpeedY;				// Y方向のスピードを適用するフラグ
		EApplyType	m_applyType;					// 動作の適用タイプ

		// ステージとのあたり判定用メッシュ
		NCollision::EStageCollisionIndex	m_colGroundIndex;
		NCollision::EStageCollisionIndex	m_colWallIndex;

		NCollision::CMesh*					m_pColMeshGround;
		NCollision::CMesh*					m_pColMeshWall;

		CActorBody			m_body;

		// 当たり判定で使用する値
		float3	m_posAfterApplyMoveXZ;			// XZ平面の移動を適用した後の座標(Y軸の速度を適用する前の座標)
		float3	m_posAfterApplyMoveY;			// Y軸の速度を適用した後の座標
		float3	m_posAfterUpdate;				// 最終的な座標

		NMath::SMatrix44	m_curMatTrans;
		NMath::SMatrix44	m_curMatRot;

		// 定数値として使用する値
		float	m_constMoveSpeedXZMax;			// XZ平面の移動量の最大値
		float	m_constMoveSpeedXZAcc;			// XZ平面の加速量
		float	m_constMoveSpeedXZDecRatio;		// XZ平面での減速率
		float	m_constMoveSpeedXZDecRatioAir;	// 空中でのXZ平面での減速率
		float	m_constFallSpeedMax;			// -Y軸の最大速度(負の値)
		float	m_constJumpSpeedMax;			// +Y軸の最大速度
		
		float	m_constGravityScale;			// 重力反映量
		float	m_constDeltaGravity;			// 適用する重力

		float	m_constRotSpeedMaxDeg;			// 回転の最大値

		//!	@brief		定数値をJSONから読み込む
		void LoadConstant(json11::Json& _json);

		//!	@brief		ジャンプの制御
		//!	@details	ジャンプ後に空中に遷移したことを確認する
		void UpdateJump();

		//!	@brief		Y軸方向の重力加速を行う
		void AccelGravity();

		//!	@brief		XZ平面での更新
		void ApplyMoveXZ();
		
		//!	@brief		壁との判定
		void CorrectWallXZ();

		//!	@brief		Y軸の更新
		void ApplyMoveY();

		//!	@brief		地面との判定(Y軸を地面の高さに補正する) 
		void CorrectGroundY();

		//!	@brief		移動速度によって移動方向に回転させる
		void RotateMoveDirWithSpeed();

		//!	@brief		移動量と回転量を適用
		void ApplyMovement();

	public:
		CActorMove(NObject::CGameObject& _rGameObject);
		~CActorMove();
	};
}
