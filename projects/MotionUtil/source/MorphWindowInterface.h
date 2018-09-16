/**
 * Morphウィンドウ.
 */
#ifndef _MORPHWINDOWINTERFACE_H
#define _MORPHWINDOWINTERFACE_H

#include "GlobalHeader.h"
#include "MorphTargetsCtrl.h"
#include "UIWidgets/uiSliderWidget.h"
#include "UIWidgets/uiMorphTargetsWidget.h"

class CMorphWindowInterface;

//--------------------------------------------------------------.
/**
 * SXULのボタン表示用.
 */
class CButtonsWidget : public sxsdk::window_interface
{
private:
	CMorphWindowInterface* m_pParent;

	checkbox_class* m_pRemoveRestoreCheckBox;
	push_button_class* m_pSetupMorphTargetsBut;
	push_button_class* m_pAppendTargetBut;
	push_button_class* m_pRemoveMorphTargetsBut;
	push_button_class* m_pSelectVerticesBut;
	push_button_class* m_pClearAllWeightsBut;


protected:
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	virtual bool is_resizable (void *aux=0) { return true; }

	/**
	 * リサイズイベント.
	 */
	virtual void resize (int x, int y, bool remake = true, void* aux = 0);

	//------------------------------------------.
	// setup時のコールバック.
	//------------------------------------------.
	virtual bool setup_push_button (sxsdk::window_interface::push_button_class &push_button, void *aux=0);
	virtual bool setup_checkbox (sxsdk::window_interface::checkbox_class &checkbox, void *aux=0);

	//------------------------------------------.
	// イベント処理のコールバック.
	//------------------------------------------.
	virtual void push_button_clicked (sxsdk::window_interface::push_button_class &push_button, void *aux=0);
	virtual void checkbox_value_changed (sxsdk::window_interface::checkbox_class &checkbox, void *aux=0);

public:
	explicit CButtonsWidget (CMorphWindowInterface* pParent);

	/**
	 * 位置とサイズの指定.
	 */
	void set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size);

	/**
	 * UIを更新.
	 */
	void updateUI ();
};

//------------------------------------------------------------.
class CMorphWindowInterface : public sxsdk::window_interface
{
private:

	sxsdk::shade_interface &shade;

	CMorphTargetsCtrl m_morphTargetsData;				// Morph Targets情報.

	CUIMorphTargetsWidget* m_pMorphTargetsWidget;		// Morph Targetsの一覧ウィジット.

	CButtonsWidget* m_pButtonsWidget;					// 操作ボタン(SXULで定義).

	bool m_removeRestoreCheck;							// Morph Targetsを削除するとき、頂点を元に戻すか.

	bool m_needUpdateMorph;								// Morphの変形を反映.
	bool m_needLoadMorph;								// 遅延でMorph Targetsのデータを読み込み、UIに反映.
	bool m_showRenameDialog;							// 名前変更のダイアログをidleから呼ぶ。mouse_downからの呼び出しではうまくダイアログが出ないため.

private:
	// 各種メッセージテキスト.
	// イベントの呼び出しタイミングによってはshade.gettext("")から取得できないのであらかじめ読み込んでおく.
	std::string m_msg_noModifyVertexMode;
	std::string m_msg_noSelectedVertices;
	std::string m_msg_noSelectedPolygonmesh;


private:
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }
	virtual sx::uuid_class get_uuid (void *) { return MORPH_WINDOW_INTERFACE_ID; }

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
	 * シーンが変更されたときに呼ばれる.
	 */
	virtual void active_scene_changed (bool &b, sxsdk::scene_interface *scene, void *aux=0);

	/**
	 * 形状の選択が変更されたときに呼ばれる.
	 */
	virtual void active_shapes_changed (bool &b, sxsdk::scene_interface *scene, int old_n, sxsdk::shape_class *const *old_shapes, int n, sxsdk::shape_class *const *shapes, void *aux=0);

	/**
	 * 背景のペイントイベント.
	 */
	virtual void clear (sxsdk::graphic_context_interface &gc, void *aux = 0);

	/**
	 * idleイベント.
	 */
	virtual void idle_task (bool &b, sxsdk::scene_interface *scene, void *aux=0);

public:

	//------------------------------------------.
	// setup時のコールバック.
	//------------------------------------------.
	virtual bool setup_checkbox (sxsdk::window_interface::checkbox_class &checkbox, void *aux=0);

	//------------------------------------------.
	// イベント処理のコールバック.
	//------------------------------------------.
	virtual void checkbox_value_changed (sxsdk::window_interface::checkbox_class &checkbox, void *aux=0);

private:
	/**
	 * カレントの選択形状より、UIを更新.
	 */
	void m_updateUI ();

public:
	/**
	 * Morph Target情報を割り当て開始.
	 */
	void setupMorphTargetData ();

	/**
	 * Morph Target情報を新たに追加.
	 */
	void appendMorphTargetData ();

	/**
	 * ウエイト値をすべてクリア.
	 */
	void clearAllWeights ();

	/**
	 * Morph Targets情報を削除.
	 */
	void removeMorphTargetsData ();

	/**
	 * Morph Targetの頂点を選択.
	 */
	void selectTargetVertices ();

public:
	explicit CMorphWindowInterface (sxsdk::shade_interface &shade);

	sxsdk::shade_interface& getShade () { return shade; }

	/**
	 * Morph Targetsクラスを取得.
	 */
	const CMorphTargetsCtrl& getMorphTargetsCtrl () const { return m_morphTargetsData; }
	CMorphTargetsCtrl& getMorphTargetsCtrl () { return m_morphTargetsData; }

	static const char *name (sxsdk::shade_interface *shade) { return shade->gettext("morph_window_title"); }

	/**
	 * Morphの反映を遅延で行う.
	 */
	void setNeedUpdateMorph () { m_needUpdateMorph = true; }

	/**
	 * idleを使用した遅延で選択形状のMorph Targets情報を読み込む.
	 */
	void setNeedLoadMorph () { m_needLoadMorph = true; }

	/**
	 * idleを使用した遅延で、Target名の変更ダイアログを表示.
	 */
	void setShowRenameDialog () { m_showRenameDialog = true; }

	/**
	 * Morph情報を更新.
	 */
	void updateMorph ();

	/**
	 * 指定のMorph Target情報を更新する.
	 * @param[in] index   Weightリストでの番号.
	 */
	void updateMorphTarget (const int index);

	/**
	 * 指定のMorph Target情報を削除.
	 * @param[in] index   Weightリストでの番号.
	 */
	void removeMorphTarget (const int index);

	/**
	 * 指定のMorph Targetの名前を変更.
	 * @param[in] index   Weightリストでの番号.
	 * @param[in] name    変更する名前.
	 */
	void renameMorphTarget (const int index, const std::string& name);

	/**
	 * 名前変更ダイアログボックスを表示.
	 * @param[in] index   Weightリストでの番号.
	 */
	bool showRenameDialog (const int index);

	/**
	 * 指定の位置が表示されているMorph Targetsウィンドウ内かチェック.
	 * mouse_downイベントから呼ばれる.
	 * @param[in] p   クリック位置.
	 */
	bool chkInnerMorphTargetsList (const sx::vec<int,2>& p);

};

#endif
