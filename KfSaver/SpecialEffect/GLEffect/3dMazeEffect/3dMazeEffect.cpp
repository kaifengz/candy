// 3dMazeEffect.cpp


// History:
// 初步完成										(2005-05-15)
// 添加光照，不成功，郁闷						(2005-05-20)


#include "stdafx.h"
#include "3dMazeEffect.h"
#include "3dMaze.h"
#include "3dMazeAI.h"
#include "EffectRegister.h"

REGISTER_EFFECT(C3dMazeEffect);


#define MAZE_TIMESLICE					30

// setting for 3d maze
#define MAZE_GL_BG_COLOR				(GLclampf)(165.0/255.0),\
										(GLclampf)(198.0/255.0),\
										(GLclampf)(239.0/255.0),\
										(GLclampf)(0.0)
#define MAZE_GL_FLOOR_COLOR				(GLclampf)(90.0/255.0),\
										(GLclampf)(135.0/255.0),\
										(GLclampf)(90.0/255.0),\
										(GLclampf)(0.0)
#define MAZE_GL_GRID_COLOR				(GLclampf)(64.0/255.0),\
										(GLclampf)(96.0/255.0),\
										(GLclampf)(64.0/255.0),\
										(GLclampf)(0.0)
#define MAZE_GL_WALL_COLOR				(GLclampf)(64.0/255.0),\
										(GLclampf)(112.0/255.0),\
										(GLclampf)(64.0/255.0),\
										(GLclampf)(0.0)
#define MAZE_GL_WALL_TOP_COLOR			(GLclampf)(64.0/255.0),\
										(GLclampf)(96.0/255.0),\
										(GLclampf)(64.0/255.0),\
										(GLclampf)(0.0)
#define MAZE_GL_TARGET_COLOR			(GLclampf)(255.0/255.0),\
										(GLclampf)(0.0/255.0),\
										(GLclampf)(0.0/255.0),\
										(GLclampf)(0.0)
#define MAZE_GL_CROSS_POINT_COLOR		(GLclampf)(64.0/255.0),\
										(GLclampf)(96.0/255.0),\
										(GLclampf)(64.0/255.0),\
										(GLclampf)(0.0)

#define MAZE_GL_WALL_HEIGHT				1.0
#define MAZE_GL_WALL_THICKNESS			0.1
#define MAZE_GL_GRID_SIZE				2.5
#define MAZE_GL_PERSION_HEIGHT			1.73

#define MAZE_GL_TARGET_SIZE				0.4
#define MAZE_GL_TARGET_BOTTOM			0.5
#define MAZE_GL_TARGET_TOP				1.5

#define MAZE_GL_LIST_FLOOR				1
#define MAZE_GL_LIST_GRIDS				2
#define MAZE_GL_LIST_WALLS				3
#define MAZE_GL_LIST_TARGET				4

#define EYE_BACK						0.3


// setting for 2d mini maze
#define MAZE_GRID_SIZE					16
#define MAZE_ORIGINAL_POS_SIZE			8
#define MAZE_TARGET_POS_SIZE			8
#define MAZE_GRID_COLOR					RGB(0, 0, 0)
#define MAZE_ORIGINAL_POS_COLOR			RGB(128, 128, 0)
#define MAZE_TARGET_POS_COLOR			RGB(128, 0, 128)
#define MAZE_CUR_POS_ARROW_COLOR		RGB(128, 0, 0)
#define MAZE_CUR_POS_ARROW_LENGTH		12
#define MAZE_CUR_POS_ARROW_ANGLE		60
#define MAZE_CUR_POS_ARROW_WING_LEN		8


// macros
#define PI 3.14159265358979323846
#define ANGLE_TO_ARC(angle)		((angle)*PI/180.0)
#define ROUND_TO_INT(d)			((d)>=0.0 ? (int)(0.5+(d)) : (int)(-0.5+(d)))


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C3dMazeEffect::C3dMazeEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CGLEffect(hWnd, hBmpBkgnd)
{
	m_pMaze = new N3dMaze::CMaze();
	ASSERT(m_pMaze != NULL);
	m_pAI = new N3dMaze::CMazeAI();
	ASSERT(m_pAI != NULL);
}

C3dMazeEffect::~C3dMazeEffect()
{
}

