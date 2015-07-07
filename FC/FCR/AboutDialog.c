
#include <windows.h>
#include "resource.h"

BOOL CALLBACK AboutDialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		return TRUE;
	}

	return FALSE;
}
