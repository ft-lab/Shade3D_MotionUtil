/**
 * window_interfaceでのSliderのウィジット.
 */
#include "uiSliderWidget.h"
#include "GraphicUtil.h"
#include "WidgetColor.h"

#define WIDGET_SLIDER_HEIGHT 18					// Sliderの高さ.
#define WIDGET_SLIDER_TUMAMI_WIDTH 8			// Sliderのつまみ部の幅.
#define WIDGET_SLIDER_MARGIN_TEXT_NUMBER 4		// テキストと数値入力部の間のマージン.
#define WIDGET_SLIDER_MARGIN_NUMBER_SLIDER 2	// 数値入力部とスライダの間のマージン.


CUISliderWidgetBase::CUISliderWidgetBase (sxsdk::window_interface &parent, const int control_id, const std::string titleText, const int textWidth, const int numberWidth, const sxsdk::vec2 range)
	: sxsdk::window_interface(parent, 0), m_titleText(titleText), m_control_id(control_id)
{
	m_textWidth   = textWidth;
	m_numberWidth = numberWidth; 
	m_pNumber = NULL;
	m_sliderMouseOver = false;
	m_range = range;

	this->set_client_size(sx::vec<int,2>(200,20));

	m_pNumber = new sxsdk::window_interface::number_class(*this, 1, 84, 0, m_numberWidth, WIDGET_SLIDER_HEIGHT, "");
	//m_pNumber->AddRef();
	m_pNumber->set_value(0.1f);

	m_rect  = sx::rectangle_class(sx::vec<int,2>(0, 0), sx::vec<int,2>(0, 0));
	m_select = false;
}

CUISliderWidgetBase::~CUISliderWidgetBase ()
{
}

/**
 * グループを選択状態にする.
 */
void CUISliderWidgetBase::setSelect (const bool selectF)
{
	m_select = selectF;
}

void CUISliderWidgetBase::clearAll ()
{
	//if (m_pNumber) m_pNumber->Release();
}

/**
 * タイトルの指定.
 */
void CUISliderWidgetBase::setTitleText (const std::string titleStr)
{
	m_titleText = titleStr;
}

/**
 * 範囲を指定.
 */
void CUISliderWidgetBase::set_range (const sxsdk::vec2& range)
{
	m_range = range;
}

void CUISliderWidgetBase::set_range (const float minV, const float maxV)
{
	m_range = sxsdk::vec2(minV, maxV);
}

/**
 * 値を取得.
 */
float CUISliderWidgetBase::get_value ()
{
	return m_pNumber->get_value();
}

/**
 * 値を指定.
 */
void CUISliderWidgetBase::set_value (const float value)
{
	m_pNumber->set_value(value);
	invalidate();
}

/**
 * 位置とサイズの指定.
 */
void CUISliderWidgetBase::set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size)
{
	this->set_frame_position(pos);
	this->set_client_size(size);
}

/**
 * リサイズイベント.
 */
void CUISliderWidgetBase::resize (int x, int y, bool remake, void* aux)
{
	if (!m_pNumber) return;

	m_rect.min = sx::vec<int,2>(0, 0);
	m_rect.max = sx::vec<int,2>(x, y); 

	int px = 2;
	int py = 0;

	px += m_textWidth + WIDGET_SLIDER_MARGIN_TEXT_NUMBER;

	m_pNumber->set_position(sx::vec<int,2>(px, py));
	m_pNumber->set_size(sx::vec<int,2>(m_numberWidth, WIDGET_SLIDER_HEIGHT));

	window_interface::resize(x, y, remake);
}

/**
 * スライダで選択されている値を計算.
 */
bool CUISliderWidgetBase::m_SliderPos (sx::vec<int,2>& mousePos, float& retValue, const bool drag)
{
	const sx::vec<int,2> size = this->get_client_size();

	const int px = 2;
	const int py = 2;
	const int dHeight = WIDGET_SLIDER_HEIGHT - 4;

	bool ret = false;
	retValue = 0.0f;

	const int sliderPx = px + m_textWidth + WIDGET_SLIDER_MARGIN_TEXT_NUMBER + m_numberWidth + WIDGET_SLIDER_MARGIN_NUMBER_SLIDER;
	const int sliderWidth = size.x - sliderPx;
	if (sliderWidth > 0) {
		if (drag || (mousePos.y >= py && mousePos.y <= py + dHeight)) {
			if (drag || (mousePos.x >= sliderPx && mousePos.x <= sliderPx + sliderWidth)) {
				float posX = mousePos.x;
				const int eMargin = 6;		// スライダの隅に来た場合に吸着.
				if (posX < sliderPx + eMargin) posX = sliderPx;
				if (posX + eMargin >= sliderPx + sliderWidth) posX = sliderPx + sliderWidth;

				float fPos = ((float)(posX - sliderPx) / (float)sliderWidth) * (m_range[1] - m_range[0]) + m_range[0];
				fPos = std::max(fPos, m_range[0]);
				fPos = std::min(fPos, m_range[1]);
				ret = true;
				retValue = fPos;
			}
		}
	}
	return ret;
}

