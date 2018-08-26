/**
 * sxsdk::graphic_context_interface を使用した描画関数.
 */
#include "GraphicUtil.h"
#include <math.h>

/**
 * テキストを描画する (位置揃え機能あり).
 * @param[in]  gc     graphic context.
 * @param[in]  str    描画するテキスト.
 * @param[in]  rect   描画領域.
 * @param[in]  align  テキストの配置指定.
 */
void GraphicUtil::drawText (sxsdk::graphic_context_interface &gc, const std::string& str, const sx::rectangle_class& rect, const WIDGET_ALIGN align)
{
	if (align == WIDGET_ALIGN_H_LEFT) {
		gc.draw_string_left_in_rectangle(str.c_str(), rect);
	} else if (align == WIDGET_ALIGN_H_CENTER) {
		gc.draw_string_center_in_rectangle(str.c_str(), rect);
	} else if (align == WIDGET_ALIGN_H_RIGHT) {
		gc.draw_string_right_in_rectangle(str.c_str(), rect);
	}
}

/**
 * 太さと色を指定したラインを描画.
 * @param[in]  gc     graphic context.
 * @param[in]  posA   ラインのポイント.
 * @param[in]  size   ラインの太さ.
 */
void GraphicUtil::drawLine (sxsdk::graphic_context_interface &gc, const std::vector<sxsdk::vec2>& posA, const float size)
{
	const float sizeH = size * 0.5f;

	const size_t pCou = posA.size();
	if (pCou < 2) return;

	// 方向ベクトルを計算.
	std::vector<sxsdk::vec2> dirB;
	dirB.resize(pCou);
	for (size_t i = 0; i < pCou - 1; ++i) {
		const sxsdk::vec2 v = normalize(posA[i + 1] - posA[i]);
		dirB[i] = sxsdk::vec2(-v.y, v.x);
	}

	// 太さのあるラインを描画.
	sxsdk::vec2 quadA[4];
	for (size_t i = 0; i < pCou - 1; ++i) {
		const sxsdk::vec2 vH = dirB[i] * sizeH;
		quadA[0] = -vH + posA[i];
		quadA[1] =  vH + posA[i];
		quadA[2] =  vH + posA[i + 1];
		quadA[3] = -vH + posA[i + 1];
		gc.paint_polygon(4, quadA);
	}
}

/**
 * チェックボックスを描画する。主に、標準ではないチェックだけの機能のため.
 * @param[in]  gc     graphic context.
 * @param[in]  checkF チェック状態.
 * @param[in]  rect   描画領域.
 */
void GraphicUtil::drawCheckBox (sxsdk::graphic_context_interface &gc, const bool checkF, const sx::rectangle_class& rect)
{
	// チェックボックスの枠を描画する.
	sx::rectangle_class rect2;
	rect2.min = sx::vec<int,2>(rect.min.x + 1, rect.min.y + 1);
	rect2.max = sx::vec<int,2>(rect.max.x - 1, rect.max.y - 1);

	gc.set_color(sxsdk::rgb_class(0.2f, 0.2f, 0.2f));
	gc.paint_rectangle(rect);
	gc.restore_color();

	gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
	gc.frame_rectangle(rect2);
	gc.restore_color();

	gc.set_color(sxsdk::rgb_class(0.4f, 0.4f, 0.4f));
	gc.frame_rectangle(rect);
	gc.restore_color();

	// チェックを描画する.
	if (checkF) {
		const float wid = (float)(rect2.max.x - rect2.min.x);
		const float hei = (float)(rect2.max.y - rect2.min.y);

		sxsdk::vec2 minPos((float)rect2.min.x, (float)rect2.min.y);
		sxsdk::vec2 posA[4];
		posA[0] = sxsdk::vec2(0.078f * wid, 0.192f * hei) + minPos;
		posA[1] = sxsdk::vec2(0.461f * wid, 0.794f * hei) + minPos;
		posA[2] = sxsdk::vec2(0.961f * wid, 0.044f * hei) + minPos;
		posA[3] = sxsdk::vec2(0.461f * wid, 0.460f * hei) + minPos;
		gc.set_color(sxsdk::rgb_class(1.0f, 1.0f, 1.0f));
		gc.paint_polygon(4, posA);
		gc.restore_color();
	}
}

/**
 * 上矢印を描画する.
 * @param[in]  gc      graphic context.
 * @param[in]  enableF enable時.
 * @param[in]  rect    描画領域.
 */
