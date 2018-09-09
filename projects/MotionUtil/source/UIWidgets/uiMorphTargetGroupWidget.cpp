/**
 * Morph Targetの1つ分のUIグループ.
 */
#include "uiMorphTargetGroupWidget.h"
#include "uiMorphTargetsWidget.h"
#include "uiSliderWidget.h"
#include "GraphicUtil.h"
#include "../MorphWindowInterface.h"

//-----------------------------------------------------------------.
CUISliderWidget::CUISliderWidget (sxsdk::window_interface &parent, const int control_id, const std::string titleText, const int textWidth, const int numberWidth, const sxsdk::vec2 range)
	: CUISliderWidgetBase(parent, control_id, titleText, textWidth, numberWidth, range), m_textWidth(textWidth)
{
	m_pParent = (CUIMorphTargetGroupWidget *)&parent;
}

/**
 * 値が変更された場合に呼ばれる.
 * @param[in] value    変更された値.
 * @param[in] dragged  スライダでドラッグ中はtrue.
 */
void CUISliderWidget::changedValue (float value, bool dragged)
{
	m_pParent->changeWeight(value);
}

/**
 * マウスダウンイベント.
 */
void CUISliderWidget::mouseDownEvent (const sx::vec<int,2>& p, const bool doubleClick)
{
	// Morph Targetsのウィンドウの外が選択されている場合はスキップ.
	if (!chkInnerMorphTargetsList(p)) return;

	// 名前変更ダイアログの表示.
	if (doubleClick) {
		if (p.x >= 2 && p.x <= 2 + m_textWidth) {
			m_pParent->showRenameDialog();
		}
	}

	m_pParent->setSelect(true, true);
}

/**
 * 指定の位置が表示されているMorph Targetsウィンドウ内かチェック.
 * mouse_downイベントから呼ばれる.
 * @param[in] p   クリック位置.
 */
bool CUISliderWidget::chkInnerMorphTargetsList (const sx::vec<int,2>& p)
{
	const sx::vec<int,2> framePos = this->get_frame_position();
	return m_pParent->chkInnerMorphTargetsList(framePos + p);
}

/**
 * pの位置がウィンドウ内かチェック.
 */
bool CUISliderWidget::chkInner (const sx::vec<int,2>& p)
{
	return chkInnerMorphTargetsList(p);
}

//-----------------------------------------------------------------.
/**
 * @param[in] parent       親ウィンドウ.
 * @param[in] control_id   識別ID.
 */
CUIMorphTargetGroupWidget::CUIMorphTargetGroupWidget (const int index, CUIMorphTargetsWidget* parent, const int control_id, const std::string title) : m_morphTargetsWidget(parent), sxsdk::window_interface(*parent, 0), m_control_id(control_id), m_index(index)
{
	m_sliderTextWidth = 200;
	m_pSlider = new CUISliderWidget(*this, 1, title, m_sliderTextWidth, 40);
	m_select = false;
	m_selectUpdateButton = select_button_none;
	m_selectDeleteButton = select_button_none;
}

/**
 * 名前を指定.
 */
void CUIMorphTargetGroupWidget::setName (const std::string& name)
{
	if (!m_pSlider) return;
	m_pSlider->setTitleText(name);
}

/**
 * ウィジットを削除.
 * ※ これは呼ばないこと!。動作不安定なので、削除ではなくhideにして再利用する.
 */
void CUIMorphTargetGroupWidget::deleteSelf ()
{
	m_pSlider->delete_self();
	this->delete_self();
}

/**
 * グループを選択状態にする.
 */
void CUIMorphTargetGroupWidget::setSelect (const bool selectF, const bool updateUI)
{
	m_select = selectF;
	m_pSlider->setSelect(selectF);

	if (updateUI) m_morphTargetsWidget->changedSelect(m_index);
}

/**
 * ウエイト値が変更された場合に呼ばれる.
 */
void CUIMorphTargetGroupWidget::changeWeight (const float weight)
{
	m_morphTargetsWidget->changedWeightValue(m_index, weight);
}

