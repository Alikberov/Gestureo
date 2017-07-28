// Gestureo.cpp : Defines the entry point for the application.
#include"stdafx.h"
#include"resource.h"
#include<math.h>
#include"user32.h"

#define		CreateListBox(hwnd, left, top, width, height, id, style)\
	CreateWindowEx\
		(WS_EX_CLIENTEDGE\
		,TEXT("ListBox")\
		,NULL\
		,WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP | style\
		,left, top, width, height\
		,hwnd\
		,HMENU(id)\
		,hInst, NULL)
#define	UpdateSBar(hWnds, text, part, flags) SendMessage(hWnds, SB_SETTEXT, part | flags, (LPARAM)text)

#define STRING_DIMENSION	4096

#define	MASK_L1		(1 << 0)	/* Left button		of Mouse #1 */
#define	MASK_M1		(1 << 1)	/* Middle button	of Mouse #1 */
#define	MASK_R1		(1 << 2)	/* Right button		of Mouse #1 */
#define	MASK_S1		(1 << 3)	/* Southern drag	of Mouse #1 */
#define	MASK_N1		(1 << 4)	/* Northern drag	of Mouse #1 */
#define	MASK_E1		(1 << 5)	/* Eastern drag		of Mouse #1 */
#define	MASK_W1		(1 << 6)	/* Western drag		of Mouse #1 */
#define	MASK_H1		(1 << 7)	/* Horizontal wheel	of Mouse #1 */
#define	MASK_X1		(1 << 8)	/* Wheel to right	of Mouse #1 */
#define	MASK_V1		(1 << 9)	/* Vertical wheel	of Mouse #1 */
#define	MASK_Y1		(1 << 10)	/* Wheel to down	of Mouse #1 */
#define	MASK_A1		(1 << 11)	/* Optional button	of Mouse #1 */
#define	MASK_B1		(1 << 12)	/* Optional button	of Mouse #1 */
#define	MASK_L2		(1 << 13)	/* Left button		of Mouse #2 */
#define	MASK_M2		(1 << 14)	/* Middle button	of Mouse #2 */
#define	MASK_R2		(1 << 15)	/* Right button		of Mouse #2 */
#define	MASK_S2		(1 << 16)	/* Southern drag	of Mouse #2 */
#define	MASK_N2		(1 << 17)	/* Northern drag	of Mouse #2 */
#define	MASK_E2		(1 << 18)	/* Eastern drag		of Mouse #2 */
#define	MASK_W2		(1 << 19)	/* Western drag		of Mouse #2 */
#define	MASK_H2		(1 << 20)	/* Horizontal wheel	of Mouse #2 */
#define	MASK_X2		(1 << 21)	/* Wheel to right	of Mouse #2 */
#define	MASK_V2		(1 << 22)	/* Vertical wheel	of Mouse #2 */
#define	MASK_Y2		(1 << 23)	/* Wheel to down	of Mouse #2 */
#define	MASK_A2		(1 << 24)	/* Optional button	of Mouse #2 */
#define	MASK_B2		(1 << 25)	/* Optional button	of Mouse #2 */
#define	MASK_0		(1 << 26)
#define	MASK_1		(1 << 27)
#define	MASK_2		(1 << 28)
#define	MASK_3		(1 << 29)
#define	MASK_4		(1 << 30)
#define	MASK_5		(1 << 31)

#define	XBUTTON1	1
#define	XBUTTON2	2

RAWINPUTDEVICE	tRIDev[256];

// Structures types

RAWINPUTDEVICELIST	tRIDList[256];

struct {
	HANDLE		handle;
	DWORD		index;
} aRIDevices[256] = {0};

HANDLE		hRaw;
BYTE		cRawData[1024];
UINT		cbRawSize;
UINT		nRawDevs;
int			nMouses = 0;

typedef struct {
	DWORD	mask;
	UINT	keycode;
} JOYSTICK_ALIAS, *PJOYSTICK_ALIAS;
typedef struct {
	TCHAR	name[12];
	DWORD	mask;
	struct {
		char	x;
		char	y;
	} position;
	struct {
		char	cx;
		char	cy;
	} size;
} FINGER_PROJECTION, *PFINGER_PROJECTION;

// Global Variables:
HINSTANCE	hInst;								// current instance
TCHAR		szTitle[STRING_DIMENSION];			// The title bar text
TCHAR		szWindowClass[STRING_DIMENSION];		// The title bar text

MENUITEMINFO	mii;
HMENU		hMenu;
HWND		hWndSBar;
HWND		hVKList;
HWND        hListView;
INT			hTimer;
       LVCOLUMN lvc = { 0 };
        LVITEM   lv  = { 0 };
HHOOK		hMouseHook;
BOOL		isGameProcess	= FALSE;
BOOL		isDescription	= FALSE;

POINT		ptCursor;
POINT		ptRange;							// Range of sensetivity
POINT		ptHold;								// Range of inertion
POINT		ptVector[2];						// Vectors for moving keys

DWORD		dwRemote = 0;
DWORD		dwRemoted = 0;
DWORD		dwRemoters = 0;

TCHAR		szNote[STRING_DIMENSION];
TCHAR		tszVKId[256][32];
TCHAR		szProfile[STRING_DIMENSION];
TCHAR		szDescription[STRING_DIMENSION];
TCHAR		szRename[32] = {0};
char		idxVK = -1, idxVKalias;
int			fPress = NULL;
int			iPreset = 0;
DWORD		idxMouse = NULL;
BOOL		isaMouse[64] = {0};
//							_Q_		_1_		_2_		_3_		_F_		_B_		_N_		_W_		_E_		_S_		_P_
TCHAR		tcRemote[16] = {0x0,	'A',	' ',	'D',	'W',	'S',	'J',	' ',	'K',	'L',	0xD};
PTCHAR	static const	pszModes[] = {"?","1","2","M","R"};
//
typedef struct {
	short	vk;
	char	*id;
} VIRTUAL_KEY;

VIRTUAL_KEY tszVK[] = {
	VK_LBUTTON				,"VK_LBUTTON",
	VK_RBUTTON				,"VK_RBUTTON",
	VK_CANCEL				,"VK_CANCEL",
	VK_MBUTTON				,"VK_MBUTTON",
	VK_BACK 				,"VK_BACK",
	VK_TAB					,"VK_TAB",
	VK_CLEAR				,"VK_CLEAR",
	VK_RETURN				,"VK_RETURN",
	VK_SHIFT				,"VK_SHIFT",
	VK_CONTROL				,"VK_CONTROL",
	VK_MENU 				,"VK_MENU",
	VK_PAUSE				,"VK_PAUSE",
	VK_CAPITAL				,"VK_CAPITAL",
	VK_KANA 				,"VK_KANA",
	VK_HANGEUL				,"VK_HANGEUL",
	VK_HANGUL				,"VK_HANGUL",
	VK_JUNJA				,"VK_JUNJA",
	VK_FINAL				,"VK_FINAL",
	VK_HANJA				,"VK_HANJA",
	VK_KANJI				,"VK_KANJI",
	VK_ESCAPE				,"VK_ESCAPE",
	VK_CONVERT				,"VK_CONVERT",
	VK_NONCONVERT			,"VK_NONCONVERT",
	VK_ACCEPT				,"VK_ACCEPT",
	VK_MODECHANGE			,"VK_MODECHANGE",
	VK_SPACE				,"VK_SPACE",
	VK_PRIOR				,"VK_PRIOR",
	VK_NEXT 				,"VK_NEXT",
	VK_END					,"VK_END",
	VK_HOME 				,"VK_HOME",
	VK_LEFT 				,"VK_LEFT",
	VK_UP					,"VK_UP",
	VK_RIGHT				,"VK_RIGHT",
	VK_DOWN 				,"VK_DOWN",
	VK_SELECT				,"VK_SELECT",
	VK_PRINT				,"VK_PRINT",
	VK_EXECUTE				,"VK_EXECUTE",
	VK_SNAPSHOT 			,"VK_SNAPSHOT",
	VK_INSERT				,"VK_INSERT",
	VK_DELETE				,"VK_DELETE",
	VK_HELP 				,"VK_HELP",
	VK_LWIN 				,"VK_LWIN",
	VK_RWIN 				,"VK_RWIN",
	VK_APPS 				,"VK_APPS",
	VK_NUMPAD0				,"VK_NUMPAD0",
	VK_NUMPAD1				,"VK_NUMPAD1",
	VK_NUMPAD2				,"VK_NUMPAD2",
	VK_NUMPAD3				,"VK_NUMPAD3",
	VK_NUMPAD4				,"VK_NUMPAD4",
	VK_NUMPAD5				,"VK_NUMPAD5",
	VK_NUMPAD6				,"VK_NUMPAD6",
	VK_NUMPAD7				,"VK_NUMPAD7",
	VK_NUMPAD8				,"VK_NUMPAD8",
	VK_NUMPAD9				,"VK_NUMPAD9",
	VK_MULTIPLY 			,"VK_MULTIPLY",
	VK_ADD					,"VK_ADD",
	VK_SEPARATOR			,"VK_SEPARATOR",
	VK_SUBTRACT 			,"VK_SUBTRACT",
	VK_DECIMAL				,"VK_DECIMAL",
	VK_DIVIDE				,"VK_DIVIDE",
	VK_F1					,"VK_F1",
	VK_F2					,"VK_F2",
	VK_F3					,"VK_F3",
	VK_F4					,"VK_F4",
	VK_F5					,"VK_F5",
	VK_F6					,"VK_F6",
	VK_F7					,"VK_F7",
	VK_F8					,"VK_F8",
	VK_F9					,"VK_F9",
	VK_F10					,"VK_F10",
	VK_F11					,"VK_F11",
	VK_F12					,"VK_F12",
	VK_F13					,"VK_F13",
	VK_F14					,"VK_F14",
	VK_F15					,"VK_F15",
	VK_F16					,"VK_F16",
	VK_F17					,"VK_F17",
	VK_F18					,"VK_F18",
	VK_F19					,"VK_F19",
	VK_F20					,"VK_F20",
	VK_F21					,"VK_F21",
	VK_F22					,"VK_F22",
	VK_F23					,"VK_F23",
	VK_F24					,"VK_F24",
	VK_NUMLOCK				,"VK_NUMLOCK",
	VK_SCROLL				,"VK_SCROLL",
	VK_LSHIFT				,"VK_LSHIFT",
	VK_RSHIFT				,"VK_RSHIFT",
	VK_LCONTROL 			,"VK_LCONTROL",
	VK_RCONTROL 			,"VK_RCONTROL",
	VK_LMENU				,"VK_LMENU",
	VK_RMENU				,"VK_RMENU",
	VK_PROCESSKEY			,"VK_PROCESSKEY",
	VK_ATTN 				,"VK_ATTN",
	VK_CRSEL				,"VK_CRSEL",
	VK_EXSEL				,"VK_EXSEL",
	VK_EREOF				,"VK_EREOF",
	VK_PLAY 				,"VK_PLAY",
	VK_ZOOM 				,"VK_ZOOM",
	VK_NONAME				,"VK_NONAME",
	VK_PA1					,"VK_PA1",
	VK_OEM_CLEAR			,"VK_OEM_CLEAR",
	VK_SLEEP                ,"VK_SLEEP",
	VK_BROWSER_BACK         ,"VK_BROWSER_BACK",
	VK_BROWSER_FORWARD      ,"VK_BROWSER_FORWARD",
	VK_BROWSER_REFRESH      ,"VK_BROWSER_REFRESH",
	VK_BROWSER_STOP         ,"VK_BROWSER_STOP",
	VK_BROWSER_SEARCH       ,"VK_BROWSER_SEARCH",
	VK_BROWSER_FAVORITIES   ,"VK_BROWSER_FAVORITIES",
	VK_BROWSER_HOME         ,"VK_BROWSER_HOME",
	VK_VOLUME_MUTE          ,"VK_VOLUME_MUTE",
	VK_VOLUME_DOWN          ,"VK_VOLUME_DOWN",
	VK_VOLUME_UP            ,"VK_VOLUME_UP",
	VK_MEDIA_NEXT_TRACK     ,"VK_MEDIA_NEXT_TRACK",
	VK_MEDIA_PREV_TRACK     ,"VK_MEDIA_PREV_TRACK",
	VK_MEDIA_STOP           ,"VK_MEDIA_STOP",
	VK_MEDIA_PLAY_PAUSE     ,"VK_MEDIA_PLAY_PAUSE",
	VK_LAUNCH_MAIL          ,"VK_LAUNCH_MAIL",
	VK_LAUNCH_MEDIA_SELECT  ,"VK_LAUNCH_MEDIA_SELECT",
	VK_LAUNCH_APP1          ,"VK_LAUNCH_APP1",
	VK_LAUNCH_APP2          ,"VK_LAUNCH_APP2",
	VK_POWER                ,"VK_POWER",
	VK_OEM_PLUS				,"VK_OEM_PLUS",
	VK_OEM_COMMA			,"VK_OEM_COMMA",
	VK_OEM_MINUS			,"VK_OEM_MINUS",
	VK_OEM_PERIOD			,"VK_OEM_PERIOD",
	VK_OEM_1				,"VK_OEM_1",
	VK_OEM_2				,"VK_OEM_2",
	VK_OEM_3				,"VK_OEM_3",
	VK_OEM_4				,"VK_OEM_4",
	VK_OEM_5				,"VK_OEM_5",
	VK_OEM_6				,"VK_OEM_6",
	VK_OEM_7				,"VK_OEM_7"
};

