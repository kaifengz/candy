
#ifndef _3D_MAZE_EFFECT_H__INCLUDED__2005_05_09_
#define _3D_MAZE_EFFECT_H__INCLUDED__2005_05_09_

#include "GLEffect/GLEffect.h"

namespace N3dMaze
{
	class CMaze;
	class CMazeAI;
}

///////////////////////////////////////////////////////////////////////////////
//	C3dMazeEffect
///////////////////////////////////////////////////////////////////////////////

class C3dMazeEffect : public CGLEffect  
{
public:
	C3dMazeEffect(HWND hWnd, HBITMAP hBmpBkgnd);
	virtual ~C3dMazeEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC, BOOL);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
    N3dMaze::CMaze		*m_pMaze;
	N3dMaze::CMazeAI	*m_pAI;

	enum Z3dMazeTexture
	{
		MT_WALL = 0,
		MT_CROSS_POINT,

		MT_MAX
	};
	GLuint				m_tex[MT_MAX];

private:
	void	Draw3dMaze(HDC);

	BOOL	InitScene(HDC);
	void	InitLists();
	void	DrawWall(GLdouble xLeft, GLdouble yBottom, GLdouble zNear,
					 GLdouble xRight, GLdouble yTop, GLdouble zFar,
					 BOOL bHorizontal);
	void	DrawCrossPoint(GLdouble xLeft, GLdouble yBottom, GLdouble zNear,
						   GLdouble xRight, GLdouble yTop, GLdouble zFar);

	void	GetTargetPos(GLfloat &x, GLfloat &y, GLfloat &z);

	// draw mini maze
	void	DrawMiniMaze(HDC);
	void	DrawMazeGaps(HDC);
	void	DrawCurrentPos(HDC);
};

#endif // _3D_MAZE_EFFECT_H__INCLUDED__2005_05_09_
