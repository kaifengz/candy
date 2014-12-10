
#ifndef _HYPOCYCLOID_WIDGET_H__INCLUDED__2011_01_15_
#define _HYPOCYCLOID_WIDGET_H__INCLUDED__2011_01_15_

class CHypocycloridWidget
{
public:
	CHypocycloridWidget();
	~CHypocycloridWidget();

public:
	void Initialize(unsigned int left, unsigned int top, unsigned int size);
	void Step();
	void Draw(HDC);

private:
	struct ZPoint { unsigned int x, y; };
	typedef vector<ZPoint> ZPointArray;
	ZPointArray m_points;
	ZPoint m_left_top;
	unsigned int m_outer_radius;
	unsigned int m_inner_radius;
	double m_theta;
	double m_theta_step;
};

#endif _HYPOCYCLOID_WIDGET_H__INCLUDED__2011_01_15_
