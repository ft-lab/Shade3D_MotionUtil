/**
 * ポリゴンメッシュの頂点の関係が変わらないまま(剛体的に)移動や回転が行われたときの.
 * 変換要素を推定する.
 */
#ifndef _CALCMESHTRANSFORM_H
#define _CALCMESHTRANSFORM_H

#include "GlobalHeader.h"

class CCalcMeshTransform
{
private:
	sxsdk::vec3 m_srcCenterPos;			// 元形状での回転の中心座標.
	sxsdk::vec3 m_dstCenterPos;			// カレント形状での回転の中心座標.

	sxsdk::mat4 m_rotM1;				// 回転行列1.
	sxsdk::mat4 m_rotM2;				// 回転行列2.

public:
	CCalcMeshTransform ();

	/**
	 * 指定のポリゴンメッシュでMorph Targets情報を持つ場合、元の位置から現在位置での変換行列を計算.
	 */
	bool calcMeshTransform (sxsdk::shape_class* shape);

	/**
	 * 変換の必要があるか.
	 */
	bool hasTransform ();

	/**
	 * Morph Targets上のベース座標上での位置より、変換後の位置を計算.
	 * @param[in]  vPos  ベース座標上での位置.
	 * @return 変換後の位置.
	 */
	sxsdk::vec3 calcMeshPos (const sxsdk::vec3& vPos);
};

#endif