void GraphicUtil::drawUpButton (sxsdk::graphic_context_interface &gc, const bool enableF, const sx::rectangle_class& rect)
{
	sx::rectangle_class rect2;
	rect2.min = sx::vec<int,2>(rect.min.x + 1, rect.min.y + 1);
	rect2.max = sx::vec<int,2>(rect.max.x - 1, rect.max.y - 1);

	const float wid = (float)(rect2.max.x - rect2.min.x);
	const float hei = (float)(rect2.max.y - rect2.min.y);

	sxsdk::vec2 minPos((float)rect2.min.x, (float)rect2.min.y);
	sxsdk::vec2 posA[7];

	posA[0] = sxsdk::vec2(0.504f * wid, 0.097f * hei) + minPos;
	posA[1] = sxsdk::vec2(0.160f * wid, 0.511f * hei) + minPos;
	posA[2] = sxsdk::vec2(0.335f * wid, 0.511f * hei) + minPos;
	posA[3] = sxsdk::vec2(0.335f * wid, 0.845f * hei) + minPos;
	posA[4] = sxsdk::vec2(0.679f * wid, 0.845f * hei) + minPos;
	posA[5] = sxsdk::vec2(0.679f * wid, 0.511f * hei) + minPos;
	posA[6] = sxsdk::vec2(0.860f * wid, 0.511f * hei) + minPos;

	sxsdk::rgb_class col = (enableF) ? sxsdk::rgb_class(1.0f, 1.0f, 1.0f) : sxsdk::rgb_class(0.43f, 0.43f, 0.43f);
	gc.set_color(col);
	gc.paint_polygon(7, posA);
	gc.restore_color();

	// 外枠を描画.
	gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
	gc.frame_rectangle(rect2);
	gc.restore_color();

	gc.set_color(sxsdk::rgb_class(0.4f, 0.4f, 0.4f));
	gc.frame_rectangle(rect);
	gc.restore_color();
}

/**
 * 下矢印を描画する.
 * @param[in]  gc      graphic context.
 * @param[in]  enableF enable時.
 * @param[in]  rect    描画領域.
 */
void GraphicUtil::drawDownButton (sxsdk::graphic_context_interface &gc, const bool enableF, const sx::rectangle_class& rect)
{
	sx::rectangle_class rect2;
	rect2.min = sx::vec<int,2>(rect.min.x + 1, rect.min.y + 1);
	rect2.max = sx::vec<int,2>(rect.max.x - 1, rect.max.y - 1);

	const float wid = (float)(rect2.max.x - rect2.min.x);
	const float hei = (float)(rect2.max.y - rect2.min.y);

	sxsdk::vec2 minPos((float)rect2.min.x, (float)rect2.min.y);
	sxsdk::vec2 posA[7];

	posA[0] = sxsdk::vec2(0.504f * wid, (1.0f - 0.097f) * hei) + minPos;
	posA[1] = sxsdk::vec2(0.160f * wid, (1.0f - 0.511f) * hei) + minPos;
	posA[2] = sxsdk::vec2(0.335f * wid, (1.0f - 0.511f) * hei) + minPos;
	posA[3] = sxsdk::vec2(0.335f * wid, (1.0f - 0.845f) * hei) + minPos;
	posA[4] = sxsdk::vec2(0.679f * wid, (1.0f - 0.845f) * hei) + minPos;
	posA[5] = sxsdk::vec2(0.679f * wid, (1.0f - 0.511f) * hei) + minPos;
	posA[6] = sxsdk::vec2(0.860f * wid, (1.0f - 0.511f) * hei) + minPos;

	sxsdk::rgb_class col = (enableF) ? sxsdk::rgb_class(1.0f, 1.0f, 1.0f) : sxsdk::rgb_class(0.43f, 0.43f, 0.43f);
	gc.set_color(col);
	gc.paint_polygon(7, posA);
	gc.restore_color();

	// 外枠を描画.
	gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
	gc.frame_rectangle(rect2);
	gc.restore_color();

	gc.set_color(sxsdk::rgb_class(0.4f, 0.4f, 0.4f));
	gc.frame_rectangle(rect);
	gc.restore_color();
}

/**
 * 削除ボタンを描画する.
 * @param[in]  gc      graphic context.
 * @param[in]  rect    描画領域.
 */