JOYSTICK_ALIAS	tjalias[16] =
	{MASK_L1 | MASK_M1 | MASK_R1 | MASK_0 | MASK_2	,VK_RETURN
	,MASK_V1 | MASK_0								,VK_VOLUME_UP
	,MASK_R1 | MASK_0								,'X'
	,MASK_L1 | MASK_0								,'Z'
	,MASK_N1 | MASK_0								,VK_UP
	,MASK_W1 | MASK_0								,VK_LEFT
	,MASK_S1 | MASK_0								,VK_DOWN
	,MASK_E1 | MASK_0								,VK_RIGHT
	,MASK_L2 | MASK_M2 | MASK_R2 | MASK_0 | MASK_2	,' '
	,MASK_Y1 | MASK_0								,VK_VOLUME_DOWN
	,MASK_R2 | MASK_0								,'P'
	,MASK_L2 | MASK_0								,'O'
	,MASK_N2 | MASK_0								,'W'
	,MASK_W2 | MASK_0								,'A'
	,MASK_S2 | MASK_0								,'S'
	,MASK_E2 | MASK_0								,'D'
};

// 

FINGER_PROJECTION	tfpButton[] = {
	"Left",	MASK_L1|MASK_L2,	1,	0,	4,  2,
	"Right",MASK_R1|MASK_R2,	9,	0,	4,	2,
	"Middle",MASK_M1|MASK_M2,	5,	0,	4,	2,
	"South",MASK_S1|MASK_S2,	4,	13,	6,	4,
	"East",	MASK_E1|MASK_E2,	10,	7,	4,	6,
	"North",MASK_N1|MASK_N2,	4,	3,	6,	4,
	"West",	MASK_W1|MASK_W2,	0,	7,	4,	6,
	"X",	MASK_X1|MASK_X2,	8,	9,	2,	2,
	"H",	MASK_H1|MASK_H2,	4,	9,	2,	2,
	"V",	MASK_V1|MASK_V2,	6,	7,	2,	2,
	"Y",	MASK_Y1|MASK_Y2,	6,	11,	2,	2,
	"A",	MASK_A1|MASK_A2,	0,	3,	3,	3,
	"B",	MASK_B1|MASK_B2,	11,	3,	3,	3,
	"",		MASK_5,				1,	19,	3,	2,
	"",		MASK_5,				4,	19,	3,	2,
	"",		MASK_5,				7,	19,	3,	2,
	"",		MASK_5,				10,	19,	3,	2,
	"",
};
///////////////////////////////////////////////////////////
LRESULT CALLBACK hookMouseProc(int iCode, WPARAM wParam, LPARAM lParam) {
	PMSLLHOOKSTRUCT	pmhs = PMSLLHOOKSTRUCT(lParam);
	short			iWheel = HIWORD(pmhs->mouseData);
	const bool		bBlockInput = ((iCode >= 0) && ((wParam >= WM_MOUSEFIRST) && (wParam <= WM_MOUSELAST))) && ((pmhs->flags & LLMHF_INJECTED) == 0);
	int				iMouse = !bBlockInput ? 0 : 1;
	int				nShift = iMouse ? 13 : 0;
/*	if(iCode >= 0)
		switch(wParam) {
		case WM_MOUSEMOVE:
			ptVector[iMouse].x += pmhs->pt.x - ptCursor.x;
			ptVector[iMouse].y += pmhs->pt.y - ptCursor.y;
			if(ptVector[iMouse].x >= ptRange.x)
				dwRemote = (dwRemote & ~(MASK_G1 << nShift)) | (MASK_E1 << nShift),
				ptVector[iMouse].x = ptRange.x;
			if(ptVector[iMouse].y >= ptRange.y)
				dwRemote = (dwRemote & ~(MASK_F1 << nShift)) | (MASK_D1 << nShift),
				ptVector[iMouse].y = ptRange.y;
			if(ptVector[iMouse].x <= -ptRange.x)
				dwRemote = (dwRemote & ~(MASK_E1 << nShift)) | (MASK_G1 << nShift),
				ptVector[iMouse].x = -ptRange.x;
			if(ptVector[iMouse].y <= -ptRange.y)
				dwRemote = (dwRemote & ~(MASK_D1 << nShift)) | (MASK_F1 << nShift),
				ptVector[iMouse].y = -ptRange.y;
			break;
		case WM_XBUTTONDOWN:
			dwRemote |= MASK_K1 << (iWheel + nShift);
			return 1;
		case WM_XBUTTONUP:
			dwRemote &= ~(MASK_K1 << (iWheel + nShift));
			return 1;
		default:
			switch(wParam) {
			case WM_MOUSEHWHEEL:
				if(iWheel > 0)
					dwRemote = (dwRemote & ~(MASK_I1 << nShift)) ^ (MASK_H1 << nShift);
				if(iWheel < 0)
					dwRemote = (dwRemote & ~(MASK_H1 << nShift)) ^ (MASK_I1 << nShift);
				break;
			case WM_MOUSEWHEEL:
				if(iWheel < 0)
					dwRemote = (dwRemote & ~(MASK_J1 << nShift)) ^ (MASK_K1 << nShift);
				if(iWheel > 0)
					dwRemote = (dwRemote & ~(MASK_K1 << nShift)) ^ (MASK_J1 << nShift);
				break;
			case WM_LBUTTONDOWN:	dwRemote |= (MASK_A1 << nShift);	break;
			case WM_LBUTTONUP:		dwRemote &= ~(MASK_A1 << nShift);	break;
			case WM_MBUTTONDOWN:	dwRemote |= (MASK_C1 << nShift);	break;
			case WM_MBUTTONUP:		dwRemote &= ~(MASK_C1 << nShift);	break;
			case WM_RBUTTONDOWN:	dwRemote |= (MASK_B1 << nShift);	break;
			case WM_RBUTTONUP:		dwRemote &= ~(MASK_B1 << nShift);	break;
			default:
				dwRemote = wParam;
			}
	}
			switch(wParam) {
			case WM_MOUSEHWHEEL:
				if(iWheel > 0)
					dwRemote = (dwRemote & ~(MASK_I1 << nShift)) ^ (MASK_H1 << nShift);
				if(iWheel < 0)
					dwRemote = (dwRemote & ~(MASK_H1 << nShift)) ^ (MASK_I1 << nShift);
				break;
			case WM_MOUSEWHEEL:
				if(iWheel < 0)
					dwRemote = (dwRemote & ~(MASK_J1 << nShift)) ^ (MASK_K1 << nShift);
				if(iWheel > 0)
					dwRemote = (dwRemote & ~(MASK_K1 << nShift)) ^ (MASK_J1 << nShift);
				break;
			}*/
	if(isGameProcess || bBlockInput)
		return 1;
	ptCursor.x = pmhs->pt.x;
	ptCursor.y = pmhs->pt.y;
	return CallNextHookEx(0, iCode, wParam, lParam);
}

