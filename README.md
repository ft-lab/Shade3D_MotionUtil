# MotionUtil for Shade3D

Shade3Dでモーションを補助するプラグインです。    
Morph Targets機能の追加/Bone補助/新しいモーションウィンドウ、を模索しています。     
別プラグインから機能をアクセスできるライブラリとしても実装しています。    

現在(ver.0.0.0.x 段階)は、Morph Targets機能の追加/Bone補助機能が実装されています。    

以下のプラグインで使用しています。
* glTF Converter for Shade3D (https://github.com/ft-lab/Shade3D_GLTFConverter)

## 機能

以下の機能があります。

* Morph Targetsとして、ポリゴンメッシュの頂点をウエイト指定で変形 (ver.0.0.0.x 段階では登録のみ)
* ライブラリとしてのボーン補助機能（ボーンの「軸方向」を統一、ボーンのリサイズ）    
このボーン補助は外部プラグインからのアクセスのために用意されており、本プラグインではUIとして用意されていません。     
ボーン補助は「BoneUtil」( https://shade3d.jp/store/marketplace/ft-lab/boneutil/boneutil.html ) の機能をライブラリ化するものになります。    

## 動作環境

* Windows 7/8/10以降のOS    
* macOS 10.11以降   
* Shade3D ver.16/17以降で、Standard/Professional版（Basic版では動作しません）  
  Shade3Dの64bit版のみで使用できます。32bit版のShade3Dには対応していません。   

## 使い方

### プラグインダウンロード

以下から最新版をダウンロードしてください。  
https://github.com/ft-lab/Shade3D_MotionUtil/releases

### プラグインを配置し、Shade3Dを起動

Windowsの場合は、ビルドされた MotionUtil64.dll をShade3Dのpluginsディレクトリに格納してShade3Dを起動。  
Macの場合は、ビルドされた MotionUtil.shdplugin をShade3Dのpluginsディレクトリに格納してShade3Dを起動。  
メインメニューの「表示」-「Morph Targets」が表示されるのを確認します。  

## Morph Targets

Shade3D ver.17/18段階では、本体機能でMorph Targetsがないため、プラグインとして拡張しています。    
  
Morph Targetsとは、ポリゴンメッシュの頂点移動による変形を登録し、ウエイト値により変形を制御する手段です。    
ver.0.0.0.xでは、Morph Targetsの登録とウエイト値の割り当てのみに対応しています。    
アニメーションでのキーフレーム登録はまだ未実装です。    

<img src="https://github.com/ft-lab/Shade3D_MotionUtil/blob/master/wiki_images/gltfConverter_MorphTargets_01.jpg"/>   
メインメニューの「表示」-「Morph Targets」でMorph Targetsウィンドウを表示します。    

※ Morph Targets情報はUNDO/REDOに対応していません。    

Morph Targets情報は、個々のポリゴンメッシュ形状ごとに持つことができます。

### Morph Targets情報を割り当て

ブラウザで選択したポリゴンメッシュに対して、Morph Targetsウィンドウの「Morph Targets情報を割り当て」ボタンを押すと、    
そのときの頂点位置がベース情報として保持されます。    
Morph Targetsの動作を行う場合は、はじめにこの処理が必要になります。    

### Morph Targets情報を削除

ブラウザで選択したポリゴンメッシュに対して、Morph Targetsウィンドウの「Morph Targets情報を削除」ボタンを押すと、    
メッシュに割り当てられたすべてのMorph Targets情報が削除されます。    
このとき、「頂点を戻す」チェックボックスをオンにしておくと、はじめに登録したベースの頂点で戻します。    

### Morph Target情報を追加登録

このターゲットの登録操作は、「Morph Targets情報を割り当て」ではじめに登録した頂点位置をベースとした差分で管理されます。    

ブラウザで選択したポリゴンメッシュが対象になります。    
形状編集モード＋頂点選択モードでポリゴンメッシュの頂点を選択して任意の形に移動させます（頂点削除や追加はしないでください！）。    
その後 Morph Targetsウィンドウの「Morph Target情報を追加登録」ボタンを押すと、   
選択された頂点がウエイト値1.0のターゲットとして新規追加されます。    
追加されたターゲットは、Morph Targetsウィンドウのリストボックスに表示されます。    
リストボックス右の「更新」ボタンを押すと、そのターゲット情報を選択された頂点とその位置で更新します。    
リストボックス右の「削除」ボタンを押すと、そのターゲット情報を削除します。    
このときに、ベースの頂点位置に戻した上でターゲットが削除されます。    
リストボックスのスライダを0.0-1.0に変化させることで、ベースとして登録した頂点位置から    
ターゲットとして追加した頂点位置に変形します。    
複数のターゲットがある場合は、変形は加算されます。    

### ウエイト値をすべてクリア

形状のすべてのウエイト値を0にします。    

### Morph Target対象の頂点を選択

リストボックス部でターゲットを選択して    
Morph Targetsウィンドウの「Morph Target対象の頂点を選択」ボタンを押すと、   
そのターゲットとして登録したときの変形対象の頂点が選択されます。   

### ターゲット名を変更

リストボックス部で、ターゲット名の箇所をダブルクリックすると名前変更ダイアログボックスが表示されます。    
デフォルトでは「target」となっています。    

### Morph Targetsの制限事項

今後、仕様が変更/追加になる場合があります。    

* ポリゴンメッシュの頂点数が変わった場合、正しく動作しません。    
全てのモデリング工程が完了した段階でMorph Targetsの変形を使用するようにしてください。
* Morph Targetsを割り当て後、オブジェクトモードで形状を移動/回転する場合は正しく動作します(ver.0.0.0.4-)。     
オブジェクトモードで形状を拡大縮小した場合は、正しく動作しません。
* アニメーションのキーフレーム割り当てにはまだ対応していません。    

## ビルド方法 (開発者向け)

Shade3DプラグインSDK( https://github.com/shadedev/pluginsdk )をダウンロードします。  
Shade3D_MotionUtil/projects/MotionUtilフォルダをShade3DのプラグインSDKのprojectsフォルダに複製します。  

Windows環境の場合は、VS2017で「win_vs2017/MotionUtil.sln」を開いてビルドします。    
Mac環境の場合は、Xcodeで「mac/plugins/Template.xcodeproj」を開いてビルドします。   

## ライセンス  

This software is released under the MIT License, see [LICENSE](./LICENSE).  

## 更新履歴

[2018/09/16] ver.0.0.0.5   
* glTFエクスポート時にクラッシュすることがある問題を修正(Morph Targetsのウエイトのキャッシュが不正に反映されていた)
* MorphTargetsの「ウエイト値をすべてクリア」後に形状を切り替えると、ウエイト値が前の状態に戻る問題を修正

[2018/09/16] ver.0.0.0.4   
* 外部アクセス関数 : バージョンを取得する関数を追加
* Morph Targets : Morph Targetsを割り当てたポリゴンメッシュを移動/回転させた場合に変形が不正になる問題を修正

[2018/09/14] ver.0.0.0.3   
* Morph Targets : 環境設定の「カラー」を変更した場合、Morph Targetsウィンドウへの反映が一部不正な問題を修正

[2018/09/11] ver.0.0.0.2   
* Morph Targets : 「ウエイト値をすべてクリア」ボタンを追加 

[2018/09/11] ver.0.0.0.1   
* 初回版
