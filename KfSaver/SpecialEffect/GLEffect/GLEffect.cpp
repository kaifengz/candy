// GLEffect.cpp: implementation of the CGLEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GLEffect.h"

///////////////////////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////////////////////

HGLRC CGLEffect::m_hrc = NULL;
CGLEffect::TexMapArray CGLEffect::m_textures;

#define GL_MAX_TEXTURES		32

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGLEffect::CGLEffect(HWND hWnd, HBITMAP hBmpBkgnd, DWORD dwEffectStyle) :
	CSpecialEffect(hWnd, hBmpBkgnd, dwEffectStyle&(~ES_FILL_BACKGROUND))
{
	InitOpenGL(hWnd);
}

CGLEffect::~CGLEffect()
{
}

///////////////////////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////////////////////

BOOL CGLEffect::DrawNewScreen(BOOL bMagic)
{
	HDC hDC = GetDC(GetWndHandle());

	BOOL bRet = FALSE;
    BOOL bInitializeFail = FALSE;

    if(GetFrameCount() == 0)
    {
        if(!Initialize(hDC, bMagic))
            bInitializeFail = TRUE;
    }

    if(bInitializeFail)
        bRet = FALSE;
    else
		bRet = (bMagic ? MagicHong(hDC) : OnTimer(hDC));

	ReleaseDC(GetWndHandle(), hDC);
	return bRet;
}

void CGLEffect::CopyOldScreen()
{
	// nothing to do for OpenGL drawing
}

///////////////////////////////////////////////////////////////////////////////
//	OpenGL Support: GLGuarder
///////////////////////////////////////////////////////////////////////////////

unsigned int CGLEffect::GLGuarder::m_nBeginGL = 0;

CGLEffect::GLGuarder::GLGuarder(HDC hDC)
{
	VERIFY(BeginGL(hDC));
}

CGLEffect::GLGuarder::~GLGuarder()
{
	VERIFY(EndGL());
}

BOOL CGLEffect::GLGuarder::BeginGL(HDC hDC)
{
	if(m_nBeginGL > 0)
	{
		++m_nBeginGL;
		return TRUE;
	}

	if(wglMakeCurrent(hDC, m_hrc))
	{
		++m_nBeginGL;
		return TRUE;
	}

	return FALSE;
}

BOOL CGLEffect::GLGuarder::EndGL()
{
	if(--m_nBeginGL > 0)
		return TRUE;

	return wglMakeCurrent(NULL, NULL);
}

////////////////////////////////////////////////////////////////////////////////
//	OpenGL Support
////////////////////////////////////////////////////////////////////////////////

GLenum CGLEffect::LogGLError()
{
	GLenum err = glGetError();
	if(err != 0)
		LOG("glGetError => 0x%08X (%u)", err, err);
	return err;
}

BOOL CGLEffect::InitOpenGL(HWND hWnd)
{
	{	// make sure only init one time
		static BOOL bInitOpenGLCalled = FALSE;
		if(bInitOpenGLCalled)
			return TRUE;
		bInitOpenGLCalled = TRUE;
	}

	// the code below will execute only once
	LOG("InitOpenGL...");

	HDC hDC = GetDC(hWnd);
	PIXELFORMATDESCRIPTOR pfd;

	{	// An OpenGL window should be created with the WS_CLIPCHILDREN
		// and WS_CLIPSIBLINGS styles, check the window styles
		DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
		LOG("..Window style = 0x%x", dwStyle);
		if( (dwStyle & WS_CLIPCHILDREN) == 0 ||
			(dwStyle & WS_CLIPSIBLINGS) == 0)
		{
			LOG("..Init OpenGL fail: An OpenGL window should be created with the "
				"WS_CLIPCHILDREN and WS_CLIPSIBLINGS styles");
			goto failed;
		}
		LOG("..Window style match");
	}

	{	// Fill in the Pixel Format Descriptor
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

		pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);   
		pfd.nVersion	= 1;						// Version number
		pfd.dwFlags		= PFD_DOUBLEBUFFER |		// Use double buffer
						  PFD_SUPPORT_OPENGL |		// Use OpenGL
						  PFD_DRAW_TO_WINDOW;		// Pixel format is for a window.
//						  PFD_DRAW_TO_BITMAP |
//						  PFD_SUPPORT_GDI;
		pfd.iPixelType	= PFD_TYPE_RGBA;
		pfd.cColorBits	= 8; // 24;						// 8-bit color
		pfd.cDepthBits	= 32;						// 32-bit depth buffer
		pfd.iLayerType	= PFD_MAIN_PLANE;			// Layer type
	}

	{	// choose pixel format & set pixel format
		int nPixelFormat = ChoosePixelFormat(hDC, &pfd);
		if(nPixelFormat == 0)
		{
			LOG("..ChoosePixelFormat fail %d", GetLastError());
			goto failed;
		}
		LOG("..Pixel Format %d", nPixelFormat) ;

		BOOL bResult = SetPixelFormat(hDC, nPixelFormat, &pfd);
		if(!bResult)
		{
			LOG("..SetPixelFormat fail %d", GetLastError());
			goto failed;
		}
		LOG("..SetPixelFormat success");
	}

	{	// Create a rendering context.
		m_hrc = wglCreateContext(hDC);
		if(m_hrc == NULL)
		{
			LOG("..wglCreateContext Failed %d", GetLastError());
			goto failed;
		}
		LOG("..wglCreateContext success, hGLRC = 0x%x", m_hrc);

		BOOL bResult = wglMakeCurrent(hDC, m_hrc);
		if(!bResult)
		{
			LOG("..wglMakeCurrent fail %d", GetLastError());
			goto failed;
		}
		LOG("..wglMakeCurrent success, hDC = 0x%x, hGLRC = 0x%x", hDC, m_hrc);
		wglMakeCurrent(NULL, NULL);
	}

	{	// gen textures...
		if(GLGuarder::BeginGL(hDC))
		{
			GLuint textures[GL_MAX_TEXTURES];
			glGenTextures(GL_MAX_TEXTURES, textures);
			GLenum err = glGetError();
			if(err == 0)
			{
				LOG("..glGenTextures success, %d texture(s) generated",
					GL_MAX_TEXTURES);
				for(int i=0; i<GL_MAX_TEXTURES; i++)
				{
					m_textures.push_back(TexMap(textures[i], 0));
				}
			}
			else
			{
				LOG("..glGenTextures fail 0x%08X", err);
			}
			GLGuarder::EndGL();
		}
		else
		{
			LOG("..BeginGL fail, will not pre-gen textures");
		}
	}

	// Release DC & return
	ReleaseDC(hWnd, hDC);
	LOG("InitOpenGL success");
	return TRUE;

