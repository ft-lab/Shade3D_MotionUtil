/**
 * sxsdk::graphic_context_interface を使用した描画関数.
 */
#include "GraphicUtil.h"
#include "../MathUtil.h"

#include <math.h>
#include <algorithm>

namespace {
	/**
	 * pos1 - pos2の間のfPosの位置の座標値を計算.
	 * @param[in] pos1  開始ポイント.
	 * @param[in] pos2  終了ポイント.
	 * @param[in] fPos  pos1-pos2の間の位置(0.0 - 1.0).
	 * @return ベジェ上のポイント.
	 */
	sxsdk::vec2 g_getBezierPos (const CBezierPoint2D& pos1, const CBezierPoint2D& pos2, const float fPos) {
		double fPos2, fx, fy;
		double t, t2, td, t2d;
		double b1, b2, b3, b4;
		sxsdk::vec2 rPos;

		typedef struct {
			double x, y;
		} FPOINT;
		FPOINT cPos[5];
		FPOINT* pC;

		cPos[0].x = (double)(pos1.point.x);
		cPos[0].y = (double)(pos1.point.y);
		cPos[1].x = (double)(pos1.outHandle.x);
		cPos[1].y = (double)(pos1.outHandle.y);
		cPos[2].x = (double)(pos2.inHandle.x);
		cPos[2].y = (double)(pos2.inHandle.y);
		cPos[3].x = (double)(pos2.point.x);
		cPos[3].y = (double)(pos2.point.y);

		fPos2 = (double)fPos;
		fPos2 = std::max(fPos2, 0.0);
		fPos2 = std::min(fPos2, 1.0);

		if (MathUtil::isZero((float)fPos2)) {
			rPos.x = (float)cPos[0].x;
			rPos.y = (float)cPos[0].y;
			return rPos;
		}
		if(MathUtil::isZero((float)fPos2 - 1.0f)) {
			rPos.x = (float)cPos[3].x;
			rPos.y = (float)cPos[3].y;
			return rPos;
		}

		pC = cPos;
		// ベジェ曲線の計算.
		// B1 = (1 - T) ^ 3
		// B2 = 3 * T * (1 - T) ^ 2
		// B3 = 3 * (T^2) * (1 - T)
		// B4 = T^3
		t   = fPos2;
		t2  = 1.0 - t;
		t2d = t2 * t2;
		td  = t * t;
		b1  = t2d * t2;
		b2  = 3.0 * t * t2d;
		b3  = 3.0 * td * t2;
		b4  = t * td;
		fx = b1 * (cPos[0].x) + b2 * (cPos[1].x) + b3 * (cPos[2].x) + b4 * (cPos[3].x);
		fy = b1 * (cPos[0].y) + b2 * (cPos[1].y) + b3 * (cPos[2].y) + b4 * (cPos[3].y);

		rPos.x = (float)fx;
		rPos.y = (float)fy;
		return rPos;
	}

	/**
	 * 一次のスプラインの計算用.
	 */
	class CSplineData
	{
	private:
		std::vector<float> m_a, m_b, m_c, m_d;
		int m_num;

		void m_init (const std::vector<float>& pos, const int num) {
			float tmp;
			std::vector<float> w;
			w.resize(num + 1);
			const int num2 = num - 1;
		
			for (int i = 0; i <= num2; i++) m_a[i] = pos[i];

			m_c[0] = m_c[num2] = 0.0f;
			for (int i = 1; i < num2; i++) {
				m_c[i] = 3.0f * (m_a[i - 1] - 2.0f * m_a[i] + m_a[i + 1]);
			}

			w[0] = 0.0f;
			for (int i = 1; i < num2; i++) {
				tmp    = 4.0f - w[i - 1];
				m_c[i] = (m_c[i] - m_c[i - 1]) / tmp;
				w[i]   = 1.0f / tmp;
			}

			for (int i = num2 - 1; i > 0; i--) {
				m_c[i] = m_c[i] - m_c[i + 1] * w[i];
			}

			m_b[num2] = m_d[num2] = 0.0f;
			for (int i = 0; i < num2; i++) {
				m_d[i] = (m_c[i + 1] - m_c[i]) / 3.0f;
				m_b[i] = m_a[i + 1] - m_a[i] - m_c[i] - m_d[i];
			}
		}

