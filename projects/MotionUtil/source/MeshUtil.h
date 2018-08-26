/**
 * Mesh操作関数.
 */

#ifndef _MESHUTIL_H
#define _MESHUTIL_H

#include "GlobalHeader.h"

namespace MeshUtil
{
	/**
	 * アクティブなシーンで選択されているポリゴンメッシュを取得.
	 */
	sxsdk::shape_class* getActivePolygonMesh (sxsdk::shade_interface& shade);

	/**
	 * 指定のポリゴンメッシュ形状せ選択頂点番号を取得.
	 */
	std::vector<int> getActiveVerticesInMesh (sxsdk::shape_class& shape);

	/**
	 * 指定の頂点インデックスでの頂点座標を取得.
	 */
	std::vector<sxsdk::vec3> getMeshVertex (sxsdk::shape_class& shape, const std::vector<int>& indices);
}

#endif
