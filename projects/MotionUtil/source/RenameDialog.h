/**
 * 名前変更ダイアログ.
 */
#ifndef _RENAMEDIALOG_H
#define _RENAMEDIALOG_H

#include "GlobalHeader.h"

#include <string>

struct CRenameDialog : public sxsdk::attribute_interface
{
private:
	sxsdk::shade_interface& shade;
	std::string m_name;				// 変更する名前.

private:

	/**
	 * SDKのビルド番号を指定（これは固定で変更ナシ）。.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	/**
	 * UUIDの指定（独自に定義したGUIDを指定）.
	 * ※ これはプラグインインターフェースごとに必ず必要。.
	 */
	virtual sx::uuid_class get_uuid (void * = 0) { return RENAME_DIALOG_ATTRIBUTE_INTERFACE_ID; }

	/**
	 * メニューに表示しない.
	 */
	virtual void accepts_shape (bool &accept, void *aux=0) { accept = false; }

	//--------------------------------------------------.
	//  ダイアログのイベント処理用.
	//--------------------------------------------------.
	/**
	 * ダイアログの初期化.
	 */
	virtual void initialize_dialog (sxsdk::dialog_interface &d, void * = 0);

	/** 
	 * ダイアログのイベントを受け取る.
	 */
	virtual bool respond (sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void * = 0);

	/**
	 * ダイアログのデータを設定する.
	 */
	virtual void load_dialog_data (sxsdk::dialog_interface &d, void * = 0);

public:
	CRenameDialog (sxsdk::shade_interface& shade);
	virtual ~CRenameDialog ();

	/**
	 * プラグイン名をSXUL(text.sxul)より取得.
	 */
	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("rename Dialog"); }

	/**
	 * 名前変更用のダイアログボックスを表示.
	 * @param[in/out] 名前.
	 */
	bool showRenameDialog (std::string& name);

	/**
	 * 名前を取得.
	 */
	std::string getName ();
};

#endif