	public:
		CSplineData (const std::vector<float>& pos) {
			m_num = (int)pos.size();
			m_a.resize(m_num + 1);
			m_b.resize(m_num + 1);
			m_c.resize(m_num + 1);
			m_d.resize(m_num + 1);
			m_init(pos, m_num);
		}

		/**
		 * 0.0 ～ m_num - 1までの実数を指定して、スプラインの位置を計算.
		 */
		float calc (const float t) {
			int i = (int)std::floor(t);
			if (i < 0) i = 0;
			else if (i >= m_num) i = m_num - 1;

			float dt = t - (float)i;
			return m_a[i] + (m_b[i] + (m_c[i] + m_d[i] * dt) * dt) * dt;
		}

		/**
		 * 格納している点数の最大.
		 */
		int GetCount () const { return m_num; }
	};
}

//------------------------------------------------------------.
/**
 * グラフィック描画クラス.
 * 矩形クリッピングなども含む.
 */
CGraphicUtil::CGraphicUtil (sxsdk::graphic_context_interface &gc) : gc(gc)
{
	m_clippingRect = sx::rectangle_class(sx::vec<int,2>(0, 0), sx::vec<int,2>(0, 0));
	m_colorList.clear();
	m_lineSize = 1.0f;
}
CGraphicUtil::~CGraphicUtil ()
{
	while (!m_colorList.empty()) {
		gc.restore_color();
		m_colorList.pop_back();
	}
}

/**
 * 色を指定.
 */
void CGraphicUtil::setColor (const sxsdk::rgb_class& col)
{
	while (!m_colorList.empty()) {
		gc.restore_color();
		m_colorList.pop_back();
	}
	gc.set_color(col);
	m_colorList.push_back(col);
}

/**
 * クリッピング領域をクリア.
 */
void CGraphicUtil::clearClippingRect ()
{
	m_clippingRect = sx::rectangle_class(sx::vec<int,2>(0, 0), sx::vec<int,2>(0, 0));
}

/**
 * クリッピング領域を指定.
 * @param[in] rect  この領域内を描画範囲とする.
 */
void CGraphicUtil::setClippingRect (const sx::rectangle_class& rect)
{
	m_clippingRect = rect;
}

/**
 * ポリゴンを描画 (クリッピング処理あり).
 */
