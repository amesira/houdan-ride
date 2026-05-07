# GolfGameDesign プロジェクト要約

## 概要

このプロジェクトは、Visual Studio 2022 / C++20 / DirectX 11 で作られた 3D アクションゲームです。  
プレイヤーが船の上を移動し、ボールに乗ったり投げたりしながら、障害物や敵を避けてゴールを目指す構成になっています。

画面サイズは `1280 x 720`、Windows デスクトップアプリとして動作します。  
描画、入力、音声、モデル読み込み、シーン管理、物理、衝突、UI などは独自実装の小さなゲームエンジン風の構造に分かれています。

## 開発環境

- ソリューション: `DX21_FirstEntry.sln`
- プロジェクト: `DX21_FirstEntry.vcxproj`
- 言語: C++20
- 主要 API: DirectX 11
- 対象: Windows アプリケーション
- 主な構成: `Debug|x64`, `Release|x64`
- 外部ライブラリ:
  - Assimp: `.fbx` モデル読み込み
  - DirectXTex: テクスチャ関連
  - stb_truetype: フォント描画

## ゲーム内容

ゲーム本編は `GameScene` が中心です。  
プレイヤー、TPS カメラ、ライト、UI、レベル、チュートリアル、ポインター、BGM などを初期化します。

主な流れは以下です。

1. タイトルシーンからゲームを開始する
2. チュートリアル表示後、カウントダウンしてプレイ開始
3. 船が前進し、一定距離ごとに障害物や敵が生成される
4. プレイヤーは移動、ジャンプ、ボール搭乗、ボール投げを行う
5. ゴール距離が 0m になるとリザルトシーンへ遷移する

スコアは `PlayerBehavior` が管理しており、敵接触や落下で減点されます。  
ゲーム終了時には `Manager_SendScore()` 経由でリザルトシーンへスコアが渡されます。

## シーン構成

シーンは `manager.cpp` で管理されています。

- `TitleScene`: タイトル画面
- `GameScene`: ゲーム本編
- `ResultScene`: 結果画面

現在のシーンは `SetScene()` で切り替えます。  
シーン切り替え時には、現在のシーンの `Finalize()` を呼び、新しいシーンの `Initialize()` を呼ぶ流れです。  
フェード演出は `fade.cpp` / `fade.h` が担当しています。

## アーキテクチャ

このプロジェクトは、GameObject / Component / Behavior / Processor を組み合わせた構成です。

### GameObject

`game_object.h` の `GameObject` は、ゲーム内オブジェクトの入れ物です。

- 名前、ID、アクティブ状態、破棄フラグを持つ
- Component を追加・取得できる
- Behavior を追加・取得できる
- 毎フレーム `Behavior::Update()` を呼ぶ

### Component

Component はデータを持つ部品です。

代表例:

- `TransformComponent`: 位置、回転、スケール
- `RigidbodyComponent`: 速度、質量、摩擦など
- `ColliderComponent`: 当たり判定
- `ModelComponent`: 3D モデル
- `ImageComponent`: 画像 UI / ワールド画像
- `TextComponent`: テキスト描画
- `CameraComponent`: カメラ情報
- `LightComponent`: ライト情報

Component は `SceneBase` が持つ `ComponentPool` で管理されています。

### Behavior

Behavior はオブジェクト固有の動作を担当します。

代表例:

- `PlayerBehavior`: プレイヤー移動、ジャンプ、ボール搭乗、ボール投げ、スコア管理
- `BallBehavior`: ボール移動、跳ね返り、回転、木箱破壊
- `EnemyBehavior`: 敵の挙動
- `TrainBehavior`: 船または列車モデルの移動と子コライダー管理
- `ButtonBehavior`: UI ボタン入力
- `TpsCameraBehavior`: TPS カメラ制御とスローモーション
- `WoodboxBehavior`: 木箱の破壊処理

### Processor

Processor はシーン全体を横断して処理するシステムです。

代表例:

