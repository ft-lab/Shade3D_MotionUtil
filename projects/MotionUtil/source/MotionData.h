/**
 * モーションウィンドウをラップする、MotionUtilとしての情報.
 */
#ifndef _MOTIONDATA_H
#define _MOTIONDATA_H

#include "GlobalHeader.h"
#include <vector>

namespace MotionUtil
{
	/**
	 * キーフレームの形状による種類.
	 */
	enum MOTION_KEYFRAME_TYPE {
		keyframe_type_none = 0,					// 未指定.
		keyframe_type_ball_bone_joint,			// ボールまたはボーンジョイント.
		keyframe_type_morph_targets,			// Morph Targets.
	};

	/**
	 * MotionGroupでのキーフレームデータのベースクラス.
	 */
	class CMotionGroupKeyFrameBase
	{
	public:
		float timeSec;		// 秒単位のキーフレーム位置での時間.
		MotionUtil::MOTION_KEYFRAME_TYPE type;		// キーフレームの種類.

	public:
		CMotionGroupKeyFrameBase ();

		void clear ();
	};

	/*
	 * MotionGroupでのボーン/ボールジョイントでのキーフレームデータ.
	 */
	class CMotionGroupKeyFrameBallBoneJoint : public CMotionGroupKeyFrameBase
	{
	public:
		sxsdk::vec3 offset;					// ボーン/ボールジョイントでのOffset値.
		sxsdk::quaternion_class rotation;	// ボーン/ボールジョイントでのRotation値.

	public:
		CMotionGroupKeyFrameBallBoneJoint ();

		void clear ();
	};

	/*
	 * MotionGroupでのMorph Targetsでのキーフレームデータ.
	 */
	class CMotionGroupKeyFrameMorphTargets : public CMotionGroupKeyFrameBase
	{
	public:
		float weight;						// Morph Targetでのウエイト値.

	public:
		CMotionGroupKeyFrameMorphTargets ();

		void clear ();
	};


	/**
	 * MotionGroupは、手を開く/腕を振る、などの動きの最小単位.
	 * これらを組み合わせてモーションを構成できる.
	 */
	class CMotionGroup
	{
	public:
		std::vector<sxsdk::shape_class *> shapes;		// 対象のボーン/ボールジョイント形状.
														// shapes[0]のジョイントの子が格納される.

	public:
		CMotionGroup ();
	};
}


#endif