void	ListView_Refresh(HWND hWnd, int row, int z) {
	const BYTE	szTag[]	= "-Ll-Mm-Rr-Ss-Nn-Ee-Ww-Xx-Hh-Vv-Yy-Aa-Bb-Ll-Mm-Rr-Ss-Nn-Ee-Ww-Xx-Hh-Vv-Yy-Aa-Bb @@";
	LV_HITTESTINFO	hit;
	DWORD	flag, hold;
	DWORD	bits;
	DWORD	tag;
	int		i = 27;
	int		x;
	if(row < 0 || row > sizeof tjalias / sizeof *tjalias) {
		row = ListView_GetNextItem(hWnd, -1, LVNI_FOCUSED);
		if(row < 0 || row > sizeof tjalias / sizeof *tjalias)
			return;
		GetCursorPos(&hit.pt);
		ScreenToClient(hWnd, &hit.pt);
		ListView_SubItemHitTest(hWnd, &hit);
		z = hit.iSubItem - 1;
		if(x < 0 && x >= i)
			z = -1;
	}
	bits = tjalias[row].mask;
	for(x = 0; x < 27; ++ x) {
		flag = 1 << x;
		hold = 1 << i;
		if(x == z && x < 26) {
			if(!(bits & flag))
				bits = (bits & ~(-1 << i)) | ((bits & (-1 << i)) << 1);
			bits ^= bits & flag ? bits & hold ? flag | hold : hold : flag;
			if(!(bits & flag))
				bits = (bits & ~(-1 << i)) | ((bits & (-2 << i)) >> 1);
			hold = 1 << i;
		} else
		if(x == z)
			bits ^= flag;
		tag = szTag[x * 3 + (bits & flag ? bits & hold ? 2 :  1 : 0)];
		ListView_SetItemText(hWnd, row, x + 1, TEXT(PTCHAR(&tag)));
		if(flag & bits)
			++ i;
	}
	if(z >= 0)
		tjalias[row].mask = bits;
	dwRemoters = 0;
}

HWND		CreateListView(HWND hParentWnd, long left, long top, UINT id) {//LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP
	const char	szHeader[]	= "LMRSNEWXHVYABLMRSNEWXHVYAB@";
	int			i;
	TCHAR		sz[STRING_DIMENSION];
	HWND		hWnd = hParentWnd;
	RECT		prc;
	LVCOLUMN	lvc;
	LVITEM		lvi;
	if(id) {
		hWnd = CreateWindow(WC_LISTVIEW, NULL, WS_CHILD|WS_VISIBLE | LVS_REPORT, left, top, 0, 0, hParentWnd, HMENU(id), hInst, NULL);
		ListView_SetExtendedListViewStyle(hWnd, LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP*0 | LVS_EX_GRIDLINES);//LVS_EX_CHECKBOXES);
		memset(&lvc, NULL, sizeof lvc);
		lvc.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH  | LVCF_FMT;
		lvc.fmt  = LVCFMT_LEFT;
		lvc.pszText = sz;
		lvc.iSubItem = 0; lvc.cx = 160;
		wsprintf(lvc.pszText, "Virtual Key");
		ListView_InsertColumn(hWnd, 0, &lvc);
		for(i = 0; i < 27; ++ i)
			lvc.cx = 32,
			wsprintf(lvc.pszText, "%c%c", szHeader[i], i < 13 ? '1' : i < 26 ? '2' : ' '),
			ListView_InsertColumn(hWnd, i + 1, &lvc);
	} else
		DestroyMenu(hMenu);
	memset(&lvi, NULL, sizeof lvi);
	memset(&mii, NULL, sizeof mii);
	mii.cbSize = sizeof mii;
	mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
	mii.fType = MFT_STRING;
	mii.fState = MFS_ENABLED;
	mii.cch = 2;
	mii.dwTypeData = sz;
	hMenu = CreatePopupMenu();
	for(lvi.iItem = 0; lvi.iItem < sizeof tjalias / sizeof *tjalias; ++ lvi.iItem) {
		lvi.iSubItem = 0;
		ListView_InsertItem(hWnd, &lvi);
		ListView_SetItemText(hWnd, lvi.iItem, 0, TEXT(tszVKId[tjalias[lvi.iItem].keycode]));
		for(i = 0; i < 27; ++ i) {
			lvi.iSubItem ++;
			wsprintf(sz, "%c", tjalias[lvi.iItem].mask & (1 << i) ? 'A' + i : '-');
			ListView_SetItemText(hWnd, lvi.iItem, i + 1, TEXT(sz));
		}
//		ListView_SetItemText(hWnd, lvi.iItem, i + 2, TEXT(tjalias[lvi.iItem].action));
		mii.wID = IDM_BUTTONS + lvi.iItem;
		wsprintf(sz, "s");
		InsertMenuItem(hMenu, lvi.iItem, 1, &mii);
	}
	ListView_GetSubItemRect(hWnd, lvi.iItem - 1, 0, 0, &prc);
	SetWindowPos(hWnd, NULL, NULL, NULL, prc.right, prc.bottom, SWP_NOMOVE);
	RECT rt;
	GetWindowRect(hWndSBar, &rt);
	int	cx =	GetSystemMetrics(SM_CYFRAME) * 2;
		cx +=	left + prc.right - prc.left;
	int	cy =	GetSystemMetrics(SM_CYFRAME) * 2;
		cy +=	GetSystemMetrics(SM_CYCAPTION);
		cy +=	GetSystemMetrics(SM_CYMENU);
		cy +=	rt.bottom - rt.top;
		cy +=	top + prc.bottom;
	SetWindowPos(GetParent(hWnd), NULL, NULL, NULL, cx, cy, SWP_NOMOVE);
	return hWnd;
	// -8,-68
}

