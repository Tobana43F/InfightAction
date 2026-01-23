#pragma once

#include	<array>
#include	"Asset\Factory\Asset_TextureFactory.h"
#include	"Polygon_Def.h"
#include	"Graphic\Graphic_Def.h"
#include	"Shader\Shader_Def.h"

namespace NPolygon {

	//!	@brief	矩形ポリゴンの基底クラス
	//!	@note	正面は3D空間上でZ+方向を向いている
	//!	@todo	クラス化して、隠ぺいすること
	struct SData {
		
		// 基本的な情報
		NMath::SMatrix44	matWorld;	// 描画時のワールド変換行列
		float2				size;		// 大きさ

		std::array<float4, EVertexID::MAX>		colors;	// 頂点カラー

		// 使用するテクスチャの参照
		NAsset::CTextureFactory::SPAsset		pRefTexture;

		// アニメーションの制御情報
		uint					anmStackNo;		// 使用するアニメーション番号
		uint					anmFrame;		// アニメーションのフレーム
		bool					isAnmEnd;		// アニメーション終了フラグ
		bool					isIgnoreAnm;	// アニメーションを無視するフラグ(手動で、連番テクスチャの番号を指定する場合)
			
		// 描画の制御情報
		bool					isReverseX;	// X反転描画(true : 反転)
		bool					isReverseY;	// Y反転描画(true : 反転)
		NGraphic::EBlendState	blendState;	// ブレンドステート
		NShader::ECullMode		cullModel;	// カリングモード
		EAlignX					alignX;		// X軸のアライメント
		EAlignY					alignY;		// Y軸のアライメント

		//!	@brief	初期化用コンストラクタ
		SData();
	};

}