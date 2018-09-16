/**
 * Morphウィンドウ.
 */
#include "MorphWindowInterface.h"
#include "StreamCtrl.h"
#include "MeshUtil.h"
#include "RenameDialog.h"

// ボタンのウィジットの高さ.
#define BUTTONS_WIDGHT_HEIGHT  110

//--------------------------------------------------------------.
CButtonsWidget::CButtonsWidget (CMorphWindowInterface* pParent) : sxsdk::window_interface(*pParent, 0), m_pParent(pParent)
{
	m_pRemoveRestoreCheckBox = NULL;
	m_pSetupMorphTargetsBut = NULL;
	m_pAppendTargetBut = NULL;
	m_pRemoveMorphTargetsBut = NULL;
	m_pSelectVerticesBut = NULL;
	m_pClearAllWeightsBut = NULL;
}

/**
 * 位置とサイズの指定.
 */
void CButtonsWidget::set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size)
{
	this->set_frame_position(pos);
	this->set_client_size(size);
}

/**
 * リサイズイベント.
 */
void CButtonsWidget::resize (int x, int y, bool remake, void*)
{
	sxsdk::window_interface::resize(x, y, remake);
}

bool CButtonsWidget::setup_push_button (sxsdk::window_interface::push_button_class &push_button, void *)
{
	const std::string name(push_button.get_control_idname());
	if (name == "setup_target_but") {
		if (!m_pSetupMorphTargetsBut) m_pSetupMorphTargetsBut = &push_button;
		return true;
	}
	if (name == "append_target_but") {
		if (!m_pAppendTargetBut) m_pAppendTargetBut = &push_button;
		return true;
	}
	if (name == "remove_target_but") {
		if (!m_pRemoveMorphTargetsBut) m_pRemoveMorphTargetsBut = &push_button;
		return true;
	}
	if (name == "select_target_vertices_but") {
		if (!m_pSelectVerticesBut) m_pSelectVerticesBut = &push_button;
		return true;
	}
	if (name == "clear_weights_but") {
		if (!m_pClearAllWeightsBut) m_pClearAllWeightsBut = &push_button;
		return true;
	}

	return false;
}

bool CButtonsWidget::setup_checkbox (sxsdk::window_interface::checkbox_class &checkbox, void *)
{
	const std::string name(checkbox.get_control_idname());
	if (name == "remove_target_restore_check") {
		if (!m_pRemoveRestoreCheckBox) m_pRemoveRestoreCheckBox = &checkbox;
		return true;
	}

	return m_pParent->setup_checkbox(checkbox);
}

void CButtonsWidget::push_button_clicked (sxsdk::window_interface::push_button_class &push_button, void *)
{
	const std::string name(push_button.get_control_idname());

	if (name == "setup_target_but") {		// Morph Target情報を割り当て.
		m_pParent->setupMorphTargetData();
	}

	if (name == "append_target_but") {		// Morph Target情報を追加登録.
		m_pParent->appendMorphTargetData();
	}

	if (name == "clear_weights_but") {		// ウエイト値をすべてクリア.
		m_pParent->clearAllWeights();
	}

	if (name == "remove_target_but") {		// Morph Target情報を削除.
		m_pParent->removeMorphTargetsData();
	}

	if (name == "select_target_vertices_but") {		// Morph Target対象の頂点を選択.
		m_pParent->selectTargetVertices();
	}
}

void CButtonsWidget::checkbox_value_changed (sxsdk::window_interface::checkbox_class &checkbox, void *)
{
	m_pParent->checkbox_value_changed(checkbox);
}

/**
 * UIを更新.
 */
