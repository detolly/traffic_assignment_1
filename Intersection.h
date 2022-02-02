#pragma once

#include "TrafficLight.h"

#include <windows.h>

#include <vector>

template<typename T>
class Vector2
{
public:
	Vector2(T x, T y) : m_x(x), m_y(y) {}
	Vector2(const Vector2& other) = default;
	Vector2(Vector2&& other) = delete;
	

public:
	T x() const { return m_x; }
	T y() const { return m_y; }

	void set_x(const T& other) { m_x = other; }
	void set_y(const T& other) { m_y = other; }

	inline void operator=(const Vector2<T>& other) { m_x = other.m_x; m_y = other.m_y; };
	inline void operator=(Vector2<T>& other) { m_x = other.m_x; m_y = other.m_y; };

	inline void operator==(const Vector2<T>& other) { return m_x == other.m_x && m_y == other.m_y; }

	inline void operator+=(const Vector2<T>& other)
	{
		m_x = other.m_x + m_x;
		m_y = other.m_y + m_y;
	}

	inline void operator*=(const Vector2<T>& other)
	{
		m_x = other.m_x * m_x;
		m_y = other.m_y * m_y;
	}

	inline Vector2 operator+(const Vector2<T>& other)
	{
		return { other.x + x, other.y + y };
	}

private:

	T m_x;
	T m_y;
};

enum class Orientation : int {
	VERTICAL = 0,
	HORIZONTAL = 1
};


template<Orientation orientation>
class Car {

public:
	
	Car(Vector2<float> position) : m_position(position) {
		m_color = rand();
		brush = CreateSolidBrush(m_color);
		if constexpr (orientation == Orientation::HORIZONTAL) m_velocity = { 5.0f, 0.0f };
		if constexpr (orientation == Orientation::VERTICAL) m_velocity = { 0.0F, 5.0f };
	}

	COLORREF color() const { return m_color; }

	void update(bool should_drive)
	{

		auto velocity = 0.0f;
		auto acceleration = 0.0f;
		if constexpr (orientation == Orientation::HORIZONTAL) velocity		= m_velocity.x();		else velocity		= m_velocity.y();
		if constexpr (orientation == Orientation::HORIZONTAL) acceleration	= m_acceleration.x();	else acceleration	= m_acceleration.y();

		if (should_drive && velocity < 500.0f)
			acceleration = 80.0f;
		else if (!should_drive && velocity > 0.0f)
			acceleration = -280.0f;
		else if (!should_drive && velocity < 0.0f)
		{
			velocity = 0;
			acceleration = 0.0f;
		}
		else {
			acceleration = 0.0f;
		}
		velocity += acceleration * DELTA_TIME;
		if constexpr (orientation == Orientation::HORIZONTAL)	{ m_position += { velocity* DELTA_TIME, 0.0f }; m_velocity = { velocity, 0.0f }; }
		else													{ m_position += { 0.0f, velocity* DELTA_TIME }; m_velocity = { 0.0f, velocity }; }

	}

	void draw(const HDC context) const
	{
		const SIZE size = (orientation == Orientation::HORIZONTAL) ? SIZE { 40, 20 } : SIZE { 20, 40 };
		const auto a = RECT{ (LONG)roundf(m_position.x()), (LONG)roundf(m_position.y()), (LONG)floorf(m_position.x()) + size.cx, (LONG)floorf(m_position.y()) + size.cy };
		FillRect(context, &a, brush);
	}

	Vector2<float> position() const { return m_position; }
	Vector2<float> velocity() const { return m_velocity; }
	Vector2<float> acceleration() const { return m_acceleration; }

private:

	Vector2<float> m_position{ 0.0f, 0.0f };
	Vector2<float> m_velocity{ 0.0f, 0.0f };
	Vector2<float> m_acceleration{ 0.0f, 0.0f };
	
	COLORREF m_color;
	HBRUSH brush;
};

template<Orientation orientation>
class Road {
public:

