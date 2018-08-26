/**
 * 名前変更ダイアログ.
 */
#include "RenameDialog.h"

enum {
	dlg_name_id = 101,			// 名前の入力フィールド.
};

CRenameDialog::CRenameDialog (sxsdk::shade_interface& shade) : shade(shade)
{
	m_name = "";
}

CRenameDialog::~CRenameDialog ()
{
}

/**
 * 名前変更用のダイアログボックスを表示.
 * @param[in/out] 名前.
 */
bool CRenameDialog::showRenameDialog (std::string& name)
{
	compointer<sxsdk::dialog_interface> dlg(shade.create_dialog_interface_with_uuid(RENAME_DIALOG_ATTRIBUTE_INTERFACE_ID));
	dlg->set_resource_name("rename_dlg");

	dlg->set_responder(this);
	this->AddRef();

	m_name = name;

	return dlg->ask();
}

/**
 * 名前を取得.
 */
std::string CRenameDialog::getName ()
{
	return m_name;
}

//--------------------------------------------------.
//  ダイアログのイベント処理用.
//--------------------------------------------------.
/**
 * ダイアログの初期化.
 */
void CRenameDialog::initialize_dialog (sxsdk::dialog_interface &d, void *)
{
}

/** 
 * ダイアログのイベントを受け取る.
 */
bool CRenameDialog::respond (sxsdk::dialog_interface &d, sxsdk::dialog_item_class &item, int action, void *)
{
	const int id = item.get_id();		// アクションがあったダイアログアイテムのID.

	if (id == dlg_name_id) {
		m_name = item.get_string();
		return true;
	}

	return false;
}

/**
 * ダイアログのデータを設定する.
 */
void CRenameDialog::load_dialog_data (sxsdk::dialog_interface &d, void *)
{
	{
		sxsdk::dialog_item_class* item;
		item = &(d.get_dialog_item(dlg_name_id));
		item->set_string(m_name.c_str());
	}
}