void CButtonsWidget::updateUI ()
{
	CMorphTargetsCtrl& morphTargetsD = m_pParent->getMorphTargetsCtrl();

	// Morph Targets情報を持つか.
	const bool hasMorphTargets = (morphTargetsD.getTargetShape() != NULL);

	if (m_pSetupMorphTargetsBut) {
		m_pSetupMorphTargetsBut->set_active(!hasMorphTargets);
		m_pSetupMorphTargetsBut->invalidate();
	}
	if (m_pAppendTargetBut) {
		m_pAppendTargetBut->set_active(hasMorphTargets);
		m_pAppendTargetBut->invalidate();
	}
	if (m_pSelectVerticesBut) {
		m_pSelectVerticesBut->set_active(hasMorphTargets);
		m_pSelectVerticesBut->invalidate();
	}
	if (m_pRemoveMorphTargetsBut) {
		m_pRemoveMorphTargetsBut->set_active(hasMorphTargets);
		m_pRemoveMorphTargetsBut->invalidate();
	}
	if (m_pClearAllWeightsBut) {
		m_pClearAllWeightsBut->set_active(hasMorphTargets);
		m_pClearAllWeightsBut->invalidate();
	}

	if (m_pRemoveRestoreCheckBox) {
		m_pRemoveRestoreCheckBox->set_active(hasMorphTargets);
		m_pRemoveRestoreCheckBox->invalidate();
	}
}

//--------------------------------------------------------------.
CMorphWindowInterface::CMorphWindowInterface (sxsdk::shade_interface &shade) : sxsdk::window_interface(shade), shade(shade)
{
	m_pMorphTargetsWidget = NULL;
	m_pButtonsWidget = NULL;
	m_needUpdateMorph = false;
	m_needLoadMorph   = false;
	m_showRenameDialog = false;
	m_removeRestoreCheck = true;
}

/**
 * 初期化処理.
 */
void CMorphWindowInterface::initialize (void *)
{
	set_trigger(sxsdk::enums::trigger_enum(sxsdk::enums::active_scene_changed | sxsdk::enums::active_shapes_changed));

	m_msg_noModifyVertexMode    = shade.gettext("msg_no_modify_vertex_mode");
	m_msg_noSelectedVertices    = shade.gettext("msg_no_selected_vertices");
	m_msg_noSelectedPolygonmesh = shade.gettext("msg_no_selected_polygonmesh");

	int control_id = 1000;
	m_pMorphTargetsWidget = new CUIMorphTargetsWidget(this, control_id++);

	m_pButtonsWidget = new CButtonsWidget(this);
	m_pButtonsWidget->load_sxul("morph_button_window");

	this->set_title(CMorphWindowInterface::name(&shade));

	const int minWidth  = 320;
	const int minHeight = 280;
	this->set_minimum_size(sx::vec<int,2>(minWidth, minHeight));
	sx::vec<int,2> size = get_layout_bounds().size();
	if (size.x < minWidth) size.x = minWidth;
	if (size.y < minHeight) size.y = minHeight;
	this->set_client_size(size);
}

/**
 * 背景のペイントイベント.
 */
void CMorphWindowInterface::clear (sxsdk::graphic_context_interface &gc, void *)
{
	sxsdk::window_interface::clear(gc);

	sx::vec<int,2> clientSize = this->get_client_size();

	const int buttonsHeight = BUTTONS_WIDGHT_HEIGHT;
	const int targetsListHeight = clientSize.y - 24 - buttonsHeight;

	// widgetリストの外枠.
	{
		sx::vec<int,2> pos, size;

		pos.x = 7;
		pos.y = 7;
		size.x = clientSize.x - (8 + 8) + 2;
		size.y = targetsListHeight + 2;

		gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
		gc.frame_rectangle(sx::rectangle_class(pos, pos + size));
		gc.restore_color();
	}
}

/**
 * リサイズ時に呼ばれる.
 */
void CMorphWindowInterface::resize (int x, int y, bool remake, void *)
{
	sx::vec<int,2> pos, size;

	const int buttonsHeight = BUTTONS_WIDGHT_HEIGHT;
	const int targetsListHeight = y - 24 - buttonsHeight;
	pos.x = 8;
	pos.y = 8;
	size.x = x - (8 + 8);
	size.y = targetsListHeight;
	if (m_pMorphTargetsWidget) {
		m_pMorphTargetsWidget->set_bounds(pos, size);
	}

	if (m_pButtonsWidget) {
		pos  = sx::vec<int,2>(8, targetsListHeight + 16);
		size = sx::vec<int,2>(x - (8 + 8), buttonsHeight);
		m_pButtonsWidget->set_bounds(pos, size);
	}

	// 既存のresizeを呼ぶ.
	sxsdk::window_interface::resize(x, y, remake);
}

