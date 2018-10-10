#include <Pokitto.h>
#include <cstddef>
#include <cmath>
#include <array>

using Pokitto::Core;
using Pokitto::Display;
using Pokitto::Buttons;

static const uint16_t palette[] =
{
	0x0000, 0xffff, 0x4B44, 0x9F29, 0xF81F,
	0xC1C7, 0xC307, 0x4A49, 0x6B4B, 0x7BCF,
	0xC5F1, 0x948D, 0x5AE8,
};

constexpr float pi = 3.14159265359f;
constexpr float radiansToDegreesFactor = 180.0f / pi;
constexpr float degreesToRadiansFactor = pi / 180.0f;

constexpr float radiansToDegrees(float radians)
{
	return radians * radiansToDegreesFactor;
}

constexpr float degreesToRadians(float degrees)
{
	return degrees * degreesToRadiansFactor;
}

constexpr int normaliseAngle(int angle)
{
	return (((-angle) % 360) + 360) % 360;
}

constexpr float normaliseAngle(float angle)
{
	return (((-angle) % 360) + 360) % 360;
}

constexpr float lengthDirectionX(float length, int direction)
{
	return length * std::cos(degreesToRadians(normaliseAngle(direction)));
}

constexpr float lengthDirectionY(float length, int direction)
{
	return length * std::sin(degreesToRadians(normaliseAngle(direction)));
}

float getAngle(float x1, float y1, float x2, float y2)
{
	return normaliseAngle(radiansToDegrees(std::atan2(y1 - y2, x1 - x2)));
}

template<typename Type>
struct Point
{
	using ValueType = Type;

	ValueType x;
	ValueType y;
	
	Point(void) = default;
	Point(ValueType x, ValueType y)
		: x(x), y(y)
	{
	}
};

class Segment
{
private:
	float x;
	float y;
	float direction;
	
public:
	Segment(void) = default;

	Segment(float x, float y, int direction)
		: x(x), y(y), direction(direction)
	{
	}

	Segment(Point<int> point, int direction)
		: Segment(point.x, point.y, direction)
	{
	}
	
	float getX(void) const
	{
		return this->x;
	}
	
	float getY(void) const
	{
		return this->y;
	}
	
	float getDirection(void) const
	{
		return this->direction;
	}

	void drawSegment(void)
	{
		Display::drawLine(this->x, this->y, this->x + lengthDirectionX(7, dir), this->y + lengthDirectionY(7, dir));
	}

	void moveSegmentTowards(Point<float> point)
	{
		this->moveSegmentTowards(point.x, point.y);
	}

	void moveSegmentTowards(float x, float y)
	{
		this->direction = getAngle(x, y, this->x, this->y);

		this->x = (x - lengthDirectionX(8, this->direction));
		this->y = (y - lengthDirectionY(8, this->direction));
	}
};

template<std::size_t N>
class Rope
{
private:
	std::array<Segment, N> segments;

public:	
	Rope(float x, float y)
	{
		for (std::size_t i = 0; i < segments.size(); ++i)
		{
			segments[i] = Segment(x, y, 90);
			y -= 8;
		}
	}
	
	Rope(Point<float> point)
		: Rope(point.x, point.y)
	{
	}

	void drawRope(void)
	{
		for (std::size_t i = 0; i < segments.size(); ++i)
			segments[i].drawSegment();
	}

	void moveRopeTowards(int x, int y)
	{
		segments[0].moveSegmentTowards(x, y);
		for (std::size_t i = 1; i < segments.size(); ++i)
			segments[i].moveSegmentTowards(segments[i - 1].getX(), segments[i - 1].getY());
	}
};

void moveCursor(Point<float> & cursor)
{
	if (Buttons::rightBtn())
		cursor.x += 3;

	if (Buttons::upBtn())
		cursor.y -= 3;

	if (Buttons::leftBtn())
		cursor.x -= 3;

	if (Buttons::downBtn())
		cursor.y += 3;
}

void drawCursor(Point<float> cursor)
{
	Display::drawPixel(static_cast<int>(cursor.x), static_cast<int>(cursor.y));
}

enum class GameState
{
	Gameplay,
};

int main(void)
{
	Core::begin();
	Core::setFrameRate(30);
	
	Display::load565Palette(palette);
	Display::bgcolor = 4;
	Display::setInvisibleColor(4);
	
	GameState gameState = GameState::Gameplay;
	Rope<10> rope = Rope(50, 87);
	Point<int> cursor;
	
	while (Core::isRunning())
	{
		if(!Core::update())
			continue;
		
		Buttons::pollButtons();
		switch(gameState)
		{
			case GameState::Gameplay:
			{
				moveCursor(cursor);
				rope.moveRopeTowards(cursor);
				
				Display::setColor(0);
				rope.drawRope();
				Display::setColor(1);
				
				drawCursor(cursor);
				break;
			}
		}
	}
}
