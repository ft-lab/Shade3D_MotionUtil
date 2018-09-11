# MotionUtil for Shade3D

Shade3Dでモーションを補助するプラグインです。    
Morph Targets機能の追加/Bone補助/新しいモーションウィンドウ、を模索しています。     
別プラグインから機能をアクセスできるライブラリとしても実装しています。    

現在(ver.0.0.0.x 段階)は、Morph Targets機能の追加/Bone補助機能が実装されています。    

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

まだバイナリは準備中、、、、

### プラグインを配置し、Shade3Dを起動

Windowsの場合は、ビルドされた MotionUtil64.dll をShade3Dのpluginsディレクトリに格納してShade3Dを起動。  
Macの場合は、ビルドされた MotionUtil.shdplugin をShade3Dのpluginsディレクトリに格納してShade3Dを起動。  
メインメニューの「表示」-「Morph Targets」が表示されるのを確認します。  

## ライセンス  

This software is released under the MIT License, see [LICENSE](./LICENSE).  

## 更新履歴

[2018/09/11] ver.0.0.0.2   
* Morph Targets : 「ウエイト値をすべてクリア」ボタンを追加 

[2018/09/11] ver.0.0.0.1   
* 初回版
