
#ifndef _CORNER_EFFECT_H__INCLUDED__2005_01_05_
#define _CORNER_EFFECT_H__INCLUDED__2005_01_05_

#include "SpecialEffect.h"

#define	MAX_VERTEX_NUM	4
#define	MAX_QUAD_NUM	10


class CCornerEffect : public CSpecialEffect
{
public:
	CCornerEffect(HWND, HBITMAP);
	virtual ~CCornerEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC hDC, BOOL);
	virtual BOOL OnTimer(HDC);

private:
	struct ZPoint
	{
		int x, y;
	};

	struct ZQuad
	{
		ZPoint pt[MAX_VERTEX_NUM];
	};

private:
	ZQuad		m_quad[MAX_QUAD_NUM];
	int			m_vx[MAX_VERTEX_NUM];
	int			m_vy[MAX_VERTEX_NUM];
	COLORREF	m_color;

private:
	void Init(HDC);
	void DrawQuads(HDC, COLORREF);
	void QuadStep();
	void ColorStep();
};

#endif // _CORNER_EFFECT_H__INCLUDED__2005_01_05_