- `CollisionProcessor`: 当たり判定と衝突解決
- `PhysicsProcessor`: Rigidbody の物理更新
- `DynamicsProcessor`: 動的処理
- `CameraProcessor`: カメラ更新
- `Renderer3DModelProcessor`: 3D モデル描画
- `RendererImageProcessor`: 画像描画
- `RendererFontProcessor`: フォント描画
- `RendererSliderProcessor`: スライダー UI 描画
- `LightProcessor`: ライト処理

`processor_manager.cpp` が各 Processor の初期化、更新、描画をまとめています。

## レベル生成

`level_manager.cpp` がゲーム中のステージ進行を管理します。

主な役割:

- 水面の生成と追従
- メインの船の生成
- メインボールの生成と再生成
- マップカメラの生成
- ゴールまでの距離 UI 更新
- 一定距離ごとの障害物、敵、ミニゴルフ風オブジェクト生成
- ゴール到達後のリザルト遷移

船の `z` 位置を基準に、先の位置へ木箱、敵、ゴルフゲームオブジェクトなどをランダム生成します。  
ゴール距離は `g_GoalDistance = 200.0f` で管理されています。

## 主要な入力

`PlayerBehavior` で確認できる主な操作は以下です。

- `W / A / S / D`: 移動
- `Space`: ジャンプ、またはボール搭乗中のジャンプ
- 左クリック長押し: ボール投げのパワー溜め
- 左クリックを離す: ボールを投げる
- `Enter`: チュートリアル終了
- `Esc`: ウィンドウを閉じる

マウス入力は `mouse.cpp` / `mouse.h`、キーボード入力は `keyboard.cpp` / `keyboard.h` が担当しています。

## アセット

アセットは `asset` フォルダにまとまっています。

- `asset/Audio`: BGM や効果音
- `asset/Font`: フォント
- `asset/Model`: `.fbx` 形式の 3D モデル
- `asset/Texture`: UI、背景、チュートリアル、水面、ポインターなどの画像

モデルには `ball.fbx`, `ship.fbx`, `train.fbx`, `goal.fbx`, `woodbox` 相当のモデルなどが含まれています。

## 主要ファイルの入口

- `main.cpp`: Windows アプリのエントリポイント、DirectX / 入力 / 音声 / Manager 初期化
- `manager.cpp`: シーン管理
- `game.cpp`: ゲーム本編シーン
- `title.cpp`: タイトルシーン
- `result.cpp`: リザルトシーン
- `factory.cpp`: GameObject 生成処理の集約
- `level_manager.cpp`: ステージ進行と自動生成
- `player_behavior.cpp`: プレイヤー操作とスコア
- `ball_behavior.cpp`: ボール挙動
- `collision_processor.cpp`: 当たり判定
- `direct3d.cpp`: DirectX 初期化と描画基盤
- `model.cpp`: モデル読み込み
- `sprite.cpp`: 画像描画
- `Audio.cpp`: 音声処理

## 注意点

- 一部コメントや文字列は文字化けして見える箇所があります。ソースの文字コードと表示環境が合っていない可能性があります。
- `Release|x64` の RuntimeLibrary が `MultiThreadedDebug` になっており、通常の Release 設定としては少し不自然です。
- `Debug`, `Release`, `x64` 配下にはビルド成果物やコピー済みアセットが含まれています。
- `assimp` ヘッダや DLL / LIB がプロジェクト配下に同梱されています。

## 改修するときの見方

ゲーム内容を変える場合は、まず以下を見ると把握しやすいです。

- プレイヤー操作を変える: `player_behavior.cpp`
- ボールの挙動を変える: `ball_behavior.cpp`
- ステージ生成を変える: `level_manager.cpp`
- オブジェクトの構成を変える: `factory.cpp`
- UI 表示を変える: `game.cpp`, `renderer_font_processor.cpp`, `renderer_image_processor.cpp`
- 当たり判定を変える: `collider_component.h`, `collision_processor.cpp`
- シーン遷移を変える: `manager.cpp`, `fade.cpp`

全体として、授業制作または学習用の DirectX 3D ゲームとして、ゲームループ、シーン、オブジェクト管理、描画、物理、衝突、UI、音声まで一通り実装されているプロジェクトです。