void GraphicUtil::drawDeleteButton (sxsdk::graphic_context_interface &gc, const sx::rectangle_class& rect)
{
	sx::rectangle_class rect2;
	rect2.min = sx::vec<int,2>(rect.min.x + 1, rect.min.y + 1);
	rect2.max = sx::vec<int,2>(rect.max.x - 1, rect.max.y - 1);

	sx::rectangle_class rect3;
	rect3.min = sx::vec<int,2>(rect2.min.x + 1, rect2.min.y + 1);
	rect3.max = sx::vec<int,2>(rect2.max.x - 1, rect2.max.y - 1);

	const float wid = (float)(rect3.max.x - rect3.min.x);
	const float hei = (float)(rect3.max.y - rect3.min.y);

	sxsdk::vec2 minPos((float)rect3.min.x, (float)rect3.min.y);

	sxsdk::vec2 posA[4], posB[4];

	posA[0] = sxsdk::vec2(0.051f * wid, (1.0f - 0.192f) * hei) + minPos;
	posA[1] = sxsdk::vec2(0.851f * wid, (1.0f - 0.957f) * hei) + minPos;
	posA[2] = sxsdk::vec2(0.945f * wid, (1.0f - 0.863f) * hei) + minPos;
	posA[3] = sxsdk::vec2(0.145f * wid, (1.0f - 0.090f) * hei) + minPos;

	posB[0] = sxsdk::vec2(0.118f * wid, (1.0f - 0.957f) * hei) + minPos;
	posB[1] = sxsdk::vec2(0.937f * wid, (1.0f - 0.172f) * hei) + minPos;
	posB[2] = sxsdk::vec2(0.831f * wid, (1.0f - 0.094f) * hei) + minPos;
	posB[3] = sxsdk::vec2(0.027f * wid, (1.0f - 0.863f) * hei) + minPos;

	sxsdk::rgb_class col = sxsdk::rgb_class(1.0f, 1.0f, 1.0f);
	gc.set_color(col);
	gc.paint_polygon(4, posA);
	gc.paint_polygon(4, posB);
	gc.restore_color();

	// 外枠を描画.
	gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
	gc.frame_rectangle(rect2);
	gc.restore_color();

	gc.set_color(sxsdk::rgb_class(0.4f, 0.4f, 0.4f));
	gc.frame_rectangle(rect);
	gc.restore_color();
}

/**
 * 更新ボタンを描画する.
 * @param[in]  gc      graphic context.
 * @param[in]  rect    描画領域.
 */
