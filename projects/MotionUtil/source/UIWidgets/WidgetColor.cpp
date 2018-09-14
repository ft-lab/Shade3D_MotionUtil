/**
 * preferenceによるウィジットの描画色の取得.
 */
#include "WidgetColor.h"

CWidgetColor::CWidgetColor (sxsdk::scene_interface* scene)
{
	compointer<sxsdk::preference_interface> prefer(scene->get_preference_interface());

	m_baseBrightness = prefer->get_base_brightness();
	m_textColor = (m_baseBrightness <= 0.2f) ? sxsdk::rgb_class(1, 1, 1) : sxsdk::rgb_class(0, 0, 0);
	m_selectTextColor = sxsdk::rgb_class(0, 0, 0);
	m_selectBackColor = prefer->get_selected_control_color();
	m_sliderBackColor = sxsdk::rgb_class(0.2f, 0.2f, 0.2f);
	m_sliderBackMouseOverColor = sxsdk::rgb_class(0.5f, 0.5f, 0.5f);
	m_sliderTabColor = sxsdk::rgb_class(0.6f, 0.6f, 0.6f);
	m_frameColor = sxsdk::rgb_class(0, 0, 0);

	{
		const float v = (m_baseBrightness + 1.0) * 0.44;
		m_buttonColor = sxsdk::rgb_class(v, v, v);
		const float v2 = std::min(v * 1.2f, 1.0f);
		m_buttonMouseOverColor = sxsdk::rgb_class(v2, v2, v2);
		m_buttonForeColor = m_textColor;
	}

	if (m_baseBrightness > 0.2f) {
		m_sliderBackColor = m_sliderBackColor * 2.0f;
		m_sliderBackMouseOverColor = m_sliderBackMouseOverColor * 1.2f;
		m_sliderTabColor = m_sliderTabColor * 1.2f;
	}
}

