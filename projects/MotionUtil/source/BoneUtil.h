/**
 * ボーン操作関数.
 */
#ifndef _BONEUTIL_H
#define _BONEUTIL_H

#include "GlobalHeader.h"

namespace BoneUtil
{
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
}

#endif