/**
 * 指定の位置が表示されているMorph Targetsウィンドウ内かチェック.
 * mouse_downイベントから呼ばれる.
 * @param[in] p   クリック位置.
 */
bool CMorphWindowInterface::chkInnerMorphTargetsList (const sx::vec<int,2>& p)
{
	if (!m_pMorphTargetsWidget) return false;
	const sx::rectangle_class rec = m_pMorphTargetsWidget->get_client_rectangle(true);
	if (p.x < rec.min.x || p.y < rec.min.y || p.x > rec.max.x || p.y > rec.max.y) return false;
	return true;
}

/**
 * シーンが変更されたときに呼ばれる.
 */
void CMorphWindowInterface::active_scene_changed (bool &b, sxsdk::scene_interface *scene, void *)
{
	if (!scene) {
		m_morphTargetsData.clear();
		m_updateUI();
		return;
	}

	// streamより、カレント形状のMorph Targets情報を遅延で取得.
	// Morph Targetsウィンドウが非表示の場合は、idle内で表示された段階で更新される.
	setNeedLoadMorph();
}

/**
 * 形状の選択が変更されたときに呼ばれる.
 */
void CMorphWindowInterface::active_shapes_changed (bool &b, sxsdk::scene_interface *scene, int old_n, sxsdk::shape_class *const *old_shapes, int n, sxsdk::shape_class *const *shapes, void *)
{
	if (!scene) {
		m_morphTargetsData.clear();
		m_updateUI();
		return;
	}

	// streamより、カレント形状のMorph Targets情報を遅延で取得.
	// Morph Targetsウィンドウが非表示の場合は、idle内で表示された段階で更新される.
	setNeedLoadMorph();
}

//------------------------------------------.
// setup時のコールバック.
//------------------------------------------.
bool CMorphWindowInterface::setup_checkbox (sxsdk::window_interface::checkbox_class &checkbox, void *)
{
	const std::string name(checkbox.get_control_idname());

	if (name == "remove_target_restore_check") {
		checkbox.set_value(m_removeRestoreCheck ? 1 : 0);
		return true;
	}
	return false;
}

//------------------------------------------.
// イベント処理のコールバック.
//------------------------------------------.
void CMorphWindowInterface::checkbox_value_changed (sxsdk::window_interface::checkbox_class &checkbox, void *)
{
	const std::string name(checkbox.get_control_idname());

	if (name == "remove_target_restore_check") {
		m_removeRestoreCheck = (checkbox.get_value() != 0);
	}
}

//-------------------------------------------------------------.
/**
 * Morph Target情報を割り当て開始.
 */
void CMorphWindowInterface::setupMorphTargetData ()
{
	// 選択されたポリゴンメッシュ形状を取得.
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (!shape) return;

	try {
		// 選択形状をMorph Targetsの対象として指定.
		m_morphTargetsData.setupShape(shape);

		// streamにMorph Targets情報を保存.
		StreamCtrl::writeMorphTargetsData(*shape, m_morphTargetsData);

		// UIの更新.
		m_updateUI();

	} catch (...) { }
}

/**
 * Morph Target情報を新たに追加.
 */
