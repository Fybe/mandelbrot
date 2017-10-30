#include "Spline.hpp"
#include <cmath>

HermiteSpline::HermiteSpline()
{}
HermiteSpline::~HermiteSpline()
{}

void HermiteSpline::push_back(float x, float y)
{
	this->x.push_back(x);
	this->y.push_back(y);
}

void HermiteSpline::build()
{
	// check length issues
	size_t length = std::min(x.size(), y.size());
	if(x.size() != y.size()) 
	{
		x.resize(length);
		y.resize(length);

		x.shrink_to_fit();
		y.shrink_to_fit();
	}

	if(length == 0)
		return;

	std::vector<float> slope;
	for(size_t i = 0; i < length - 1; i++)
		slope.push_back(0);
	for(size_t i = 0; i < length; i++)
		m.push_back(0);

	// slope of secant lines
	for(size_t k = 0; k < length - 1; k++)
	{
		slope[k] = (y[k+1]-y[k])/(x[k+1]-x[k]);
	}

	// slope of tangent lines
	m[0] = slope[0];
	m[length] = slope[length-1];
	for(size_t k = 1; k < length - 1; k++)
	{
		if((slope[k-1] < 0 && slope[k] > 0)
			|| (slope[k-1] > 0 && slope[k] < 0))
		{
			m[k] = 0;
		}
		else
		{
			m[k] = 0.5f * (slope[k-1] + slope[k]);
		}
	}

	// check for monotonicity
	for(size_t k = 1; k < length - 1; k++)
	{
		if((slope[k-1] < 0 && slope[k] > 0)
			|| (slope[k-1] > 0 && slope[k] < 0))
			continue;

		float a = m[k]/slope[k];
		float b = m[k]/slope[k-1];

		if(a < 0 || b < 0)
			m[k] = 0;
	}

	// prevent overshooting
	for(size_t k = 0; k < length - 1; k++)
	{
		if((slope[k-1] < 0 && slope[k] > 0)
			|| (slope[k-1] > 0 && slope[k] < 0))
			continue;

		float a = m[k]/slope[k];
		float b = m[k+1]/slope[k];

		float x = a*a + b*b;
		if(x > 9)
		{
			m[k] = 3*a*slope[k]/std::sqrt(x);
			m[k+1] = m[k];
		}
	}
}
void HermiteSpline::build(std::vector<float> const &x_values, std::vector<float> const &y_values)
{
	this->x = std::vector<float>(x_values);
	this->y = std::vector<float>(y_values);
	build();
}

// basis functions for hermite spline
float HermiteSpline::h00(float t) const
{
	float tm = 1-t;
	return (1+2*t)*tm*tm;
}
float HermiteSpline::h10(float t) const
{
	float tm = 1-t;
	return t*tm*tm;
}
float HermiteSpline::h01(float t) const
{
	float tm = 3-2*t;
	return t*t*tm;
}
float HermiteSpline::h11(float t) const
{
	return t*t*(t-1);
}

float HermiteSpline::operator()(float t) const
{
	if(x.size() == 0)
		return 0;
	if(x.size() == 1)
		return y.front();

	size_t k_lower = 0;
	size_t k_upper = 0;

	if(t < x.front())
		return x.front();
	else if(t > x.back())
		return x.back();

	for(size_t k = 0; k < x.size(); k++)
	{
		if(x[k] > t)
		{
			k_lower = k-1;
			k_upper = k;
			break;
		}
	}

	float h = x[k_upper] - x[k_lower];
	float ts = (t - x[k_lower]) / h;

	return y[k_lower]*h00(ts) + h*m[k_lower]*h10(ts) + y[k_upper]*h01(ts) + h*m[k_upper]*h11(ts);
}