void	DrawButtons(HWND hWnd) {
	TCHAR	sz[16];
	PFINGER_PROJECTION	pfpt;
	PTCHAR	psz;
	POINT	pt;
	RECT	rt;
	UINT	style;
	INPUT	kbdInput[32];
	HDC		hdc;
	DWORD	dwRemoting = dwRemote;
	DWORD	mask;
	DWORD	ignore = 0xFFFFFFFF;
	DWORD	cleaner = 0;
	int		i = 0, n = 0;
	int		k;

	idxVK = -1;
	GetCursorPos(&pt),
	ScreenToClient(hWnd, &pt);
	fPress ^= 1;
	hdc = GetDC(hListView);
	mask = dwRemoters ^ dwRemote;
	dwRemoters = dwRemote;
	for(k = 1; k < 27; ++ k) {
		ListView_GetSubItemRect(hListView, 15,k, 0, &rt);
		rt.top = 0;
		rt.left += 3;
		rt.right -= 3;
		if((mask >> k - 1) & 1)
			DrawFocusRect(hdc, &rt);
	}
	ReleaseDC(hListView, hdc);
	if(hWnd) {
		hdc = GetDC(hWnd);
		pfpt = &tfpButton[0];
		while(pfpt->name[0]) {
			style = pfpt->mask ? dwRemote & pfpt->mask ? NULL : SBT_POPOUT : dwRemote ? SBT_POPOUT : NULL;
			if(i > 12)
				style = idxMouse == i - 12 ? NULL : SBT_POPOUT;
			psz = pfpt->name;//tjalias[pfpt->alias].button;
			SetRect(&rt, pfpt->position.x * 12, pfpt->position.y * 12, (pfpt->position.x + pfpt->size.cx) * 12 - 1, (pfpt->position.y + pfpt->size.cy) * 12 - 1);
			DrawStatusText(hdc, &rt, psz, style);
			if(!isGameProcess && PtInRect(&rt, pt))
				SetRect(&rt, pfpt->position.x * 12 + 2, pfpt->position.y * 12 + 2, (pfpt->position.x + pfpt->size.cx) * 12 - 3, (pfpt->position.y + pfpt->size.cy) * 12 - 3),
				DrawFocusRect(hdc, &rt),
				idxVK = i;
			++ pfpt;
			++ i;
		}
		ReleaseDC(hWnd, hdc);
	} else {
		while(i < 16) {
			mask = tjalias[i].mask & 0x03FFFFFF;
			if((tjalias[i].keycode & 0x80008000) == 0x80008000) {
				kbdInput[n].type = INPUT_MOUSE;
				kbdInput[n].mi.dx = LOWORD(tjalias[i].keycode) & 0x7FFF;
				kbdInput[n].mi.dy = HIWORD(tjalias[i].keycode) & 0x7FFF;
				kbdInput[n].mi.mouseData = 0;
				kbdInput[n].mi.dwExtraInfo = 0;
				kbdInput[n].mi.time = 0;
				kbdInput[n].mi.dwFlags = ((dwRemoting & mask) == mask ? MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTDOWN : 0)| MOUSEEVENTF_ABSOLUTE;// | MOUSEEVENTF_MOVE ;
				//SendInput(1, &kbdInput[n], sizeof(INPUT));
				if((dwRemoting & mask) == mask) {
					pt.x = LOWORD(tjalias[i].keycode) & 0x7FFF,
					pt.y = HIWORD(tjalias[i].keycode) & 0x7FFF;
					HWND	hWin = WindowFromPoint(pt);
					while(GetParent(hWin))
						hWin = GetParent(hWin);
					BringWindowToTop(hWin);
					SetForegroundWindow(hWin);
					SetActiveWindow(hWin);
					SetFocus(hWin);
					ScreenToClient(hWin, &pt);
					PostMessage(hWin, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
					PostMessage(hWin, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
//					SetCursorPos(LOWORD(tjalias[i].keycode) & 0x7FFF, HIWORD(tjalias[i].keycode) & 0x7FFF);
//					mouse_event(MOUSEEVENTF_LEFTDOWN, LOWORD(tjalias[i].keycode) & 0x7FFF, HIWORD(tjalias[i].keycode) & 0x7FFF, 0, 0);
//					mouse_event(MOUSEEVENTF_LEFTUP, LOWORD(tjalias[i].keycode) & 0x7FFF, HIWORD(tjalias[i].keycode) & 0x7FFF, 0, 0);
				}
			} else {
				kbdInput[n].type = INPUT_KEYBOARD;
				kbdInput[n].ki.wScan = 0;
				kbdInput[n].ki.time = 0;
				kbdInput[n].ki.wVk = tjalias[i].keycode;
				kbdInput[n].ki.dwFlags = (dwRemoting & mask) == mask ? 0 : KEYEVENTF_KEYUP;
			}
			if((((dwRemoting | dwRemoted) & mask) == mask) && (mask != 0) && (tjalias[i].keycode != 0) && (mask & ignore) == mask) {
				mask = tjalias[i].mask;
				ignore &= 0xFC000000 | ~mask;
				n += ((mask >> 26) & 1) == (((dwRemoting | dwRemoted) >> 26) & 1) ? 1 : 0;
				//n += isGameProcess ? 1 : (mask & (dwRemoting | dwRemoted)) >= MASK_L ? 1 : NULL;
				int	x;
				DWORD	k = mask >> 27;
				for(x = 0; x < 27; ++ x) {
					if(mask & (1 << x))
						cleaner |= k & 1 ? 1 << x : 0,
						k >>= 1;
				}
//				if(!isGameProcess && (kbdInput[n].ki.dwFlags == NULL))
//					ListView_SetHotItem(hListView, i);
			}
			++ i;
		}
	}
	if(!hWnd)
		SendInput(n, kbdInput, sizeof(INPUT)),
		dwRemoted = dwRemoting;
	dwRemote &= ~cleaner;
}

void	SendButtons(HWND	hWnd) {
	WPARAM	wParam;
	LPARAM	lParam;
	TCHAR	sz[16];
	PFINGER_PROJECTION	pfpt;
	PTCHAR	psz;
	POINT	pt;
	RECT	rt;
	UINT	style;
	INPUT	kbdInput[32];
	HDC		hdc;
	DWORD	dwRemoting = dwRemote;
	DWORD	mask;
	DWORD	cleaner = 0;
	int		i = 0, n = 0;
	idxVK = -1;
	GetCursorPos(&pt),
	ScreenToClient(hWnd, &pt);
	fPress ^= 1;
	for(i = 0; i < 16; ++ i) {
	//		SendMessage(hWin, WM_KEYDOWN, 0x20,0);
	//		SendMessage(hWin, WM_KEYUP, 0x20,0);
		mask = tjalias[i].mask & 0x03FFFFFF;
//		SendMessage(hWnd, WM_CHAR, 0x20,0);
		wParam = WPARAM(tjalias[i].keycode);
		lParam = LPARAM(0);
		if((dwRemoting & mask) == mask)
			SendMessage(hWnd, (dwRemoting & mask) == mask ? WM_CHAR : WM_KEYUP, wParam, lParam);
		SendMessage(hWnd, (dwRemoting & mask) == mask ? WM_KEYDOWN : WM_KEYUP, wParam, lParam);
		if((((dwRemoting | dwRemoted) & mask) == mask) && (mask != 0) && (tjalias[i].keycode != 0)) {
			mask = tjalias[i].mask;
			n += ((mask >> 26) & 1) == (((dwRemoting | dwRemoted) >> 26) & 1) ? 1 : 0;
			//n += isGameProcess ? 1 : (mask & (dwRemoting | dwRemoted)) >= MASK_L ? 1 : NULL;
			int	x;
			DWORD	k = mask >> 27;
			for(x = 0; x < 27; ++ x) {
				if(mask & (1 << x))
					cleaner |= k & 1 ? 1 << x : 0,
					k >>= 1;
			}
//				if(!isGameProcess && (kbdInput[n].ki.dwFlags == NULL))
//					ListView_SetHotItem(hListView, i);
		}
		++ i;
	}
	dwRemoted = dwRemoting;
	dwRemote &= ~cleaner;
}
// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

/*#include "RawInputMouse.h"
void OnRawInput(bool inForeground, HRAWINPUT hRawInput){
UINT dataSize;
GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

if (dataSize == 0)
    return;
if (dataSize > m_RawInputMessageData.size())
    m_RawInputMessageData.resize(dataSize);

void* dataBuf = &m_RawInputMessageData[0];
GetRawInputData(hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));

const RAWINPUT *raw = (const RAWINPUT*)dataBuf;
if (raw->header.dwType == RIM_TYPEMOUSE){
    HANDLE deviceHandle = raw->header.hDevice;
    const RAWMOUSE& mouseData = raw->data.mouse;
    USHORT flags = mouseData.usButtonFlags;
    short wheelDelta = (short)mouseData.usButtonData;
    LONG x = mouseData.lLastX, y = mouseData.lLastY;

    wprintf(L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
    deviceHandle, flags, wheelDelta, x, y);
}*/
//

BOOL	InitializeStatusBar(HWND hWnd, int nrOfParts) {
const int	cSpaceInBetween = 8;
int		ptArray[40];   // Array defining the number of parts/sections
RECT	rect;
HDC		hDC;
	// Fill in the ptArray...
	hDC = GetDC(hWnd);
	GetClientRect(hWnd, &rect);

	ptArray[2] = rect.right;
	ptArray[1] = ptArray[2] - 128;
	ptArray[0] = ptArray[1] - 256;
	//---TODO--- Add code to calculate the size of each part of the status bar here.

	ReleaseDC(hWnd, hDC);
	SendMessage(hWndSBar, SB_SETPARTS, 3, (LPARAM)(LPINT)ptArray);

	UpdateSBar(hWndSBar, "Ready", 0, 0);
	//---TODO--- Add code to update all fields of the status bar here.
	// As an example, look at the calls commented out below.

//	UpdateStatusBar("Cursor Pos:", 1, SBT_POPOUT);
//	UpdateStatusBar("Time:", 3, SBT_POPOUT);
	return TRUE;
}

//
int APIENTRY	WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// TODO: Place code here.
	MSG			msg;
	HACCEL		hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, STRING_DIMENSION);
	LoadString(hInstance, IDC_GESTUREO, szWindowClass, STRING_DIMENSION);
	MyRegisterClass(hInstance);

	// Perform application initialization:

	// Main message loop:
	if(msg.message = User32_DLL()) {
		wsprintf(szTitle, "Error mask is 0x%08X", msg.message);
		MessageBox(NULL, szTitle, "Sorry", MB_OK);
	} else
	if(InitInstance(hInstance, nCmdShow)) {
		hAccelTable = LoadAccelerators(hInstance, LPCTSTR(IDC_GESTUREO));
		while(GetMessage(&msg, NULL, 0, 0)) {
			if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
	User32_DLL();

	return msg.wParam;
}
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WNDPROC(WndProc);
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, LPCTSTR(IDI_GESTUREO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= HBRUSH(COLOR_BACKGROUND+1);
	wcex.lpszMenuName	= LPCSTR(IDC_GESTUREO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, LPCTSTR(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable
//	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0, 0, 1024 + GetSystemMetrics(SM_CXBORDER) * 4 + GetSystemMetrics(SM_CXSIZE), 328
		+ GetSystemMetrics(SM_CYBORDER) * 4
		+ (GetSystemMetrics(SM_CYCAPTION) - 1) * 2
		+ (GetSystemMetrics(SM_CYMENU) + 1)
		+ GetSystemMetrics(SM_CYSIZE)
		, NULL, NULL, hInstance, NULL);

	if(!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void	DrawJoystick(HWND hWnd) {
	RECT		rt;
	POINT		pt;
	HDC			hdc;
	LOGFONT		lf;
	bool		style;
	INPUT	kbdInput;
	kbdInput.type = INPUT_KEYBOARD;
	kbdInput.ki.wScan = 0;
	kbdInput.ki.time = 0;
	if(hWnd) {
		hdc = GetDC(hWnd);
		GetObject(GetStockObject(ANSI_FIXED_FONT), sizeof lf, &lf);
		if(isGameProcess)
			lf.lfWidth *= 4,
			lf.lfHeight *= 4,
			lf.lfWidth /= 3,
			lf.lfHeight /= 3,
			lf.lfWeight = 800;
		SelectObject(hdc, CreateFontIndirect(&lf));
//		DrawButtons(hWnd, tfpButton, hdc, dwRemote);
		DeleteObject(SelectObject(hdc, GetStockObject(OEM_FIXED_FONT)));
		ReleaseDC(hWnd, hdc);
	} else
;//		DrawButtons(hWnd, tfpButton, hdc, dwRemote);
/*	wsprintf(szTitle, "%5d:%5d %5d", ptVector.x, ptVector.y);
	UpdateSBar(hWndSBar, szTitle, 0, 0);*/
//	dwRemote &= ~(FINGER_P | FINGER_Q);
//							_Q_		_1_		_2_		_3_		_F_		_B_		_N_		_W_		_E_		_S_		_P_
}

void	GlobalHooker(bool fHook) {
	RECT		rt;
	if(fHook) {
		GetCursorPos(&ptCursor);
		SetRect(&rt, 0, 0, 0, 0);
		ClipCursor(&rt);
		hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, hookMouseProc, hInst, NULL);
	} else {
		UnhookWindowsHookEx(hMouseHook);
		hMouseHook = NULL;
		dwRemote = NULL;
		DrawJoystick(NULL);
		ClipCursor(NULL);
		SetCursorPos(ptCursor.x, ptCursor.y);
	}
}

void	MenuLoad() {
	TCHAR	lpsz[STRING_DIMENSION];
	TCHAR	name[STRING_DIMENSION];
	BYTE	array[16];
	HKEY	hKey;
	DWORD	nKey;
	DWORD	type;
	HWND	hWnd;
	int		i, index;
	wsprintf(lpsz, "SOFTWARE\\Alikberov\\Gestureo\\Preset_%d", index);
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpsz, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, NULL);
	wsprintf(lpsz, "caption");
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&szProfile), &nKey);
		wsprintf(lpsz, "%s - %s", szProfile, szTitle);
		SetWindowText(hWnd, lpsz);
	} else
		RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&szProfile), strlen(szProfile));
	wsprintf(lpsz, "description");
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&szDescription), &nKey);
		UpdateSBar(hWndSBar, szDescription, 0, 0);
	} else
		RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&szDescription), sizeof lpsz);
	wsprintf(lpsz, "buttons", i);
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&array), &nKey);
		for(i = 0; i < 16; ++ i)
			tjalias[i].keycode = array[i];
	} else {
		for(i = 0; i < 16; ++ i)
			array[i] = tjalias[i].keycode;
		RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&array), sizeof array);
	}
}

