# InfightAction

## 🎮ゲーム紹介

|||
|---|---|
|タイトル|InfightAction| 
|ジャンル|3Dアクションゲーム| 
|プレイ人数|1人| 
|動作環境|Windows| 

### ゲーム概要

攻撃をどんどんあてて敵を倒していく、爽快な3Dアクションゲームです。

デビルメイクライ4のブラッディパレスを企画の種として実装しました。

## 🛠️技術的な見どころ

### DirectX11とFBXSDKを使った3Dプログラミング

ゲームエンジンを使わずに1からフルスクラッチで開発を行い、3Dゲームを制作しました。

以下の実装を行い、3Dプログラミングの仕組を理解しました。
- FBXデータの読み込みとパース処理
- レンダリングパイプラインやシェーダーの実装
- 行列計算によるスケルタルアニメーションの実装

この経験から、ゲームエンジンでの開発でも技術を応用することができます。

### templateを活用した汎用クラスの作成

C++のtemplateを活用し、型の抽象化を行いコードの再利用性を向上させ、効率的なコーディングを行っています。

具体的には以下の場所で活用しています。
- [アセット管理クラスの抽象化](https://github.com/Tobana43F/InfightAction/blob/main/Sources/Framework/Asset/Factory/Asset_Factory.h)
- [シングルトンの基底クラス](https://github.com/Tobana43F/InfightAction/blob/main/Sources/Framework/Util/Util_SingletonTemplate.h)
- [シーン生成処理のユーティリティ](https://github.com/Tobana43F/InfightAction/blob/addfc916f61099444f6343d20c33c42300429849/Sources/Framework/Scene/Scene_Def.cpp#L17-L21)
- [ステート管理ユーティリティクラス](https://github.com/Tobana43F/InfightAction/blob/main/Sources/Framework/Util/Util_StateManager.h)

### 当たり判定パイプラインの実装

キャラクターや敵の当たり判定を以下のような構造で管理しています。

- 単純な形状（OBB）の当たり判定1つを管理するクラス（[NCollision::CGame](https://github.com/Tobana43F/InfightAction/blob/main/Sources/GameFramework/Collision/Holder/Collision_Game.h)）
- 複数のOBBをまとめて1つの当たり判定を構成するグループクラス（[NCollision::CGroup](https://github.com/Tobana43F/InfightAction/blob/main/Sources/GameFramework/Collision/Holder/Collision_Group.h)）
- 複数のグループを管理し、発生から消滅までを管理するシーケンスクラス（[NCollision::CSequence](https://github.com/Tobana43F/InfightAction/blob/main/Sources/GameFramework/Collision/Holder/Collision_Sequence.h)）
- 同じ属性（攻撃判定、 被ダメージ判定）の複数のシーケンスを管理するホルダークラス（[NCollision::CHolder](https://github.com/Tobana43F/InfightAction/blob/main/Sources/GameFramework/Collision/Holder/Collision_Holder.h)）

json形式で保存した当たり判定情報を読み取り、その情報をもとに上記クラスを構築。  
そしてそれらを処理するパイプラインを以下のコードで実装しています。   
[NCollision::CProc](https://github.com/Tobana43F/InfightAction/blob/main/Sources/GameFramework/Collision/CollisionProc/Collision_Proc.h)

モデルのアニメーションに合わせた当たり判定を処理し、アクションゲームに必要な当たり判定処理を実装しています。

### jsonを活用したパラメーターの外部ファイル化

キャラクターのパラメータや、当たり判定情報、ステージの構成情報などを全てjsonデータ化し、  
データを変更することでゲーム内の様々な設定を変更できるように実装しています。

これによりコンパイルやビルドを待たず調整を行えるので、レベルデザインのイテレーションを高速化できます。

### ビルド時間の短縮

ヘッダーファイルを変更するとinclude先でも変更が発生しビルド時間が伸びてしまうので、  
ヘッダーでは前方宣言などを活用し、詳細な実装や具体的な参照はcpp側で記載することで、  
ビルド時間が長くならないように心がけて実装しました。

### doxygenによるドキュメント生成

doxygenフォーマットに則りコメントを記述することで、自動でソースコードのドキュメントを生成できるようにしています。

チームでの開発を想定し、個人開発でも常に必要十分なコメントを丁寧に記載することを心がけています。

GitHub Pagesを利用して、doxygenで生成したドキュメントを閲覧できるようにしています。  
[Doxygen ページ](https://tobana43f.github.io/InfightAction/doxygen/html/index.html)

## ✨こだわり・アピールポイント

### アクションゲームに適したデバッグ機能

攻撃やダメージ判定をOBBに合わせてボックスメッシュで表示するデバッグ機能を実装し、  
直観的に判定範囲や状態が分かるようにしています。


アクションゲームではゲームスピードやアクションの難しさでデバッグが難しい場合がある。
そこで、ゲームスピードの変更機能、ポーズとコマ送り機能、を実装しました。


デバッグ時に全体を見渡せるように、自由に移動できるデバッグカメラの実装も行っています。


当たり判定の表示機能も実装。


## 🔍自己レビュー

### スケルタルアニメーション処理について

当時はスケルタルアニメーションのアルゴリズムを理解するために、慣れているC++で実装を行っていた。  
しかし、CPUで処理するには負荷が高くポリゴン数が多いと動作が重くなっていた。

今実装するのであれば、GPGPUを活用した並列処理で高速化し、実用できるものを実装します。

### シェーダーについて

この制作ではシェーダーよりもゲームアプリケーションや基盤部分の実装に注力していたため、   
テクスチャの色をそのまま表示するシンプルなシェーダーのみが実装されています。  

シェーダーの実装については、UnityのShaderLabやShaderGraphの実務経験があり、  
様々な表現を実装することができます。

### 継承ではなくイベントドリブンな設計をしたい

当たり判定で衝突した際の処理を、クラスの関数として実装して、それを呼び出すことで実装しているが、  
拡張性がなく、新しい処理を書く際にはクラスと呼び出し元を変更する必要がある。

当たり判定パイプラインの機能として、衝突時の処理をイベントとして登録できるようすれば、依存関係が減り柔軟な対応ができるようになる。

## 🛠開発情報

|||
|---|---|
|開発環境|C++, DirectX11|
|開発人数|1人|
|開発時期|2018年|
|開発期間|約5カ月|

### 環境構築について

[FBX SDK 2018.0 VS2015](http://download.autodesk.com/us/fbx/2018/2018.0/fbx20180_fbxsdk_vs2015_win.exe)のインストールが必要です。

### 使用ツール

- Visual Studio, VSCode
- SVN, TortoiseSVN
- Excel
- Blender
- GIMP2
- Audacity
