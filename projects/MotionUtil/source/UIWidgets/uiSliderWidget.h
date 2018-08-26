/**
 * window_interfaceでのSliderのウィジットのベースクラス.
 * マウスドラッグ中も変更イベントを受け取れる.
 * イベントを受け取るため、これを派生して使用のこと.
 */
#ifndef _UISLIDERWIDGET_H
#define _UISLIDERWIDGET_H

#include "../GlobalHeader.h"

class CUISliderWidgetBase : public sxsdk::window_interface
{
private:
	std::string m_titleText;		// タイトルテキスト.
	int m_textWidth;				// テキスト部のサイズ(幅).
	int m_numberWidth;				// 数値入力部の幅.
	sx::rectangle_class m_rect;		// 描画領域.

	sxsdk::vec2 m_range;			// 数値の範囲.

	sxsdk::window_interface::number_class* m_pNumber;		//　数値表示部.
	bool m_sliderMouseOver;									// スライダ上でマウスオーバー時.

	int m_control_id;					// 識別用コントロールID.
	bool m_select;						// 選択状態か.

private:

	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	//------------------------------------------.
	// setup時のコールバック.
	//------------------------------------------.
	virtual bool setup_number (sxsdk::window_interface::number_class& number, void* aux = 0);

	//------------------------------------------.
	// イベント処理のコールバック.
	//------------------------------------------.
	virtual void number_value_changed (sxsdk::window_interface::number_class& number, void* aux = 0);

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
	 * マウス移動イベント (マウスオーバー時).
	 */
	virtual void mouse_move (sx::vec<int, 2> p, void *aux=0);

	/**
	 * マウスEnter.
	 */
	virtual void mouse_enter (void *aux=0);

	/**
	 * マウスExit.
	 */
	virtual void mouse_exit (void *aux=0);

	/**
	 * 背景のペイントイベント.
	 */
	virtual void clear (sxsdk::graphic_context_interface &gc, void *aux = 0);

private:
	/**
	 * スライダで選択されている値を計算.
	 */
	bool m_SliderPos (sx::vec<int,2>& mousePos, float& retValue, const bool drag = false);

protected:
	/**
	 * マウスドラッグ時に呼ぶ.
	 */
	void mouseDrag (sx::vec<int, 2> p);

	/**
	 * マウスアップ時に呼ぶ.
	 */
	void mouseRelease ();

public:
	/**
	 * @param[in] parent       親ウィンドウ.
	 * @param[in] control_id   識別ID.
	 * @param[in] titleText    表示するタイトルテキスト.
	 * @param[in] textWidth    タイトルテキストの幅.
	 * @param[in] numberWidth  数値入力部の幅.
	 * @param[in] range        スライダの範囲.
	 */
	explicit CUISliderWidgetBase (sxsdk::window_interface &parent, const int control_id, const std::string titleText, const int textWidth, const int numberWidth = 60, const sxsdk::vec2 range = sxsdk::vec2(0.0f, 1.0f));
	virtual ~CUISliderWidgetBase ();

	void clearAll ();

	/**
	 * 範囲を指定.
	 */
	void set_range (const sxsdk::vec2& range);
	void set_range (const float minV, const float maxV);

	/**
	 * 値を指定.
	 */
	void set_value (const float value);

	/**
	 * 値を取得.
	 */
	float get_value ();

	/**
	 * 位置とサイズの指定.
	 */
	void set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size);

	/**
	 * 値が変更された場合に呼ばれる.
	 * @param[in] value    変更された値.
	 * @param[in] dragged  スライダでドラッグ中はtrue.
	 */
	virtual void changedValue (float value, bool dragged) = 0;

	/**
	 * マウスダウンイベント.
	 */
	virtual void mouseDownEvent (const sx::vec<int,2>& p, const bool doubleClick) = 0;

	/**
	 * 識別用のIDを取得.
	 */
	int get_control_id () { return m_control_id; }

	/**
	 * 識別用のIDを指定.
	 */
	void set_control_id (const int id_value) { m_control_id = id_value; }

	/**
	 * タイトルの指定.
	 */
	void setTitleText (const std::string titleStr);

	/**
	 * グループを選択状態にする.
	 */
	void setSelect (const bool selectF);

};

#endif
