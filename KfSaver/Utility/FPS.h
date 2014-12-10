
#ifndef _FPS_H__INCLUDED__2007_06_25_
#define _FPS_H__INCLUDED__2007_06_25_

#include <queue>
#include <time.h>

class FramePerSecond
{
public:
	FramePerSecond();
	~FramePerSecond();

	void IncreaseCount();
	unsigned int GetFPS();

protected:
	void Purge();

protected:
	typedef std::queue<clock_t> times;
	times m_times;
};

#endif // _FPS_H__INCLUDED__2007_06_25_
