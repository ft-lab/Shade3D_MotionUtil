/**
 * パネルのUI.
 * 入れ子構造にできる.
 */
#ifndef _UIPANELWIDGET_H
#define _UIPANELWIDGET_H

#include "../GlobalHeader.h"

class CUIPanelWidget : public sxsdk::window_interface
{
protected:
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual bool is_resizable (void *aux=0) { return true; }

	/**
	 * リサイズイベント.
	 */
	virtual void resize (int x, int y, bool remake = true, void* aux = 0);

public:
	/**
	 * @param[in] parent       親ウィンドウ.
	 */
	explicit CUIPanelWidget (sxsdk::window_interface* parent);

};

#endif
