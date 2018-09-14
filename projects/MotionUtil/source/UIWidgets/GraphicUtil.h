/**
 * sxsdk::graphic_context_interface を使用した描画関数.
 */
#ifndef _GRAPHICUTIL_H
#define _GRAPHICUTIL_H

#include "../GlobalHeader.h"
#include "../MotionExternalAccess.h"

/**
 * 位置揃え.
 */
enum WIDGET_ALIGN {
	WIDGET_ALIGN_H_LEFT   = 0x0001,
	WIDGET_ALIGN_H_RIGHT  = 0x0002,
	WIDGET_ALIGN_H_CENTER = 0x0004,
	WIDGET_ALIGN_V_TOP    = 0x0008,
	WIDGET_ALIGN_V_BOTTOM = 0x0010,
	WIDGET_ALIGN_V_MIDDLE = 0x0020,
	WIDGET_ALIGN_CENTER   = 0x0004 | 0x0020,
};

//------------------------------------------------------------.
/**
 * グラフィック描画クラス.
 * これは描画イベント内で作成/破棄すること.
 * 矩形クリッピングなども含む.
 */
class CGraphicUtil
{
private:
	sxsdk::graphic_context_interface &gc;

	sx::rectangle_class m_clippingRect;				// クリッピング領域.
	std::vector<sxsdk::rgb_class> m_colorList;		// 色を指定した場合の格納バッファ.
	float m_lineSize;								// ラインの太さ.

public:
	CGraphicUtil (sxsdk::graphic_context_interface &gc);
	~CGraphicUtil ();

	/**
	 * 色を指定.
	 */
	void setColor (const sxsdk::rgb_class& col);

	/**
	 * ラインのサイズを指定.
	 */
	void setLineSize (const float size) { m_lineSize = size; }

	/**
	 * ラインのサイズを取得.
	 */
	float getLineSize () const { return m_lineSize; }

	/**
	 * クリッピング領域をクリア.
	 */
	void clearClippingRect ();

	/**
	 * クリッピング領域を指定.
	 * @param[in] rect  この領域内を描画範囲とする.
	 */
	void setClippingRect (const sx::rectangle_class& rect);

	/**
	 * ポリゴンを描画 (クリッピング処理あり).
	 */
	void drawPolygon (const std::vector<sxsdk::vec2>& points);

	/**
	 * テキストを描画する (位置揃え機能あり).
	 * @param[in]  str    描画するテキスト.
	 * @param[in]  rect   描画領域.
	 * @param[in]  align  テキストの配置指定.
	 */
	void drawText (const std::string& str, const sx::rectangle_class& rect, const int align = WIDGET_ALIGN_CENTER);

	/**
	 * テキストのサイズを取得.
	 * @param[in]  str    描画するテキスト.
	 */
	sx::vec<int,2> getTextSize (const std::string& str);

	/**
	 * ラインを描画 (クリッピング処理あり).
	 * @param[in]  posA   ラインのポイント.
	 */
	void drawLine (const std::vector<sxsdk::vec2>& posA);

	/**
	 * ベジェ曲線を描画 (クリッピング処理あり).
	 * @param[in] points  ベジェを構成するポイント情報.
	 * @param[in] divCou  分割数.
	 */
	void drawBezier (const std::vector<CBezierPoint2D>& points, const int divCou = 20);

	/**
	 * スプライン曲線を描画 (クリッピング処理あり).
	 * @param[in] points  スプラインを構成するポイント情報.
	 * @param[in] divCou  分割数.
	 */
	void drawSpline (const std::vector<sxsdk::vec2>& points, const int divCou = 20);

	/**
	 * 四角形の枠を描画 (クリッピング処理あり).
	 */
	void drawFrame (const sx::rectangle_class& rec);

	/**
	 * 四角形を塗りつぶし描画 (クリッピング処理あり).
	 */
	void paintRectangle (const sx::rectangle_class& rec);
};

//------------------------------------------------------------.
/*
 * 描画関数群.
 */
namespace GraphicUtil {
	/**
	 * テキストを描画する (位置揃え機能あり).
	 * @param[in]  gc     graphic context.
	 * @param[in]  str    描画するテキスト.
	 * @param[in]  rect   描画領域.
	 * @param[in]  align  テキストの配置指定.
	 */
	void drawText (sxsdk::graphic_context_interface &gc, const std::string& str, const sx::rectangle_class& rect, const int align = WIDGET_ALIGN_CENTER);

	/**
	 * テキストのサイズを取得.
	 * @param[in]  gc     graphic context.
	 * @param[in]  str    描画するテキスト.
	 */
	sx::vec<int,2> getTextSize (sxsdk::graphic_context_interface &gc, const std::string& str);

	/**
	 * 太さと色を指定したラインを描画.
	 * @param[in]  gc     graphic context.
	 * @param[in]  posA   ラインのポイント.
	 * @param[in]  size   ラインの太さ.
	 */
	void drawLine (sxsdk::graphic_context_interface &gc, const std::vector<sxsdk::vec2>& posA, const float size = 1.0f);

	/**
	 * チェックボックスを描画する。主に、標準ではないチェックだけの機能のため.
	 * @param[in]  gc     graphic context.
	 * @param[in]  checkF チェック状態.
	 * @param[in]  rect   描画領域.
	 */
	void drawCheckBox (sxsdk::graphic_context_interface &gc, const bool checkF, const sx::rectangle_class& rect);

	/**
	 * 上矢印を描画する.
	 * @param[in]  gc      graphic context.
	 * @param[in]  enableF enable時.
	 * @param[in]  rect    描画領域.
	 */
	void drawUpButton (sxsdk::graphic_context_interface &gc, const bool enableF, const sx::rectangle_class& rect);

	/**
	 * 下矢印を描画する.
	 * @param[in]  gc      graphic context.
	 * @param[in]  enableF enable時.
	 * @param[in]  rect    描画領域.
	 */
	void drawDownButton (sxsdk::graphic_context_interface &gc, const bool enableF, const sx::rectangle_class& rect);

	/**
	 * 削除ボタンを描画する.
	 * @param[in]  gc      graphic context.
	 * @param[in]  rect    描画領域.
	 * @param[in]  foreCol 描画色.
	 */
	void drawDeleteButton (sxsdk::graphic_context_interface &gc, const sx::rectangle_class& rect, const sxsdk::rgb_class& foreCol);

	/**
	 * 更新ボタンを描画する.
	 * @param[in]  gc      graphic context.
	 * @param[in]  rect    描画領域.
	 * @param[in]  foreCol 描画色.
	 */
	void drawUpdateButton (sxsdk::graphic_context_interface &gc, const sx::rectangle_class& rect, const sxsdk::rgb_class& foreCol);

	/**
	 * 指定のイメージをリサイズした画像を生成する.
	 * sxsdk::image_interface::create_duplicate_image_interface を使うと、深度が32固定でアルファが1.0で埋められるため.
	 * @param[in] scene      シーンクラス.
	 * @param[in] image      元の画像.
	 * @param[in] newSize    新しい画像のサイズ.
	 * @param[in] depth      ピクセル深度.
	 * @param[in] alphaFill  アルファを1.0で埋める場合はtrue.
	 */
	compointer<sxsdk::image_interface> createDuplicateImageInterface (sxsdk::scene_interface* scene, sxsdk::image_interface* image, const sx::vec<int,2> newSize, const int depth = 64, const bool alphaFill = false);

	/**
	 * 指定のイメージのアルファ値を1.0にする.
	 */
	void clearImageAlphaOne (sxsdk::image_interface* image);

}

#endif