void GraphicUtil::drawUpdateButton (sxsdk::graphic_context_interface &gc, const sx::rectangle_class& rect)
{
	sx::rectangle_class rect2;
	rect2.min = sx::vec<int,2>(rect.min.x + 1, rect.min.y + 1);
	rect2.max = sx::vec<int,2>(rect.max.x - 1, rect.max.y - 1);

	sx::rectangle_class rect3;
	rect3.min = sx::vec<int,2>(rect2.min.x + 1, rect2.min.y + 1);
	rect3.max = sx::vec<int,2>(rect2.max.x - 1, rect2.max.y - 1);

	const float wid = (float)(rect3.max.x - rect3.min.x);
	const float hei = (float)(rect3.max.y - rect3.min.y);

	sxsdk::vec2 minPos((float)rect3.min.x, (float)rect3.min.y);
	const sxsdk::vec2 centerPos((rect.min.x + rect.max.x) * 0.5f, (rect.min.y + rect.max.y) * 0.5f);

	gc.set_color(sxsdk::rgb_class(1.0f, 1.0f, 1.0f));

	// 円状のラインを描く.
	std::vector<sxsdk::vec2> posA;
	const int divCou = 16;
	const float r = wid * 0.35f;
	const float startAngle = 135.0f * sx::pi / 180.0f; 
	const float endAngle   = (360.0f + 45.0f) * sx::pi / 180.0f; 
	{
		const float dAngle = (endAngle - startAngle) / (float)(divCou - 1);
		posA.resize(divCou);

		float fAngle = startAngle;
		for (int i = 0; i < divCou; ++i) {
			posA[i].x =  (std::cos(fAngle) * r) + centerPos.x;
			posA[i].y = -(std::sin(fAngle) * r) + centerPos.y;
			fAngle += dAngle;
		}
	}
	const float size = 1.5f;
	GraphicUtil::drawLine(gc, posA, size);

	// posA[divCou - 1] の位置を中心に矢印の三角形を描く.
	{
		const sxsdk::vec2 cPos = posA[divCou - 1];
		const sxsdk::vec2 dirV = normalize(cPos - posA[divCou - 2]);
		const sxsdk::vec2 dirVH(-dirV.y, dirV.x);
		const float len  = r * 1.0f;
		const float lenH = len * 0.6f;

		sxsdk::vec2 cPosA[3];
		cPosA[0] = (-dirVH * lenH) + cPos;
		cPosA[1] = ( dirVH * lenH) + cPos;
		cPosA[2] = ( dirV  * len ) + cPos;
		gc.paint_polygon(3, cPosA);
	}
	gc.restore_color();

	// 外枠を描画.
	gc.set_color(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
	gc.frame_rectangle(rect2);
	gc.restore_color();

	gc.set_color(sxsdk::rgb_class(0.4f, 0.4f, 0.4f));
	gc.frame_rectangle(rect);
	gc.restore_color();
}

/**
 * 指定のイメージをリサイズした画像を生成する.
 * sxsdk::image_interface::create_duplicate_image_interface を使うと、深度が32固定でアルファが1.0で埋められるため.
 * @param[in] scene      シーンクラス.
 * @param[in] image      元の画像.
 * @param[in] newSize    新しい画像のサイズ.
 * @param[in] depth      ピクセル深度.
 * @param[in] alphaFill  アルファを1.0で埋める場合はtrue.
 */
compointer<sxsdk::image_interface> GraphicUtil::createDuplicateImageInterface (sxsdk::scene_interface* scene, sxsdk::image_interface* image, const sx::vec<int,2> newSize, const int depth, const bool alphaFill)
{
	compointer<sxsdk::image_interface> dstImage;
	if (!image) return dstImage;
	const sx::vec<int,2> srcImageSize = image->get_size();
	if (srcImageSize.x <= 0 || srcImageSize.y <= 0) return dstImage;
	if (newSize.x <= 0 || newSize.y <= 0) return dstImage;

	const int srcWidth  = srcImageSize.x;
	const int srcHeight = srcImageSize.y;
	const int dstWidth  = newSize.x;
	const int dstHeight = newSize.y;

	try {
		dstImage = scene->create_image_interface(newSize, depth);
		if (!dstImage) return dstImage;

		// 同一サイズの場合はそのまま転送.
		{
			std::vector<sxsdk::rgba_class> colLines;
			colLines.resize(dstWidth);
			if (srcWidth == dstWidth && srcHeight == dstHeight) {
				for (int y = 0; y < dstHeight; y++) {
					image->get_pixels_rgba_float(0, y, dstWidth, 1, &(colLines[0]));
					if (alphaFill) {
						for (int x = 0; x < dstWidth; x++) colLines[x].alpha = 1.0f;
					}
					dstImage->set_pixels_rgba_float(0, y, dstWidth, 1, &(colLines[0]));
				}
				dstImage->update();
				return dstImage;
			}
		}

		// 線形補間なしで転送.
		{
			const float ddx = (float)srcWidth / (float)dstWidth;
			const float ddy = (float)srcHeight / (float)dstHeight;

			std::vector<sxsdk::rgba_class> colLines;
			colLines.resize(srcWidth);

			float py = 0.0f;
			sxsdk::rgba_class col;
			for (int y = 0; y < dstHeight; y++) {
				float px = 0.0f;
				const int ipy = std::min((int)std::floor(py + 0.5f), srcHeight - 1);
				image->get_pixels_rgba_float(0, ipy, srcWidth, 1, &(colLines[0]));
				for (int x = 0; x < dstWidth; x++) {
					const int ipx = std::min((int)std::floor(px + 0.5f), srcWidth - 1);
					col = colLines[ipx];
					if (alphaFill) col.alpha = 1.0f;	// 透過を無効にする.
					dstImage->set_pixel_rgba_float(x, y, col);
					px += ddx;
				}
				py += ddy;
			}
			dstImage->update();
			return dstImage;
		}

	} catch (...) { }

	return dstImage;
}

/**
 * 指定のイメージのアルファ値を1.0にする.
 */
void GraphicUtil::clearImageAlphaOne (sxsdk::image_interface* image)
{
	const sx::vec<int,2> size = image->get_size();
	if (size.x <= 0 || size.y <= 0) return;

	const int srcWidth  = size.x;
	const int srcHeight = size.y;

	try {
		std::vector<sxsdk::rgba_class> colLines;
		colLines.resize(srcWidth);
		for (int y = 0; y < srcHeight; y++) {
			image->get_pixels_rgba_float(0, y, srcWidth, 1, &(colLines[0]));
			for (int x = 0; x < srcWidth; x++) colLines[x].alpha = 1.0f;
			image->set_pixels_rgba_float(0, y, srcWidth, 1, &(colLines[0]));
		}
		image->update();
	} catch (...) { }
}