void	PresetLoad(HWND hWnd, char index) {
	TCHAR	lpsz[STRING_DIMENSION];
	TCHAR	name[STRING_DIMENSION];
	BYTE	array[16];
	DWORD	masks[16];
	HKEY	hKey;
	DWORD	nKey;
	DWORD	type;
	int		i;
	wsprintf(lpsz, "SOFTWARE\\Alikberov\\Gestureo\\Preset_%d", index);
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpsz, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, NULL);
	wsprintf(lpsz, "caption");
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&szProfile), &nKey);
		wsprintf(lpsz, "%s - %s", szProfile, szTitle);
		SetWindowText(hWnd, lpsz);
	} else
		RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&szProfile), strlen(szProfile));
	wsprintf(lpsz, "description");
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&szDescription), &nKey);
		strcpy(szNote, szDescription);
		isDescription = true;
	} else
		RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&szDescription), sizeof lpsz);
	wsprintf(lpsz, "actions");
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&array), &nKey);
		for(i = 0; i < 16; ++ i)
			tjalias[i].keycode = array[i];
	} else {
		for(i = 0; i < 16; ++ i)
			array[i] = tjalias[i].keycode;
		RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&array), sizeof array);
	}
	wsprintf(lpsz, "buttons");
	if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
		RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&masks), &nKey);
		for(i = 0; i < 16; ++ i) {
			tjalias[i].mask = masks[i];
			ListView_Refresh(hListView, i, -1);
		}
	} else {
		for(i = 0; i < 16; ++ i)
			masks[i] = tjalias[i].mask;
		RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&masks), sizeof masks);
	}
	for(i = 0; i < 16; ++ i) {
		wsprintf(lpsz, "action%02d", i);
		if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
			RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&name), &nKey);
			ListView_SetItemText(hListView, i, 0, TEXT(name));
			wsprintf(lpsz, "%s", tszVKId[array[i]]);
			//ListView_SetItemText(hListView, i, 1, TEXT(lpsz));
		} else {
			ListView_GetItemText(hListView, i, 0, TEXT(name), sizeof name);
			RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&name), sizeof name);
		}
	}
	for(i = 0; i < sizeof tfpButton / sizeof tfpButton[0]; ++ i) {
		wsprintf(lpsz, "button%02d", i);
		if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
			RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&tfpButton[i]), &nKey);
		} else {
			RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&tfpButton[i]), sizeof tfpButton[i]);
		}
	}
	dwRemoters = 0;
}
void	PresetSave(HWND hWnd, char index) {
	TCHAR	lpsz[STRING_DIMENSION];
	TCHAR	name[STRING_DIMENSION];
	BYTE	array[16];
	DWORD	masks[16];
	HKEY	hKey;
	DWORD	nKey;
	DWORD	type;
	int		i;
	wsprintf(lpsz, "SOFTWARE\\Alikberov\\Gestureo\\Preset_%d", index);
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpsz, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKey, NULL);
	wsprintf(lpsz, "caption");
	RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&szProfile), strlen(szProfile));
	wsprintf(lpsz, "description");
	RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&szDescription), sizeof lpsz);
	for(i = 0; i < 16; ++ i)
		array[i] = tjalias[i].keycode,
		masks[i] = tjalias[i].mask;
	wsprintf(lpsz, "actions");
	RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&array), sizeof array);
	wsprintf(lpsz, "buttons");
	RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&masks), sizeof masks);
	for(i = 0; i < 16; ++ i) {
		wsprintf(lpsz, "action%02d", i);
		ListView_GetItemText(hListView, i, 0, TEXT(name), sizeof name);
		RegSetValueEx(hKey, lpsz, NULL, REG_SZ, PBYTE(&name), sizeof name);
	}
	for(i = 0; i < sizeof tfpButton / sizeof tfpButton[0]; ++ i) {
		wsprintf(lpsz, "button%02d", i);
		RegSetValueEx(hKey, lpsz, NULL, REG_BINARY, PBYTE(&tfpButton[i]), sizeof tfpButton[i]);
	}
}

