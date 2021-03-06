﻿/**
 * Morph Targets制御クラス.
 * フェイシャルアニメーションなどのポリゴンメッシュの頂点変形のサポート.
 */
#include "MorphTargetsCtrl.h"
#include "StreamCtrl.h"
#include "BSPPoint.h"
#include "MathUtil.h"
#include "CalcMeshTransform.h"

/*
	ポリゴンメッシュのすべての変形前の頂点をあらかじめ保持.
	Morph Targetsの1つの要素をTargetとし、
      ・頂点インデックス.
      ・頂点座標.
      ・法線.
	を保持する.

*/

//-------------------------------------------------.
CMorphTargetsWeightCache::CMorphTargetsWeightCache ()
{
	clear();
}
CMorphTargetsWeightCache::CMorphTargetsWeightCache (const CMorphTargetsWeightCache& v)
{
	this->shapeHandle = v.shapeHandle;
	this->weights     = v.weights;
}
CMorphTargetsWeightCache::~CMorphTargetsWeightCache ()
{
}

void CMorphTargetsWeightCache::clear ()
{
	shapeHandle = NULL;
	weights.clear();
}

//-------------------------------------------------.
CMorphTargetsData::CMorphTargetsData ()
{
	clear();
}

CMorphTargetsData::CMorphTargetsData (const CMorphTargetsData& v)
{
	this->name     = v.name;
	this->vIndices = v.vIndices;
	this->vertices = v.vertices;
	this->normals  = v.normals;
	this->weight   = v.weight;
}

CMorphTargetsData::~CMorphTargetsData ()
{
}

void CMorphTargetsData::clear ()
{
	name = "";
	vIndices.clear();
	vertices.clear();
	normals.clear();
	weight = 0.0f;
}

//-------------------------------------------------.
namespace {
	std::vector< std::vector<CMorphTargetsWeightCache> > g_shapeWeightCache;	// 形状ごとのMorph Targetsのウエイト値の一時保持用.
}

CMorphTargetsCtrl::CMorphTargetsCtrl ()
{
	clear();
}

void CMorphTargetsCtrl::clear ()
{
	m_pTargetShape = NULL;
	m_orgVertices.clear();
	m_morphTargetsData.clear();
	m_selectTargetIndex = -1;
}

/**
 * Morph Targets情報を持つ形状をシーンから再帰的に探して格納.
 * @param[in]   shape  検索形状.
 * @param[out]  shapeList  ポリゴンメッシュでMorph Targetsを持つ形状が返る.
 */
void CMorphTargetsCtrl::m_findMorphTargetsShape (sxsdk::shape_class* shape, std::vector<sxsdk::shape_class *>& shapeList)
{
	if (shape->get_type() == sxsdk::enums::polygon_mesh) {
		if (StreamCtrl::hasMorphTargetsData(*shape)) {
			shapeList.push_back(shape);
		}
	}

	if (shape->has_son()) {
		sxsdk::shape_class* pShape = shape->get_son();
		while (pShape->has_bro()) {
			pShape = pShape->get_bro();
			m_findMorphTargetsShape(pShape, shapeList);
		}
	}
}

//---------------------------------------------------------------.
// 登録用.
//---------------------------------------------------------------.
/**
 * 対象のポリゴンメッシュ形状クラスを渡す.
 * これは変形前のもので、これを呼び出した後に位置移動した選択頂点をtargetとして登録していく.
 * @param[in] pShaoe   対象形状.
 */
bool CMorphTargetsCtrl::setupShape (sxsdk::shape_class* pShape)
{
	clear();
	if (pShape->get_type() != sxsdk::enums::polygon_mesh) return false;

	try {
		sxsdk::polygon_mesh_class& pMesh = pShape->get_polygon_mesh();
		const int versCou = pMesh.get_total_number_of_control_points();

		// 頂点座標を効率よく取得するためのsaverクラス.
		sxsdk::polygon_mesh_saver_class* pMeshSaver = pMesh.get_polygon_mesh_saver();

		m_orgVertices.resize(versCou);
		for (int i = 0; i < versCou; ++i) m_orgVertices[i] = pMeshSaver->get_point(i);

		pMeshSaver->release();

		m_pTargetShape = pShape;

		return true;
	} catch (...) { }

	return false;
}

