/**
 * ボーンの便利機能を提供するインターフェース.
 */
#include "HiddenBoneUtilInterface.h"
#include "BoneUtil.h"

CHiddenBoneUtilInterface::CHiddenBoneUtilInterface (sxsdk::shade_interface& shade) : shade(shade)
{
}

CHiddenBoneUtilInterface::~CHiddenBoneUtilInterface ()
{
}

/**
 * 指定の形状がボーンかどうか.
 */
bool CHiddenBoneUtilInterface::isBone (sxsdk::shape_class& shape)
{
	return BoneUtil::isBone(shape);
}

/**
 * ボーンのワールド座標での中心位置とボーンサイズを取得.
 */
sxsdk::vec3 CHiddenBoneUtilInterface::getBoneCenter (sxsdk::shape_class& shape, float *size)
{
	return BoneUtil::getBoneCenter(shape, size);
}

/**
 * ボーンの向きをそろえる.
 * @param[in] boneRoot  対象のボーンルート.
 */
void CHiddenBoneUtilInterface::adjustBonesDirection (sxsdk::shape_class* boneRoot)
{
	return BoneUtil::adjustBonesDirection(boneRoot);
}

/**
 * ボーンサイズの自動調整.
 * @param[in] boneRoot  対象のボーンルート.
 */
void CHiddenBoneUtilInterface::resizeBones (sxsdk::shape_class* boneRoot)
{
	return BoneUtil::resizeBones(boneRoot);
}