void	ConsoleInput(char key) {
	char	buffer[STRING_DIMENSION];
	char	ascii;
	int		length = SendMessage(hWndSBar, SB_GETTEXT, 1, LPARAM(buffer));
	if(buffer[length - 1] == '|' || buffer[length - 1] == '\t')
		buffer[-- length] = 0;
	switch(key) {
	case VK_TAB:
		isDescription = !isDescription;
		strcpy(buffer, isDescription ? szDescription : szProfile);
		break;
	case VK_RETURN:
		strcpy(isDescription ? szDescription : szProfile, buffer);
		break;
	case VK_ESCAPE:
		*buffer = NULL;
		break;
	case VK_BACK:
		if(length)
			buffer[length - 1] = NULL;
		break;
	case ' ': case '!': case '#': case '&': case '(': case ')': case '*': case '+': case ',': case '-': case '.': case '/': case '?': case '@': case '_':
		if(idxVK > 12) {
			isaMouse[idxVK - 12] = (isaMouse[idxVK - 12] + 1) % (sizeof pszModes / sizeof *pszModes);
		} else {
			ascii = buffer[length - 1];
			if(length + 1 < sizeof szRename && (ascii >= '0' && ascii <= '9') || (ascii >= 'A' && ascii <= 'Z') || (ascii >= 'a' && ascii <= 'z') || (ascii != ' ' && key != ' ') && length)
				buffer[length + 1] = NULL,
				buffer[length] = key;
		}
		break;
	default:
		if(length + 1 < sizeof szRename && ((key >= '0' && key <= '9') || (key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z')))
			buffer[length + 1] = NULL,
			buffer[length] = key;
		break;
	}
	SendMessage(hWndSBar, SB_SETTEXT, 0, LPARAM(isDescription ? "Type description text:" : "Type profile name"));
	wsprintf(szNote, "%s|", buffer);
}

void	RenameMenuItem(HMENU hMnu, int idMin, int idMax, int stMin, int stMax) {
	TCHAR	lpsz[STRING_DIMENSION];
	TCHAR	name[STRING_DIMENSION];
	BYTE	array[16];
	HKEY	hKey;
	DWORD	nKey;
	DWORD	type;
	MENUITEMINFO	mmi;
	TCHAR			szText[STRING_DIMENSION];
	int				i = 0;
	ZeroMemory(&mmi, sizeof(MENUITEMINFO));
	mmi.cbSize = sizeof(MENUITEMINFO);
	mmi.dwTypeData  = szText;
	mmi.cch = sizeof(szText);
	mmi.fMask = MIIM_TYPE | MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU;
	while(GetMenuItemInfo(hMnu, i, TRUE, &mmi)) {
		mmi.cch = sizeof(szText);
		if(mmi.wID >= idMin && mmi.wID <= idMax) {
			wsprintf(lpsz, "SOFTWARE\\Alikberov\\Gestureo\\Preset_%d", mmi.wID - idMin);
			RegCreateKeyEx(HKEY_LOCAL_MACHINE, lpsz, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, NULL);
			wsprintf(lpsz, "caption");
			if(RegQueryValueEx(hKey, lpsz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS) {
				RegQueryValueEx(hKey, lpsz, NULL, &type, PBYTE(&lpsz), &nKey);
				wsprintf(szText, "&%d %s", mmi.wID - idMin, lpsz),
				SetMenuItemInfo(hMnu, i, TRUE, &mmi);
			}
			RegCloseKey(hKey);
		} else
		if(mmi.wID >= stMin && mmi.wID <= stMax)
			wsprintf(szText, "&%d %s", mmi.wID - stMin, szProfile),
			SetMenuItemInfo(hMnu, i, TRUE, &mmi);
		++ i;
	}
}
void	GetProfileDescription(PTCHAR sz, int id) {
	BYTE	array[16];
	HKEY	hKey;
	DWORD	nKey;
	DWORD	type;
	wsprintf(sz, "SOFTWARE\\Alikberov\\Gestureo\\Preset_%d", id);
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, sz, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, NULL);
	wsprintf(sz, "description");
	if(RegQueryValueEx(hKey, sz, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS)
		RegQueryValueEx(hKey, sz, NULL, &type, PBYTE(sz), &nKey);
	else
		*sz = NULL;
	RegCloseKey(hKey);
}

char	m_RawInputMessageData[256];
void OnRawInput_1(bool inForeground, HRAWINPUT hRawInput) {
	char	sz[4096];
	UINT	dataSize;
	GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
//	GetRawInputData(hRawInput, RID_INPUT, NULL, &dataSize, sizeof(RAWINPUT));
	GetLastError();
	if(dataSize == 0)
		return;
//    if(dataSize > sizeof m_RawInputMessageData)
  //      m_RawInputMessageData.resize(dataSize);
	void* dataBuf = &m_RawInputMessageData[0];
	RAWINPUT *raw = (RAWINPUT*)dataBuf;
	//raw->header.dwSize = sizeof RAWINPUT;
	GetRawInputData(hRawInput, RID_INPUT, dataBuf, &dataSize, sizeof(RAWINPUTHEADER));
	GetLastError();
	if(raw->header.dwType == RIM_TYPEMOUSE) {
		HANDLE deviceHandle = raw->header.hDevice;
		const RAWMOUSE& mouseData = raw->data.mouse;
		USHORT flags = mouseData.usButtonFlags;
		short wheelDelta = (short)mouseData.usButtonData;
		LONG x = mouseData.lLastX, y = mouseData.lLastY;
		int i=0;
		while(tRIDList[i].hDevice && (tRIDList[i].hDevice != deviceHandle))
			++ i;
		if(i < sizeof tRIDList / sizeof tRIDList[0])
		wsprintf(sz, "%d#%4dx%-4d:%3d /%d %08X %08X", i, x, y, flags, wheelDelta, hRawInput, tRIDList[3].hDevice);
		else
		wsprintf(sz, "%d#%4dx%-4d:%3d /%d=%08X %08X", i, x, y, flags, wheelDelta, hRawInput, tRIDList[3].hDevice);
		UpdateSBar(hWndSBar, sz, 0, 0);
  /*      wprintf(
            L"Mouse: Device=0x%08X, Flags=%04x, WheelDelta=%d, X=%d, Y=%d\n",
            deviceHandle, flags, wheelDelta, x, y);*/
	}
}
RID_DEVICE_INFO	rdi;

LRESULT	OnRawInput(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL inForeground) {
	BYTE		aBuff[4096];
	INPUT		input = {0};
	UINT		nSize = 0;
	int			nDevices = 0;
	DWORD		index	= NULL;
    // нам очень важно тут не попасть на какую-то ошибку приводящую к exception,
    // по весь код обработкчика закрываем в try...catch()
//    RMT_TRY {
	if(GetRawInputData(HRAWINPUT(lParam), RID_INPUT, NULL, &nSize, sizeof RAWINPUTHEADER) != NULL)
		return 1;
	if(GetRawInputData(HRAWINPUT(lParam), RID_INPUT, &aBuff, &nSize, sizeof RAWINPUTHEADER) != nSize)
		return 1;
	const PRAWINPUT pRaw = PRAWINPUT(aBuff);
	// работаем только с мышкой и пропускаем софтверный ввод (hDevice==NULL)...
	if((pRaw->header.dwType != RIM_TYPEMOUSE) || (pRaw->header.hDevice == NULL))
		return 1;
	// если это не пользовательская мышка...
//	currentInputDev = pRaw->header.hDevice;
	while(aRIDevices[nDevices].handle && (aRIDevices[nDevices].handle != pRaw->header.hDevice))
		++ nDevices;
	if(nDevices < sizeof aRIDevices / sizeof aRIDevices[0] && aRIDevices[nDevices].handle)
		idxMouse = aRIDevices[nDevices].index;
	else
	if(nMouses < 4)
		aRIDevices[nDevices].index = ++ nMouses,
		aRIDevices[nDevices].handle = pRaw->header.hDevice,
		idxMouse = nMouses;
	index = isaMouse[idxMouse];
	if(isGameProcess || (index == 1 || index == 2)) {
		int	nShift = index & 1 ? 0 : 13;
		int	iMouse = index & 1 ? 1 : 0;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
			dwRemote |= (MASK_L1 << nShift);
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
			dwRemote &= ~(MASK_L1 << nShift);
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
			dwRemote |= (MASK_R1 << nShift);
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
			dwRemote &= ~(MASK_R1 << nShift);
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
			dwRemote |= (MASK_M1 << nShift);
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
			dwRemote &= ~(MASK_M1 << nShift);

		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) != 0)
			dwRemote |= (MASK_A1 << nShift);
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) != 0)
			dwRemote &= ~(MASK_A1 << nShift);
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) != 0)
			dwRemote |= (MASK_B1 << nShift);
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) != 0)
			dwRemote &= ~(MASK_B1 << nShift);
		ptVector[iMouse].x = pRaw->data.mouse.lLastX;
		ptVector[iMouse].y = pRaw->data.mouse.lLastY;
		if(ptVector[iMouse].x >= ptRange.x)
			dwRemote = (dwRemote & ~(MASK_W1 << nShift)) | (MASK_E1 << nShift),
			ptVector[iMouse].x = ptRange.x;
		if(ptVector[iMouse].y >= ptRange.y)
			dwRemote = (dwRemote & ~(MASK_N1 << nShift)) | (MASK_S1 << nShift),
			ptVector[iMouse].y = ptRange.y;
		if(ptVector[iMouse].x <= -ptRange.x)
			dwRemote = (dwRemote & ~(MASK_E1 << nShift)) | (MASK_W1 << nShift),
			ptVector[iMouse].x = -ptRange.x;
		if(ptVector[iMouse].y <= -ptRange.y)
			dwRemote = (dwRemote & ~(MASK_S1 << nShift)) | (MASK_N1 << nShift),
			ptVector[iMouse].y = -ptRange.y;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_HORIZONTAL_WHEEL) != 0) {
			if(SHORT(pRaw->data.mouse.usButtonData) > 0)
				dwRemote = (dwRemote & ~(MASK_H1 << nShift)) ^ (MASK_X1 << nShift);
			if(SHORT(pRaw->data.mouse.usButtonData) < 0)
				dwRemote = (dwRemote & ~(MASK_X1 << nShift)) ^ (MASK_H1 << nShift);
		}
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) != 0) {
			if(SHORT(pRaw->data.mouse.usButtonData) > 0)
				dwRemote = (dwRemote & ~(MASK_Y1 << nShift)) ^ (MASK_V1 << nShift);
			if(SHORT(pRaw->data.mouse.usButtonData) < 0)
				dwRemote = (dwRemote & ~(MASK_V1 << nShift)) ^ (MASK_Y1 << nShift);
		}
		
		return 0;
	} else
	if(index == 0 || index == 3 || index == 4) {
		// ... тогда разрешаем от неё ввод (дублируем софтверно)
		input.type				= INPUT_MOUSE;
		input.mi.dx				= pRaw->data.mouse.lLastX;
		input.mi.dy				= pRaw->data.mouse.lLastY;
		input.mi.dwExtraInfo	= pRaw->data.mouse.ulExtraInformation;
		// флаги перемещения ...
		if(pRaw->data.mouse.usFlags == 0) // MOUSE_MOVE_RELATIVE
			input.mi.dwFlags |= MOUSEEVENTF_MOVE;
		else
		if((pRaw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE) {
			input.mi.dwFlags |= MOUSEEVENTF_MOVE;
			input.mi.dwFlags |= MOUSEEVENTF_ABSOLUTE;
			if((pRaw->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) == MOUSE_VIRTUAL_DESKTOP)
				input.mi.dwFlags |= MOUSEEVENTF_VIRTUALDESK;
        }
		// кнопки ...
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
			dwRemote = (dwRemote | (MASK_E1 | MASK_W1 | MASK_L1)) | MASK_5,
			input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
			dwRemote = (dwRemote & ~(MASK_E1 | MASK_W1 | MASK_L1)) | MASK_5,
			input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
			dwRemote = (dwRemote | (MASK_E1 | MASK_W1 | MASK_R1)) | MASK_5,
			input.mi.dwFlags |= index == 4 ? 0 : MOUSEEVENTF_RIGHTDOWN;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
			dwRemote = (dwRemote & ~(MASK_E1 | MASK_W1 | MASK_R1)) | MASK_5,
			input.mi.dwFlags |= index == 4 ? 0 : MOUSEEVENTF_RIGHTUP;
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
			dwRemote = (dwRemote | (MASK_E1 | MASK_W1 | MASK_M1)) | MASK_5,
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_MIDDLEDOWN;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
			dwRemote = (dwRemote & ~(MASK_E1 | MASK_W1 | MASK_M1)) | MASK_5,
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_MIDDLEUP;
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) != 0)
			dwRemote = (dwRemote | (MASK_E1 | MASK_W1 | MASK_A1)) | MASK_5,
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_XDOWN,
			input.mi.mouseData |= !!index ? 0 : XBUTTON1;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) != 0)
			dwRemote = (dwRemote & ~(MASK_E1 | MASK_W1 | MASK_A1)) | MASK_5,
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_XUP,
			input.mi.mouseData |= !!index ? 0 : XBUTTON1;
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) != 0)
			dwRemote = (dwRemote | (MASK_E1 | MASK_W1 | MASK_B1)) | MASK_5,
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_XDOWN,
			input.mi.mouseData |= !!index ? 0 : XBUTTON2;
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) != 0)
			dwRemote = (dwRemote & ~(MASK_E1 | MASK_W1 | MASK_B1)) | MASK_5,
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_XUP,
			input.mi.mouseData |= !!index ? 0 : XBUTTON2;
		
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_HORIZONTAL_WHEEL) != 0) {
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_WHEEL,
			input.mi.mouseData = SHORT(pRaw->data.mouse.usButtonData);
			//
			if(SHORT(pRaw->data.mouse.usButtonData) < 0)
				dwRemote = (dwRemote & ~(dwRemote & MASK_X1 ? MASK_X1 | MASK_N1 | MASK_S1 | MASK_5 : 0)) ^ (MASK_H1 | MASK_N1 | MASK_S1 | MASK_5);
			if(SHORT(pRaw->data.mouse.usButtonData) > 0)
				dwRemote = (dwRemote & ~(dwRemote & MASK_H1 ? MASK_H1 | MASK_N1 | MASK_S1 | MASK_5 : 0)) ^ (MASK_Y1 | MASK_N1 | MASK_S1 | MASK_5);
		}
		if((pRaw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) != 0) {
			input.mi.dwFlags |= !!index ? 0 : MOUSEEVENTF_WHEEL,
			input.mi.mouseData = SHORT(pRaw->data.mouse.usButtonData);
			//
			if(SHORT(pRaw->data.mouse.usButtonData) > 0)
				dwRemote = (dwRemote & ~(dwRemote & MASK_Y1 ? MASK_Y1 | MASK_N1 | MASK_S1 | MASK_5 : 0)) ^ (MASK_V1 | MASK_N1 | MASK_S1 | MASK_5);
			if(SHORT(pRaw->data.mouse.usButtonData) < 0)
				dwRemote = (dwRemote & ~(dwRemote & MASK_V1 ? MASK_V1 | MASK_N1 | MASK_S1 | MASK_5 : 0)) ^ (MASK_Y1 | MASK_N1 | MASK_S1 | MASK_5);
		}
		// настройку данных закончили - делаем софтверный ввод.
		SendInput(1, &input, sizeof(input));
	} else {
		int ko = 0;// специальный случай - обработка пользовательской мышки...
	}
