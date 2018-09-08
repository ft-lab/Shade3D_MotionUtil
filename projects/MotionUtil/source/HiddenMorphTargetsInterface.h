/**
 * Morph Targets アクセスInterface.
 */
#ifndef _HIDDENMORPHTARGETSINTERFACE_H
#define _HIDDENMORPHTARGETSINTERFACE_H

#include "GlobalHeader.h"
#include "MorphTargetsCtrl.h"
#include "MotionExternalAccess.h"

struct CHiddenMorphTargetsInterface : public CMorphTargetsAccess
{
private:
	sxsdk::shade_interface& shade;
	CMorphTargetsCtrl m_morphTargetsData;				// Morph Targets情報.

private:
	/**
	 * SDKのビルド番号を指定（これは固定で変更ナシ）。.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	/**
	 * UUIDの指定（独自に定義したGUIDを指定）.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual sx::uuid_class get_uuid (void * = 0) { return HIDDEN_MORPH_TARGETS_INTERFACE_ID; }

	/**
	 * メニューに表示しない.
	 */
	virtual void accepts_shape (bool &accept, void *aux=0) { accept = false; }

public:
	CHiddenMorphTargetsInterface (sxsdk::shade_interface& shade);
	virtual ~CHiddenMorphTargetsInterface ();

	/**
	 * プラグイン名をSXUL(text.sxul)より取得.
	 */
	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("morph_targets_title"); }

	//------------------------------------------------------------------------------.
	//---------------------------------------------------------------.
	// Stream保存/読み込み用.
	//---------------------------------------------------------------.
	/**
	 * Morph Targets情報を持つか.
	 */
	bool hasMorphTargets (sxsdk::shape_class& shape);

	/**
	 * 現在のMorph Targets情報をstreamに保存.
	 */
	void writeMorphTargetsData ();

	/**
	 * Morph Targets情報をstreamから読み込み.
	 */
	bool readMorphTargetsData (sxsdk::shape_class& shape);

	//---------------------------------------------------------------.
	// 登録/編集用.
	//---------------------------------------------------------------.
	/**
	 * 対象形状のポインタを取得.
	 */
	sxsdk::shape_class* getTargetShape ();

	/**
	 * オリジナルの頂点座標数を取得.
	 */
	int getOrgVerticesCount () const;

	/**
	 * オリジナルの頂点座標を取得.
	 */
	int getOrgVertices (sxsdk::vec3* vertices) const;

	/**
	 * 対象のポリゴンメッシュ形状クラスを渡す.
	 * これは変形前のもので、これを呼び出した後に位置移動した選択頂点をtargetとして登録していく.
	 * @param[in] pShaoe   対象形状.
	 */
	bool setupShape (sxsdk::shape_class* pShape);

	/**
	 * baseの頂点座標を格納。streamからの読み込み時に呼ばれる.
	 */
	void setOrgVertices (const int vCou, const sxsdk::vec3* vertices);

	/**
	 * 選択頂点座標をMorphTargetsの頂点として追加.
	 * @param[in] name      target名.
	 * @param[in] vCou      頂点数.
	 * @param[in] indices   登録する頂点インデックス (選択された頂点).
	 * @param[in] vertices  登録する頂点座標.
	 * @return Morph Targets番号.
	 */
	int appendTargetVertices (const char* name, const int vCou, const int* indices, const sxsdk::vec3* vertices);

	/**
	 * Morph Targetsの数.
	 */
	int getTargetsCount () const;

	/**
	 * Morph Targetの名前を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[out] name      名前が入る.
	 */
	bool getTargetName (const int tIndex, char* name) const;

	/**
	 * Morph Targetの名前を指定.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[in]  name      名前.
	 */
	void setTargetName (const int tIndex, const char* name);

	/**
	 * Morph Targetsの頂点数を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 */
	int getTargetVerticesCount (const int tIndex);

	/**
	 * Morph Targetsの頂点座標を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[out] indices   頂点インデックスが返る.
	 * @param[out] vertices  頂点座標が返る.
	 */
	bool getTargetVertices (const int tIndex, int* indices, sxsdk::vec3* vertices);

	/**
	 * Morph Targetsのウエイト値を指定.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[in]  weight    ウエイト値(0.0 - 1.0).
	 */
	bool setTargetWeight (const int tIndex, const float weight);

	/**
	 * Morph Targetsのウエイト値を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @return ウエイト値.
	 */
	float getTargetWeight (const int tIndex) const;

	/**
	 * 指定のMorph Target情報を削除.
	 * @param[in]  tIndex    Morph Targets番号.
	 */
	bool removeTarget (const int tIndex);

	/**
	 * Morph Targets情報を削除して、元の頂点に戻す.
	 * @param[in] restoreVertices  頂点を元に戻す場合はtrue.
	 */
	void removeMorphTargets (const bool restoreVertices = true);

	/**
	 * すべてのウエイト値を0にする。.
	 * この後にupdateMeshを呼ぶと、ウエイト前の頂点となる.
	 */
	void setZeroAllWeight ();

	/**
	 * シーンのすべての形状で、Morph Targets情報を持つ形状のウエイト値を一時保持.
	 * (いったんすべてのウエイト値を0にして戻す、という操作で使用).
	 */
	void pushAllWeight (sxsdk::scene_interface* scene, const bool setZeroWeight = false);

	/**
	 * シーンのすべての形状のMorph Targets情報のウエイト値を戻す.
	 */
	void popAllWeight (sxsdk::scene_interface* scene);

	/**
	 * 重複頂点をマージする.
	 * ポリゴンメッシュの「sxsdk::polygon_mesh_class::cleanup_redundant_vertices」と同等で、Morph Targetsも考慮したもの.
	 */
	bool cleanupRedundantVertices (sxsdk::shape_class& shape);

	/**
	 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
	 */
	void updateMesh ();
};

#endif
