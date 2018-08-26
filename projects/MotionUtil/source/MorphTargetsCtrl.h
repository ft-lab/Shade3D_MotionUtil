/**
 * Morph Targets制御クラス.
 * フェイシャルアニメーションなどのポリゴンメッシュの頂点変形のサポート.
 */
#ifndef _MORPHTARGETS_CTRL_H
#define _MORPHTARGETS_CTRL_H

#include "GlobalHeader.h"
#include <vector>

//-------------------------------------------------.
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
	int updateTargetVertices (const int tIndex, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices);

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
	 * Morph Targetsの頂点座標を取得.
	 * @param[in]  tIndex    Morph Targets番号.
	 * @param[out] indices   頂点インデックスが返る.
	 * @param[out] vertices  頂点座標が返る.
	 */
	bool getTargetVertices (const int tIndex, std::vector<int>& indices, std::vector<sxsdk::vec3>& vertices);

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
	 * 重複頂点をマージする.
	 * ポリゴンメッシュの「sxsdk::polygon_mesh_class::cleanup_redundant_vertices」と同等で、Morph Targetsも考慮したもの.
	 */
	bool cleanupRedundantVertices ();

	/**
	 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
	 */
	void updateMesh ();

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