/**
 * ウエイト値を変更.
 */
void CUIMorphTargetGroupWidget::setWeight (const float weight)
{
	m_pSlider->set_value(weight);
}

/**
 * 位置とサイズの指定.
 */
void CUIMorphTargetGroupWidget::set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size)
{
	this->set_frame_position(pos);
	this->set_client_size(size);
}

/**
 * リサイズイベント.
 */
void CUIMorphTargetGroupWidget::resize (int x, int y, bool remake, void*)
{
	sx::vec<int,2> pos(4, 2);
	sx::vec<int,2> size(std::max(16, x - 60), y - 4);

	m_pSlider->set_bounds(pos, size);

	window_interface::resize(x, y, remake);
}

/**
 * 指定の位置が表示されているMorph Targetsウィンドウ内かチェック.
 * mouse_downイベントから呼ばれる.
 * @param[in] p   クリック位置.
 */
bool CUIMorphTargetGroupWidget::chkInnerMorphTargetsList (const sx::vec<int,2>& p)
{
	return m_morphTargetsWidget->chkInnerMorphTargetsList(m_index, p);
}

/**
 * マウスプッシュイベント.
 */
bool CUIMorphTargetGroupWidget::mouse_down (int button, sx::ivec2 p, int key_down, int time, bool double_click, void*)
{
	// ウィンドウ外がクリックされた場合はスキップ.
	if (!chkInnerMorphTargetsList(p)) return false;

	const sx::vec<int,2> size = this->get_client_size();
	if (p.x < 0 || p.y < 0 || p.x > size.x || p.y > size.y) return false;

	// マウス位置がボタン上にあるかチェック.
	m_selectUpdateButton = m_selectDeleteButton = select_button_none;
	bool updateBut, deleteBut;
	m_checkMouseButton(p, updateBut, deleteBut);
	m_selectUpdateButton = updateBut ? select_button_mouseclick : select_button_none;
	m_selectDeleteButton = deleteBut ? select_button_mouseclick : select_button_none;

	if (m_selectUpdateButton) m_morphTargetsWidget->clickedUpdateButton(m_index);
	if (m_selectDeleteButton) m_morphTargetsWidget->clickedDeleteButton(m_index);

	m_morphTargetsWidget->changedSelect(m_index);

	return true;
}

/**
 * マウス移動イベント.
 */
void CUIMorphTargetGroupWidget::mouse_move (sx::vec<int, 2> p, void *)
{
	// マウスオーバーでのボタンのハイライト.
	const SELECT_BUTTON_TYPE oldUpdateBut = m_selectUpdateButton;
	const SELECT_BUTTON_TYPE oldDeleteBut = m_selectDeleteButton;
	m_selectUpdateButton = m_selectDeleteButton = select_button_none;

	// ウィンドウ内かチェック.
	if (chkInnerMorphTargetsList(p)) {
		bool updateBut, deleteBut;
		m_checkMouseButton(p, updateBut, deleteBut);
		m_selectUpdateButton = updateBut ? select_button_mouseover : select_button_none;
		m_selectDeleteButton = deleteBut ? select_button_mouseover : select_button_none;
	}
	if (oldUpdateBut != m_selectUpdateButton || oldDeleteBut != m_selectDeleteButton) {
		invalidate();
	}
}

/**
 * 名前変更ダイアログボックスを表示.
 */
void CUIMorphTargetGroupWidget::showRenameDialog ()
{
	m_morphTargetsWidget->renameTargetName(m_index);
}

/**
 * 更新ボタンの位置とサイズを取得.
 */
sx::rectangle_class CUIMorphTargetGroupWidget::m_getUpdateButtonRectangle ()
{
	const sx::vec<int,2> size = this->get_client_size();

	// ボタンの高さ.
	const int buttonSize    = std::min(size.y - 4, 16); 
	const int buttonMarginY = (size.y - buttonSize) / 2;

	const sx::vec<int,2> p1(size.x - buttonSize - 4 - buttonSize - 4, buttonMarginY);
	const sx::vec<int,2> p2(p1.x + buttonSize, p1.y + buttonSize);

	sx::rectangle_class rec(p1, p2);
	return rec;
}

