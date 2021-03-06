﻿/**
 * Morph Targetsの一覧表示ウィンドウ.
 */
#include "uiMorphTargetsWidget.h"
#include "uiMorphTargetGroupWidget.h"
#include "../MorphWindowInterface.h"
#include "../MorphTargetsCtrl.h"

#include "../MeshUtil.h"
#include "../StreamCtrl.h"

/**
 * @param[in] pParent      CMorphWindowInterfaceのポインタ.
 * @param[in] control_id   識別ID.
 */
CUIMorphTargetsWidget::CUIMorphTargetsWidget (CMorphWindowInterface* pParent, const int control_id) : sxsdk::window_interface(*pParent, sx::v_scroll_bar | sx::v_scroll), m_morphWindow(pParent), m_control_id(control_id)
{
	m_updateUI();
}

/**
 * 位置とサイズの指定.
 */
void CUIMorphTargetsWidget::set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size)
{
	this->set_frame_position(pos);
	this->set_client_size(size);

	updateUI();
}

/**
 * UIの更新.
 */
void CUIMorphTargetsWidget::updateUI (const bool forceUpdate)
{
	sx::vec<int,2> size = this->get_client_size();

	const int groupHeight = 24;
	sx::vec<int,2> groupPos(2, 2);
	sx::vec<int,2> groupSize(size.x - 8, groupHeight);

	// Morph Targetsクラスより、targetの要素数を取得.
	const CMorphTargetsCtrl& morphD = m_morphWindow->getMorphTargetsCtrl();
	const int targetsCou = morphD.getTargetsCount();
	m_updateUI();

	if (!m_MorphTargetsList.empty()) {
		for (int i = 0; i < targetsCou; ++i) {
			CUIMorphTargetGroupWidget* widget = m_MorphTargetsList[i];
			widget->set_bounds(groupPos, groupSize);
			groupPos.y += groupHeight - 1;
		}
	}

	// スクロールする高さのサイズ.
	sx::vec<int,2> vSize = size;
	vSize.y = std::max(vSize.y, targetsCou * groupHeight);
	this->set_canvas_size(vSize);

	this->invalidate();
}

/**
 * リサイズイベント.
 */
void CUIMorphTargetsWidget::resize (int x, int y, bool remake, void*)
{
	window_interface::resize(x, y, remake);
}

/**
 * 指定の位置が表示されているMorph Targetsウィンドウ内かチェック.
 * mouse_downイベントから呼ばれる.
 * @param[in] index   Weightリストでの番号.
 * @param[in] p   クリック位置.
 */
bool CUIMorphTargetsWidget::chkInnerMorphTargetsList (const int index, const sx::vec<int,2>& p)
{
	const sx::rectangle_class rect = m_MorphTargetsList[index]->get_frame_rectangle();
	const sx::vec<int,2> p2 = p + rect.min;
	return m_morphWindow->chkInnerMorphTargetsList(p2);
}

/**
 * マウスプッシュイベント.
 */
bool CUIMorphTargetsWidget::mouse_down (int button, sx::ivec2 p, int key_down, int time, bool double_click, void*)
{
	// Targetのグループ内が選択されている場合はスキップ.
	bool inTargetList = false;

	const CMorphTargetsCtrl& morphD = m_morphWindow->getMorphTargetsCtrl();
	const int targetsCou = morphD.getTargetsCount();

	for (size_t i = 0; i < targetsCou; ++i) {
		const sx::rectangle_class rect = m_MorphTargetsList[i]->get_frame_rectangle();
		if (p.x >= rect.min.x && p.x <= rect.max.x && p.y >= rect.min.y && p.y <= rect.max.y) {
			inTargetList = true;
			break;
		}
	}
	if (inTargetList) return false;

	// 選択解除.
	changedSelect(-1);

	return false;
}

/**
 * Morph Targetsの数を取得してUIを更新.
 */
void CUIMorphTargetsWidget::m_updateUI ()
{
	// Widgets(sxsdk::window_interface)を削除（delete/delete_self）は不安定なので、未使用のものは隠す.
	if (!m_MorphTargetsList.empty()) {
		const size_t cou = m_MorphTargetsList.size();
		for (size_t i = 0; i < cou; ++i) {
			m_MorphTargetsList[i]->hide();
		}
	}

	// Morph Targetsクラスより、targetの要素数を取得.
	const CMorphTargetsCtrl& morphD = m_morphWindow->getMorphTargetsCtrl();
	const int targetsCou = morphD.getTargetsCount();

	// UIを追加/表示.
	{
		const int oldCou = (int)m_MorphTargetsList.size();
		for (int i = oldCou; i < targetsCou; ++i) {
			m_MorphTargetsList.push_back(new CUIMorphTargetGroupWidget(i, this, i + 1, ""));
		}
		for (int i = 0; i < targetsCou; ++i) {
			m_MorphTargetsList[i]->show();
			m_MorphTargetsList[i]->setSelect(false);
		}
	}

	m_MorphTargetsList.resize(targetsCou, NULL);
	for (int i = 0; i < targetsCou; ++i) {
		const std::string name = morphD.getTargetName(i);
		const float weight     = morphD.getTargetWeight(i);
		CUIMorphTargetGroupWidget* widget = m_MorphTargetsList[i];
		widget->setName(name);
		widget->setWeight(weight);

		// 選択状態の場合.
		if (morphD.getSelectTargetIndex() == i) {
			widget->setSelect(true);
		}
	}
}

/**
 * Weight値が変更された場合に呼ばれるイベント.
 * @param[in] index   Weightリストでの番号.
 * @param[in] weight  Weight値.
 */
void CUIMorphTargetsWidget::changedWeightValue (const int index, const float weight)
{
	// ウエイト値を変更.
	CMorphTargetsCtrl& morphD = m_morphWindow->getMorphTargetsCtrl();
	morphD.setTargetWeight(index, weight);

	// Morph情報を更新.
	//m_morphWindow->setNeedUpdateMorph();
	m_morphWindow->updateMorph();
}

/**
 * 選択が変更された場合の呼ばれるイベント.
 * @param[in] index   Weightリストでの番号.
 */
void CUIMorphTargetsWidget::changedSelect (const int index)
{
	CMorphTargetsCtrl& morphD = m_morphWindow->getMorphTargetsCtrl();
	if (morphD.getSelectTargetIndex() != index) {
		const int targetsCou = morphD.getTargetsCount();

		for (int i = 0; i < targetsCou; ++i) {
			const std::string name = morphD.getTargetName(i);
			m_MorphTargetsList[i]->setSelect(i == index);
		}
		morphD.setSelectTargetIndex(index);

		this->invalidate();
	}
}

/**
 * 更新ボタンが押された場合に呼ばれるイベント.
 * @param[in] index   Weightリストでの番号.
 */
void CUIMorphTargetsWidget::clickedUpdateButton (const int index)
{
	m_morphWindow->updateMorphTarget(index);
}

/**
 * 削除ボタンが押された場合に呼ばれるイベント.
 * @param[in] index   Weightリストでの番号.
 */
void CUIMorphTargetsWidget::clickedDeleteButton (const int index)
{
	m_morphWindow->removeMorphTarget(index);
}

/**
 * Target名の変更時に呼ばれるイベント.
 */
void CUIMorphTargetsWidget::renameTargetName (const int index)
{
	// mouse_downイベント内でのダイアログの表示はうまくいかないようなので、.
	// 遅延でidleで名前変更ダイアログを出すフラグを立てる.
	m_morphWindow->setShowRenameDialog();
}