void CMorphWindowInterface::appendMorphTargetData ()
{
	// 選択されたポリゴンメッシュ形状を取得.
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (!shape) {
		shade.show_message_box(m_msg_noSelectedPolygonmesh.c_str(), false);
		return;
	}

	// まだsetupが呼ばれていない場合.
	if (m_morphTargetsData.getTargetShape() == NULL) {
		return;
	}

	try {
		// 形状編集モード＋頂点選択モードかチェック.
		compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
		if (!scene->is_modify_mode() || scene->get_selection_mode() != sxsdk::enums::vertex_selection_mode) {
			shade.show_message_box(m_msg_noModifyVertexMode.c_str(), false);
			return;
		}
		sxsdk::polygon_mesh_class& pMesh = shape->get_polygon_mesh();
		if (pMesh.get_number_of_active_control_points() == 0) {
			shade.show_message_box(m_msg_noSelectedVertices.c_str(), false);
			return;
		}

		// 選択された頂点インデックスを取得.
		std::vector<int> vIndices;
		vIndices = MeshUtil::getActiveVerticesInMesh(*shape);

		// 選択頂点をMorph Target情報として登録.
		const std::vector<sxsdk::vec3> vers = MeshUtil::getMeshVertex(*shape, vIndices);
		const int tIndex = m_morphTargetsData.appendTargetVertices("target", vIndices, vers);

		// 追加された要素を選択状態にする.
		m_morphTargetsData.setSelectTargetIndex(tIndex);

		// streamにMorph Targets情報を保存.
		StreamCtrl::writeMorphTargetsData(*shape, m_morphTargetsData);

		// UIの更新.
		m_updateUI();

	} catch (...) { }
}

/**
 * ウエイト値をすべてクリア.
 */
void CMorphWindowInterface::clearAllWeights ()
{
	m_morphTargetsData.setZeroAllWeight();
	m_updateUI();
}

/**
 * Morph Targets情報を削除.
 */
void CMorphWindowInterface::removeMorphTargetsData ()
{
	// カレント形状でのMorph Targets情報を削除.
	m_morphTargetsData.removeMorphTargets(m_removeRestoreCheck);
	m_updateUI();
}

/**
 * カレントの選択形状より、UIを更新.
 */
void CMorphWindowInterface::m_updateUI ()
{
	// ボタン部を更新.
	if (m_pButtonsWidget) m_pButtonsWidget->updateUI();

	// Targetリスト部を更新.
	if (m_pMorphTargetsWidget) m_pMorphTargetsWidget->updateUI(true);

	// ウエイト値によりメッシュを更新.
	m_morphTargetsData.updateMesh();
}

/**
 * Morph Targetの頂点を選択.
 */
void CMorphWindowInterface::selectTargetVertices ()
{
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (!shape) return;

	const int tIndex = m_morphTargetsData.getSelectTargetIndex();
	if (tIndex < 0) return;

	// Morph Targetsの対象の頂点インデックスを取得.
	std::vector<int> vIndices;
	std::vector<sxsdk::vec3> vertices;
	m_morphTargetsData.getTargetVertices(tIndex, vIndices, vertices);

	try {
		compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
		scene->enter_modify_mode();
		scene->set_selection_mode(sxsdk::enums::vertex_selection_mode);

		sxsdk::polygon_mesh_class& pMesh = shape->get_polygon_mesh();
		pMesh.select_all(false);
		for (size_t i = 0; i < vIndices.size(); ++i) {
			const int vIndex = vIndices[i];
			pMesh.vertex(vIndex).set_active(true);
		}
	} catch (...) { }

	// ボタンを更新するために呼ぶ.
	m_pButtonsWidget->updateUI();
}

/**
 * idleイベント.
 */
void CMorphWindowInterface::idle_task (bool &b, sxsdk::scene_interface *scene, void *)
{
	if (!this->is_shown()) return;

	if (m_needUpdateMorph) {
		updateMorph();
	}

	if (m_needLoadMorph) {
		m_needLoadMorph = false;
		if (scene) {
			sxsdk::shape_class& shape = scene->active_shape();
			if (StreamCtrl::readMorphTargetsData(shape, m_morphTargetsData)) {
				m_updateUI();
			} else {
				m_morphTargetsData.clear();
				m_updateUI();
			}
		}
	}

	// Target名変更ダイアログの表示.
	if (m_showRenameDialog) {
		m_showRenameDialog = false;
		showRenameDialog(m_morphTargetsData.getSelectTargetIndex());
	}
}