/**
 * Morph Targetsのベースの頂点座標を更新.
 */
bool CMorphTargetsCtrl::updateMorphTargetsBase (sxsdk::shape_class* pShape)
{
	if (pShape->get_type() != sxsdk::enums::polygon_mesh) return false;
	try {
		sxsdk::polygon_mesh_class& pMesh = pShape->get_polygon_mesh();
		const int versCou = pMesh.get_total_number_of_control_points();

		// 頂点座標を効率よく取得するためのsaverクラス.
		sxsdk::polygon_mesh_saver_class* pMeshSaver = pMesh.get_polygon_mesh_saver();

		m_orgVertices.resize(versCou);
		for (int i = 0; i < versCou; ++i) m_orgVertices[i] = pMeshSaver->get_point(i);

		pMeshSaver->release();

		m_pTargetShape = pShape;

		return true;
	} catch (...) { }
	return false;
}

/**
 * baseの頂点座標を格納。streamからの読み込み時に呼ばれる.
 */
void CMorphTargetsCtrl::setOrgVertices (const std::vector<sxsdk::vec3>& vertices)
{
	m_orgVertices = vertices;
}

/**
 * 選択頂点座標をMorphTargetsの頂点として追加.
 * @param[in] name      target名.
 * @param[in] indices   登録する頂点インデックス (選択された頂点).
 * @param[in] vertices  登録する頂点座標.
 * @return Morph Targets番号.
 */
int CMorphTargetsCtrl::appendTargetVertices (const std::string& name, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices)
{
	if (vertices.empty() || indices.empty()) return -1;
	if (vertices.size() != indices.size()) return -1;

	const int index = (int)m_morphTargetsData.size();
	m_morphTargetsData.push_back(CMorphTargetsData());
	CMorphTargetsData& targetData = m_morphTargetsData.back();
	targetData.name     = name;
	targetData.vIndices = indices;
	targetData.vertices = vertices;
	targetData.weight   = 1.0f;

	return index;
}

/**
 * 選択頂点座標をMorphTargetsの頂点として更新.
 * 更新時の頂点はウエイト値1.0とする.
 * @param[in] indices   更新する頂点インデックス (選択された頂点).
 * @param[in] vertices  更新する頂点座標.
 * @return Morph Targets番号.
 */
int CMorphTargetsCtrl::updateTargetVertices (sxsdk::scene_interface* scene, const int tIndex, const std::vector<int>& indices, const std::vector<sxsdk::vec3>& vertices)
{
	if (tIndex < 0 || tIndex >= (int)m_morphTargetsData.size()) return -1;

	// tIndexのTargetを一度ウエイト0.0に戻す.
	{
		setTargetWeight(tIndex, 0.0f);
		updateMesh(scene);
	}

	CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	targetData.vIndices = indices;
	targetData.vertices = vertices;
	targetData.weight   = 1.0f;

	return tIndex;
}

/**
 * Morph Targetsの数.
 */
int CMorphTargetsCtrl::getTargetsCount () const
{
	return (int)m_morphTargetsData.size();
}

/**
 * Morph Targetsの頂点数を取得.
 * @param[in]  tIndex    Morph Targets番号.
 */
int CMorphTargetsCtrl::getTargetVerticesCount (const int tIndex) const
{
	if (tIndex < 0 || tIndex >= (int)m_morphTargetsData.size()) return 0;
	const CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	return targetData.vIndices.size();
}

/**
 * Morph Targetsの頂点座標を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[out] indices   頂点インデックスが返る.
 * @param[out] vertices  頂点座標が返る.
 */
bool CMorphTargetsCtrl::getTargetVertices (const int tIndex, std::vector<int>& indices, std::vector<sxsdk::vec3>& vertices)
{
	indices.clear();
	vertices.clear();

	if (tIndex < 0 || tIndex >= (int)m_morphTargetsData.size()) return false;
	CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	indices  = targetData.vIndices;
	vertices = targetData.vertices;
	return true;
}

/**
 * Morph Targetの名前を指定.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[in]  name      名前.
 */
void CMorphTargetsCtrl::setTargetName (const int tIndex, const std::string& name)
{
	CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	targetData.name = name;
}

