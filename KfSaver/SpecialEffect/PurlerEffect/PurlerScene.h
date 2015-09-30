
#ifndef _PURLER_SCENE_H__INCLUDED__2007_08_25_
#define _PURLER_SCENE_H__INCLUDED__2007_08_25_

#include "PurlerObject.h"

namespace NPurler {

class CPurlerScene
{
public:
	CPurlerScene() {}

public:
	BOOL Initialize(const RECT rect);
	BOOL Act();
	BOOL Draw(HDC hDC) const;
	BOOL IsEnd() const;

	DWORD GetTimeslice() const;

protected:
	BOOL InitNails(int nWidth, int nHeight);
	MEASURE_T ActMoving(MEASURE_T total);

private:
	RECT m_rect;

	CPurlerBar m_bar;

	typedef vector<CPurlerNail>	NailList;
	NailList m_nails;

	CPurlerBoundary m_boundary;
};

} // namespace NPurler

#endif // _PURLER_SCENE_H__INCLUDED__2007_08_25_
