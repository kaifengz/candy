
#ifndef _3_BODY_EFFECT_H__INCLUDED__2007_06_18_
#define _3_BODY_EFFECT_H__INCLUDED__2007_06_18_

#include "SpecialEffect.h"
#include "GLEffect/GLEffect.h"

namespace N3Body {
	class C3BodyScene;
}

///////////////////////////////////////////////////////////////////////////////
//	C3BodyEffect
///////////////////////////////////////////////////////////////////////////////

// TODO: Use OpenGL to draw the scene
class C3BodyEffect : public CSpecialEffect
{
public:
	C3BodyEffect(HWND hWnd, HBITMAP hBmpBkgnd);
	virtual ~C3BodyEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

protected:
	N3Body::C3BodyScene *m_pScene;
};

#endif // _3_BODY_EFFECT_H__INCLUDED__2007_06_18_