/**
 * Morph情報を更新.
 */
void CMorphWindowInterface::updateMorph ()
{
	m_needUpdateMorph = false;

	// streamにMorph Targets情報を保存.
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (shape) {
		StreamCtrl::writeMorphTargetsData(*shape, m_morphTargetsData);

		// ウエイト値によりメッシュを更新.
		m_morphTargetsData.updateMesh();
	}
}

/**
 * 指定のMorph Target情報を更新する.
 * @param[in] index   Weightリストでの番号.
 */
void CMorphWindowInterface::updateMorphTarget (const int index)
{
	// 選択されたポリゴンメッシュ形状を取得.
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (!shape) {
		shade.show_message_box(m_msg_noSelectedPolygonmesh.c_str(), false);
		return;
	}

	try {
		// 形状編集モード＋頂点選択モードかチェック.
		compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
		if (!scene->is_modify_mode() || scene->get_selection_mode() != sxsdk::enums::vertex_selection_mode) {
			shade.show_message_box(m_msg_noModifyVertexMode.c_str(), false);
			return;
		}
		sxsdk::polygon_mesh_class& pMesh = shape->get_polygon_mesh();
		if (pMesh.get_number_of_active_control_points() == 0) {
			shade.show_message_box(m_msg_noSelectedVertices.c_str(), false);
			return;
		}

		// 選択された頂点インデックスを取得.
		std::vector<int> vIndices;
		vIndices = MeshUtil::getActiveVerticesInMesh(*shape);

		// 選択頂点をMorph Target情報として登録.
		const std::vector<sxsdk::vec3> vers = MeshUtil::getMeshVertex(*shape, vIndices);
		const int tIndex = m_morphTargetsData.updateTargetVertices(index, vIndices, vers);

		// 追加された要素を選択状態にする.
		m_morphTargetsData.setSelectTargetIndex(tIndex);

		// streamにMorph Targets情報を保存.
		StreamCtrl::writeMorphTargetsData(*shape, m_morphTargetsData);

		// UIの更新.
		m_updateUI();

	} catch (...) { }
}

/**
 * 指定のMorph Target情報を削除.
 * @param[in] index   Weightリストでの番号.
 */
void CMorphWindowInterface::removeMorphTarget (const int index)
{
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (shape) {
		// 一度ウエイト値を0に戻す.
		m_morphTargetsData.setTargetWeight(index, 0.0f);
		m_morphTargetsData.updateMesh();

		if (m_morphTargetsData.removeTarget(index)) {
			StreamCtrl::writeMorphTargetsData(*shape, m_morphTargetsData);
			m_updateUI();
		}
	}
}

/**
 * 指定のMorph Targetの名前を変更.
 * @param[in] index   Weightリストでの番号.
 * @param[in] name    変更する名前.
 */
void CMorphWindowInterface::renameMorphTarget (const int index, const std::string& name)
{
	sxsdk::shape_class* shape = MeshUtil::getActivePolygonMesh(shade);
	if (shape) {
		m_morphTargetsData.setTargetName(index, name);
		StreamCtrl::writeMorphTargetsData(*shape, m_morphTargetsData);
		m_updateUI();
	}
}

/**
 * 名前変更ダイアログボックスを表示.
 * @param[in] index   Weightリストでの番号.
 */
bool CMorphWindowInterface::showRenameDialog (const int index)
{
	if (index < 0) return false;
	try {
		sxsdk::plugin_interface* pluginI = shade.find_plugin_interface_with_uuid(RENAME_DIALOG_ATTRIBUTE_INTERFACE_ID);
		if (pluginI) {
			// ダイアログを表示.
			std::string name = m_morphTargetsData.getTargetName(index);
			CRenameDialog* pA = (CRenameDialog *)pluginI;
			if (!pA->showRenameDialog(name)) return false;

			// 変更された名前.
			name = pA->getName();

			renameMorphTarget(index, name);
		}
	} catch (...) { }
	return false;
}
