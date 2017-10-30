#include "ColorScheme.hpp"

MonotonicColorScheme::MonotonicColorScheme()
{
	rSpline.build();
	gSpline.build();
	bSpline.build();
}
MonotonicColorScheme::MonotonicColorScheme(std::vector<std::pair<float, Pixel>> nodes)
{
	for(auto p : nodes)
	{
		rSpline.push_back(p.first, p.second.red);
		gSpline.push_back(p.first, p.second.green);
		bSpline.push_back(p.first, p.second.blue);
	}

	rSpline.build();
	gSpline.build();
	bSpline.build();
}
MonotonicColorScheme::~MonotonicColorScheme()
{}

Pixel MonotonicColorScheme::GetColor(float value) const
{
	return Pixel(
			rSpline(value),
			gSpline(value),
			bSpline(value)
			);
}

ColorScheme *MonotonicColorScheme::clone() const
{
	return new MonotonicColorScheme(*this);
}