//	RMT_END_TRY
	return 1;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int			wpId, wpEvent, i, j, k;
	HKEY	hKey;
	DWORD	nKey;
	DWORD	type;
	UINT		size;
	PAINTSTRUCT	ps;
	POINT		pt;
	RECT		rt;
	HDC			hdc;
	HMENU		hMnu;
	MENUITEMINFO	mmi;
	TCHAR		szText[STRING_DIMENSION];
	TCHAR		sz[STRING_DIMENSION];
	TCHAR		szKey[STRING_DIMENSION];
		PFINGER_PROJECTION	pfpt = &tfpButton[13];
	
	switch(message) {
	case WM_TIMER:
		if(!(GetKeyState(VK_SCROLL) & 1) != !isGameProcess) {
			isGameProcess = !isGameProcess;
			UnhookWindowsHookEx(hMouseHook); hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, hookMouseProc, hInst, NULL);
			dwRemote = isGameProcess ? MASK_0 : NULL;
			KillTimer(hWnd, 1);
			if(isGameProcess) {
				GetCursorPos(&ptCursor);
				SetRect(&rt, ptCursor.x, ptCursor.y, ptCursor.x + 1, ptCursor.y + 1);
				ClipCursor(&rt);
				hTimer = SetTimer(hWnd, 1, 10, NULL);
				SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED),
				SetLayeredWindowAttributes(hWnd, 0, 127, LWA_ALPHA);
				SendMessage(WindowFromPoint(ptCursor), WM_ACTIVATE, WA_ACTIVE,0);
//			SetLayeredWindowAttributes(hWnd, RGB(255,0,0), 0, LWA_COLORKEY);
//			GetRawInputDeviceList(NULL, &nRawDevs, sizeof RAWINPUTDEVICELIST);
			} else
				SendMessage(WindowFromPoint(ptCursor), WM_ACTIVATE, WA_INACTIVE,0);
				ClipCursor(NULL),
				hTimer = SetTimer(hWnd, 1, 100, NULL),
				SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED),
				SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
		}
		DrawButtons(hWnd);
		if(isGameProcess ||
				(
					(GetActiveWindow() != hWnd) && !(GetKeyState(VK_NUMLOCK) & 1) &&
					(	(dwRemote & MASK_5)
					||	(dwRemote & (MASK_E1 | MASK_W1)) == (MASK_E1 | MASK_W1)
					||	(dwRemote & (MASK_N1 | MASK_S1)) == (MASK_N1 | MASK_S1)
					)
				)
			)
			dwRemote &= ~MASK_5,
			DrawButtons(NULL);
		if(ptVector[0].x)
			dwRemote &= abs(ptVector[0].x) <= ptHold.x ? ~(MASK_E1 | MASK_W1) : ~NULL,
			ptVector[0].x += ptVector[0].x > 0 ? -1 : +1;
		if(ptVector[0].y)
			dwRemote &= abs(ptVector[0].y) <= ptHold.y ? ~(MASK_S1 | MASK_N1) : ~NULL,
			ptVector[0].y += ptVector[0].y > 0 ? -1 : +1;
		if(ptVector[1].x)
			dwRemote &= abs(ptVector[1].x) <= ptHold.x ? ~(MASK_E2 | MASK_W2) : ~NULL,
			ptVector[1].x += ptVector[1].x > 0 ? -1 : +1;
		if(ptVector[1].y)
			dwRemote &= abs(ptVector[1].y) <= ptHold.y ? ~(MASK_S2 | MASK_N2) : ~NULL,
			ptVector[1].y += ptVector[1].y > 0 ? -1 : +1;
		wsprintf(szText, "%08X", dwRemote);
		UpdateSBar(hWndSBar, szText, 2, 0);
//		OnRawInput(false, HRAWINPUT(tRIDList[3].hDevice));
		i = 0;
		hdc = GetDC(hWnd);
		SetRect(&rt, pfpt->position.x * 20, pfpt->position.y * 20, (pfpt->position.x + pfpt->size.cx) * 20 - 1, (pfpt->position.y + pfpt->size.cy) * 20 - 1);
		while(aRIDevices[i].handle) {
			wsprintf(pfpt->name, "%d#%s", aRIDevices[i].index, (pszModes[isaMouse[aRIDevices[i].index]]), aRIDevices[i].index, aRIDevices[i].index);
			++ pfpt;
//			OffsetRect(&rt, 0, pfpt->size.cy * 20);
//			DrawStatusText(hdc, &rt, sz, SBT_POPOUT);
			++ i;
		}
		ReleaseDC(hWnd, hdc);
		if(szNote[strlen(szNote) - 1] == '|' || szNote[strlen(szNote) - 1] == '\t')
			szNote[strlen(szNote) - 1] ^= 0x7C ^ 0x09;
		SendMessage(hWndSBar, SB_SETTEXT, 1, LPARAM(szNote));
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if(idxVK >= 0) {
			if(idxVK > 12) {
				isaMouse[idxVK - 12] = (isaMouse[idxVK - 12] + 1) % (sizeof pszModes / sizeof *pszModes);
			} else {
				i = ListView_GetNextItem(hListView, -1, LVNI_FOCUSED);
				ListView_Refresh(hListView, i, idxVK);
			}
			break;
			GetCursorPos(&ptCursor);
			idxVKalias = idxVK;
			TrackPopupMenu(hMenu, TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, 0, hWnd, NULL);
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		if(idxVK >= 0) {
			if(idxVK > 12) {
				isaMouse[idxVK - 12] = 3;
			} else {
				i = ListView_GetNextItem(hListView, -1, LVNI_FOCUSED);
				ListView_Refresh(hListView, i, idxVK + 13);
			}
			break;
			GetCursorPos(&ptCursor);
			idxVKalias = idxVK;
			TrackPopupMenu(hMenu, TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, 0, hWnd, NULL);
		}
		break;
	case WM_MENUSELECT:
		wpId = short(wParam);
		hMnu = HMENU(lParam);
		ZeroMemory(&mmi, sizeof(MENUITEMINFO));
		mmi.cbSize = sizeof(MENUITEMINFO);
		mmi.dwTypeData  = szText;
		mmi.cch = sizeof(szText);
		mmi.fMask = MIIM_TYPE | MIIM_CHECKMARKS | MIIM_DATA | MIIM_ID | MIIM_STATE | MIIM_SUBMENU;
		if(GetMenuItemInfo(hMnu, wpId, TRUE, &mmi) && (mmi.fMask & MIIM_SUBMENU)) {
			hMnu = GetSubMenu(HMENU(lParam), wpId);
			RenameMenuItem(hMnu, IDM_PROFILE_0, IDM_PROFILE_9, IDM_PROFILE_SET_0, IDM_PROFILE_SET_9);
		} else
		if(wpId >= IDM_PROFILE_0 && wpId <= IDM_PROFILE_9) {
			GetProfileDescription(szText, wpId - IDM_PROFILE_0),
			UpdateSBar(hWndSBar, szText, 0, 0);
		}
		break;
	case WM_NOTIFY: {
		NMHDR* pnmhdr = (NMHDR *)lParam;
		switch(pnmhdr->idFrom) {
		case IDC_BUTTONS:
			LV_DISPINFO* lv_dispInfo = (LV_DISPINFO*) lParam;
			if(lv_dispInfo->hdr.code == LVN_KEYDOWN) {
				LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN) lParam;
				wpId = pnkd->wVKey;
				i = ListView_GetNextItem(hListView, -1, LVNI_FOCUSED);//LVNI_SELECTED);
				if(wpId == VK_NUMLOCK) {
					GetCursorPos(&pt);
					tjalias[i].keycode = 0x80008000 | (pt.y << 16) | pt.x;
					wsprintf(sz, "%d,%d", pt.x, pt.y);
					ListView_SetItemText(hListView, i, 0, TEXT(sz));
				} else {
					tjalias[i].keycode = wpId;
					ListView_SetItemText(hListView, i, 0, TEXT(tszVKId[tjalias[i].keycode]));
				}
				ListView_EditLabel(hListView, 0);
				SetFocus(hWnd);
			} else
			if(lv_dispInfo->hdr.code == NM_CLICK || lv_dispInfo->hdr.code == NM_DBLCLK) {
				i = ListView_GetNextItem(hListView, -1, LVNI_FOCUSED);
				ListView_Refresh(hListView, i, lv_dispInfo->item.iItem - 1);
				break;
			}
			break;
		}
		}
		break;
	case WM_COMMAND:
		wpId    = LOWORD(wParam); 
		wpEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch(wpId) {
		case IDM_BUTTONS+0:
		case IDM_BUTTONS+1:
		case IDM_BUTTONS+2:
		case IDM_BUTTONS+3:
		case IDM_BUTTONS+4:
		case IDM_BUTTONS+5:
		case IDM_BUTTONS+6:
		case IDM_BUTTONS+7:
		case IDM_BUTTONS+8:
		case IDM_BUTTONS+9:
		case IDM_BUTTONS+10:
		case IDM_BUTTONS+11:
		case IDM_BUTTONS+12:
		case IDM_BUTTONS+13:
		case IDM_BUTTONS+14:
		case IDM_BUTTONS+15:
//			tfpButton[idxVKalias].alias = wpId - IDM_BUTTONS;
			break;
		case IDM_PROFILE_SET_0:
		case IDM_PROFILE_SET_1:
		case IDM_PROFILE_SET_2:
		case IDM_PROFILE_SET_3:
		case IDM_PROFILE_SET_4:
		case IDM_PROFILE_SET_5:
		case IDM_PROFILE_SET_6:
		case IDM_PROFILE_SET_7:
		case IDM_PROFILE_SET_8:
		case IDM_PROFILE_SET_9:
			PresetSave(hWnd, wpId - IDM_PROFILE_SET_0);
			break;
		case IDM_PROFILE_DESC:
			strcpy(szDescription, szRename);
			UpdateSBar(hWndSBar, szDescription, 0, 0);
			break;
		case IDM_PROFILE_0:
		case IDM_PROFILE_1:
		case IDM_PROFILE_2:
		case IDM_PROFILE_3:
		case IDM_PROFILE_4:
		case IDM_PROFILE_5:
		case IDM_PROFILE_6:
		case IDM_PROFILE_7:
		case IDM_PROFILE_8:
		case IDM_PROFILE_9:
			PresetLoad(hWnd, wpId - IDM_PROFILE_0);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, LPCTSTR(IDD_ABOUTBOX), hWnd, DLGPROC(About));
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		GetClientRect(hWnd, &rt);
		dwRemoters = 0;
		DrawJoystick(hWnd);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		SendMessage(hWndSBar, message, wParam, lParam),
		InitializeStatusBar(hWndSBar, 1);
		break;
	case WM_KEYDOWN:
		wpId = short(wParam);
		if(idxVK >= 0)
			switch(wpId) {
			case VK_LEFT:
				if(GetKeyState(VK_CONTROL) < 0)
					tfpButton[idxVK].size.cx -= tfpButton[idxVK].size.cx > 2 ? 1 : 0;
				else
					tfpButton[idxVK].position.x -= tfpButton[idxVK].position.x > 0 ? 1 : 0;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case VK_RIGHT:
				if(GetKeyState(VK_CONTROL) < 0)
					tfpButton[idxVK].size.cx += tfpButton[idxVK].position.x + tfpButton[idxVK].size.cx < 16 ? 1 : 0;
				else
					tfpButton[idxVK].position.x += tfpButton[idxVK].position.x + tfpButton[idxVK].size.cx < 16 ? 1 : 0;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case VK_UP:
				if(GetKeyState(VK_CONTROL) < 0)
					tfpButton[idxVK].size.cy -= tfpButton[idxVK].size.cy > 2 ? 1 : 0;
				else
					tfpButton[idxVK].position.y -= tfpButton[idxVK].position.y > 0 ? 1 : 0;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			case VK_DOWN:
				if(GetKeyState(VK_CONTROL) < 0)
					tfpButton[idxVK].size.cy += tfpButton[idxVK].position.y + tfpButton[idxVK].size.cy < 24 ? 1 : 0;
				else
					tfpButton[idxVK].position.y += tfpButton[idxVK].position.y + tfpButton[idxVK].size.cy < 24 ? 1 : 0;
				InvalidateRect(hWnd, NULL, TRUE);
				break;
			}
		else
		if(wpId == VK_APPS)
			isaMouse[idxMouse] = (isaMouse[idxMouse] + 1) % (sizeof pszModes / sizeof *pszModes);
		break;
	case WM_CHAR:
		wpId = short(wParam);
		ConsoleInput(wpId);
		break;
		switch(wpId) {
		case VK_BACK:
			i = strlen(szRename);
			if(i) {
				szRename[i - 1] = NULL;
				UpdateSBar(hWndSBar, szRename, 1, 0);
			}
			break;
		case VK_RETURN:
			if(strlen(szRename))
				strcpy(szProfile, szRename);
			wsprintf(szText, "%s: %s", szTitle, szProfile);
			SetWindowText(hWnd, szText);
			break;
		case ' ': case '!': case '#': case '&': case '(': case ')': case '*': case '+': case ',': case '-': case '.': case '?':
			if(idxVK > 12) {
				isaMouse[idxVK - 12] = (isaMouse[idxVK - 12] + 1) % 3;
			} else {
				i = strlen(szRename);
				if(i + 1 < sizeof szRename && i > 0) {
					if((szRename[i - 1] >= '0' && szRename[i - 1] <= '9') || (szRename[i - 1] >= 'A' && szRename[i - 1] <= 'Z') || (szRename[i - 1] >= 'a' && szRename[i - 1] <= 'z')) {
						szRename[i + 1] = NULL;
						szRename[i] = wpId;
						UpdateSBar(hWndSBar, szRename, 1, 0);
						break;
					}
				}
			}
			break;
		default:
			i = strlen(szRename);
			if(i + 1 < sizeof szRename && ((wpId >= '0' && wpId <= '9') || (wpId >= 'A' && wpId <= 'Z') || (wpId >= 'a' && wpId <= 'z'))) {
				szRename[i + 1] = NULL;
				szRename[i] = wpId;
				UpdateSBar(hWndSBar, szRename, 1, 0);
			}
			break;
		}
		break;
	case WM_INPUT:
		if(GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT) {
			//OnRawInput(false, HRAWINPUT(tRIDList[1].hDevice));
			OnRawInput(message, wParam, lParam, true);
			//OnRawInput_1(true, HRAWINPUT(lParam));
		} else
		if(GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUTSINK) {
			//OnRawInput(false, HRAWINPUT(tRIDList[1].hDevice));
			OnRawInput(message, wParam, lParam, false);
			//OnRawInput_1(false, HRAWINPUT(lParam));
		}
		break;
	case WM_DESTROY:
		UnhookWindowsHookEx(hMouseHook);