/**
 * 削除ボタンの位置とサイズを取得.
 */
sx::rectangle_class CUIMorphTargetGroupWidget::m_getDeleteButtonRectangle ()
{
	const sx::vec<int,2> size = this->get_client_size();

	// ボタンの高さ.
	const int buttonSize    = std::min(size.y - 4, 16); 
	const int buttonMarginY = (size.y - buttonSize) / 2;

	const sx::vec<int,2> p1(size.x - buttonSize - 4, buttonMarginY);
	const sx::vec<int,2> p2(p1.x + buttonSize, p1.y + buttonSize);

	sx::rectangle_class rec(p1, p2);
	return rec;
}

/**
 * 指定のマウス位置がボタンの範囲内にあるか調べる.
 * @param[in]  mousePos   マウス位置.
 * @param[out] updateBut  更新ボタン上の場合はtrue.
 * @param[out] deleteBut  削除ボタン上の場合はtrue.
 */
void CUIMorphTargetGroupWidget::m_checkMouseButton (const sx::vec<int,2>& mousePos, bool& updateBut, bool& deleteBut)
{
	updateBut = deleteBut = false;

	{
		const sx::rectangle_class rec = m_getUpdateButtonRectangle();
		if (mousePos.x >= rec.min.x && mousePos.x <= rec.max.x && mousePos.y >= rec.min.y && mousePos.y <= rec.max.y) {
			updateBut = true;
		}
	}
	{
		const sx::rectangle_class rec = m_getDeleteButtonRectangle();
		if (mousePos.x >= rec.min.x && mousePos.x <= rec.max.x && mousePos.y >= rec.min.y && mousePos.y <= rec.max.y) {
			deleteBut = true;
		}
	}
}

/**
 * 背景のペイントイベント.
 */
void CUIMorphTargetGroupWidget::clear (sxsdk::graphic_context_interface &gc, void *)
{
	// これを入れないと、再描画でアンチエイリアスの発生するエッジ部が濃くなってしまう.
	sxsdk::window_interface::clear(gc);

	const sx::vec<int,2> size = this->get_client_size();

	compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
	if (!scene) return;
	compointer<sxsdk::preference_interface> prefer(scene->get_preference_interface());
	if (!prefer) return;

	// 背景選択色.
	const sxsdk::rgb_class groupSelectCol(0.4f, 0.3f, 0.3f);

	// ボタンのハイライト色.
	const sxsdk::rgb_class buttonLightCol(0.7f, 0.7f, 0.7f);

	// 背景.
	if (m_select) {
		gc.set_color(groupSelectCol);
		gc.paint_rectangle(sx::rectangle_class(sx::vec<int,2>(0, 0), size));
		gc.restore_color();
	}

	// 削除ボタン.
	{
		const sx::rectangle_class rec = m_getDeleteButtonRectangle();
		if (m_selectDeleteButton != select_button_none) {
			// ボタンのハイライトとして背景を描画.
			gc.set_color(buttonLightCol);
			gc.paint_rectangle(rec);
			gc.restore_color();
		}
		GraphicUtil::drawDeleteButton(gc, rec);
	}

	// 更新ボタン.
	{
		const sx::rectangle_class rec = m_getUpdateButtonRectangle();
		if (m_selectUpdateButton != select_button_none) {
			// ボタンのハイライトとして背景を描画.
			gc.set_color(buttonLightCol);
			gc.paint_rectangle(rec);
			gc.restore_color();
		}
		GraphicUtil::drawUpdateButton(gc, rec);
	}

	// 外枠.
	{
		gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
		gc.frame_rectangle(sx::rectangle_class(sx::vec<int,2>(0, 0), size));
		gc.restore_color();
	}
}

