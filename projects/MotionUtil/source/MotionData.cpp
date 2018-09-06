/**
 * モーションウィンドウをラップする、MotionUtilとしての情報.
 */

#include "MotionData.h"

//------------------------------------------------------------------.
MotionUtil::CMotionGroupKeyFrameBase::CMotionGroupKeyFrameBase ()
{
	clear();
}

void MotionUtil::CMotionGroupKeyFrameBase::clear ()
{
	timeSec = 0.0f;
	type    = MotionUtil::keyframe_type_none;
}

//------------------------------------------------------------------.
MotionUtil::CMotionGroupKeyFrameBallBoneJoint::CMotionGroupKeyFrameBallBoneJoint () : MotionUtil::CMotionGroupKeyFrameBase()
{
	clear();
}

void MotionUtil::CMotionGroupKeyFrameBallBoneJoint::clear ()
{
	offset   = sxsdk::vec3(0, 0, 0);
	rotation = sxsdk::quaternion_class::identity;
	MotionUtil::CMotionGroupKeyFrameBase::clear();
}

//------------------------------------------------------------------.
MotionUtil::CMotionGroupKeyFrameMorphTargets::CMotionGroupKeyFrameMorphTargets () : MotionUtil::CMotionGroupKeyFrameBase()
{
	clear();
}

void MotionUtil::CMotionGroupKeyFrameMorphTargets::clear ()
{
	weight = 0.0f;
	MotionUtil::CMotionGroupKeyFrameBase::clear();
}

//------------------------------------------------------------------.
MotionUtil::CMotionGroup::CMotionGroup ()
{
}