#pragma once
#include "guilib.hpp"


struct Button {
	Button(const Point &center_pos, const int w=200, const int h=36) :
		buttonRect(Rect(Arg::center = center_pos, w, h).rounded(4.8)){}

	bool clicked() const{
		return buttonRect.leftClicked();
	}

	void draw(const String &str) const{
		buttonRect.draw(buttonRect.mouseOver() ? mouseOverColor : backgroundColor);
		buttonRect.drawFrame(1, 0, frameColor);
		font(str).drawAt(buttonRect.center(), fontColor);
	}

private:
	const RoundRect buttonRect;
	const Font font{ 12, font_path };
	static constexpr ColorF mouseOverColor{ 0.9, 0.95, 1.0 };
	static constexpr ColorF backgroundColor{ 1.0 };
	static constexpr ColorF frameColor{ 0.67 };
	static constexpr ColorF fontColor{ 0.25 };
};
