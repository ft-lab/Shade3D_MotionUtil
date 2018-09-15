/**
 * ポリゴンメッシュの頂点の関係が変わらないまま(剛体的に)移動や回転が行われたときの.
 * 変換要素を推定する.
 */
#include "CalcMeshTransform.h"
#include "MorphTargetsCtrl.h"
#include "StreamCtrl.h"
#include "MathUtil.h"

CCalcMeshTransform::CCalcMeshTransform ()
{
}

/**
 * 指定のポリゴンメッシュでMorph Targets情報を持つ場合、元の位置から現在位置での変換行列を計算.
 */
bool CCalcMeshTransform::calcMeshTransform (sxsdk::shape_class* shape)
{
	if (shape->get_type() != sxsdk::enums::polygon_mesh) return false;
	CMorphTargetsCtrl morphCtrl;
	if (!StreamCtrl::readMorphTargetsData(*shape, morphCtrl)) return false;
	const int targetsCou = morphCtrl.getTargetsCount();

	try {
		sxsdk::polygon_mesh_class& pMesh = shape->get_polygon_mesh();
		const int versCou = pMesh.get_total_number_of_control_points();
		const std::vector<sxsdk::vec3>& orgVertices = morphCtrl.getOrgVertices();
		if (versCou != (int)orgVertices.size()) return false;

		// Morph Targetsとして使用していて、ウエイト値が0.0でない頂点は対象から外すためのフラグ.
		std::vector<bool> useIList;
		useIList.resize(versCou, true);
		for (int i = 0; i < targetsCou; ++i) {
			const float weight = morphCtrl.getTargetWeight(i);
			if (MathUtil::isZero(weight)) continue;

			const CMorphTargetsData& targetD = morphCtrl.getMorphTargetData(i);
			const size_t vCou = targetD.vIndices.size();
			for (size_t j = 0; j < vCou; ++j) {
				useIList[ targetD.vIndices[j] ] = false;
			}
		}

		// サンプリング用の頂点座標.
		sxsdk::polygon_mesh_saver_class* pMeshSaver = pMesh.get_polygon_mesh_saver();
		std::vector<sxsdk::vec3> tSrcVertices, tDstVertices;
		for (int i = 0; i < versCou; ++i) {
			if (useIList[i]) {
				tSrcVertices.push_back(orgVertices[i]);
				tDstVertices.push_back(pMeshSaver->get_point(i));
			}
		}
		pMeshSaver->release();
		const int tVersCou = (int)tSrcVertices.size();
		if (tVersCou < 3) return false;

		sxsdk::vec3 bbMin, bbMax;
		MathUtil::calcBoundingBox(tSrcVertices, bbMin, bbMax);

		// 回転の中心とする座標を取得.
		// これは、bbMinに一番近い頂点を採用とする.
		int centerPosI = -1;
		{
			float minDist = 0.0f;
			for (int i = 0; i < tVersCou; ++i) {
				const sxsdk::vec3& v = tSrcVertices[i];
				const float dist = sxsdk::absolute(v - bbMin);
				if (centerPosI < 0 || dist < minDist) {
					centerPosI = i;
					minDist = dist;
				}
			}
		}
		if (centerPosI < 0) return false;

		// bbMaxに一番近い頂点を回転の先の頂点とする.
		int tPosI = -1;
		{
			float minDist = 0.0f;
			for (int i = 0; i < tVersCou; ++i) {
				if (i == centerPosI) continue;
				const sxsdk::vec3& v = tSrcVertices[i];
				const float dist = sxsdk::absolute(v - bbMax);
				if (tPosI < 0 || dist < minDist) {
					tPosI = i;
					minDist = dist;
				}
			}
		}
		if (tPosI < 0) return false;

		const sxsdk::vec3 vSrcPos0 = tSrcVertices[centerPosI];
		const sxsdk::vec3 vSrcPos1 = tSrcVertices[tPosI];
		const sxsdk::vec3 vDstPos0 = tDstVertices[centerPosI];
		const sxsdk::vec3 vDstPos1 = tDstVertices[tPosI];

		m_srcCenterPos = vSrcPos0;
		m_dstCenterPos = vDstPos0;
		m_rotM1 = sxsdk::mat4::identity;
		m_rotM2 = sxsdk::mat4::identity;

		// tSrcVertices[] の vSrcPos0 - vSrcPos1 を軸として、.
		// tDstVertices[]での対応点 vDstPos0 - vDstPos1 への回転行列を計算.
		const sxsdk::vec3 dSrcV = normalize(vSrcPos1 - vSrcPos0);
		const sxsdk::vec3 dDstV = normalize(vDstPos1 - vDstPos0);
		const sxsdk::mat4 rotM1 = sxsdk::mat4::rotate(dSrcV, dDstV);
		m_rotM1 = rotM1;

		// tSrcVertices[]内で、centerPosIやtPosIではなくdSrcVと平行でない頂点を取得.
		int tPosI2 = -1;
		const float maxAngleV = 0.95f;
		for (int i = 0; i < tVersCou; ++i) {
			if (i == centerPosI || i == tPosI) continue;
			const sxsdk::vec3& v = tSrcVertices[i];
			const float dist = sxsdk::absolute(v - vSrcPos0);
			if (MathUtil::isZero(dist)) continue;
			const sxsdk::vec3 eSrcV = (v - vSrcPos0) / dist;
			if (sx::inner_product(eSrcV, dSrcV) > maxAngleV) continue;

			tPosI2 = i;
			break;
		}
		if (tPosI2 < 0) return false;

		const sxsdk::vec3 srcV1 = (tSrcVertices[tPosI2] - vSrcPos0) * m_rotM1;
		const sxsdk::vec3 dstV1 = (tDstVertices[tPosI2] - vDstPos0);

		// dDstVの軸回りにsrcV1を回転させて、dstV1の位置に収束する回転行列を収束で計算.
		float lastAngle = 0.0f;
		{
			const int divCou = 8;
			float angleList[8];
			sxsdk::mat4 rotMList[8];
			float minAngle = 0.0f;
			float maxAngle = sx::pi * 2.0f;
			const float exitAngleV = 0.1f * sx::pi / 180.0f;

			for (int loop = 0; loop < 20; ++loop) {
				const float dAngle = (maxAngle - minAngle) / (float)divCou;
				float angleP = minAngle;
				for (int i = 0; i < divCou; ++i) {
					angleList[i] = angleP;
					rotMList[i]  = sxsdk::mat4::rotate(dDstV, angleP);
					angleP += dAngle;
				}

				int minIndex = -1;
				float minLen = 0.0f;
				for (int i = 0; i < divCou; ++i) {
					const sxsdk::vec3 v = srcV1 * rotMList[i];
					const float len = sxsdk::absolute(v - dstV1);
					if (minIndex < 0 || minLen > len) {
						minIndex = i;
						minLen   = len;
					}
				}
				lastAngle = angleList[minIndex];

				const float dAngleH = (maxAngle - minAngle) * 0.2f;
				if (dAngleH < exitAngleV) break;
				minAngle = lastAngle - dAngleH;
				maxAngle = lastAngle + dAngleH;
			}
		}
		m_rotM2 = sxsdk::mat4::rotate(dDstV, lastAngle);

		return true;
	} catch (...) { }

	return false;
}

/**
 * 変換の必要があるか.
 */
bool CCalcMeshTransform::hasTransform ()
{
	if (!MathUtil::isZero(m_srcCenterPos - m_dstCenterPos)) return true;
	if (!MathUtil::isIdentity(m_rotM1)) return true;
	if (!MathUtil::isIdentity(m_rotM2)) return true;
	return false;
}

/**
 * Morph Targets上のベース座標上での位置より、変換後の位置を計算.
 * @param[in]  vPos  ベース座標上での位置.
 * @return 変換後の位置.
 */
sxsdk::vec3 CCalcMeshTransform::calcMeshPos (const sxsdk::vec3& vPos)
{
	const sxsdk::vec3 srcP1 = (vPos - m_srcCenterPos) * m_rotM1;
	return (srcP1 * m_rotM2) + m_dstCenterPos;
}