/**
 * マウスプッシュイベント.
 */
bool CUISliderWidgetBase::mouse_down (int button, sx::ivec2 p, int key_down, int time, bool double_click, void*)
{
	// ウィンドウ外の場合はスキップ.
	if (!this->chkInner(p)) return false;

	// マウスダウンイベントを呼ぶ.
	this->mouseDownEvent(p, double_click);

	// スライダ部でのクリック処理.
	float fPos = 0.0f;
	if (m_SliderPos(p, fPos)) {
		// 値の更新.
		const float currentVal = m_pNumber->get_value();
		if (!sx::zero(currentVal - fPos)) {
			m_pNumber->set_value(fPos);
			m_pNumber->invalidate();
			this->invalidate();

			changedValue(fPos, false);		// コールバック.
		}

		// マウスドラッグでのハンドリング.
		class CMouseTracker : public mouse_tracker_class {
		public:
			friend CUISliderWidgetBase;

			CMouseTracker (sxsdk::window_interface &w, const sx::vec< int, 2 > &whereP) : mouse_tracker_class(w, whereP) { }

			// ドラッグ時.
			virtual void mouse_move (sx::vec<int, 2> p) {
				((CUISliderWidgetBase &)w).mouseDrag(p);
			}

			// Release時.
			virtual void mouse_release () {
				((CUISliderWidgetBase &)w).mouseRelease();
			}
		};

		new CMouseTracker(*this, p);
	}

	// 以下を呼ばないと、thisに関連付けられたウィジットのイベントが呼ばれなくなる.
	return window_interface::mouse_down(button, p, key_down, time, double_click);
}

/**
 * マウスドラッグ時に呼ぶ.
 */
void CUISliderWidgetBase::mouseDrag (sx::vec<int, 2> p)
{
	float fPos = 0.0f;
	if (m_SliderPos(p, fPos, true)) {
		const float currentVal = m_pNumber->get_value();
		if (!sx::zero(currentVal - fPos)) {
			m_pNumber->set_value(fPos);
			m_pNumber->invalidate();
			this->invalidate();

			changedValue(fPos, true);		// コールバック.
		}
	}
}

/**
 * マウスアップ時に呼ぶ.
 */
void CUISliderWidgetBase::mouseRelease ()
{
}

/**
 * マウス移動イベント (マウスオーバー時).
 */
void CUISliderWidgetBase::mouse_move (sx::vec<int, 2> p, void *)
{
	const sx::vec<int,2> size = this->get_client_size();
	const bool oldSliderMouseOver = m_sliderMouseOver;
	m_sliderMouseOver = false;

	// スライダ部でのクリック処理.
	const int px = 2;
	const int py = 2;
	const int dHeight = WIDGET_SLIDER_HEIGHT - 4;

	const int sliderPx = px + m_textWidth + WIDGET_SLIDER_MARGIN_TEXT_NUMBER + m_numberWidth + WIDGET_SLIDER_MARGIN_NUMBER_SLIDER;
	const int sliderWidth = size.x - sliderPx;
	if (sliderWidth > 0) {
		if (p.x >= sliderPx && p.x <= sliderPx + sliderWidth && p.y >= py && p.y <= py + dHeight) {
			m_sliderMouseOver = true;
		}
	}

	// ウィンドウ外の場合.
	if (!this->chkInner(p)) m_sliderMouseOver = false;

	if (oldSliderMouseOver != m_sliderMouseOver) {
		this->invalidate();
	}

	// 以下を呼ばないと、thisに関連付けられたウィジットのイベントが呼ばれなくなる.
	return window_interface::mouse_move(p);
}

/**
 * マウスEnter.
 */
void CUISliderWidgetBase::mouse_enter (void *)
{
	// 以下を呼ばないと、thisに関連付けられたウィジットのイベントが呼ばれなくなる.
	return window_interface::mouse_enter();
}

/**
 * マウスExit.
 */
void CUISliderWidgetBase::mouse_exit (void *)
{
	const bool oldSliderMouseOver = m_sliderMouseOver;
	m_sliderMouseOver = false;
	if (oldSliderMouseOver != m_sliderMouseOver) {
		this->invalidate();
	}

	// 以下を呼ばないと、thisに関連付けられたウィジットのイベントが呼ばれなくなる.
	return window_interface::mouse_exit();
}

/**
 * 背景のペイントイベント.
 */
