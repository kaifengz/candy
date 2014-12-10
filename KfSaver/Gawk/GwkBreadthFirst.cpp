// GwkBreadthFirst.cpp: implementation of the CGwkBreadthFirst class.
//

#include "GwkBreadthFirst.h"
#include "GwkBreadthFirstImpl.h"
#include "util.h"

CGwkBreadthFirst::CGwkBreadthFirst() :
	CGwkBase(new CGwkBreadthFirstImpl())
{
}
