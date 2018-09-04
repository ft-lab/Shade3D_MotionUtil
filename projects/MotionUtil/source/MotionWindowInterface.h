/**
 * 独自のモーションウィンドウ、Morph Targetsにも対応.
 */
#ifndef _MOTIONWINDOWINTERFACE_H
#define _MOTIONWINDOWINTERFACE_H

#include "GlobalHeader.h"
#if _WINDOWS
#include "windows.h"
#endif

class CMotionWindowInterface : public sxsdk::window_interface
{
private:
	sxsdk::shade_interface &shade;
	sx::vec<int,2> m_windowSize;		// ウィンドウのサイズ.

private:
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void *) { return MOTION_WINDOW_INTERFACE_ID; }

	virtual int get_placement_flags (void *aux=0) {
		return sxsdk::window_interface::view_menu_placement_flag;
	}
	virtual int get_flags (void *aux=0) {
		return 0;
	}

	/**
	 * リサイズの有効化.
	 */
	virtual bool is_resizable (void *aux=0) { return true; }

	/**
	 * リサイズ時に呼ばれる.
	 */
	virtual void resize (int x, int y, bool remake=true, void *aux=0);

	/**
	 * 初期化処理.
	 */
	virtual void initialize (void *);

	/**
	 * 背景のペイントイベント.
	 */
	virtual void clear (sxsdk::graphic_context_interface &gc, void *aux = 0);

public:
	explicit CMotionWindowInterface (sxsdk::shade_interface &shade);

	sxsdk::shade_interface& getShade () { return shade; }

	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("motion_window_title"); }
};

#endif