void CUISliderWidgetBase::clear (sxsdk::graphic_context_interface &gc, void *aux)
{
	sxsdk::window_interface::clear(gc);		// これを入れないと、再描画でアンチエイリアスの発生するエッジ部が濃くなってしまう.

	const sx::vec<int,2> size = this->get_client_size();

	compointer<sxsdk::scene_interface> scene(shade.get_scene_interface());
	if (!scene) return;

	// ウィジットの色情報を取得するクラス.
	CWidgetColor widgetColor(scene);

	// テキスト色は環境設定のものを参照.
	sxsdk::rgb_class textColor = widgetColor.getTextColor();
	if (m_select) textColor = widgetColor.getSelectTextColor();

	const sxsdk::rgb_class sliderBackColor = widgetColor.getSliderBackColor();
	const sxsdk::rgb_class sliderBackMouseOverColor = widgetColor.getSliderBackMouseOverColor();
	const sxsdk::rgb_class sliderTabColor = widgetColor.getSliderTabColor();
	const sxsdk::rgb_class selectColor = widgetColor.getSelectBackColor();
	const sxsdk::rgb_class frameColor = widgetColor.getFrameColor();

	if (m_select) {
		gc.set_color(selectColor);
		gc.paint_rectangle(sx::rectangle_class(sx::vec<int,2>(0, 0), size));
		gc.restore_color();
	}

	// テキスト部を描画.
	const int px = 2;
	const int py = 2;
	const int dHeight = WIDGET_SLIDER_HEIGHT - 4;
	{
		gc.set_color(textColor);
		sx::rectangle_class rect;
		rect.min = sx::vec<int,2>(px, py);
		rect.max = sx::vec<int,2>(px + m_textWidth, py + dHeight);
		GraphicUtil::drawText(gc, m_titleText, rect, WIDGET_ALIGN_H_RIGHT);
		gc.restore_color();
	}

	// スライダ部を描画.
	const int sliderPx = px + m_textWidth + WIDGET_SLIDER_MARGIN_TEXT_NUMBER + m_numberWidth + WIDGET_SLIDER_MARGIN_NUMBER_SLIDER;
	const int sliderWidth = size.x - sliderPx - 1;
	if (sliderWidth > 0) {
		const float currentVal = m_pNumber->get_value();
		int iVal = (int)(((float)sliderWidth * (currentVal - m_range[0]) / (m_range[1] - m_range[0])) + 0.5f);
		iVal = std::min(iVal, sliderWidth);
		iVal = std::max(iVal, 0);

		// 背景の塗り.
		sx::rectangle_class sliderRect;
		sliderRect.min = sx::vec<int,2>(sliderPx, py);
		sliderRect.max = sx::vec<int,2>(sliderPx + sliderWidth, py + dHeight);
		{
			sxsdk::rgb_class col = (m_sliderMouseOver) ? sliderBackMouseOverColor : sliderBackColor;
			gc.set_color(col);
			gc.paint_rectangle(sliderRect);
			gc.restore_color();
		}

		// つまみ部.
		{
			sx::rectangle_class rect;
			const int margin = WIDGET_SLIDER_TUMAMI_WIDTH >> 1;
			int px1 = std::max(iVal - margin, 0);
			int px2 = px1 + (margin + margin);
			if (px2 >= sliderWidth) {
				px2 = sliderWidth;
				px1 = sliderWidth - (margin + margin);
			}

			rect.min = sx::vec<int,2>(sliderPx + px1, py);
			rect.max = sx::vec<int,2>(sliderPx + px2, py + dHeight);

			{
				gc.set_color(sliderTabColor);
				gc.paint_rectangle(rect);
				gc.restore_color();
			}
			{
				gc.set_color(frameColor);
				gc.frame_rectangle(rect);
				gc.restore_color();
			}
		}

		// 外枠.
		{
			gc.set_color(frameColor);
			gc.frame_rectangle(sliderRect);
			gc.restore_color();
		}
		{
			sx::rectangle_class rect = sliderRect;
			rect.min = sx::vec<int,2>(rect.min.x - 1, rect.min.y - 1);
			rect.max = sx::vec<int,2>(rect.max.x + 1, rect.max.y + 1);
			gc.set_color(sxsdk::rgb_class(0.5f, 0.5f, 0.5f));
			gc.frame_rectangle(rect);
			gc.restore_color();
		}

	}
}


//------------------------------------------.
// setup時のコールバック.
//------------------------------------------.
bool CUISliderWidgetBase::setup_number (sxsdk::window_interface::number_class& number, void *)
{
	return true;
}

//------------------------------------------.
// イベント処理のコールバック.
//------------------------------------------.
void CUISliderWidgetBase::number_value_changed (sxsdk::window_interface::number_class& number, void *)
{
	this->invalidate();

	const float currentVal = m_pNumber->get_value();
	changedValue(currentVal, false);		// コールバック.
}
