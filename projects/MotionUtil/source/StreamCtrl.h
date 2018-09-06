/**
 * streamに情報を保存.
 */

#ifndef _STREAMCTRL_H
#define _STREAMCTRL_H

#include "GlobalHeader.h"
#include "MorphTargetsCtrl.h"

namespace StreamCtrl
{
	/**
	 * Morph Targets情報を保存.
	 */
	void writeMorphTargetsData (sxsdk::shape_class& shape, const CMorphTargetsCtrl& data);

	/**
	 * Morph Targets情報を読み込み.
	 */
	bool readMorphTargetsData (sxsdk::shape_class& shape, CMorphTargetsCtrl& data);

	/**
	 * Morph Targets情報を持つか.
	 */
	bool hasMorphTargetsData (sxsdk::shape_class& shape);

	/**
	 * Morph Targets情報を削除.
	 */
	void removeMorphTargetsData (sxsdk::shape_class& shape);
}

#endif
