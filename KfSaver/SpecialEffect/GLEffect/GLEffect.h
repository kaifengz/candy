
#ifndef _GL_EFFECT_H__INCLUDED__2007_03_15_
#define _GL_EFFECT_H__INCLUDED__2007_03_15_

#include "SpecialEffect.h"

class CGLEffect : public CSpecialEffect  
{
public:
	CGLEffect(HWND hWnd, HBITMAP hBmpBkgnd, DWORD dwEffectStyle=0);
	virtual ~CGLEffect();

private:
	virtual BOOL	DrawNewScreen(BOOL bMagic);
	virtual void	CopyOldScreen();

///////////////////////////////////////////////////////////////////////////////
//	OpenGL Support
///////////////////////////////////////////////////////////////////////////////

protected:
	class GLGuarder
	{
	public:
		GLGuarder(HDC);
		~GLGuarder();

		static BOOL BeginGL(HDC);
		static BOOL EndGL();

	protected:
		static unsigned int m_nBeginGL;
	};

protected:
	static GLenum LogGLError();

	static GLuint LoadTexture(HDC hDC, UINT uIDBmp,
							  int nWidth, int nHeight);

private:
	static BOOL InitOpenGL(HWND); 
	static BOOL LoadNewTexture(GLuint texture,
							   HDC hDC, UINT uIDBmp,
							   int nWidth, int nHeight);

private:
	static HGLRC m_hrc;

	struct TexMap
	{
		GLuint texture;
		UINT uIDBmp;

		TexMap(GLuint tex, UINT id) :
			texture(tex), uIDBmp(id)
		{
		}
	};
	typedef vector<TexMap> TexMapArray;
	static TexMapArray m_textures;
};

#endif // _GL_EFFECT_H__INCLUDED__2007_03_15_
