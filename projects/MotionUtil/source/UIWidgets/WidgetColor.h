/**
 * preferenceによるウィジットの描画色の取得.
 * 色を取得する際にこのクラスを構築する.
 */
#ifndef _WIDGETCOLOR_H
#define _WIDGETCOLOR_H

#include "../GlobalHeader.h"

class CWidgetColor
{
private:
	float m_baseBrightness;					// ベースの明るさ.
	sxsdk::rgb_class m_textColor;			// テキスト色.
	sxsdk::rgb_class m_selectTextColor;		// 選択時のテキスト色.
	sxsdk::rgb_class m_selectBackColor;		// 選択時の背景色.

	sxsdk::rgb_class m_sliderBackColor;					// スライダの背景色.
	sxsdk::rgb_class m_sliderBackMouseOverColor;		// スライダのマウスオーバー時の背景色.
	sxsdk::rgb_class m_sliderTabColor;					// スライダのつまみの色.

	sxsdk::rgb_class m_buttonColor;						// プッシュボタンの色.
	sxsdk::rgb_class m_buttonForeColor;					// プッシュボタンの前景色.
	sxsdk::rgb_class m_buttonMouseOverColor;			// プッシュボタンのマウスオーバー色.

	sxsdk::rgb_class m_frameColor;						// 枠の色.

public:
	CWidgetColor (sxsdk::scene_interface* scene);

	/**
	 * ベースの明るさを取得.
	 */
	float getBaseBrightness () const { return m_baseBrightness; }

	/**
	 * テキスト色を取得.
	 */
	sxsdk::rgb_class getTextColor () const { return m_textColor; }

	/**
	 * 選択時のテキスト色を取得.
	 */
	sxsdk::rgb_class getSelectTextColor () const { return m_selectTextColor; }

	/**
	 * 選択時の背景色を取得.
	 */
	sxsdk::rgb_class getSelectBackColor () const { return m_selectBackColor; }

	/**
	 * スライダの背景色を取得.
	 */
	sxsdk::rgb_class getSliderBackColor () const { return m_sliderBackColor; }

	/**
	 * スライダのマウスオーバー時の背景色を取得.
	 */
	sxsdk::rgb_class getSliderBackMouseOverColor () const { return m_sliderBackMouseOverColor; }

	/**
	 * スライダのつまみの色.
	 */
	sxsdk::rgb_class getSliderTabColor () const { return m_sliderTabColor; }

	/**
	 * プッシュボタンの背景色を取得.
	 */
	sxsdk::rgb_class getPushButtonBackColor () const { return m_buttonColor; }

	/**
	 * プッシュボタンのマウスオーバー時の背景色を取得.
	 */
	sxsdk::rgb_class getPushButtonBackMouseOverColor () const { return m_buttonMouseOverColor; }

	/**
	 * プッシュボタンの前景色を取得.
	 */
	sxsdk::rgb_class getPushButtonForeColor () const { return m_buttonForeColor; }

	/**
	 * 枠の色.
	 */
	sxsdk::rgb_class getFrameColor () const { return m_frameColor; }
};

#endif