const char* C3dMazeEffect::GetEffectName() const
{
	return "3dMazeEffect";
}

///////////////////////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////////////////////

BOOL C3dMazeEffect::Initialize(HDC hDC, BOOL)
{
	if(! m_pMaze->InitRandomMaze())
		return FALSE;
	if(! m_pAI->InitAI(m_pMaze))
		return FALSE;
	if(!InitScene(hDC))
		return FALSE;
	return TRUE;
}

BOOL C3dMazeEffect::OnTimer(HDC hDC)
{
	BOOL bEndMazeShow = FALSE;

    if(GetFrameCount() != 0)
	{
		bEndMazeShow = !(m_pAI->WalkAhead());
	}

	Draw3dMaze(hDC);
// 	DrawMiniMaze(hDC);

	return !bEndMazeShow;
}

DWORD C3dMazeEffect::GetTimeslice() const
{
	return MAZE_TIMESLICE;
}

///////////////////////////////////////////////////////////////////////////////
//	Draw mini maze
///////////////////////////////////////////////////////////////////////////////

void C3dMazeEffect::DrawMiniMaze(HDC hDC)
{
	DrawMazeGaps(hDC);
	DrawCurrentPos(hDC);
}

void C3dMazeEffect::DrawMazeGaps(HDC hDC)
{
	int i, j;

	HPEN hPen = CreatePen(PS_SOLID, 1, MAZE_GRID_COLOR);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	const int xOffset = GetClientArea().right - m_pMaze->GetMazeSize()*MAZE_GRID_SIZE -1;
	const int yOffset = GetClientArea().top;

	// horizontal gaps
	for(i=0; i<m_pMaze->GetMazeSize()+1; i++)
	{
		for(j=0; j<m_pMaze->GetMazeSize(); j++)
		{
			if(m_pMaze->IsGap(j, i, TRUE))
			{
				MoveToEx(hDC, xOffset+j*MAZE_GRID_SIZE, yOffset+i*MAZE_GRID_SIZE, NULL);
				LineTo(hDC, xOffset+(j+1)*MAZE_GRID_SIZE, yOffset+i*MAZE_GRID_SIZE);
			}
		}
	}

	// vertical gaps
	for(i=0; i<m_pMaze->GetMazeSize(); i++)
	{
		for(j=0; j<m_pMaze->GetMazeSize()+1; j++)
		{
			if(m_pMaze->IsGap(j, i, FALSE))
			{
				MoveToEx(hDC, xOffset+j*MAZE_GRID_SIZE, yOffset+i*MAZE_GRID_SIZE, NULL);
				LineTo(hDC, xOffset+j*MAZE_GRID_SIZE, yOffset+(i+1)*MAZE_GRID_SIZE);
			}
		}
	}

	{	// original pos
		HBRUSH hBrush = CreateSolidBrush(MAZE_ORIGINAL_POS_COLOR);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		SelectObject(hDC, GetStockObject(NULL_PEN));

		int x, y;
		m_pMaze->GetOriginalPos(x, y);

		Ellipse(hDC,
			xOffset + MAZE_GRID_SIZE/2 + x*MAZE_GRID_SIZE - MAZE_ORIGINAL_POS_SIZE/2,
			yOffset + MAZE_GRID_SIZE/2 + y*MAZE_GRID_SIZE - MAZE_ORIGINAL_POS_SIZE/2,
			xOffset + MAZE_GRID_SIZE/2 + x*MAZE_GRID_SIZE + MAZE_ORIGINAL_POS_SIZE/2,
			yOffset + MAZE_GRID_SIZE/2 + y*MAZE_GRID_SIZE + MAZE_ORIGINAL_POS_SIZE/2);

		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
	}

	{	// target pos
		HBRUSH hBrush = CreateSolidBrush(MAZE_TARGET_POS_COLOR);
		HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
		SelectObject(hDC, GetStockObject(NULL_PEN));

		int x, y;
		m_pMaze->GetTargetPos(x, y);

		Ellipse(hDC,
			xOffset + MAZE_GRID_SIZE/2 + x*MAZE_GRID_SIZE - MAZE_TARGET_POS_SIZE/2,
			yOffset + MAZE_GRID_SIZE/2 + y*MAZE_GRID_SIZE - MAZE_TARGET_POS_SIZE/2,
			xOffset + MAZE_GRID_SIZE/2 + x*MAZE_GRID_SIZE + MAZE_TARGET_POS_SIZE/2,
			yOffset + MAZE_GRID_SIZE/2 + y*MAZE_GRID_SIZE + MAZE_TARGET_POS_SIZE/2);

		SelectObject(hDC, hOldBrush);
		DeleteObject(hBrush);
	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

void C3dMazeEffect::DrawCurrentPos(HDC hDC)
{
	double x, y;
	int dir;

	if(m_pAI->GetCurrentPos(x, y, dir))
	{
		const int xOffset = GetClientArea().right - m_pMaze->GetMazeSize()*MAZE_GRID_SIZE -1;
		const int yOffset = GetClientArea().top;

		HPEN hPen = CreatePen(PS_SOLID, 2, MAZE_CUR_POS_ARROW_COLOR);
		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

		// arrow trunk
		const int xHead = ROUND_TO_INT( MAZE_GRID_SIZE*x +
			MAZE_CUR_POS_ARROW_LENGTH/2.0*cos(ANGLE_TO_ARC(dir)) );
		const int yHead = ROUND_TO_INT( MAZE_GRID_SIZE*y +
			MAZE_CUR_POS_ARROW_LENGTH/2.0*sin(ANGLE_TO_ARC(dir)) );
		const int xTail = ROUND_TO_INT( MAZE_GRID_SIZE*x -
			MAZE_CUR_POS_ARROW_LENGTH/2.0*cos(ANGLE_TO_ARC(dir)) );
		const int yTail = ROUND_TO_INT( MAZE_GRID_SIZE*y -
			MAZE_CUR_POS_ARROW_LENGTH/2.0*sin(ANGLE_TO_ARC(dir)) );

		MoveToEx(hDC, xOffset+xHead, yOffset+yHead, NULL);
		LineTo(hDC, xOffset+xTail, yOffset+yTail);

		{	// left wing
			const double wing_angle_1 =
				ANGLE_TO_ARC( (-dir+360+MAZE_CUR_POS_ARROW_ANGLE/2)%360 );
			const int x1 = xHead -
				ROUND_TO_INT( MAZE_CUR_POS_ARROW_WING_LEN*cos(wing_angle_1) );
			const int y1 = yHead +
				ROUND_TO_INT( MAZE_CUR_POS_ARROW_WING_LEN*sin(wing_angle_1) );

			MoveToEx(hDC, xOffset+xHead, yOffset+yHead, NULL);
			LineTo(hDC, xOffset+x1, yOffset+y1);
		}

		{	// right wing
			const double wing_angle_2 =
				ANGLE_TO_ARC( (-dir+360-MAZE_CUR_POS_ARROW_ANGLE/2+360)%360 );
			const int x2 = xHead -
				ROUND_TO_INT( MAZE_CUR_POS_ARROW_WING_LEN*cos(wing_angle_2) );
			const int y2 = yHead +
				ROUND_TO_INT( MAZE_CUR_POS_ARROW_WING_LEN*sin(wing_angle_2) );

			MoveToEx(hDC, xOffset+xHead, yOffset+yHead, NULL);
			LineTo(hDC, xOffset+x2, yOffset+y2);
		}

		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Draw 3d Maze
///////////////////////////////////////////////////////////////////////////////

void C3dMazeEffect::Draw3dMaze(HDC hDC)
{
	double xCur, yCur;
	int dir;

	if(!m_pAI->GetCurrentPos(xCur, yCur, dir))
		return;

	GLGuarder guarder(hDC);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	{	// look at
		const GLdouble xEye = xCur - EYE_BACK*cos(ANGLE_TO_ARC(dir));
		const GLdouble yEye = yCur - EYE_BACK*sin(ANGLE_TO_ARC(dir));
		const GLdouble xCenter = xCur + 1*cos(ANGLE_TO_ARC(dir));
		const GLdouble yCenter = yCur + 1*sin(ANGLE_TO_ARC(dir));

		glLoadIdentity();
		gluLookAt(
			xEye*MAZE_GL_GRID_SIZE,			// x of eye
			MAZE_GL_PERSION_HEIGHT,			// y of eye
			yEye*MAZE_GL_GRID_SIZE,			// z of eye
			xCenter * MAZE_GL_GRID_SIZE,	// x of center
			MAZE_GL_PERSION_HEIGHT,			// y of center
			yCenter * MAZE_GL_GRID_SIZE,	// z of center
			0.0, 1.0, 0.0);
	}

	// floor
	glCallList(MAZE_GL_LIST_FLOOR);
	// grids in floor
	glCallList(MAZE_GL_LIST_GRIDS);
	// walls
	glCallList(MAZE_GL_LIST_WALLS);
	// target
	glCallList(MAZE_GL_LIST_TARGET);

	glFlush();

	//Swap Buffers
	SwapBuffers(hDC);
}

BOOL C3dMazeEffect::InitScene(HDC hDC)
{
	GLGuarder guarder(hDC);

	{	// init
		glClearColor(MAZE_GL_BG_COLOR);
		glShadeModel(GL_SMOOTH);

		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	{	// reshape
		const RECT &client = GetClientArea();
		const RECT &wnd = GetWindowArea();
		glViewport(client.left, wnd.bottom - client.bottom,
			client.right - client.left, client.bottom - client.top);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-1.0, 1.0, -1.0,
			1.0, 1.0, m_pMaze->GetMazeSize()*MAZE_GL_GRID_SIZE*2);
		glMatrixMode(GL_MODELVIEW);
	}

	{	// texture
		m_tex[MT_WALL] = LoadTexture(hDC, IDBMP_3DM_WALL, 128, 128);
		m_tex[MT_CROSS_POINT] = LoadTexture(hDC, IDBMP_3DM_CROSS_POINT, 128, 128);
		glEnable(GL_TEXTURE_2D);
	}

	// init lists
	InitLists();
	return TRUE;
}

void C3dMazeEffect::InitLists()
{
	{	// floor
		const GLdouble dMazeSize = m_pMaze->GetMazeSize()*MAZE_GL_GRID_SIZE;
		const GLfloat fMSz = (GLfloat)dMazeSize;
		glNewList(MAZE_GL_LIST_FLOOR, GL_COMPILE);
		glColor4f(MAZE_GL_FLOOR_COLOR);
		glBegin(GL_QUADS);
			glVertex3d(        0, 0,         0);
			glVertex3d(        0, 0, dMazeSize);
			glVertex3d(dMazeSize, 0, dMazeSize);
			glVertex3d(dMazeSize, 0,         0);
		glEnd();
		glEndList();
	}

	{	// Grids in the floor
		const GLdouble dMazeSize = m_pMaze->GetMazeSize()*MAZE_GL_GRID_SIZE;
		glNewList(MAZE_GL_LIST_GRIDS, GL_COMPILE);
		glBegin(GL_QUADS);
		glColor4f(MAZE_GL_GRID_COLOR);
		for(int i=0; i<=m_pMaze->GetMazeSize(); i++)
		{
			GLdouble d1 = i*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2.0;
			GLdouble d2 = i*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2.0;

			// vertical
			glVertex3d(d1, 0.01,         0);
			glVertex3d(d2, 0.01,         0);
			glVertex3d(d2, 0.01, dMazeSize);
			glVertex3d(d1, 0.01, dMazeSize);

			// honrizontal
			glVertex3d(        0, 0.01, d1);
			glVertex3d(dMazeSize, 0.01, d1);
			glVertex3d(dMazeSize, 0.01, d2);
			glVertex3d(        0, 0.01, d2);
		}
		glEnd();
		glEndList();
	}

	{	// walls
		int i, j;
		glNewList(MAZE_GL_LIST_WALLS, GL_COMPILE);

		// horizontal gaps
		for(i=0; i<m_pMaze->GetMazeSize()+1; i++)
		{
			for(j=0; j<m_pMaze->GetMazeSize(); j++)
			{
				if(m_pMaze->IsGap(j, i, TRUE))
				{
					DrawWall(
						j*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2,
						0.0,
						i*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2,
						(j+1)*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2,
						MAZE_GL_WALL_HEIGHT,
						i*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2,
						TRUE);
				}
			}
		}

		// vertical gaps
		for(i=0; i<m_pMaze->GetMazeSize(); i++)
		{
			for(j=0; j<m_pMaze->GetMazeSize()+1; j++)
			{
				if(m_pMaze->IsGap(j, i, FALSE))
				{
					DrawWall(
						j*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2,
						0.0,
						i*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2,
						j*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2,
						MAZE_GL_WALL_HEIGHT,
						(i+1)*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2,
						FALSE);
				}
			}
		}

		// cross points
		for(i=0; i<m_pMaze->GetMazeSize()+1; i++)
		{
			for(j=0; j<m_pMaze->GetMazeSize()+1; j++)
			{
				DrawCrossPoint(
					j*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2,
					0.0,
					i*MAZE_GL_GRID_SIZE - MAZE_GL_WALL_THICKNESS/2,
					j*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2,
					MAZE_GL_WALL_HEIGHT,
					i*MAZE_GL_GRID_SIZE + MAZE_GL_WALL_THICKNESS/2);
			}
		}

		glEndList();
	}

	{	// target point
		int xTgt, yTgt;
		m_pMaze->GetTargetPos(xTgt, yTgt);

		GLdouble xLeft   = (xTgt + 0.5) * MAZE_GL_GRID_SIZE - MAZE_GL_TARGET_SIZE/2.0;
		GLdouble xRight  = (xTgt + 0.5) * MAZE_GL_GRID_SIZE + MAZE_GL_TARGET_SIZE/2.0;
		GLdouble yBottom = MAZE_GL_TARGET_BOTTOM;
		GLdouble yTop    = MAZE_GL_TARGET_TOP;
		GLdouble zNear   = (yTgt + 0.5) * MAZE_GL_GRID_SIZE - MAZE_GL_TARGET_SIZE/2.0;
		GLdouble zFar    = (yTgt + 0.5) * MAZE_GL_GRID_SIZE + MAZE_GL_TARGET_SIZE/2.0;

		glNewList(MAZE_GL_LIST_TARGET, GL_COMPILE);
		glBegin(GL_QUADS);
			glColor4f(MAZE_GL_TARGET_COLOR);

			glVertex3d(xLeft,  yBottom, zNear);
			glVertex3d(xRight, yBottom, zNear);
			glVertex3d(xRight, yTop,    zNear);
			glVertex3d(xLeft,  yTop,    zNear);

			glVertex3d(xLeft,  yTop,    zFar);
			glVertex3d(xRight, yTop,    zFar);
			glVertex3d(xRight, yBottom, zFar);
			glVertex3d(xLeft,  yBottom, zFar);

			glVertex3d(xLeft,  yTop, zFar);
			glVertex3d(xRight, yTop, zFar);
			glVertex3d(xRight, yTop, zNear);
			glVertex3d(xLeft,  yTop, zNear);

			glVertex3d(xLeft,  yBottom, zNear);
			glVertex3d(xRight, yBottom, zNear);
			glVertex3d(xRight, yBottom, zFar);
			glVertex3d(xLeft,  yBottom, zFar);

			glVertex3d(xLeft, yBottom, zNear);
			glVertex3d(xLeft, yTop,    zNear);
			glVertex3d(xLeft, yTop,    zFar);
			glVertex3d(xLeft, yBottom, zFar);

			glVertex3d(xRight, yBottom, zFar);
			glVertex3d(xRight, yTop,    zFar);
			glVertex3d(xRight, yTop,    zNear);
			glVertex3d(xRight, yBottom, zNear);
		glEnd();
		glEndList();
	}
}

void C3dMazeEffect::DrawWall(GLdouble xLeft, GLdouble yBottom, GLdouble zNear,
							 GLdouble xRight, GLdouble yTop, GLdouble zFar,
							 BOOL bHorizontal)
{
 	glBindTexture(GL_TEXTURE_2D, m_tex[MT_WALL]);

	glBegin(GL_QUADS);

	if(bHorizontal)
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3d(xRight, yBottom, zNear);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(xRight, yTop,    zNear);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(xLeft,  yTop,    zNear);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(xLeft,  yBottom, zNear);

		glTexCoord2f(0.0f, 0.0f); glVertex3d(xLeft,  yBottom, zFar);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(xLeft,  yTop,    zFar);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(xRight, yTop,    zFar);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(xRight, yBottom, zFar);
	}
	else
	{
		glTexCoord2f(0.0f, 0.0f); glVertex3d(xLeft, yBottom, zNear);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(xLeft, yTop,    zNear);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(xLeft, yTop,    zFar);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(xLeft, yBottom, zFar);

		glTexCoord2f(0.0f, 0.0f); glVertex3d(xRight, yBottom, zFar);
		glTexCoord2f(0.0f, 1.0f); glVertex3d(xRight, yTop,    zFar);
		glTexCoord2f(1.0f, 1.0f); glVertex3d(xRight, yTop,    zNear);
		glTexCoord2f(1.0f, 0.0f); glVertex3d(xRight, yBottom, zNear);
	}

	glColor4f(MAZE_GL_WALL_TOP_COLOR);

	glVertex3d(xLeft,  yTop, zNear);
	glVertex3d(xRight, yTop, zNear);
	glVertex3d(xRight, yTop, zFar);
	glVertex3d(xLeft,  yTop, zFar);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void C3dMazeEffect::DrawCrossPoint(GLdouble xLeft, GLdouble yBottom, GLdouble zNear,
								   GLdouble xRight, GLdouble yTop, GLdouble zFar)
{
 	glBindTexture(GL_TEXTURE_2D, m_tex[MT_CROSS_POINT]);

	glBegin(GL_QUADS);

	const GLfloat y_x_ratio = 1.0f; // (yTop-yBottom)/(xRight-xLeft);
	const GLfloat y_z_ratio = 1.0f; // (yTop-yBottom)/(zFar-zNear);

	glTexCoord2f(0.0f, 0.0f);		glVertex3d(xRight, yBottom, zNear);
	glTexCoord2f(0.0f, y_x_ratio);	glVertex3d(xRight, yTop,    zNear);
	glTexCoord2f(1.0f, y_x_ratio);	glVertex3d(xLeft,  yTop,    zNear);
	glTexCoord2f(1.0f, 0.0f);		glVertex3d(xLeft,  yBottom, zNear);

	glTexCoord2f(0.0f, 0.0f);		glVertex3d(xLeft,  yBottom, zFar);
	glTexCoord2f(0.0f, y_x_ratio);	glVertex3d(xLeft,  yTop,    zFar);
	glTexCoord2f(1.0f, y_x_ratio);	glVertex3d(xRight, yTop,    zFar);
	glTexCoord2f(1.0f, 0.0f);		glVertex3d(xRight, yBottom, zFar);

	glTexCoord2f(0.0f, 0.0f);		glVertex3d(xLeft, yBottom, zNear);
	glTexCoord2f(0.0f, y_z_ratio);	glVertex3d(xLeft, yTop,    zNear);
	glTexCoord2f(1.0f, y_z_ratio);	glVertex3d(xLeft, yTop,    zFar);
	glTexCoord2f(1.0f, 0.0f);		glVertex3d(xLeft, yBottom, zFar);

	glTexCoord2f(0.0f, 0.0f);		glVertex3d(xRight, yBottom, zFar);
	glTexCoord2f(0.0f, y_z_ratio);	glVertex3d(xRight, yTop,    zFar);
	glTexCoord2f(1.0f, y_z_ratio);	glVertex3d(xRight, yTop,    zNear);
	glTexCoord2f(1.0f, 0.0f);		glVertex3d(xRight, yBottom, zNear);

	glColor4f(MAZE_GL_CROSS_POINT_COLOR);

	glVertex3d(xLeft,  yTop, zNear);
	glVertex3d(xRight, yTop, zNear);
	glVertex3d(xRight, yTop, zFar);
	glVertex3d(xLeft,  yTop, zFar);

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void C3dMazeEffect::GetTargetPos(GLfloat &x, GLfloat &y, GLfloat &z)
{
	int xTgt, yTgt;
	m_pMaze->GetTargetPos(xTgt, yTgt);

	x = (GLfloat)((xTgt + 0.5) * MAZE_GL_GRID_SIZE);
	y = (GLfloat)((MAZE_GL_TARGET_BOTTOM + MAZE_GL_TARGET_TOP)/2.0);
	z = (GLfloat)((yTgt + 0.5) * MAZE_GL_GRID_SIZE);
}
