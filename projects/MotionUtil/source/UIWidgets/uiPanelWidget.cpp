/**
 * パネルのUI.
 */
#include "uiPanelWidget.h"

/**
 * @param[in] parent       親ウィンドウ.
 */
CUIPanelWidget::CUIPanelWidget (sxsdk::window_interface* parent) : sxsdk::window_interface(*parent, 0)
{
}

/**
 * リサイズイベント.
 */
void CUIPanelWidget::resize (int x, int y, bool remake, void*)
{
}