/**
 * Morph Targetの名前を取得.
 * @param[in]  tIndex    Morph Targets番号.
 */
const std::string CMorphTargetsCtrl::getTargetName (const int tIndex) const
{
	const CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	return targetData.name;
}

/**
 * Morph Targetsのウエイト値を指定.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[in]  weight    ウエイト値(0.0 - 1.0).
 */
bool CMorphTargetsCtrl::setTargetWeight (const int tIndex, const float weight)
{
	CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	targetData.weight = std::min(1.0f, std::max(0.0f, weight));
	return true;
}

/**
 * Morph Targetsのウエイト値を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @return ウエイト値.
 */
float CMorphTargetsCtrl::getTargetWeight (const int tIndex) const
{
	const CMorphTargetsData& targetData = m_morphTargetsData[tIndex];
	return targetData.weight;
}

/**
 * すべてのウエイト値を0にする。.
 * この後にupdateMeshを呼ぶと、ウエイト前の頂点となる.
 */
void CMorphTargetsCtrl::setZeroAllWeight ()
{
	const size_t tCou = m_morphTargetsData.size();
	for (size_t i = 0; i < tCou; ++i) {
		CMorphTargetsData& targetD = m_morphTargetsData[i];
		targetD.weight = 0.0f;
	}
}

/**
 * 指定のMorph Target情報を削除.
 * @param[in]  tIndex    Morph Targets番号.
 */
bool CMorphTargetsCtrl::removeTarget (const int tIndex)
{
	m_selectTargetIndex = -1;
	const int tCou = (int)m_morphTargetsData.size();
	if (tIndex < 0 || tIndex >= tCou) return false;
	m_morphTargetsData.erase(m_morphTargetsData.begin() + tIndex);

	return true;
}

/**
 * Morph Targets情報を削除して、元の頂点に戻す.
 * @param[in] restoreVertices  頂点を元に戻す場合はtrue.
 */
void CMorphTargetsCtrl::removeMorphTargets (sxsdk::scene_interface* scene, const bool restoreVertices)
{
	if (!m_pTargetShape) return;
	if (restoreVertices) {
		setZeroAllWeight();
		updateMesh(scene);
	}
	StreamCtrl::removeMorphTargetsData(*m_pTargetShape);
	clear();
}

/**
 * 重複頂点をマージする.
 * ポリゴンメッシュの「sxsdk::polygon_mesh_class::cleanup_redundant_vertices」と同等で、Morph Targetsも考慮したもの.
 */
