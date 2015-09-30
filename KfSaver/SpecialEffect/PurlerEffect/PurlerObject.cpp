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

BOOL CPurlerBar::Draw(HDC hDC, int xDrawOffset, int yDrawOffset, RECT screen) const
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
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_1), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_1), screen),
		NULL);
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_2), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_2), screen));
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_3), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_3), screen));
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_4), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_4), screen));
	LineTo(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x_vertex_1), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y_vertex_1), screen));
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

BOOL CPurlerNail::Draw(HDC hDC, int xDrawOffset, int yDrawOffset, RECT screen) const
{
	const double x = xDrawOffset + this->barycenter.x;
	const double y = yDrawOffset + this->barycenter.y;

	if (x + this->radius <= 0 ||
		x - this->radius >= screen.right - screen.left ||
		y + this->radius <= 0 ||
		y - this->radius >= screen.bottom - screen.top)
	{
		// out of screen, does not need to draw
		return TRUE;
	}

	HBRUSH hBrush = CreateSolidBrush(this->color);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	HPEN hPen = CreatePen(PS_SOLID, 1, this->color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	Ellipse(hDC,
		X_SPACE_TO_SCREEN(RoundToInt(x - radius), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y - radius), screen),
		X_SPACE_TO_SCREEN(RoundToInt(x + radius), screen),
		Y_SPACE_TO_SCREEN(RoundToInt(y + radius), screen));

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

BOOL CPurlerBoundary::Draw(HDC hDC, int xDrawOffset, int yDrawOffset, RECT screen) const
{
	const int xLeft = screen.left + RoundToInt(xDrawOffset + this->left);
	const int xRight = screen.left + RoundToInt(xDrawOffset + this->right);

	HPEN hPen = CreatePen(PS_SOLID, 1, this->color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	MoveToEx(hDC, xLeft, screen.top, NULL);
	LineTo(hDC, xLeft, screen.bottom);

	MoveToEx(hDC, xRight, screen.top, NULL);
	LineTo(hDC, xRight, screen.bottom);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	return TRUE;
}

} // namespace NPurler