	Road();
	~Road() = default;

	void draw(const HDC context) const;

	constexpr void set_position(POINT position) { m_position = position; }
	constexpr void set_size(SIZE size) { m_size = size; }

private:
	POINT m_position{ 0, 0 };
	SIZE m_size{ 60, 300 };

	HBRUSH background_brush;
	HBRUSH lane_brush;

	constexpr static COLORREF background_color = 0x00101010;
	constexpr static COLORREF lane_color = 0x00AFAFAF;
	
};

template<Orientation orientation>
Road<orientation>::Road()
{
	background_brush = CreateSolidBrush(background_color);
	lane_brush = CreateSolidBrush(lane_color);
}

template<Orientation orientation>
void Road<orientation>::draw(const HDC context) const
{
	RECT road_rect;
	if constexpr (orientation == Orientation::VERTICAL)
		 road_rect = { m_position.x, m_position.y, m_position.x + m_size.cx, m_position.y + m_size.cy };
	if constexpr (orientation == Orientation::HORIZONTAL)
		road_rect = { m_position.x, m_position.y, m_position.x + m_size.cy, m_position.y + m_size.cx };
	
	FillRect(context, &road_rect, background_brush);
}

class Intersection
{
public:
	Intersection();
	~Intersection() = default;

	void draw(const HDC context) const;

	void iterate_trafficlight();
	void iterate_frame();

private:

	std::size_t seconds_since_last_switch{ 0 };

	TrafficLightDrawable west_light;
	Road<Orientation::HORIZONTAL> west_road;
	Road<Orientation::HORIZONTAL> east_road;

	TrafficLightDrawable north_light;
	Road<Orientation::VERTICAL> north_road;
	Road<Orientation::VERTICAL> south_road;

	HBRUSH background_brush;
	constexpr static COLORREF background_color = 0x0040404040;

	std::vector<Car<Orientation::HORIZONTAL>> m_horizontal_cars;
	std::vector<Car<Orientation::VERTICAL>> m_vertical_cars;

	enum class State {
		WEST_DRIVING_NORTH_STOPPED,
		WEST_STOPPING_NORTH_STOPPED,
		WEST_STOPPED_NORTH_STARTING,
		WEST_STOPPED_NORTH_DRIVING,
		WEST_STOPPED_NORTH_STOPPING,
		WEST_STARTING_NORTH_STOPPED,
	} current_state{ State::WEST_DRIVING_NORTH_STOPPED };

};

void Intersection::iterate_frame()
{
	bool should_make_new_one_top = rand() % 150 == 0;
	bool should_make_new_one_left = rand() % 150 == 0;

	if (should_make_new_one_top)
		m_vertical_cars.push_back(Car<Orientation::VERTICAL>{ { (float)(500 + rand() % (60 - 30)), float(100) } });
	if (should_make_new_one_left)
		m_horizontal_cars.push_back(Car<Orientation::HORIZONTAL>{ { (float)(200), (float)(400 + rand() % (60 - 30))  } });

	std::vector<std::vector<Car<Orientation::HORIZONTAL>>::iterator> h_iterators;
	for (auto iter = m_horizontal_cars.begin(); iter != m_horizontal_cars.end(); iter++) {
		auto& car = *iter;
		const bool can_drive = current_state == State::WEST_DRIVING_NORTH_STOPPED || current_state == State::WEST_STARTING_NORTH_STOPPED;
		bool should_drive = can_drive || car.position().x() < 200 + 200 || car.position().x() > 200 + 230 + 60;
		car.update(should_drive);
		if (car.position().x() > 200 + 300 + 300)
			h_iterators.push_back(iter);
	}
	for(int i = h_iterators.size()-1; i >= 0; i--)
		m_horizontal_cars.erase(h_iterators[i]);

	std::vector<std::vector<Car<Orientation::VERTICAL>>::iterator> iterators;
	for (auto iter = m_vertical_cars.begin(); iter != m_vertical_cars.end(); iter++) {
		auto& car = *iter;
		const bool can_drive = current_state == State::WEST_STOPPED_NORTH_DRIVING || current_state == State::WEST_STOPPED_NORTH_STARTING;
		bool should_drive = can_drive || car.position().y() < 300 || car.position().y() > 300+60;
		car.update(should_drive);
		if (car.position().y() > 100 + 300 + 300 + 60)
			iterators.push_back(iter);
	}
	for (int i = iterators.size() - 1; i >= 0; i--)
		m_vertical_cars.erase(iterators[i]);
}