bool CMorphTargetsCtrl::cleanupRedundantVertices (sxsdk::shape_class& shape)
{
	if (m_pTargetShape && m_pTargetShape->get_type() != sxsdk::enums::polygon_mesh) return false;
	if (shape.get_type() != sxsdk::enums::polygon_mesh) return false;

	if (!m_pTargetShape) {
		sxsdk::polygon_mesh_class& pMesh = shape.get_polygon_mesh();
		pMesh.cleanup_redundant_vertices();
		return false;
	}

	try {
		compointer<sxsdk::scene_interface> scene(m_pTargetShape->get_scene_interface());

		const int versCou = (int)m_orgVertices.size();
		sxsdk::polygon_mesh_class& pMesh = m_pTargetShape->get_polygon_mesh();
		if (pMesh.get_total_number_of_control_points() != versCou) {
			pMesh.cleanup_redundant_vertices();
			return false;
		}

		// 空間分割クラスに頂点を渡して空間分割を実行.
		CBSPPoint bspPoint(m_orgVertices);
		bspPoint.build();

		// verIndices[]に同一頂点位置の場合のインデックスを割り当て.
		const float fMin = (float)(1e-6);
		std::vector<int> verIndices, verNewIndices;
		std::vector<int> indices;
		{
			verIndices.resize(versCou, -1);
			verNewIndices.resize(versCou, -1);
			int iPos = 0;
			for (int i = 0; i < versCou; ++i) {
				if (verIndices[i] < 0) {
					const int cou = bspPoint.searchVertices(m_orgVertices[i], fMin, indices);
					if (cou > 0) {
						for (int j = 0; j < cou; ++j) verIndices[ indices[j] ] = i;
						verNewIndices[i] = iPos++;
					}
				}
			}
		}

		// 面の頂点インデックスを置き換え.
		const int facesCou = pMesh.get_number_of_faces();
		for (int i = 0; i < facesCou; ++i) {
			sxsdk::face_class& f = pMesh.face(i);
			const int vCou = f.get_number_of_vertices();
			indices.resize(vCou);
			f.get_vertex_indices(&(indices[0]));
			for (int j = 0; j < vCou; ++j) indices[j] = verIndices[ indices[j] ];
			f.set_vertex_indices(vCou, &(indices[0]));
		}

		// 重複頂点を削除.
		{
			pMesh.begin_removing_control_points ();
			for (int i = versCou - 1; i >= 0; --i) {
				if (verNewIndices[i] < 0) {
					m_orgVertices.erase(m_orgVertices.begin() + i);
					pMesh.remove_control_point(i);
				}
			}
			pMesh.end_removing_control_points();
		}

		// Morph Targetsでの頂点インデックスを置き換え、重複しているものを削除.
		const size_t targetsCou = m_morphTargetsData.size();
		for (size_t i = 0; i < targetsCou; ++i) {
			CMorphTargetsData& morphD = m_morphTargetsData[i];
			const int cou = (int)morphD.vIndices.size();
			for (int j = 0; j < cou; ++j) {
				morphD.vIndices[j] = verNewIndices[ morphD.vIndices[j] ];
			}
			for (int j = cou - 1; j >= 0; --j) {
				if (morphD.vIndices[j] < 0) {
					morphD.vIndices.erase(morphD.vIndices.begin() + j);
					morphD.vertices.erase(morphD.vertices.begin() + j);
					if (!morphD.normals.empty()) {
						morphD.normals.erase(morphD.normals.begin() + j);
					}
				}
			}
		}

		pMesh.update();
		pMesh.make_edges();

		return true;

	} catch (...) { }

	return false;
}

/**
 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
 */
void CMorphTargetsCtrl::m_updateMesh ()
{
	if (m_pTargetShape) {
		if (m_orgVertices.size() != (m_pTargetShape->get_total_number_of_control_points())) return;
	}
	if (!m_pTargetShape || m_morphTargetsData.empty()) return;

	try {
		std::vector<sxsdk::vec3> meshVers = m_orgVertices;
		const int versCou = (int)meshVers.size();
		std::vector<int> useMeshVers;
		useMeshVers.resize(versCou, false);

		const int targetsCou = (int)m_morphTargetsData.size();

		// 更新が必要な頂点をフラグ立て.
		for (int loop = 0; loop < targetsCou; ++loop) {
			const CMorphTargetsData& targetD = m_morphTargetsData[loop];
			const int vCou = (int)targetD.vIndices.size();
			for (int i = 0; i < vCou; ++i) {
				const int vIndex = targetD.vIndices[i];
				useMeshVers[vIndex] = true; 
			}
		}

		// Morph Targetsの情報をウエイト値により、tVerticesの頂点座標を変換.
		for (int loop = 0; loop < targetsCou; ++loop) {
			const CMorphTargetsData& targetD = m_morphTargetsData[loop];
			if (sx::zero(targetD.weight)) continue;

			const int vCou = (int)targetD.vIndices.size();
			const float weight = std::min(1.0f, std::max(0.0f, targetD.weight));

			for (int i = 0; i < vCou; ++i) {
				const int vIndex = targetD.vIndices[i];
				meshVers[vIndex] += (targetD.vertices[i] - m_orgVertices[vIndex]) * weight;
			}
		}

		// ポリゴンメッシュの頂点座標を更新.
		sxsdk::polygon_mesh_class& pMesh = m_pTargetShape->get_polygon_mesh();
		for (int i = 0; i < versCou; ++i) {
			if (!useMeshVers[i]) continue;
			sxsdk::vertex_class& v = pMesh.vertex(i);
			v.set_position(meshVers[i]);
		}
		pMesh.update();

	} catch (...) { }
}

/**
 * Morph Targetsの情報より、m_pTargetShapeのポリゴンメッシュを更新.
 * @param[in] checkVerticesModify  頂点の移動や回転を補正.
 */
