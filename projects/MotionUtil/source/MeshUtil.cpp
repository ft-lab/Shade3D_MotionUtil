/**
 * Mesh操作関数.
 */

#include "MeshUtil.h"

/**
 * アクティブなシーンで選択されているポリゴンメッシュを取得.
 */
sxsdk::shape_class* MeshUtil::getActivePolygonMesh (sxsdk::shade_interface& shade)
{
	try {
		compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
		if (!scene) return NULL;
		sxsdk::shape_class& shape = scene->active_shape();
		if (shape.get_type() != sxsdk::enums::polygon_mesh) return NULL;
		return &shape;
	} catch (...) { }
	return NULL;
}

/**
 * 指定のポリゴンメッシュ形状せ選択頂点番号を取得.
 */
std::vector<int> MeshUtil::getActiveVerticesInMesh (sxsdk::shape_class& shape)
{
	std::vector<int> vIndices;
	try {
		if (shape.get_type() != sxsdk::enums::polygon_mesh) return vIndices;
		sxsdk::polygon_mesh_class& pMesh = shape.get_polygon_mesh();
		const int activeCou = pMesh.get_number_of_active_control_points();
		vIndices.resize(activeCou);
		pMesh.get_active_vertex_indices(&vIndices[0]);

	} catch (...) { }
	return vIndices;
}

/**
 * 指定の頂点インデックスでの頂点座標を取得.
 */
std::vector<sxsdk::vec3> MeshUtil::getMeshVertex (sxsdk::shape_class& shape, const std::vector<int>& indices)
{
	std::vector<sxsdk::vec3> vers;
	if (indices.empty()) return vers;

	try {
		if (shape.get_type() != sxsdk::enums::polygon_mesh) return vers;

		const int cou = (int)indices.size();
		vers.resize(cou);

		sxsdk::polygon_mesh_class& pMesh = shape.get_polygon_mesh();
		sxsdk::polygon_mesh_saver_class* saver = pMesh.get_polygon_mesh_saver();
		for (int i = 0; i < cou; ++i) {
			vers[i] = saver->get_point(indices[i]);
		}
		saver->release();
	} catch (...) { }
	return vers;
}
