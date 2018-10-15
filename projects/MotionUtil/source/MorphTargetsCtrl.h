/**
 * Morph Targets制御クラス.
 * フェイシャルアニメーションなどのポリゴンメッシュの頂点変形のサポート.
 */
#ifndef _MORPHTARGETS_CTRL_H
#define _MORPHTARGETS_CTRL_H

#include "GlobalHeader.h"
#include <vector>

//-------------------------------------------------.
/**
 * Morph Targetsのウエイト情報の一時保持用.
 */
class CMorphTargetsWeightCache
{
public:
	void *shapeHandle;				// sxsdk::shape_classのハンドル(形状識別用).
	std::vector<float> weights;		// ウエイト情報.

public:
	CMorphTargetsWeightCache ();
	CMorphTargetsWeightCache (const CMorphTargetsWeightCache& v);
	~CMorphTargetsWeightCache ();

    CMorphTargetsWeightCache& operator = (const CMorphTargetsWeightCache &v) {
		this->shapeHandle = v.shapeHandle;
		this->weights     = v.weights;
		return (*this);
	}

	void clear ();
};

//-------------------------------------------------.
/**
 * Morph Targetsの1つの情報.
 */
class CMorphTargetsData
{
public:
	std::string name;						// target名.
	std::vector<int> vIndices;				// 対応する頂点インデックス.
	std::vector<sxsdk::vec3> vertices;		// 頂点座標.
	std::vector<sxsdk::vec3> normals;		// 法線.

	float weight;							// ウエイト値.

public:
	CMorphTargetsData ();
	CMorphTargetsData (const CMorphTargetsData& v);
	~CMorphTargetsData ();

    CMorphTargetsData& operator = (const CMorphTargetsData &v) {
		this->name     = v.name;
		this->vIndices = v.vIndices;
		this->vertices = v.vertices;
		this->normals  = v.normals;
		this->weight   = v.weight;
		return (*this);
	}

	void clear ();
};

//-------------------------------------------------.
class CMorphTargetsCtrl
{
private:
	sxsdk::shape_class* m_pTargetShape;						// ターゲットとなるポリゴンメッシュ形状.
	std::vector<sxsdk::vec3> m_orgVertices;					// オリジナルのすべての頂点座標.

	std::vector<CMorphTargetsData> m_morphTargetsData;		// MorphTargetsのリスト.

	int m_selectTargetIndex;								// 選択されているTarget番号.

private:
	/**
	 * Morph Targets情報を持つ形状をシーンから再帰的に探して格納.
	 * @param[in]   shape  検索形状.
	 * @param[out]  shapeList  ポリゴンメッシュでMorph Targetsを持つ形状が返る.
	 */
	void m_findMorphTargetsShape (sxsdk::shape_class* shape, std::vector<sxsdk::shape_class *>& shapeList);

	/**
	 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
	 */
	void m_updateMesh ();

	/**
	 * 頂点が移動、回転する場合に仮想的なpivot(これはバウンディングボックスの中心座標)でどれだけ移動/回転するか推定し、.
	 * stream内の情報を更新.
	 * @return 頂点が正しく更新された場合はtrue。頂点数が変わったなど、更新できない場合はfalseを返す.
	 */
	bool m_updateMeshVertices ();

public:
	CMorphTargetsCtrl ();

	void clear ();

	//---------------------------------------------------------------.
	// 登録/編集用.
	//---------------------------------------------------------------.
	/**
	 * 対象形状のポインタを取得.
	 */
	sxsdk::shape_class* getTargetShape () { return m_pTargetShape; }

	/**
	 * オリジナルの頂点座標を取得.
	 */
	const std::vector<sxsdk::vec3>& getOrgVertices () const { return m_orgVertices;  }

	/**
	 * Morph Targetの情報を取得.
	 */
	const CMorphTargetsData& getMorphTargetData (const int tIndex) const { return m_morphTargetsData[tIndex]; }

	/**
	 * 対象のポリゴンメッシュ形状クラスを渡す.
	 * これは変形前のもので、これを呼び出した後に位置移動した選択頂点をtargetとして登録していく.
	 * @param[in] pShaoe   対象形状.
	 */
	bool setupShape (sxsdk::shape_class* pShape);

	/**
	 * baseの頂点座標を格納。streamからの読み込み時に呼ばれる.
	 */
	void setOrgVertices (const std::vector<sxsdk::vec3>& vertices);

	/**
	 * 選択頂点座標をMorphTargetsの頂点として追加.
	 * @param[in] name      target名.
	 * @param[in] indices   登録する頂点インデックス (選択された頂点).
	 * @param[in] vertices  登録する頂点座標.
	 * @return Morph Targets番号.
	 */
	int appendTargetVertices (const std::string& name, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices);

	/**
	 * 選択頂点座標をMorphTargetsの頂点として更新.
	 * 更新時の頂点はウエイト値1.0とする.
	 * @param[in] indices   更新する頂点インデックス (選択された頂点).
	 * @param[in] vertices  更新する頂点座標.
	 * @return Morph Targets番号.
	 */
	int updateTargetVertices (sxsdk::scene_interface* scene, const int tIndex, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices);

	/**
	 * Morph Targetsの数.
	 */
	int getTargetsCount () const;

	/**
	 * Morph Targetの名前を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 */
	const std::string getTargetName (const int tIndex) const;

	/**
	 * Morph Targetの名前を指定.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[in]  name      名前.
	 */
	void setTargetName (const int tIndex, const std::string& name);

	/**
	 * Morph Targetsの頂点数を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 */
	int getTargetVerticesCount (const int tIndex) const;

	/**
	 * Morph Targetsの頂点座標を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[out] indices   頂点インデックスが返る.
	 * @param[out] vertices  頂点座標が返る.
	 */
	bool getTargetVertices (const int tIndex, std::vector<int>& indices, std::vector<sxsdk::vec3>& vertices);

	/**
	 * 指定の形状の(Cacheでの)カレントウエイト値を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[out] weights   targetごとのウエイト値が返る.
	 */
	bool getShapeCurrentWeights (const sxsdk::shape_class* shape, std::vector<float>& weights);

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
	void removeMorphTargets (sxsdk::scene_interface* scene, const bool restoreVertices = true);

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
	 * @param[in] checkVerticesModify  頂点の移動や回転を補正.
	 */
	void updateMesh (sxsdk::scene_interface* scene, const bool checkVerticesModify = true);

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
	// UI用.
	//---------------------------------------------------------------.
	/**
	 * 指定のTargetを選択状態にする。-1の場合は選択なし.
	 */
	void setSelectTargetIndex (const int tIndex = -1);

	/**
	 * 選択されているTarget番号を取得.
	 */
	int getSelectTargetIndex () const;

};

#endif
