/**
 * Morph Targetsの一覧表示ウィンドウ.
 */
#ifndef _UIMORPHTARGETSWIDGET_H
#define _UIMORPHTARGETSWIDGET_H

#include "../GlobalHeader.h"

class CMorphWindowInterface;
class CUIMorphTargetGroupWidget;

class CUIMorphTargetsWidget : public sxsdk::window_interface
{
protected:
	CMorphWindowInterface* m_morphWindow;	// ウィンドウの参照.

	std::vector<CUIMorphTargetGroupWidget *> m_MorphTargetsList;		// Morph Targetごとのリスト (スライダがついたもの).

	sx::rectangle_class m_rect;			// 描画領域.

	int m_control_id;					// 識別用コントロールID.

protected:
	virtual int get_shade_version () const { return SHADE_BUILD_NUMBER; }

	virtual bool is_resizable (void *aux=0) { return true; }

	/**
	 * マウスプッシュイベント.
	 */
	virtual bool mouse_down (int button, sx::ivec2 p, int key_down, int time, bool double_click, void*);

	/**
	 * リサイズイベント.
	 */
	virtual void resize (int x, int y, bool remake = true, void* aux = 0);

	/**
	 * Morph Targetsの数を取得してUIを更新.
	 */
	void m_updateUI ();

public:
	/**
	 * @param[in] pParent      CMorphWindowInterfaceのポインタ.
	 * @param[in] control_id   識別ID.
	 */
	explicit CUIMorphTargetsWidget (CMorphWindowInterface* pParent, const int control_id);

	/**
	 * 指定の位置が表示されているMorph Targetsウィンドウ内かチェック.
	 * mouse_downイベントから呼ばれる.
	 * @param[in] index   Weightリストでの番号.
	 * @param[in] p   クリック位置.
	 */
	bool chkInnerMorphTargetsList (const int index, const sx::vec<int,2>& p);

	/**
	 * 位置とサイズの指定.
	 */
	void set_bounds (const sx::vec<int,2>& pos, const sx::vec<int,2>& size);

	/**
	 * 識別用のIDを取得.
	 */
	int get_control_id () { return m_control_id; }

	/**
	 * UIの更新.
	 */
	void updateUI (const bool forceUpdate = false);

	/**
	 * Weight値が変更された場合に呼ばれるイベント.
	 * @param[in] index   Weightリストでの番号.
	 * @param[in] weight  Weight値.
	 */
	void changedWeightValue (const int index, const float weight);

	/**
	 * 選択が変更された場合の呼ばれるイベント.
	 * @param[in] index   Weightリストでの番号.
	 */
	void changedSelect (const int index);

	/**
	 * 更新ボタンが押された場合に呼ばれるイベント.
	 * @param[in] index   Weightリストでの番号.
	 */
	void clickedUpdateButton (const int index);

	/**
	 * 削除ボタンが押された場合に呼ばれるイベント.
	 * @param[in] index   Weightリストでの番号.
	 */
	void clickedDeleteButton (const int index);

	/**
	 * Target名の変更時に呼ばれるイベント.
	 */
	void renameTargetName (const int index);

};

#endif
