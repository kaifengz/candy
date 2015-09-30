
#ifndef _PURLER_OBJECT_H__INCLUDED__2007_08_25_
#define _PURLER_OBJECT_H__INCLUDED__2007_08_25_

#include "ZkfGraphics.h"

namespace NPurler {

class CPurlerBar;
class CPurlerNail;
class CPurlerBoundary;



class CPurlerBar : public ZBar
{
public:

public:
	BOOL Initialize(double x, double y,
		double length, double dummy_thickness,
		double mass, COLORREF color);
	BOOL Draw(HDC hDC, int xDrawOffset, int yDrawOffset,
		RECT screen) const;

private:
	COLORREF color;
	double drawing_thickness;
};

class CPurlerNail : public ZMovableCircle
{
public:
	CPurlerNail(double x, double y, double radius, double mass, COLORREF color);

public:
	BOOL Draw(HDC hDC, int xDrawOffset, int yDrawOffset,
		RECT screen) const;
	BOOL Stop();

private:
	COLORREF color;
};

class CPurlerBoundary : public ZBoundary
{
public:

public:
	void Initialize(double width, double height, COLORREF color);
	BOOL Draw(HDC hDC, int xDrawOffset, int yDrawOffset,
		RECT screen) const;

private:
	COLORREF color;
};

} // namespace NPurler

#endif // _PURLER_OBJECT_H__INCLUDED__2007_08_25_