//		if(GetKeyState(VK_SCROLL) & 1)
//			GlobalHooker(false);
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		if(hWndSBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER | SBARS_SIZEGRIP, "Ready", hWnd, IDC_STATUSBAR))
			InitializeStatusBar(hWnd, 1);
		GetRawInputDeviceList(NULL, &nRawDevs, sizeof RAWINPUTDEVICELIST);
		if(nRawDevs) {
			GetRawInputDeviceList(tRIDList, &nRawDevs, sizeof RAWINPUTDEVICELIST);
		}
		wsprintf(szNote, "TYPE DESCRIPTI0N AT HERE...");
		memset(&mii, NULL, sizeof mii);
		mii.cbSize = sizeof mii;
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID;
		mii.fType = MFT_STRING;
		mii.fState = MFS_ENABLED;
		mii.cch = 2;
		mii.dwTypeData = szText;
		hMnu = GetMenu(hWnd);
		hMnu = GetSubMenu(hMnu, 1);
		mii.wID = IDM_DEVICES;
		rdi.cbSize = sizeof RID_DEVICE_INFO;
		DeleteMenu(hMnu, 0, 0);
		for(i = 0; i < nRawDevs; ++ i) {
		UINT	cbSize = rdi.cbSize;
			size = sizeof szText;
			GetRawInputDeviceInfo(tRIDList[i].hDevice, RIDI_DEVICEINFO, &rdi, &cbSize);
			if(tRIDList[i].dwType != RIM_TYPEMOUSE)
				continue;
			cbSize = sizeof szText;
			GetRawInputDeviceInfo(tRIDList[i].hDevice, RIDI_DEVICENAME, &szText, &cbSize);
			j = 0, k = 0;
			while(j < 3 && szText[k])
				j += szText[k ++] == '#' ? 1 : 0;
			//
/*			wsprintf(szKey, "DeviceDisplayObject\\InterfaceClass\\%s\\Shell\\Mouse", &szText[k]);
	RegCreateKeyEx(HKEY_CLASSES_ROOT, szKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_QUERY_VALUE, NULL, &hKey, NULL);
			wsprintf(szKey, "OpenControlPanel", &szText[k]);
	if(RegQueryValueEx(hKey, szKey, NULL, NULL, NULL, &nKey) == ERROR_SUCCESS)
		RegQueryValueEx(hKey, szKey, NULL, &type, PBYTE(szKey), &nKey);
	RegCloseKey(hKey);
			//
*/			wsprintf(sz, "%d: %d %d %s", mii.wID - IDM_DEVICES, rdi.mouse.dwNumberOfButtons, rdi.mouse.fHasHorizontalWheel, &szText[k]);
			AppendMenu(hMnu, 0, mii.wID, sz),
			mii.wID ++;
		}
		tRIDev[0].dwFlags = RIDEV_INPUTSINK; //= RIDEV_DEVNOTIFY | RIDEV_CAPTUREMOUSE;
		tRIDev[0].hwndTarget = hWnd;
		tRIDev[0].usUsage = 2;
		tRIDev[0].usUsagePage = 1;
		RegisterRawInputDevices(tRIDev, 1, sizeof RAWINPUTDEVICE);
		hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, hookMouseProc, hInst, NULL);
		ptHold.x = 188;
		ptHold.y = 188;
		ptRange.x = 192;
		ptRange.y = 192;
		ptVector[0].x = 0;
		ptVector[0].y = 0;
		ptVector[1].x = 0;
		ptVector[1].y = 0;
		ptHold.x /= 8;
		ptHold.y /= 8;
		ptRange.x /= 8;
		ptRange.y /= 8;
		i = sizeof tszVKId / sizeof *tszVKId;
		while(i --)
			wsprintf(&tszVKId[i][0], "%-23c", i);
		i = sizeof tszVK / sizeof *tszVK;
		while(i --)
			wsprintf(tszVKId[tszVK[i].vk], "%-23s", tszVK[i].id + 3);
		hListView = CreateListView(hWnd, 200, 0, IDC_BUTTONS);
		for(i = 0; i < 16; ++ i)
			ListView_Refresh(hListView, i, -1);
		wsprintf(szProfile, "NES-emulator");
		wsprintf(szDescription, "NES-console emulator");
		wsprintf(szText, "%s: %s", szTitle, szProfile);
		SetWindowText(hWnd, szText);
		if(GetKeyState(VK_SCROLL) & 1)
			;//GlobalHooker(true);
		hTimer = SetTimer(hWnd, 1, 100, NULL);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
    return FALSE;
}