// PurlerObject.cpp
//

#include "StdAfx.h"
#include "PurlerObject.h"

namespace NPurler {

////////////////////////////////////////////////////////////////////////////////
//	CPurlerBar
////////////////////////////////////////////////////////////////////////////////

BOOL CPurlerBar::Initialize(double x, double y,
		double length, double drawing_thickness,
		double mass, COLORREF color)
{
	this->ao = 0;
	this->av = 0;
	this->barycenter.x = x;
	this->barycenter.y = y;
	this->color = color;
	this->drawing_thickness = drawing_thickness;
	this->length = length;
	this->mass = mass;
	this->velocity.dx = 0;
	this->velocity.dy = 0;
	return TRUE;
}

BOOL CPurlerBar::Draw(HDC hDC, int xDrawOffset, int yDrawOffset,
		int nScreenWidth, int nScreenHeight) const
{
	const double x = xDrawOffset + this->barycenter.x;
	const double y = yDrawOffset + this->barycenter.y;

	const double x_vertex_m = x + this->length * cos(this->ao) / 2.0;
	const double y_vertex_m = y + this->length * sin(this->ao) / 2.0;

	const double x_vertex_1 = x_vertex_m +
		this->drawing_thickness * cos(this->ao - PI/2.0) / 2.0;
	const double y_vertex_1 = y_vertex_m +
		this->drawing_thickness * sin(this->ao - PI/2.0) / 2.0;

	const double x_vertex_2 = x_vertex_m * 2 - x_vertex_1;
	const double y_vertex_2 = y_vertex_m * 2 - y_vertex_1;

	const double x_vertex_3 = x * 2 - x_vertex_1;
	const double y_vertex_3 = y * 2 - y_vertex_1;

	const double x_vertex_4 = x * 2 - x_vertex_2;
	const double y_vertex_4 = y * 2 - y_vertex_2;

	HBRUSH hBrush = CreateSolidBrush(this->color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	HPEN hPen = CreatePen(PS_SOLID, 1, this->color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	BeginPath(hDC);
	MoveToEx(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_1), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_1), nScreenHeight),
		NULL);
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_2), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_2), nScreenHeight));
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_3), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_3), nScreenHeight));
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_4), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_4), nScreenHeight));
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_1), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_1), nScreenHeight));
	EndPath(hDC);
	FillPath(hDC);

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	CPurlerNail
////////////////////////////////////////////////////////////////////////////////

CPurlerNail::CPurlerNail(double x, double y, double radius, double mass, COLORREF color)
{
	this->barycenter.x = x;
	this->barycenter.y = y;
	this->radius = radius;
	this->mass = mass;
	this->velocity.dx = 0;
	this->velocity.dy = 0;

	this->color = color;
}

BOOL CPurlerNail::Draw(HDC hDC, int xDrawOffset, int yDrawOffset,
		int nScreenWidth, int nScreenHeight) const
{
	const double x = xDrawOffset + this->barycenter.x;
	const double y = yDrawOffset + this->barycenter.y;

	if (x + this->radius <= 0 ||
		x - this->radius >= nScreenWidth ||
		y + this->radius <= 0 ||
		y - this->radius >= nScreenHeight)
	{
		// out of screen, does not need to draw
		return TRUE;
	}

	HBRUSH hBrush = CreateSolidBrush(this->color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	HPEN hPen = CreatePen(PS_SOLID, 1, this->color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	Ellipse(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x - radius), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y - radius), nScreenHeight),
		X_SPACE_TO_SCREEN(RoundToInt(x + radius), nScreenWidth),
		Y_SPACE_TO_SCREEN(RoundToInt(y + radius), nScreenHeight));

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	return TRUE;
}

BOOL CPurlerNail::Stop()
{
	this->velocity.dx = 0;
	this->velocity.dy = 0;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	CPurlerBoundary
////////////////////////////////////////////////////////////////////////////////

void CPurlerBoundary::Initialize(double width, double height, COLORREF color)
{
	ZBoundary::Initialize(0, 0, width, height);
	this->color = color;
}

BOOL CPurlerBoundary::Draw(HDC hDC, int xDrawOffset, int yDrawOffset,
		int nScreenWidth, int nScreenHeight) const
{
	const int xLeft = RoundToInt(xDrawOffset + this->left);
	const int xRight = RoundToInt(xDrawOffset + this->right);

	HPEN hPen = CreatePen(PS_SOLID, 1, this->color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, xLeft, 0, NULL);
	LineTo(hDC, xLeft, nScreenHeight);

	MoveToEx(hDC, xRight, 0, NULL);
	LineTo(hDC, xRight, nScreenHeight);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	return TRUE;
}

} // namespace NPurler
