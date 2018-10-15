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

/**
 * クラスバージョンを取得 (ver.0.0.0.4 - ).
 */
int CHiddenMorphTargetsInterface::getVersion ()
{
	return MORPHTARGETS_ATTRIBUTE_ACCESS_VERSION;
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
 * オリジナルの頂点座標数を取得.
 */
int CHiddenMorphTargetsInterface::getOrgVerticesCount () const
{
	const std::vector<sxsdk::vec3>& vers = m_morphTargetsData.getOrgVertices();
	return (int)vers.size();
}

/**
 * オリジナルの頂点座標を取得.
 */
int CHiddenMorphTargetsInterface::getOrgVertices (sxsdk::vec3* vertices) const
{
	const std::vector<sxsdk::vec3>& vers = m_morphTargetsData.getOrgVertices();
	const int vCou = (int)vers.size();
	for (int i = 0; i < vCou; ++i)  vertices[i] = vers[i];
	return vCou;
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
void CHiddenMorphTargetsInterface::setOrgVertices (const int vCou, const sxsdk::vec3* vertices)
{
	std::vector<sxsdk::vec3> vers;
	vers.resize(vCou);
	for (int i = 0; i < vCou; ++i) vers[i] = vertices[i];
	m_morphTargetsData.setOrgVertices(vers);
}

/**
 * 選択頂点座標をMorphTargetsの頂点として追加.
 * @param[in] name      target名.
 * @param[in] vCou      頂点数.
 * @param[in] indices   登録する頂点インデックス (選択された頂点).
 * @param[in] vertices  登録する頂点座標.
 * @return Morph Targets番号.
 */
int CHiddenMorphTargetsInterface::appendTargetVertices (const char* name, const int vCou, const int* indices, const sxsdk::vec3* vertices)
{
	if (vCou <= 0) return -1;
	std::vector<int> indices2;
	std::vector<sxsdk::vec3> vertices2;
	indices2.resize(vCou);
	vertices2.resize(vCou);
	for (int i = 0; i < vCou; ++i) {
		indices2[i]  = indices[i];
		vertices2[i] = vertices[i];
	}
	return m_morphTargetsData.appendTargetVertices(name, indices2, vertices2);
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
 * @param[out] name      名前が入る.
 */
bool CHiddenMorphTargetsInterface::getTargetName (const int tIndex, char* name) const
{
	const std::string nameS = m_morphTargetsData.getTargetName(tIndex);
	std::strcpy(name, nameS.c_str());
	return true;
}

/**
 * Morph Targetの名前を指定.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[in]  name      名前.
 */
void CHiddenMorphTargetsInterface::setTargetName (const int tIndex, const char* name)
{
	m_morphTargetsData.setTargetName(tIndex, name);
}

/**
 * Morph Targetsの頂点数を取得.
 * @param[in]  tIndex    Morph Targets番号.
 */
int CHiddenMorphTargetsInterface::getTargetVerticesCount (const int tIndex)
{
	return m_morphTargetsData.getTargetVerticesCount(tIndex);
}

/**
 * Morph Targetsの頂点座標を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[out] indices   頂点インデックスが返る.
 * @param[out] vertices  頂点座標が返る.
 */
bool CHiddenMorphTargetsInterface::getTargetVertices (const int tIndex, int* indices, sxsdk::vec3* vertices)
{
	std::vector<int> indices2;
	std::vector<sxsdk::vec3> vertices2;

	if (!m_morphTargetsData.getTargetVertices(tIndex, indices2, vertices2)) return false;
	const int vCou = (int)indices2.size();
	for (int i = 0; i < vCou; ++i) {
		indices[i]  = indices2[i];
		vertices[i] = vertices2[i];
	}

	return true;
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
	try {
		compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
		if (scene) {
			m_morphTargetsData.removeMorphTargets(scene, restoreVertices);
		}
	} catch (...) { }
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
 * シーンのすべての形状で、Morph Targets情報を持つ形状のウエイト値を一時保持.
 * (いったんすべてのウエイト値を0にして戻す、という操作で使用).
 */
void CHiddenMorphTargetsInterface::pushAllWeight (sxsdk::scene_interface* scene, const bool setZeroWeight)
{
	m_morphTargetsData.pushAllWeight(scene, setZeroWeight);
}

/**
 * シーンのすべての形状のMorph Targets情報のウエイト値を戻す.
 */
void CHiddenMorphTargetsInterface::popAllWeight (sxsdk::scene_interface* scene)
{
	m_morphTargetsData.popAllWeight(scene);
}

/**
 * 指定の形状の(Cacheでの)カレントウエイト値を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[out] weights   targetごとのウエイト値が返る.
 */
bool CHiddenMorphTargetsInterface::getShapeCurrentWeights (const sxsdk::shape_class* shape, float* weights)
{
	std::vector<float> weight2;
	if (!m_morphTargetsData.getShapeCurrentWeights(shape, weight2)) return false;
	for (size_t i = 0; i < weight2.size(); ++i) weights[i] = weight2[i];
	return true;
}

/**
 * 重複頂点をマージする.
 * ポリゴンメッシュの「sxsdk::polygon_mesh_class::cleanup_redundant_vertices」と同等で、Morph Targetsも考慮したもの.
 */
bool CHiddenMorphTargetsInterface::cleanupRedundantVertices (sxsdk::shape_class& shape)
{
	return m_morphTargetsData.cleanupRedundantVertices(shape);
}

/**
 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
 */
void CHiddenMorphTargetsInterface::updateMesh ()
{
	try {
		compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
		if (scene) {
			m_morphTargetsData.updateMesh(scene);
		}
	} catch (...) { }
}
