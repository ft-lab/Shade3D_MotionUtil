/**
 * ボーン操作関数.
 */
#include "BoneUtil.h"

namespace {
	/**
	 * ボーンの向きをそろえる再帰.
	 */
	void adjustBonesDirectionLoop (sxsdk::shape_class& shape) {
		if (!BoneUtil::isBone(shape)) return;

		const sxsdk::mat4 lwMat = shape.get_transformation() * shape.get_local_to_world_matrix();
		const sxsdk::mat4 wlMat = inv(lwMat);

		bool firstF = true;
		if (shape.has_son()) {
			sxsdk::part_class* pPart   = &shape.get_part();
			sxsdk::shape_class* pShape = shape.get_son();
			while (pShape->has_bro()) {
				pShape = pShape->get_bro();

				if (firstF) {
					firstF = false;
					if (BoneUtil::isBone(*pShape)) {
						const sxsdk::vec3 v0 = BoneUtil::getBoneCenter(shape, NULL) * wlMat;
						const sxsdk::vec3 v1 = BoneUtil::getBoneCenter(*pShape, NULL) * wlMat;

						const sxsdk::vec3 vDir = normalize(v1 - v0);
						try {
							compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
							if (!(bone->get_auto_direction())) {
								bone->set_axis_dir(vDir);
							}
						} catch (...) { }
					}
				}
				adjustBonesDirectionLoop(*pShape);
			}
		}
	}

	/**
	 * ボーンの大きさを調整する再帰.
	 */
	void resizeBonesLoop (sxsdk::shape_class& shape, sxsdk::shape_class* prevShape) {
		if (!BoneUtil::isBone(shape)) return;

		float size = 0.0f;

		sxsdk::vec3 center = BoneUtil::getBoneCenter(shape, &size);

		{
			const float scale = 1.0f;
			float dist = 0.0f;
			float sizePrev = size;
			sxsdk::vec3 centerPrev = center;
			if (prevShape) {
				centerPrev = BoneUtil::getBoneCenter(*prevShape, &sizePrev);
				dist = sxsdk::distance3(centerPrev, center);
			}
			if (dist > 0.0f) {
				size = (dist / 8.0f) * scale;
				try {
					compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
					bone->set_size(size);
				} catch (...) { }
				if (prevShape) {
					try {
						compointer<sxsdk::bone_joint_interface> bone(prevShape->get_bone_joint_interface());
						bone->set_size(size);
					} catch (...) { }
				}
			}
		}

		if (shape.has_son()) {
			sxsdk::shape_class* pShape = shape.get_son();
			while (pShape->has_bro()) {
				pShape = pShape->get_bro();
				resizeBonesLoop(*pShape, &shape);
			}
		}
	}
}

/**
 * 指定の形状がボーンかどうか.
 */
bool BoneUtil::isBone (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::part) return false;
	sxsdk::part_class& part = shape.get_part();
	if (part.get_part_type() == sxsdk::enums::bone_joint) return true;
	return false;
}

/**
 * ボーンのワールド座標での中心位置とボーンサイズを取得.
 */
sxsdk::vec3 BoneUtil::getBoneCenter (sxsdk::shape_class& shape, float *size)
{
	if (size) *size = 0.0f;
	if (!BoneUtil::isBone(shape)) return sxsdk::vec3(0, 0, 0);

	// シーケンスOff時の中心位置を取得する.
	// この場合は、bone->get_matrix()を使用.
	// shape.get_transformation() を取得すると、これはシーケンスOn時の変換行列になる.
	try {
		compointer<sxsdk::bone_joint_interface> bone(shape.get_bone_joint_interface());
		const sxsdk::mat4 m = bone->get_matrix();

		const sxsdk::mat4 lwMat = shape.get_local_to_world_matrix();
		const sxsdk::vec3 center = sxsdk::vec3(0, 0, 0) * m * lwMat;

		if (size) *size = bone->get_size();

		return center;
	} catch (...) { }

	return sxsdk::vec3(0, 0, 0);
}

/**
 * ボーンの向きをそろえる.
 * @param[in] boneRoot  対象のボーンルート.
 */
void BoneUtil::adjustBonesDirection (sxsdk::shape_class* boneRoot)
{
	::adjustBonesDirectionLoop(*boneRoot);
}

/**
 * ボーンサイズの自動調整.
 * @param[in] boneRoot  対象のボーンルート.
 */
void BoneUtil::resizeBones (sxsdk::shape_class* boneRoot)
{
	::resizeBonesLoop(*boneRoot, NULL);
}