void Intersection::iterate_trafficlight()
{
	seconds_since_last_switch++;

	switch (current_state)
	{
	case State::WEST_DRIVING_NORTH_STOPPED:
		if (seconds_since_last_switch >= 9) {
			current_state = State::WEST_STOPPING_NORTH_STOPPED;
			west_light.set_state(TrafficLight::State::YELLOW);
			north_light.set_state(TrafficLight::State::RED);
			seconds_since_last_switch = 0;
		}
		break;
	case State::WEST_STOPPING_NORTH_STOPPED:
		if (seconds_since_last_switch >= 2)
		{
			current_state = State::WEST_STOPPED_NORTH_STARTING;
			west_light.set_state(TrafficLight::State::RED);
			north_light.set_state(TrafficLight::State::ALMOST_GREEN);
			seconds_since_last_switch = 0;
		}
		break;
	case State::WEST_STOPPED_NORTH_STARTING:
		if (seconds_since_last_switch >= 2)
		{
			current_state = State::WEST_STOPPED_NORTH_DRIVING;
			west_light.set_state(TrafficLight::State::RED);
			north_light.set_state(TrafficLight::State::GREEN);
			seconds_since_last_switch = 0;
		}
	case State::WEST_STOPPED_NORTH_DRIVING:
		if (seconds_since_last_switch >= 9)
		{
			current_state = State::WEST_STOPPED_NORTH_STOPPING;
			west_light.set_state(TrafficLight::State::RED);
			north_light.set_state(TrafficLight::State::YELLOW);
			seconds_since_last_switch = 0;
		}
		break;
	case State::WEST_STOPPED_NORTH_STOPPING:
		if (seconds_since_last_switch >= 2)
		{
			current_state = State::WEST_STARTING_NORTH_STOPPED;
			west_light.set_state(TrafficLight::State::ALMOST_GREEN);
			north_light.set_state(TrafficLight::State::RED);
			seconds_since_last_switch = 0;
		}
		break;
	case State::WEST_STARTING_NORTH_STOPPED:
		if (seconds_since_last_switch >= 2)
		{
			current_state = State::WEST_DRIVING_NORTH_STOPPED;
			west_light.set_state(TrafficLight::State::GREEN);
			north_light.set_state(TrafficLight::State::RED);
			seconds_since_last_switch = 0;
		}
		break;
	}

	
}

Intersection::Intersection()
{
	west_light.set_size(100);
	west_light.set_position({ 140, 140 });
	west_light.set_state(TrafficLight::State::GREEN);

	north_light.set_size(100);
	north_light.set_position({ 600, 550 });
	north_light.set_state(TrafficLight::State::RED);

	west_road.set_position({ 200, 400 });
	east_road.set_position({ 200+300+60, 400 });
	north_road.set_position({ 500, 100 });
	south_road.set_position({ 500, 460 });

	background_brush = CreateSolidBrush(background_color);

}

void Intersection::draw(const HDC context) const
{
	west_road.draw(context);
	east_road.draw(context);
	north_road.draw(context);
	south_road.draw(context);
	north_light.draw(context);
	west_light.draw(context);

	constexpr static RECT intersection_rect{ 500, 400, 560, 460 };
	
	FillRect(context, &intersection_rect, background_brush);

	for (const auto& car : m_horizontal_cars) {
		car.draw(context);
	}

	for (const auto& car : m_vertical_cars) {
		car.draw(context);
	}
}