#pragma once

#include <windows.h>

class TrafficLight
{
public:
	enum class State
	{
		GREEN,
		YELLOW,
		RED,
		ALMOST_GREEN,
	};

public:
	TrafficLight() = default;
	~TrafficLight() = default;

	const State state() const { return m_state; }
	void set_state(State state) { m_state = state; }

	void iteration()
	{
		switch (state())
		{
		case State::GREEN: set_state(State::YELLOW); return;
		case State::YELLOW: set_state(State::RED); return;
		case State::RED: set_state(State::ALMOST_GREEN); return;
		case State::ALMOST_GREEN: set_state(State::GREEN); return;
		}
	}


protected:

	State m_state{ State::RED };
};

class TrafficLightDrawable : public TrafficLight
{
public:
	TrafficLightDrawable();
	~TrafficLightDrawable() = default;

	void draw(const HDC context) const;

	constexpr void set_position(const POINT point) { m_position = point; recalc_everything();  }
	constexpr void set_size(const int size) { m_size = { size, (int)(size*2.2f) }; recalc_everything(); }

	constexpr RECT rect() const { return m_rect; }

private:

	constexpr void recalc_everything()
	{
		m_rect = RECT{ m_position.x, m_position.y, m_position.x + m_size.cx, m_position.y + m_size.cy };

		const auto circle_left_origin = (m_size.cx / 4);
		const auto circle_top_origin = (m_size.cy / 12);
		const auto circle_right_origin = (m_size.cx / 2) + (m_size.cx / 4);
		const auto circle_bottom_origin = circle_top_origin + (circle_right_origin - circle_left_origin);

		m_circle1 = { circle_left_origin, circle_top_origin, circle_right_origin, circle_bottom_origin };
		m_circle2 = { circle_left_origin, circle_top_origin + (m_size.cy / 3) - (m_size.cy / 20), circle_right_origin, circle_bottom_origin + (m_size.cy / 3) - (m_size.cy / 20) };
		m_circle3 = { circle_left_origin, circle_top_origin + 2 * (m_size.cy / 3) - 2*(m_size.cy / 20), circle_right_origin, circle_bottom_origin + 2 * (m_size.cy / 3) - 2*(m_size.cy / 20)  };
	}

private:
	HBRUSH background_brush{ nullptr };
	HBRUSH dark_brush{ nullptr };

	HBRUSH red_brush{ nullptr };
	HBRUSH yellow_brush{ nullptr };
	HBRUSH green_brush{ nullptr };

	POINT m_position{ 0, 0 };
	SIZE m_size{ 10, 50 };

	RECT m_circle1{ 0, 0 };
	RECT m_circle2{ 0, 0 };
	RECT m_circle3{ 0, 0 };

	int m_circle_radius{ 0 };

	RECT m_rect{ m_position.x, m_position.y, m_position.x + m_size.cx, m_position.y + m_size.cy };

	constexpr static COLORREF black_color	= 0x00303030;
	constexpr static COLORREF dark_color	= 0x00101010;
	constexpr static COLORREF red_color		= 0x000000FF;
	constexpr static COLORREF yellow_color	= 0x0000FFFF;
	constexpr static COLORREF green_color	= 0x0000FF00;
};

TrafficLightDrawable::TrafficLightDrawable()
{
	background_brush = CreateSolidBrush(black_color);
	dark_brush = CreateSolidBrush(dark_color);
	red_brush = CreateSolidBrush(red_color);
	yellow_brush = CreateSolidBrush(yellow_color);
	green_brush = CreateSolidBrush(green_color);

	recalc_everything();
}

void TrafficLightDrawable::draw(const HDC context) const
{
	//left top right bottom
	FillRect(context, &m_rect, background_brush);

	if (state() == State::RED || state() == State::ALMOST_GREEN)
		SelectObject(context, red_brush);
	else
		SelectObject(context, dark_brush);

	Ellipse(context, m_rect.left + m_circle1.left, m_rect.top + m_circle1.top, m_rect.left + m_circle1.right, m_rect.top + m_circle1.bottom);


	if (state() == State::YELLOW || state() == State::ALMOST_GREEN)
		SelectObject(context, yellow_brush);
	else
		SelectObject(context, dark_brush);

	Ellipse(context, m_rect.left + m_circle2.left, m_rect.top + m_circle2.top, m_rect.left + m_circle2.right, m_rect.top + m_circle2.bottom);

	if (state() == State::GREEN)
		SelectObject(context, green_brush);
	else
		SelectObject(context, dark_brush);

	Ellipse(context, m_rect.left + m_circle3.left, m_rect.top + m_circle3.top, m_rect.left + m_circle3.right, m_rect.top + m_circle3.bottom);
	
}