void CGraphicUtil::drawPolygon (const std::vector<sxsdk::vec2>& points)
{
	const int clipWid = m_clippingRect.size().x;
	const int clipHei = m_clippingRect.size().y;
	const bool clipF  = (clipWid > 0 && clipHei > 0);

	if (points.size() < 3) return;
	if (!clipF) {
		gc.paint_polygon((int)points.size(), &(points[0]));
		return;
	}

	// クリッピングを行う.
	std::vector<sxsdk::vec2> points3;
	{
		float clipX1 = (float)m_clippingRect.min.x;
		float clipX2 = (float)m_clippingRect.max.x;
		float clipY1 = (float)m_clippingRect.min.y;
		float clipY2 = (float)m_clippingRect.max.y;
		if (clipX1 > clipX2) std::swap(clipX1, clipX2);
		if (clipY1 > clipY2) std::swap(clipY1, clipY2);

		std::vector<sxsdk::vec2> points2 = points;
		points2.push_back(points[0]);

		int vCou = (int)points2.size();
		points3.resize(vCou * 2);

		// left.
		{
			int u = 0;
			for (int i = 0;i < vCou; ++i) {
				const float x  = points2[i].x;
				const float x1 = points2[i + 1].x;
				if (x >= clipX1 && x1 >= clipX1) {
					points3[u] = points2[i]; u++;
					points3[u] = points2[i + 1];
					continue;
				}
				if (x >= clipX1 && x1 < clipX1) {
					points3[u] = points2[i]; u++;
					points3[u].x = clipX1;
					points3[u].y = (points2[i + 1].y - points2[i].y) * (clipX1 - x) / (x1 - x) + points2[i].y;
					u++;
					continue;
				}
				if (x < clipX1 && x1 >= clipX1) {
					points3[u].x = clipX1;
					points3[u].y = (points2[i + 1].y - points2[i].y) * (clipX1 - x) / (x1 - x) + points2[i].y;
					u++;
					points3[u] = points2[i + 1];
					continue;
				}
			}
			points3[u] = points3[0];
			vCou = u;
			if (vCou < 3) return;
			points2 = points3;
			points3.resize(vCou * 2);
		}

		// right.
		{
			int u = 0;
			for (int i = 0;i < vCou; ++i) {
				const float x  = points2[i].x;
				const float x1 = points2[i + 1].x;
				if (x <= clipX2 && x1 <= clipX2) {
					points3[u] = points2[i]; u++;
					points3[u] = points2[i + 1];
					continue;
				}
				if (x <= clipX2 && x1 > clipX2) {
					points3[u] = points2[i]; u++;
					points3[u].x = clipX2;
					points3[u].y = (points2[i + 1].y - points2[i].y) * (clipX2 - x) / (x1 - x) + points2[i].y;
					u++;
					continue;
				}
				if (x > clipX2 && x1 <= clipX2) {
					points3[u].x = clipX2;
					points3[u].y = (points2[i + 1].y - points2[i].y) * (clipX2 - x) / (x1 - x) + points2[i].y;
					u++;
					points3[u] = points2[i + 1];
					continue;
				}
			}
			points3[u] = points3[0];
			vCou = u;
			if (vCou < 3) return;
			points2 = points3;
			points3.resize(vCou * 2);
		}

		// top.
		{
			int u = 0;
			for (int i = 0;i < vCou; ++i) {
				const float y  = points2[i].y;
				const float y1 = points2[i + 1].y;
				if (y >= clipY1 && y1 >= clipY1) {
					points3[u] = points2[i]; u++;
					points3[u] = points2[i + 1];
					continue;
				}
				if (y >= clipY1 && y1 < clipY1) {
					points3[u] = points2[i]; u++;
					points3[u].x = (points2[i + 1].x - points2[i].x) * (clipY1 - y) / (y1 - y) + points2[i].x;
					points3[u].y = clipY1;
					u++;
					continue;
				}
				if (y < clipY1 && y1 >= clipY1) {
					points3[u].x = (points2[i + 1].x - points2[i].x) * (clipY1 - y) / (y1 - y) + points2[i].x;
					points3[u].y = clipY1;
					u++;
					points3[u] = points2[i + 1];
					continue;
				}
			}
			points3[u] = points3[0];
			vCou = u;
			if (vCou < 3) return;
			points2 = points3;
			points3.resize(vCou * 2);
		}

		// bottom.
		{
			int u = 0;
			for (int i = 0;i < vCou; ++i) {
				const float y  = points2[i].y;
				const float y1 = points2[i + 1].y;
				if (y <= clipY2 && y1 <= clipY2) {
					points3[u] = points2[i]; u++;
					points3[u] = points2[i + 1];
					continue;
				}
				if (y <= clipY2 && y1 > clipY2) {
					points3[u] = points2[i]; u++;
					points3[u].x = (points2[i + 1].x - points2[i].x) * (clipY2 - y) / (y1 - y) + points2[i].x;
					points3[u].y = clipY2;
					u++;
					continue;
				}
				if (y > clipY2 && y1 <= clipY2) {
					points3[u].x = (points2[i + 1].x - points2[i].x) * (clipY2 - y) / (y1 - y) + points2[i].x;
					points3[u].y = clipY2;
					u++;
					points3[u] = points2[i + 1];
					continue;
				}
			}
			points3[u] = points3[0];
			vCou = u;
			if (vCou < 3) return;
			points2 = points3;
		}

		points3.resize(vCou);
		for (int i = 0; i < vCou; ++i) {
			float x = points2[i].x;
			float y = points2[i].y;
			x = std::min(clipX2, std::max(clipX1, x));
			y = std::min(clipY2, std::max(clipY1, y));
			points3[i] = sxsdk::vec2(x, y);
		}
	}

	gc.paint_polygon((int)points3.size(), &(points3[0]));
}

