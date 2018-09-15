/**
 * ボーンの便利機能を提供するインターフェース.
 */
#ifndef _HIDDENBONEUTILINTERFACE_H
#define _HIDDENBONEUTILINTERFACE_H

#include "GlobalHeader.h"
#include "MotionExternalAccess.h"

struct CHiddenBoneUtilInterface : public CBoneAttributeAccess
{
private:
	sxsdk::shade_interface& shade;

	/**
	 * SDKのビルド番号を指定（これは固定で変更ナシ）。.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	/**
	 * UUIDの指定（独自に定義したGUIDを指定）.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual sx::uuid_class get_uuid (void * = 0) { return HIDDEN_BONE_UTIL_INTERFACE_ID; }

	/**
	 * メニューに表示しない.
	 */
	virtual void accepts_shape (bool &accept, void *aux=0) { accept = false; }

public:
	CHiddenBoneUtilInterface (sxsdk::shade_interface& shade);
	virtual ~CHiddenBoneUtilInterface ();

	/**
	 * プラグイン名をSXUL(text.sxul)より取得.
	 */
	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("boneutil_title"); }

	/**
	 * クラスバージョンを取得 (ver.0.0.0.4 - ).
	 */
	int getVersion ();

	//------------------------------------------------------------------------------.
	/**
	 * 指定の形状がボーンかどうか.
	 */
	bool isBone (sxsdk::shape_class& shape);

	/**
	 * ボーンのワールド座標での中心位置とボーンサイズを取得.
	 */
	sxsdk::vec3 getBoneCenter (sxsdk::shape_class& shape, float *size);

	/**
	 * ボーンの向きをそろえる.
	 * @param[in] boneRoot  対象のボーンルート.
	 */
	void adjustBonesDirection (sxsdk::shape_class* boneRoot);

	/**
	 * ボーンサイズの自動調整.
	 * @param[in] boneRoot  対象のボーンルート.
	 */
	void resizeBones (sxsdk::shape_class* boneRoot);
};

#endif
