
#ifndef _SPECIAL_EFFECT_H__INCLUDED__2005_01_03_
#define _SPECIAL_EFFECT_H__INCLUDED__2005_01_03_

#define MAX_FRAME_NUM	1000

class CSpecialEffect  
{
public:
	CSpecialEffect(HWND hWnd, HBITMAP hBmpBkgnd, DWORD dwEffectStyle=0);
	virtual ~CSpecialEffect();

	enum EffectStyle
	{
		ES_FILL_BACKGROUND	= 0x00000001,
 		ES_SHOW_FPS			= 0x00000002,
	};

public:
	BOOL DrawFrame(BOOL bMagic = FALSE);
	virtual const char* GetEffectName() const = 0;

protected:

    // Initialize, called before the first call OnTimer, do nothing by default
    virtual BOOL Initialize(HDC hDC,
        BOOL bMagicHong=FALSE)            { return TRUE; }

	// Draw each frame, each effect should do it itsself
	virtual BOOL OnTimer(HDC) = 0;

	// MagicHong, equals to OnTimer() default
	virtual BOOL MagicHong(HDC hDC)	    { return OnTimer(hDC); }

	// The timeslice between two frames, 0 default
	virtual DWORD GetTimeslice() const  { return 0;	}

protected:
    UINT    GetFrameCount()  const;
    HWND    GetWndHandle()   const;
    HBITMAP GetBkgndBmp()    const;
    int     GetWndWidth()    const;
    int     GetWndHeight()   const;
    DWORD   GetElapsedTime() const;

protected:
	void			IncreaseFrameCount();
	unsigned int	GetFPS();
	virtual BOOL	ShouldDrawFPS() const;
	virtual void	DrawFPS(HDC);

private:
	virtual BOOL	DrawNewScreen(BOOL bMagic);
	virtual void	CopyOldScreen();

private:
	const HWND		m_hWnd;
	const HBITMAP	m_hBmpBkgnd;
	int				m_nWndWidth;
	int				m_nWndHeight;

	const DWORD		m_dwEffectStyle;

private:
	RECT			m_rcWnd;
    DWORD           m_tFirstFrame;
	DWORD			m_tLastFrame;
	HBITMAP			m_hBmpScreen;

    UINT            m_uFrameCount;

	FramePerSecond	m_fps;
};

#endif // _SPECIAL_EFFECT_H__INCLUDED__2005_01_03_