failed:
	// Release DC & return
	ReleaseDC(hWnd, hDC);
	LOG("InitOpenGL fail");
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//	OpenGL Utilities
///////////////////////////////////////////////////////////////////////////////

GLuint CGLEffect::LoadTexture(HDC hDC, UINT uIDBmp,
							  int nWidth, int nHeight)
{
	for(unsigned int i=0; i<m_textures.size(); i++)
	{
		const TexMap &tex = m_textures[i];
		if(tex.uIDBmp == uIDBmp)
		{
			LOG("LoadTexture(0x%08X) => %u",
				uIDBmp, tex.texture);
			return tex.texture;
		}
	}

	for(unsigned int i=0; i<m_textures.size(); i++)
	{
		TexMap &tex = m_textures[i];
		if(tex.uIDBmp == 0)
		{
			if(LoadNewTexture(tex.texture, hDC, uIDBmp, nWidth, nHeight))
			{
				LOG("LoadTexture(0x%08X) => %u",
					uIDBmp, tex.texture);
				tex.uIDBmp = uIDBmp;
				return tex.texture;
			}

			return 0;
		}
	}

	LOG("No more space to load texture for bitmap 0x%08X",
		uIDBmp);
	return 0;
}

BOOL CGLEffect::LoadNewTexture(GLuint texture,
							   HDC hDC, UINT uIDBmp,
							   int nWidth, int nHeight)
{
	const HBITMAP hBmp = LoadBmpFromSrc(hDC, uIDBmp, 0, 0,
		nWidth, nHeight, nWidth, nHeight);
	if(hBmp == NULL)
	{
		LOG("Failed to LoadBmpFromSrc(%u, %d, %d)",
			uIDBmp, nWidth, nHeight);
		return FALSE;
	}

	// setup 24 bits bitmap structure works on 8 bits bitmaps also!
	BITMAPINFO info;
	memset(&info, 0, sizeof(info));
	BITMAPINFOHEADER &header = info.bmiHeader;
	header.biSize = sizeof(BITMAPINFOHEADER);
	header.biWidth = nWidth;
	header.biHeight = nHeight;
	header.biPlanes = 1;
	header.biBitCount = 24;
	header.biCompression = BI_RGB;
	header.biSizeImage = 0;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	// store bitmap data in a vector
	const int size = nWidth*nHeight*3;
	unsigned char *data = new unsigned char[size];
	if(data == NULL)
	{
		LOG("No memory: %d bytes needed", size);
		DeleteObject(hBmp);
		return FALSE;
	}
	const int lines = GetDIBits(hDC, hBmp, 0, nHeight,
		data, &info, DIB_RGB_COLORS);
	if(lines != nHeight)
	{
		LOG("Failed to GetDIBits, error = %u", GetLastError());
		delete[] data;
		DeleteObject(hBmp);
		return FALSE;
	}

	for(int i=0; i<nWidth*nHeight; i++)
	{
		const unsigned char tmp = data[i*3];
		data[i*3] = data[i*3+2];
		data[i*3+2] = tmp;
	}

	GLGuarder guarder(hDC);

	GLenum rc = 0;
	glBindTexture(GL_TEXTURE_2D, texture); rc = (rc!=0 ? rc : glGetError());
	glTexImage2D(GL_TEXTURE_2D, 0, 3, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data); rc = (rc!=0 ? rc : glGetError());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); rc = (rc!=0 ? rc : glGetError());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); rc = (rc!=0 ? rc : glGetError());

	delete[] data;
	DeleteObject(hBmp);

	if(rc != 0)
	{
		LOG("Failed to set texture parameters, error = 0x%08X", rc);
		glDeleteTextures(1, &texture);
		return FALSE;
	}

	return TRUE;
}
