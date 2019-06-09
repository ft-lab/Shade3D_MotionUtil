/**
 * モーションの制御を行う機能。外部プラグインから機能にアクセスできるようにする.
 * 以下の機能を予定.
 *
   ・ Morph Target
       変形のストック表示と、モーショングラフ。.

   ・ ボーン操作の便利関数.
       ボーンの向きを調整、ボーンサイズをまとめて変更、など。.

   ・ 新しいモーションウィンドウ.
 */

#include "GlobalHeader.h"
#include "HiddenMorphTargetsInterface.h"
#include "HiddenBoneUtilInterface.h"
#include "MorphWindowInterface.h"
#include "MotionWindowInterface.h"
#include "RenameDialog.h"

//**************************************************//
//	グローバル関数									//
//**************************************************//
/**
 * プラグインインターフェースの生成.
 */
extern "C" SXSDKEXPORT void STDCALL create_interface (const IID &iid, int i, void **p, sxsdk::shade_interface *shade, void *) {
	unknown_interface *u = NULL;
	
	if (iid == attribute_iid) {
		if (i == 0) {
			u = new CHiddenMorphTargetsInterface(*shade);
		} else if (i == 1) {
			u = new CHiddenBoneUtilInterface(*shade);
		} else if (i == 2) {
			u = new CRenameDialog(*shade);
		}
	}
	if (iid == window_iid) {
		if (i == 0) {
			u = new CMorphWindowInterface(*shade);
		} else if (i == 1) {
			u = new CMotionWindowInterface(*shade);
		}
	}

	if (u) {
		u->AddRef();
		*p = (void *)u;
	}
}

/**
 * インターフェースの数を返す.
 */
extern "C" SXSDKEXPORT int STDCALL has_interface (const IID &iid, sxsdk::shade_interface *shade) {

	if (iid == attribute_iid) return 3;
	if (iid == window_iid) return 1;		// TODO.

	return 0;
}

/**
 * インターフェース名を返す.
 */
extern "C" SXSDKEXPORT const char * STDCALL get_name (const IID &iid, int i, sxsdk::shade_interface *shade, void *) {
	// SXULより、プラグイン名を取得して渡す.
	if (iid == attribute_iid) {
		if (i == 0) {
			return CHiddenMorphTargetsInterface::name(shade);
		} else if (i == 1) {
			return CHiddenBoneUtilInterface::name(shade);
		} else if (i == 2) {
			return CRenameDialog::name(shade);
		}
	}
	if (iid == window_iid) {
		if (i == 0) {
			return CMorphWindowInterface::name(shade);
		} else if (i == 1) {
			return CMotionWindowInterface::name(shade);
		}
	}

	return 0;
}

/**
 * プラグインのUUIDを返す.
 */
extern "C" SXSDKEXPORT sx::uuid_class STDCALL get_uuid (const IID &iid, int i, void *) {
	if (iid == attribute_iid) {
		if (i == 0) {
			return HIDDEN_MORPH_TARGETS_INTERFACE_ID;
		} else if (i == 1) {
			return HIDDEN_BONE_UTIL_INTERFACE_ID;
		} else if (i == 2) {
			return RENAME_DIALOG_ATTRIBUTE_INTERFACE_ID;
		}
	}
	if (iid == window_iid) {
		if (i == 0) {
			return MORPH_WINDOW_INTERFACE_ID;
		} else if (i == 1) {
			return MOTION_WINDOW_INTERFACE_ID;
		}
	}

	return sx::uuid_class(0, 0, 0, 0);
}

/**
 * バージョン情報.
 */
extern "C" SXSDKEXPORT void STDCALL get_info (sxsdk::shade_plugin_info &info, sxsdk::shade_interface *shade, void *) {
	info.sdk_version = SHADE_BUILD_NUMBER;
	info.recommended_shade_version = 410000;
	info.major_version = 0;
	info.minor_version = 0;
	info.micro_version = 0;
	info.build_number =  8;
}

/**
 * 常駐プラグイン.
 */
extern "C" SXSDKEXPORT bool STDCALL is_resident (const IID &iid, int i, void *) {
	return true;
}

