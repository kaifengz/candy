
#include "StdAfx.h"
#include "FPS.h"

FramePerSecond::FramePerSecond()
{
}

FramePerSecond::~FramePerSecond()
{
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void FramePerSecond::IncreaseCount()
{
	m_times.push(clock());
}

unsigned int FramePerSecond::GetFPS()
{
	Purge();
	return (unsigned int)(m_times.size());
}

void FramePerSecond::Purge()
{
	const clock_t now = clock();
	while(!m_times.empty() && now - m_times.front() >= CLOCKS_PER_SEC)
		m_times.pop();
}