/**
 * テキストを描画する (位置揃え機能あり).
 * @param[in]  str    描画するテキスト.
 * @param[in]  rect   描画領域.
 * @param[in]  align  テキストの配置指定.
 */
void CGraphicUtil::drawText (const std::string& str, const sx::rectangle_class& rect, const int align)
{
	GraphicUtil::drawText(gc, str, rect, align);
}

/**
 * テキストのサイズを取得.
 * @param[in]  str    描画するテキスト.
 */
sx::vec<int,2> CGraphicUtil::getTextSize ( const std::string& str)
{
	return GraphicUtil::getTextSize(gc, str);
}

/**
 * ラインを描画.
 * @param[in]  posA   ラインのポイント.
 */
void CGraphicUtil::drawLine (const std::vector<sxsdk::vec2>& posA)
{
	const float sizeH = m_lineSize * 0.5f;
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
	std::vector<sxsdk::vec2> quadA;
	quadA.resize(4);
	for (size_t i = 0; i < pCou - 1; ++i) {
		const sxsdk::vec2 vH = dirB[i] * sizeH;
		quadA[0] = -vH + posA[i];
		quadA[1] =  vH + posA[i];
		quadA[2] =  vH + posA[i + 1];
		quadA[3] = -vH + posA[i + 1];
		drawPolygon(quadA);
	}
}

/**
 * ベジェ曲線を描画.
 * @param[in] points  ベジェを構成するポイント情報.
 * @param[in] divCou  分割数.
 */
void CGraphicUtil::drawBezier (const std::vector<CBezierPoint2D>& points, const int divCou)
{
	const float dV = 1.0f / (float)divCou;
	std::vector<sxsdk::vec2> vPosList;
	vPosList.resize(divCou + 1);

	for (int i = 0; i < divCou - 1; ++i) {
		const CBezierPoint2D& bP1 = points[i];
		const CBezierPoint2D& bP2 = points[i + 1];
		float dPos = 0.0f;
		for (int j = 0; j <= divCou; ++j) {
			vPosList[j] = ::g_getBezierPos(bP1, bP2, dPos);
			dPos += dV;
		}
		drawLine(vPosList);
	}
}

/**
 * スプライン曲線を描画.
 * @param[in] points  スプラインを構成するポイント情報.
 * @param[in] divCou  分割数.
 */
void CGraphicUtil::drawSpline (const std::vector<sxsdk::vec2>& points, const int divCou)
{
	std::vector<float> pXList, pYList;
	const int vCou = (int)points.size();

	pXList.resize(vCou);
	pYList.resize(vCou);
	for (int i = 0; i < vCou; ++i) {
		pXList[i] = points[i].x;
		pYList[i] = points[i].y;
	}
	CSplineData splineX(pXList);
	CSplineData splineY(pYList);

	std::vector<sxsdk::vec2> newVList;
	const float dV = (float)(vCou - 1) / (float)divCou;
	float dPos = 0.0f;
	for (int i = 0; i < vCou * divCou; ++i) {
		const float px = splineX.calc(dPos);
		const float py = splineY.calc(dPos);
		newVList.push_back(sxsdk::vec2(px, py));
		dPos += dV;
	}

	drawLine(newVList);
}

/**
 * 四角形の枠を描画.
 */
void CGraphicUtil::drawFrame (const sx::rectangle_class& rec)
{
	int x1 = rec.min.x;
	int y1 = rec.min.y;
	int x2 = rec.max.x;
	int y2 = rec.max.y;

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (x1 == x2 || y1 == y2) return;

	std::vector<sxsdk::vec2> quadA;
	quadA.resize(5);
	quadA[0] = sxsdk::vec2(x1, y1);
	quadA[1] = sxsdk::vec2(x2, y1);
	quadA[2] = sxsdk::vec2(x2, y2);
	quadA[3] = sxsdk::vec2(x1, y2);
	quadA[4] = quadA[0];
	drawLine(quadA);
}