void CMorphTargetsCtrl::updateMesh (sxsdk::scene_interface* scene, const bool checkVerticesModify)
{
	if (m_pTargetShape) {
		if (m_orgVertices.size() != (m_pTargetShape->get_total_number_of_control_points())) return;
	}
	if (!m_pTargetShape || m_morphTargetsData.empty()) return;

	if (!checkVerticesModify) {
		// メッシュ情報を更新.
		m_updateMesh();

	} else {
		// オリジナルの頂点より、移動/回転があるかチェック.
		const bool ret = m_updateMeshVertices();

		// メッシュ情報を更新.
		m_updateMesh();
	}
}

/**
 * シーンのすべての形状で、Morph Targets情報を持つ形状のウエイト値を一時保持.
 * (いったんすべてのウエイト値を0にして戻す、という操作で使用).
 */
void CMorphTargetsCtrl::pushAllWeight (sxsdk::scene_interface* scene, const bool setZeroWeight)
{
	g_shapeWeightCache.push_back(std::vector<CMorphTargetsWeightCache>());
	std::vector<CMorphTargetsWeightCache>& wCache = g_shapeWeightCache.back();

	try {
		// Morph Targets情報を持つ形状を取得.
		std::vector<sxsdk::shape_class *> shapeList;
		sxsdk::shape_class& rootShape = scene->get_shape();
		m_findMorphTargetsShape(&rootShape, shapeList);
		if (shapeList.empty()) return;

		const size_t shapeCou = shapeList.size();
		wCache.resize(shapeCou);
		for (size_t i = 0; i < shapeCou; ++i) {
			CMorphTargetsWeightCache& weightC = wCache[i];
			weightC.clear();
			weightC.shapeHandle = shapeList[i]->get_handle();

			CMorphTargetsCtrl targetC;
			StreamCtrl::readMorphTargetsData(*shapeList[i], targetC);
			const int tCou = targetC.getTargetsCount();
			weightC.weights.resize(tCou, 0.0f);
			for (int j = 0; j < tCou; ++j) {
				weightC.weights[j] = targetC.getTargetWeight(j);
			}
		}

		// ウエイト値を0にする.
		if (setZeroWeight) {
			for (size_t i = 0; i < shapeCou; ++i) {
				const CMorphTargetsWeightCache& weightC = wCache[i];

				CMorphTargetsCtrl targetC;
				StreamCtrl::readMorphTargetsData(*shapeList[i], targetC);
				targetC.setZeroAllWeight();
				StreamCtrl::writeMorphTargetsData(*shapeList[i], targetC);
				targetC.updateMesh(scene);
			}
		}
	} catch (...) { }
}

/**
 * シーンのすべての形状のMorph Targets情報のウエイト値を戻す.
 */
void CMorphTargetsCtrl::popAllWeight (sxsdk::scene_interface* scene)
{
	if (g_shapeWeightCache.empty()) return;

	const std::vector<CMorphTargetsWeightCache>& wCache = g_shapeWeightCache.back();
	try {
		const size_t shapeCou = wCache.size();
		for (size_t i = 0; i < shapeCou; ++i) {
			const CMorphTargetsWeightCache& weightC = wCache[i];
			sxsdk::shape_class* shape = scene->get_shape_by_handle(weightC.shapeHandle);
			if (!shape) continue;

			CMorphTargetsCtrl targetC;
			StreamCtrl::readMorphTargetsData(*shape, targetC);
			const int tCou = targetC.getTargetsCount();
			for (int j = 0; j < tCou; ++j) {
				targetC.setTargetWeight(j, weightC.weights[j]);
			}
			StreamCtrl::writeMorphTargetsData(*shape, targetC);
			targetC.updateMesh(scene);
		}
	} catch (...) { }

	g_shapeWeightCache.pop_back();
}

/**
 * 指定の形状の(Cacheでの)カレントウエイト値を取得.
 * @param[in]  tIndex    Morph Targets番号.
 * @param[out] weights   targetごとのウエイト値が返る.
 */
