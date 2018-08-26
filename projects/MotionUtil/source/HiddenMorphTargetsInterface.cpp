/**
 * Morph Targets アクセスInterface.
 */

#include "HiddenMorphTargetsInterface.h"

CHiddenMorphTargetsInterface::CHiddenMorphTargetsInterface (sxsdk::shade_interface& shade) : shade(shade)
{
}

CHiddenMorphTargetsInterface::~CHiddenMorphTargetsInterface ()
{
}

//---------------------------------------------------------------.
// Stream保存/読み込み用.
//---------------------------------------------------------------.
/**
 * Morph Targets情報を持つか.
 */
bool CHiddenMorphTargetsInterface::hasMorphTargets (sxsdk::shape_class& shape)
{
	return m_morphTargetsData.hasMorphTargets(shape);
}

/**
 * 現在のMorph Targets情報をstreamに保存.
 */
void CHiddenMorphTargetsInterface::writeMorphTargetsData ()
{
	return m_morphTargetsData.writeMorphTargetsData();
}

/**
 * Morph Targets情報をstreamから読み込み.
 */
bool CHiddenMorphTargetsInterface::readMorphTargetsData (sxsdk::shape_class& shape)
{
	return m_morphTargetsData.readMorphTargetsData(shape);
}

//---------------------------------------------------------------.
// 登録/編集用.
//---------------------------------------------------------------.
/**
 * 対象形状のポインタを取得.
 */
sxsdk::shape_class* CHiddenMorphTargetsInterface::getTargetShape ()
{
	return m_morphTargetsData.getTargetShape();
}

/**
 * オリジナルの頂点座標を取得.
 */
const std::vector<sxsdk::vec3>& CHiddenMorphTargetsInterface::getOrgVertices () const
{
	return m_morphTargetsData.getOrgVertices();
}

/**
 * 対象のポリゴンメッシュ形状クラスを渡す.
 * これは変形前のもので、これを呼び出した後に位置移動した選択頂点をtargetとして登録していく.
 * @param[in] pShaoe   対象形状.
 */
bool CHiddenMorphTargetsInterface::setupShape (sxsdk::shape_class* pShape)
{
	return m_morphTargetsData.setupShape(pShape);
}

/**
 * baseの頂点座標を格納。streamからの読み込み時に呼ばれる.
 */
void CHiddenMorphTargetsInterface::setOrgVertices (const std::vector<sxsdk::vec3>& vertices)
{
	m_morphTargetsData.setOrgVertices(vertices);
}

/**
 * 選択頂点座標をMorphTargetsの頂点として追加.
 * @param[in] name      target名.
 * @param[in] indices   登録する頂点インデックス (選択された頂点).
 * @param[in] vertices  登録する頂点座標.
 * @return Morph Targets番号.
 */
int CHiddenMorphTargetsInterface::appendTargetVertices (const std::string& name, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices)
{
	return m_morphTargetsData.appendTargetVertices(name, indices, vertices);
}

/**
 * Morph Targetsの数.
 */
int CHiddenMorphTargetsInterface::getTargetsCount () const
{
	return m_morphTargetsData.getTargetsCount();
}

/**
 * Morph Targetの名前を取得.
 * @param[in]  tIndex    Morph Targets番号.
 */
const std::string CHiddenMorphTargetsInterface::getTargetName (const int tIndex) const
{
	return m_morphTargetsData.getTargetName(tIndex);
}

/**
 * Morph Targetの名前を指定.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[in]  name      名前.
 */
void CHiddenMorphTargetsInterface::setTargetName (const int tIndex, const std::string& name)
{
	m_morphTargetsData.setTargetName(tIndex, name);
}

/**
 * Morph Targetsの頂点座標を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[out] indices   頂点インデックスが返る.
 * @param[out] vertices  頂点座標が返る.
 */
bool CHiddenMorphTargetsInterface::getTargetVertices (const int tIndex, std::vector<int>& indices, std::vector<sxsdk::vec3>& vertices)
{
	return m_morphTargetsData.getTargetVertices(tIndex, indices, vertices);
}

/**
 * Morph Targetsのウエイト値を指定.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[in]  weight    ウエイト値(0.0 - 1.0).
 */
bool CHiddenMorphTargetsInterface::setTargetWeight (const int tIndex, const float weight)
{
	return m_morphTargetsData.setTargetWeight(tIndex, weight);
}

/**
 * Morph Targetsのウエイト値を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @return ウエイト値.
 */
float CHiddenMorphTargetsInterface::getTargetWeight (const int tIndex) const
{
	return m_morphTargetsData.getTargetWeight(tIndex);
}

/**
 * 指定のMorph Target情報を削除.
 * @param[in]  tIndex    Morph Targets番号.
 */
bool CHiddenMorphTargetsInterface::removeTarget (const int tIndex)
{
	return m_morphTargetsData.removeTarget(tIndex);
}

/**
 * Morph Targets情報を削除して、元の頂点に戻す.
 * @param[in] restoreVertices  頂点を元に戻す場合はtrue.
 */
void CHiddenMorphTargetsInterface::removeMorphTargets (const bool restoreVertices)
{
	m_morphTargetsData.removeMorphTargets(restoreVertices);
}

/**
 * すべてのウエイト値を0にする。.
 * この後にupdateMeshを呼ぶと、ウエイト前の頂点となる.
 */
void CHiddenMorphTargetsInterface::setZeroAllWeight ()
{
	m_morphTargetsData.setZeroAllWeight();
}

/**
 * 重複頂点をマージする.
 * ポリゴンメッシュの「sxsdk::polygon_mesh_class::cleanup_redundant_vertices」と同等で、Morph Targetsも考慮したもの.
 */
bool CHiddenMorphTargetsInterface::cleanupRedundantVertices ()
{
	return m_morphTargetsData.cleanupRedundantVertices();
}

/**
 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
 */
void CHiddenMorphTargetsInterface::updateMesh ()
{
	m_morphTargetsData.updateMesh();
}
