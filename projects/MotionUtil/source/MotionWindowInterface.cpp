/**
 * 独自のモーションウィンドウ、Morph Targetsにも対応.
 */
#include "MotionWindowInterface.h"
#include "UIWidgets/GraphicUtil.h"

CMotionWindowInterface::CMotionWindowInterface (sxsdk::shade_interface &shade) : sxsdk::window_interface(shade), shade(shade)
{
	m_windowSize = sx::vec<int,2>(0, 0);
}

/**
 * リサイズ時に呼ばれる.
 */
void CMotionWindowInterface::resize (int x, int y, bool remake, void *)
{
	m_windowSize.x = x;
	m_windowSize.y = y;

}

/**
 * 初期化処理.
 */
void CMotionWindowInterface::initialize (void *)
{
	this->set_title(CMotionWindowInterface::name(&shade));

	this->set_minimum_size(sx::vec<int,2>(200, 150));
}

/**
 * 背景のペイントイベント.
 */
void CMotionWindowInterface::clear (sxsdk::graphic_context_interface &gc, void *)
{
	sxsdk::window_interface::clear(gc);

	const sx::vec<int,2> clientSize = this->get_client_size();

	CGraphicUtil graphicUtil(gc);

	std::vector<sxsdk::vec2> posA, posB, posC;
	posA.resize(2);
	posA[0] = sxsdk::vec2(0, 0);
	posA[1] = sxsdk::vec2(clientSize.x, clientSize.y);

	posB.resize(4);
	posB[0] = sxsdk::vec2(10.0f + 0, 80.0f + 80.0f);
	posB[1] = sxsdk::vec2(10.0f + 100, 30.0f + 80.0f);
	posB[2] = sxsdk::vec2(10.0f + 200, 110.0f + 80.0f);
	posB[3] = sxsdk::vec2(10.0f + 300, 90.0f + 80.0f);

	sx::rectangle_class clipRect(sx::vec<int,2>(10, 30), sx::vec<int,2>(250, 300));
	graphicUtil.setColor(sxsdk::rgb_class(0.0f, 0.0f, 0.5f));
	graphicUtil.setLineSize(0.5f);
	graphicUtil.drawFrame(clipRect);

	graphicUtil.setClippingRect(clipRect);

	for (int i = 0; i < 10; ++i) {
		const int x1 = (rand() % 300) + 20;
		const int y1 = (rand() % 300) + 20;
		const int x2 = (rand() % 300) + 20;
		const int y2 = (rand() % 300) + 20;

		sx::rectangle_class rec(sx::vec<int,2>(x1, y1), sx::vec<int,2>(x2, y2));
		const float colR = (rand() & 255) / 255.0f;
		const float colG = (rand() & 255) / 255.0f;
		const float colB = (rand() & 255) / 255.0f;

		graphicUtil.setColor(sxsdk::rgb_class(colR, colG, colB));
		graphicUtil.paintRectangle(rec);
	}

	graphicUtil.setColor(sxsdk::rgb_class(0.0f, 0.0f, 1.0f));
	graphicUtil.setLineSize(2.0f);
	graphicUtil.drawSpline(posB);

	{
		graphicUtil.setColor(sxsdk::rgb_class(0.0f, 0.2f, 1.0f));
		float dSize = 0.5f;
		for (int i = 0; i < 5; ++i) {
			const float dy = (float)(i + 1) * 40.0f;
			posC = posB;
			for (int j = 0; j < 4; ++j) {
				posC[j].y += dy;
			}
			graphicUtil.setLineSize(dSize);
			graphicUtil.drawSpline(posC);
			dSize += 0.5f;
		}
	}

	graphicUtil.setColor(sxsdk::rgb_class(1.0f, 0.8f, 0.0f));
	graphicUtil.setLineSize(1.0f);
	graphicUtil.drawLine(posB);

	graphicUtil.setColor(sxsdk::rgb_class(1.0f, 0.0f, 0.0f));
	const float pSize = 4.0f;
	for (int i = 0; i < (int)posB.size(); ++i) {
		sxsdk::vec2 p = posB[i];
		sx::rectangle_class rec;
		rec.min.x = p.x - pSize;
		rec.min.y = p.y - pSize;
		rec.max.x = p.x + pSize;
		rec.max.y = p.y + pSize;
		graphicUtil.drawFrame(rec);
	}

	//graphicUtil.clearClippingRect();

	const std::string textStr = shade.gettext("test_text");
	{
		const sx::vec<int,2> size = graphicUtil.getTextSize(textStr);

		graphicUtil.setColor(sxsdk::rgb_class(1.0f, 1.0f, 1.0f));
		graphicUtil.drawText(textStr, sx::rectangle_class(sx::vec<int,2>(8, 8), sx::vec<int,2>(8 + 200, 8 + size.y)), WIDGET_ALIGN_H_CENTER);  

		graphicUtil.setColor(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
		graphicUtil.drawFrame(sx::rectangle_class(sx::vec<int,2>(8, 8), sx::vec<int,2>(8 + 200, 8 + size.y)));
	}

	{
		const sx::vec<int,2> size = graphicUtil.getTextSize(textStr);

		graphicUtil.setColor(sxsdk::rgb_class(1.0f, 1.0f, 1.0f));
		graphicUtil.drawText(textStr, sx::rectangle_class(sx::vec<int,2>(8, 28), sx::vec<int,2>(8 + 200, 28 + 40)), WIDGET_ALIGN_H_LEFT | WIDGET_ALIGN_V_MIDDLE);  

		graphicUtil.setColor(sxsdk::rgb_class(0.0f, 0.0f, 0.0f));
		graphicUtil.setLineSize(0.5f);
		graphicUtil.drawFrame(sx::rectangle_class(sx::vec<int,2>(8, 28), sx::vec<int,2>(8 + 200, 28 + 40)));
	}
}

