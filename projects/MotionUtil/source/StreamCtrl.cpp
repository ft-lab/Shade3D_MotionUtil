/**
 * streamに情報を保存.
 */
#include "StreamCtrl.h"

/**
 * Morph Targets情報を削除.
 */
void StreamCtrl::removeMorphTargetsData (sxsdk::shape_class& shape)
{
	try {
		shape.delete_attribute_with_uuid(MORPH_TARGETS_STREAM_ID);
	} catch (...) { }
}

/**
 * Morph Targets情報を保存.
 */
void StreamCtrl::writeMorphTargetsData (sxsdk::shape_class& shape, CMorphTargetsCtrl& data)
{
	try {
		compointer<sxsdk::stream_interface> stream(shape.create_attribute_stream_interface_with_uuid(MORPH_TARGETS_STREAM_ID));
		if (!stream) return;

		stream->set_size(0);
		stream->set_pointer(0);

		int iVersion = MORPH_TARGETS_STREAM_VERSION;
		stream->write_int(iVersion);

		const std::vector<sxsdk::vec3>& orgVertices = data.getOrgVertices();
		{
			const int cou = (int)orgVertices.size();
			stream->write_int(cou);
			for (int i = 0; i < cou; ++i) {
				stream->write_float(orgVertices[i].x);
				stream->write_float(orgVertices[i].y);
				stream->write_float(orgVertices[i].z);
			}
		}
		{
			char szName[130];
			const int targetsCou = data.getTargetsCount();
			stream->write_int(targetsCou);
			for (int loop = 0; loop < targetsCou; ++loop) {
				const CMorphTargetsData& morphD = data.getMorphTargetData(loop);
				memset(szName, 0, 128);
				strcpy(szName, morphD.name.c_str());
				stream->write(128, szName);

				{
					const int cou = (int)morphD.vIndices.size();
					stream->write_int(cou);
					for (int i = 0; i < cou; ++i) {
						stream->write_int(morphD.vIndices[i]);
					}
				}
				{
					const int cou = (int)morphD.vertices.size();
					stream->write_int(cou);
					for (int i = 0; i < cou; ++i) {
						stream->write_float(morphD.vertices[i].x);
						stream->write_float(morphD.vertices[i].y);
						stream->write_float(morphD.vertices[i].z);
					}
				}

				stream->write_float(morphD.weight);
			}
		}

	} catch (...) { }
}

/**
 * Morph Targets情報を持つか.
 */
bool StreamCtrl::hasMorphTargetsData (sxsdk::shape_class& shape)
{
	if (shape.get_type() != sxsdk::enums::polygon_mesh) return false;

	try {
		compointer<sxsdk::stream_interface> stream(shape.get_attribute_stream_interface_with_uuid(MORPH_TARGETS_STREAM_ID));
		if (!stream) return false;

		stream->set_pointer(0);

		int iVersion;
		stream->read_int(iVersion);

		return true;

	} catch (...) { }

	return false;
}

/**
 * Morph Targets情報を読み込み.
 */
bool StreamCtrl::readMorphTargetsData (sxsdk::shape_class& shape, CMorphTargetsCtrl& data)
{
	data.clear();

	if (shape.get_type() != sxsdk::enums::polygon_mesh) return false;

	try {
		compointer<sxsdk::stream_interface> stream(shape.get_attribute_stream_interface_with_uuid(MORPH_TARGETS_STREAM_ID));
		if (!stream) return false;

		stream->set_pointer(0);

		int iVersion;
		stream->read_int(iVersion);

		data.setupShape(&shape);
		
		std::vector<sxsdk::vec3> orgVertices;
		{
			int cou;
			stream->read_int(cou);
			if (cou > 0) orgVertices.resize(cou);
			for (int i = 0; i < cou; ++i) {
				stream->read_float(orgVertices[i].x);
				stream->read_float(orgVertices[i].y);
				stream->read_float(orgVertices[i].z);
			}
		}
		data.setOrgVertices(orgVertices);
		
		{
			char szName[130];
			int targetsCou;
			stream->read_int(targetsCou);
			for (int loop = 0; loop < targetsCou; ++loop) {
				stream->read(128, szName);

				std::vector<sxsdk::vec3> vList;
				std::vector<int> vIndices;
				int cou;
				{
					stream->read_int(cou);
					if (cou > 0) vIndices.resize(cou);
					for (int i = 0; i < cou; ++i) {
						stream->read_int(vIndices[i]);
					}
				}
				{
					stream->read_int(cou);
					if (cou > 0) vList.resize(cou);
					for (int i = 0; i < cou; ++i) {
						stream->read_float(vList[i].x);
						stream->read_float(vList[i].y);
						stream->read_float(vList[i].z);
					}
				}
				float weight;
				stream->read_float(weight);

				if (cou > 0) {
					const int tIndex = data.appendTargetVertices(szName, vIndices, vList);
					data.setTargetWeight(tIndex, weight);
				}
			}
		}
		return true;

	} catch (...) { }

	return false;
}

