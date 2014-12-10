
#ifndef _HYPOCYCLOID_EFFECT_H__INCLUDED__2011_01_15_
#define _HYPOCYCLOID_EFFECT_H__INCLUDED__2011_01_15_

#include "SpecialEffect.h"
#include "HypocycloridWidget.h"

class CHypocycloidEffect : public CSpecialEffect  
{
public:
	CHypocycloidEffect(HWND, HBITMAP);
	virtual ~CHypocycloidEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
	struct ZWidget
	{
		CHypocycloridWidget widget;
		COLORREF color;
		unsigned int left, top;
		unsigned int size;
		unsigned int birth;
		unsigned int dying;
		unsigned int dead;
		bool alive;
	};
	typedef vector<ZWidget> ZWidgetArray;

	ZWidgetArray m_widgets;

private:
	void NewWidget();
	void DrawWidget(HDC, ZWidget &);
};

#endif // _HYPOCYCLOID_EFFECT_H__INCLUDED__2011_01_15_