/**
 * 四角形を塗りつぶし描画.
 */
void CGraphicUtil::paintRectangle (const sx::rectangle_class& rec)
{
	int x1 = rec.min.x;
	int y1 = rec.min.y;
	int x2 = rec.max.x;
	int y2 = rec.max.y;

	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (x1 == x2 || y1 == y2) return;

	const int clipWid = m_clippingRect.size().x;
	const int clipHei = m_clippingRect.size().y;
	const bool clipF  = (clipWid > 0 && clipHei > 0);

	if (!clipF) {
		gc.paint_rectangle(rec);
		return;
	}

	// クリッピングを行う.
	float clipX1 = (float)m_clippingRect.min.x;
	float clipX2 = (float)m_clippingRect.max.x;
	float clipY1 = (float)m_clippingRect.min.y;
	float clipY2 = (float)m_clippingRect.max.y;
	if (clipX1 > clipX2) std::swap(clipX1, clipX2);
	if (clipY1 > clipY2) std::swap(clipY1, clipY2);
	if (x2 < clipX1 || x1 > clipX2 || y2 < clipY1 || y1 > clipY2) return;

	if (x1 < clipX1) x1 = clipX1;
	if (x2 >= clipX2) x2 = clipX2;
	if (y1 < clipY1) y1 = clipY1;
	if (y2 >= clipY2) y2 = clipY2;

	const sx::rectangle_class rec2(sx::vec<int,2>((int)x1, (int)y1), sx::vec<int,2>((int)x2, (int)y2));
	gc.paint_rectangle(rec2);
}

//------------------------------------------------------------.
/*
 * 描画関数群.
 */

/**
 * テキストを描画する (位置揃え機能あり).
 * @param[in]  gc     graphic context.
 * @param[in]  str    描画するテキスト.
 * @param[in]  rect   描画領域.
 * @param[in]  align  テキストの配置指定.
 */
void GraphicUtil::drawText (sxsdk::graphic_context_interface &gc, const std::string& str, const sx::rectangle_class& rect, const int align)
{
	const int recWid = rect.size().x;
	const int recHei = rect.size().y;

	const int tWid = gc.get_string_width(str.c_str());
	const int tHei = gc.get_string_height();

	sx::rectangle_class rect2;
	const int marginH = (recWid - tWid) / 2;
	const int marginV = (recHei - tHei) / 2;
	if (align & WIDGET_ALIGN_H_CENTER) {
		rect2.min.x = rect.min.x + marginH;
		rect2.max.x = rect.max.x - marginH;
	} else {
		if (align & WIDGET_ALIGN_H_RIGHT) {
			rect2.min.x = rect.max.x - tWid;
			rect2.max.x = rect.max.x;
		} else {
			rect2.min.x = rect.min.x;
			rect2.max.x = rect.min.x + tWid;
		}
	}
	if (align & WIDGET_ALIGN_V_MIDDLE) {
		rect2.min.y = rect.min.y + marginV;
		rect2.max.y = rect.max.y - marginV;
	} else {
		if (align & WIDGET_ALIGN_V_BOTTOM) {
			rect2.min.y = rect.max.y - tHei;
			rect2.max.y = rect.max.y;
		} else {
			rect2.min.y = rect.min.y;
			rect2.max.y = rect.min.y + tHei;
		}
	}

	gc.draw_string_center_in_rectangle(str.c_str(), rect2);
}

/**
 * テキストのサイズを取得.
 * @param[in]  gc     graphic context.
 * @param[in]  str    描画するテキスト.
 */
sx::vec<int,2> GraphicUtil::getTextSize (sxsdk::graphic_context_interface &gc, const std::string& str)
{
	const int tWid = gc.get_string_width(str.c_str());
	const int tHei = gc.get_string_height();

	return sx::vec<int,2>(tWid, tHei);
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

