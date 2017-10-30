#ifndef SPLINE_HPP
#define SPLINE_HPP
#include <vector>

class HermiteSpline
{
public:
	HermiteSpline();
	~HermiteSpline();

	void push_back(float x, float y);

	void build();
	void build(std::vector<float> const &x_values, std::vector<float> const &y_values);

	float operator()(float t) const;

private:
	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> m;

	float h00(float t) const;
	float h10(float t) const;
	float h01(float t) const;
	float h11(float t) const;

};

#endif