bool CMorphTargetsCtrl::getShapeCurrentWeights (const sxsdk::shape_class* shape, std::vector<float>& weights)
{
	if (!shape) return false;
	if (g_shapeWeightCache.empty()) return false;

	const std::vector<CMorphTargetsWeightCache>& wCache = g_shapeWeightCache.back();

	try {
		bool ret = false;
		const size_t shapeCou = g_shapeWeightCache.size();
		for (size_t i = 0; i < shapeCou; ++i) {
			const CMorphTargetsWeightCache& weightC = wCache[i];
			if (shape->get_handle() == weightC.shapeHandle) {
				const size_t tCou = weightC.weights.size();
				weights.resize(tCou);
				for (size_t j = 0; j < tCou; ++j) weights[j] = weightC.weights[j];
				ret = true;
				break;
			}
		}
		return ret;
	} catch (...) { }

	return false;
}

/**
 * 頂点が移動、回転する場合に仮想的なpivot(これはバウンディングボックスの中心座標)でどれだけ移動/回転するか推定し、.
 * stream内の情報を更新.
 * @return 頂点が正しく更新された場合はtrue。頂点数が変わったなど、更新できない場合はfalseを返す.
 */
bool CMorphTargetsCtrl::m_updateMeshVertices ()
{
	if (!m_pTargetShape) return false;
	if (m_pTargetShape->get_type() != sxsdk::enums::polygon_mesh) return false;
	if (m_orgVertices.size() != (m_pTargetShape->get_total_number_of_control_points())) return false;

	try {
		compointer<sxsdk::scene_interface> scene(m_pTargetShape->get_scene_interface());
		sxsdk::polygon_mesh_class& pMesh = m_pTargetShape->get_polygon_mesh();
		const int versCou = pMesh.get_total_number_of_control_points();

		// 変換用の前処理計算.
		// これは、Morph Targetsの変形の影響を受けない頂点座標を元に、変換後の姿勢を求めるための計算.
		CCalcMeshTransform meshTransC;
		if (!meshTransC.calcMeshTransform(m_pTargetShape)) return false;

		// 変換の必要がない場合.
		if (!meshTransC.hasTransform()) return false;

		// ベース座標値を更新.
		for (int i = 0; i < versCou; ++i) {
			const sxsdk::vec3 v = m_orgVertices[i];
			m_orgVertices[i] = meshTransC.calcMeshPos(v);		// ベース座標を、現在のメッシュの座標上に変換.
		}

		// 各targetごとの座標値を更新.
		const int targetsCou = this->getTargetsCount();
		for (int loop = 0; loop < targetsCou; ++loop) {
			CMorphTargetsData& targetsD = m_morphTargetsData[loop];
			const size_t vCou = targetsD.vertices.size();
			for (int i = 0; i < vCou; ++i) {
				targetsD.vertices[i] = meshTransC.calcMeshPos(targetsD.vertices[i]);
			}
		}

		// streamを更新.
		StreamCtrl::writeMorphTargetsData(*m_pTargetShape, *this);
	} catch (...) { }

	return true;
}

//---------------------------------------------------------------.
// Stream保存/読み込み用.
//---------------------------------------------------------------.
/**
 * Morph Targets情報を持つか.
 */
bool CMorphTargetsCtrl::hasMorphTargets (sxsdk::shape_class& shape)
{
	return StreamCtrl::hasMorphTargetsData(shape);
}

/**
 * 現在のMorph Targets情報をstreamに保存.
 */
void CMorphTargetsCtrl::writeMorphTargetsData ()
{
	if (!m_pTargetShape) return;

	StreamCtrl::writeMorphTargetsData(*m_pTargetShape, *this);
}

/**
 * Morph Targets情報をstreamから読み込み.
 */
bool CMorphTargetsCtrl::readMorphTargetsData (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::polygon_mesh) return false;

	const int oldSelectIndex = m_selectTargetIndex;
	const bool ret = StreamCtrl::readMorphTargetsData(shape, *this);
	const int tCou = this->getTargetsCount();
	if (oldSelectIndex < 0 || oldSelectIndex >= tCou) m_selectTargetIndex = -1;

	return ret;
}

//---------------------------------------------------------------.
// UI用.
//---------------------------------------------------------------.
/**
 * 指定のTargetを選択状態にする。-1の場合は選択なし.
 */
void CMorphTargetsCtrl::setSelectTargetIndex (const int tIndex)
{
	m_selectTargetIndex = tIndex;
}

/**
 * 選択されているTarget番号を取得.
 */
int CMorphTargetsCtrl::getSelectTargetIndex () const
{
	return m_selectTargetIndex;
}
