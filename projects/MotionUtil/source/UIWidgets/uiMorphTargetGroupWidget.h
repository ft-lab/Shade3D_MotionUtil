/**
 * Morph Targetの1つ分のUIグループ.
 * スライダ、更新ボタン、削除ボタン、を持つ.
 */
#ifndef _UIMORPHTARGETGROUPWIDGET_H
#define _UIMORPHTARGETGROUPWIDGET_H

#include "../GlobalHeader.h"
#include "uiMorphTargetsWidget.h"
#include "uiSliderWidget.h"

class CUIMorphTargetsWidget;
class CUIMorphTargetGroupWidget;

class CUISliderWidget : public CUISliderWidgetBase
{
private:
	CUIMorphTargetGroupWidget* m_pParent;
	int m_textWidth;		// テキスト部の幅.

public:
	CUISliderWidget (sxsdk::window_interface &parent, const int control_id, const std::string titleText, const int textWidth, const int numberWidth = 60, const sxsdk::vec2 range = sxsdk::vec2(0.0f, 1.0f));

	/**
	 * 値が変更された場合に呼ばれる.
	 * @param[in] value    変更された値.
	 * @param[in] dragged  スライダでドラッグ中はtrue.
	 */
	void changedValue (float value, bool dragged);

	/**
	 * マウスダウンイベント.
	 */
	void mouseDownEvent (const sx::vec<int,2>& p, const bool doubleClick);
};

class CUIMorphTargetGroupWidget : public sxsdk::window_interface
{
private:
	enum SELECT_BUTTON_TYPE {
		select_button_none = 0,			// 選択なし.
		select_button_mouseover,		// マウスオーバー.
		select_button_mouseclick,		// マウスクリック.
	};

protected:
	CUIMorphTargetsWidget* m_morphTargetsWidget;	// 親ウィジットの参照.

	CUISliderWidget* m_pSlider;			// スライダ.
	int m_sliderTextWidth;				// スライダでのテキスト部の横幅(pixel値).

	sx::rectangle_class m_rect;			// 描画領域.

	int m_control_id;					// 識別用コントロールID.
	int m_index;						// Weightリスト番号.
	bool m_select;						// 選択状態か.

	SELECT_BUTTON_TYPE m_selectUpdateButton;			// 更新ボタンが選択されている場合.
	SELECT_BUTTON_TYPE m_selectDeleteButton;			// 削除ボタンが選択されている場合.

protected:
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	virtual bool is_resizable (void *aux=0) { return true; }

	/**
	 * リサイズイベント.
	 */
	virtual void resize (int x, int y, bool remake = true, void* aux = 0);

	/**
	 * マウスプッシュイベント.
	 */
	virtual bool mouse_down (int button, sx::ivec2 p, int key_down, int time, bool double_click, void*);

	/**
	 * マウス移動イベント.
	 */
	virtual void mouse_move (sx::vec<int, 2> p, void *);

	/**
	 * 背景のペイントイベント.
	 */
	virtual void clear (sxsdk::graphic_context_interface &gc, void *aux = 0);

private:
	/**
	 * 更新ボタンの位置とサイズを取得.
	 */
	sx::rectangle_class m_getUpdateButtonRectangle ();

	/**
	 * 削除ボタンの位置とサイズを取得.
	 */
	sx::rectangle_class m_getDeleteButtonRectangle ();

	/**
	 * 指定のマウス位置がボタンの範囲内にあるか調べる.
	 * @param[in]  mousePos   マウス位置.
	 * @param[out] updateBut  更新ボタン上の場合はtrue.
	 * @param[out] deleteBut  削除ボタン上の場合はtrue.
	 */
	void m_checkMouseButton (const sx::vec<int,2>& mousePos, bool& updateBut, bool& deleteBut);

public:
	/**
	 * @param[in] parent       親ウィンドウ.
	 * @param[in] control_id   識別ID.
	 */
	explicit CUIMorphTargetGroupWidget (const int index, CUIMorphTargetsWidget* parent, const int control_id, const std::string title);

	/**
	 * 位置とサイズの指定.
	 */
	void set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size);

	/**
	 * 識別用のIDを取得.
	 */
	int get_control_id () { return m_control_id; }

	/**
	 * ウエイト値が変更された場合に呼ばれる.
	 */
	void changeWeight (const float weight);

	/**
	 * ウエイト値を変更.
	 */
	void setWeight (const float weight);

	/**
	 * グループを選択状態にする.
	 */
	void setSelect (const bool selectF, const bool updateUI = false);

	/**
	 * ウィジットを削除.
	 */
	void deleteSelf ();

	/**
	 * 名前変更ダイアログボックスを表示.
	 */
	void showRenameDialog ();
};

#endif
