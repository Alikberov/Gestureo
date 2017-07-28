enum RawInputDeviceFlags {
	NONE					= 0x00000000,	// No flags.
	REMOVE					= 0x00000001,	// If set, this removes the top level collection from the inclusion list. This tells the operating system to stop reading from a device which matches the top level collection.
	EXCLUDE					= 0x00000010,	// If set, this specifies the top level collections to exclude when reading a complete usage page. This flag only affects a TLC whose usage page is already specified with PageOnly.
	PAGEONLY				= 0x00000020,	// If set, this specifies all devices whose top level collection is from the specified UsagePage. Note that Usage must be zero. To exclude a particular top level collection, use Exclude.
	NOLEGACY				= 0x00000030,	// If set, this prevents any devices specified by UsagePage or Usage from generating legacy messages. This is only for the mouse and keyboard.
	INPUTSINK				= 0x00000100,	// If set, this enables the caller to receive the input even when the caller is not in the foreground. Note that WindowHandle must be specified.
	CAPTUREMOUSE			= 0x00000200,	// If set, the mouse button click does not activate the other window.
	NOHOTKEYS				= 0x00000200,	// If set, the application-defined keyboard device hotkeys are not handled. However, the system hotkeys; for example, ALT+TAB and CTRL+ALT+DEL, are still handled. By default, all keyboard hotkeys are handled. NoHotKeys can be specified even if NoLegacy is not specified and WindowHandle is NULL.
	APPKEYS					= 0x00000400,	// If set, application keys are handled.  NoLegacy must be specified.  Keyboard only.
	EXINPUTSINK				= 0x00001000,	// If set, this enables the caller to receive input in the background only if the foreground application does not process it. In other words, if the foreground application is not registered for raw input, then the background application that is registered will receive the input.
	DEVNOTIFY				= 0x00002000
};
enum HidUsagePage/*:ushort*/ {
	UNDEFINED				= 0x00,			// Unknown usage page.
	GENERIC					= 0x01,			// Generic desktop controls.
	SIMULATION				= 0x02,			// Simulation controls.
	VR						= 0x03,			// Virtual reality controls.
	SPORT					= 0x04,			// Sports controls.
	GAME					= 0x05,			// Games controls.
	KEYBOARD				= 0x07,			// Keyboard controls.
};
enum HidUsage/*:ushort*/ {
	Undefined				= 0x00,			// Unknown usage.
	Pointer					= 0x01,			// Pointer
	Mouse					= 0x02,			// Mouse
	Joystick				= 0x04,			// Joystick
	Gamepad					= 0x05,			// Game Pad
	Keyboard				= 0x06,			// Keyboard
	Keypad					= 0x07,			// Keypad
	SystemControl			= 0x80,			// Muilt-axis Controller
	Tablet					= 0x80,			// Tablet PC controls
	Consumer				= 0x0C,			// Consumer
};
#define	RIDI_DEVICENAME		0x20000007
#define	RIDI_DEVICEINFO		0x2000000b
#define	RIDI_PREPARSEDDATA	0x20000005

#define	RIDEV_APPKEYS		0x00000400
#define	RIDEV_CAPTUREMOUSE	0x00000200
#define	RIDEV_DEVNOTIFY		0x00002000
#define	RIDEV_EXCLUDE		0x00000010
#define	RIDEV_EXINPUTSINK	0x00001000
#define	RIDEV_INPUTSINK		0x00000100
#define	RIDEV_NOHOTKEYS		0x00000200
#define	RIDEV_NOLEGACY		0x00000030
#define	RIDEV_PAGEONLY		0x00000020
#define	RIDEV_REMOVE		0x00000001

#define	RIM_INPUT			0
#define	RIM_INPUTSINK		1
#define	RIM_TYPEMOUSE		0
#define	RIM_TYPEKEYBOARD	1
#define	RIM_TYPEHID			2
#define	RID_INPUT			0x10000003

#define	HRAWINPUT			HANDLE
#define	HRAWIN				HANDLE

#define RI_MOUSE_LEFT_BUTTON_DOWN   0x0001
#define RI_MOUSE_LEFT_BUTTON_UP     0x0002
#define RI_MOUSE_RIGHT_BUTTON_DOWN  0x0004
#define RI_MOUSE_RIGHT_BUTTON_UP    0x0008
#define RI_MOUSE_MIDDLE_BUTTON_DOWN 0x0010
#define RI_MOUSE_MIDDLE_BUTTON_UP   0x0020

#define RI_MOUSE_BUTTON_1_DOWN RI_MOUSE_LEFT_BUTTON_DOWN
#define RI_MOUSE_BUTTON_1_UP   RI_MOUSE_LEFT_BUTTON_UP
#define RI_MOUSE_BUTTON_2_DOWN RI_MOUSE_RIGHT_BUTTON_DOWN
#define RI_MOUSE_BUTTON_2_UP   RI_MOUSE_RIGHT_BUTTON_UP
#define RI_MOUSE_BUTTON_3_DOWN RI_MOUSE_MIDDLE_BUTTON_DOWN
#define RI_MOUSE_BUTTON_3_UP   RI_MOUSE_MIDDLE_BUTTON_UP

#define RI_MOUSE_BUTTON_4_DOWN    0x0040
#define RI_MOUSE_BUTTON_4_UP      0x0080
#define RI_MOUSE_BUTTON_5_DOWN    0x0100
#define RI_MOUSE_BUTTON_5_UP      0x0200
#define RI_MOUSE_WHEEL            0x0400
#define RI_MOUSE_HORIZONTAL_WHEEL 0x0800

#define MOUSE_MOVE_RELATIVE      0x00
#define MOUSE_MOVE_ABSOLUTE      0x01
#define MOUSE_VIRTUAL_DESKTOP    0x02
#define MOUSE_ATTRIBUTES_CHANGED 0x04
#define MOUSE_MOVE_NOCOALESCE    0x08

#define MOUSEEVENTF_XDOWN       0x0080
#define MOUSEEVENTF_XUP         0x0100
#define MOUSEEVENTF_VIRTUALDESK     0x4000

#define	WM_INPUT			0x000000FF
#define	GET_RAWINPUT_CODE_WPARAM(wParam)	short(wParam)

typedef struct tagRAWINPUTDEVICE {
	USHORT			usUsagePage;
	USHORT			usUsage;
	DWORD			dwFlags;
	HWND			hwndTarget;
} RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;

typedef struct tagRAWINPUTDEVICELIST {
	HANDLE			hDevice;
	DWORD			dwType;
} RAWINPUTDEVICELIST, *PRAWINPUTDEVICELIST;

typedef struct tagRAWINPUTHEADER {
	DWORD			dwType;
	DWORD			dwSize;
	HANDLE			hDevice;
	WPARAM			wParam;
} RAWINPUTHEADER, *PRAWINPUTHEADER;

typedef struct tagRAWMOUSE {
	USHORT			usFlags;
	union {
		ULONG		ulButtons;
		struct {
			USHORT	usButtonFlags;
			USHORT	usButtonData;
		};
	};
	ULONG			ulRawButtons;
	LONG			lLastX;
	LONG			lLastY;
	ULONG			ulExtraInformation;
} RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

typedef struct tagRAWKEYBOARD {
	USHORT			MakeCode;
	USHORT			Flags;
	USHORT			Reserved;
	USHORT			VKey;
	UINT			Message;
	ULONG			ExtraInformation;
} RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;

typedef struct tagRAWHID {
	DWORD			dwSizeHid;
	DWORD			dwCount;
	BYTE			bRawData[1];
} RAWHID, *PRAWHID, *LPRAWHID;

typedef struct tagRAWINPUT {
	RAWINPUTHEADER	header;
	union {
		RAWMOUSE	mouse;
		RAWKEYBOARD	keyboard;
		RAWHID		hid;
	} data;
} RAWINPUT, *PRAWINPUT, *LPRAWINPUT;

typedef struct tagRID_DEVICE_INFO_HID {
	DWORD			dwVendorId;
	DWORD			dwProductId;
	DWORD			dwVersionNumber;
	USHORT			usUsagePage;
	USHORT			usUsage;
} RID_DEVICE_INFO_HID, *PRID_DEVICE_INFO_HID;

typedef struct tagRID_DEVICE_INFO_KEYBOARD {
	DWORD			dwType;
	DWORD			dwSubType;
	DWORD			dwKeyboardMode;
	DWORD			dwNumberOfFunctionKeys;
	DWORD			dwNumberOfIndicators;
	DWORD			dwNumberOfKeysTotal;
} RID_DEVICE_INFO_KEYBOARD, *PRID_DEVICE_INFO_KEYBOARD;

typedef struct tagRID_DEVICE_INFO_MOUSE {
	DWORD			dwId;
	DWORD			dwNumberOfButtons;
	DWORD			dwSampleRate;
	BOOL			fHasHorizontalWheel;
} RID_DEVICE_INFO_MOUSE, *PRID_DEVICE_INFO_MOUSE;
typedef struct tagRID_DEVICE_INFO {
	DWORD			cbSize;
	DWORD			dwType;
	union {
		RID_DEVICE_INFO_MOUSE		mouse;
		RID_DEVICE_INFO_KEYBOARD	keyboard;
		RID_DEVICE_INFO_HID			hid;
	};
} RID_DEVICE_INFO, *PRID_DEVICE_INFO, *LPRID_DEVICE_INFO;

static union {
	struct {
		BOOL	(WINAPI * SetLayeredWindowAttributes)	(HWND hWnd, COLORREF, BYTE, DWORD);
		UINT	(WINAPI * GetRawInputDeviceInfo)		(HANDLE hDevice, UINT uiCommand, LPVOID pData, PUINT pcbSize);
		UINT	(WINAPI * GetRawInputBuffer)			(HRAWINPUT pData, PUINT pcbSize, UINT cbSizeHeader);
		UINT	(WINAPI * GetRawInputData)				(HRAWINPUT hRawInput, UINT uiCommand, LPVOID pData, PUINT pcbSize, UINT cbSizeHeader);
		UINT	(WINAPI * GetRawInputDeviceList)		(PRAWINPUTDEVICELIST pRawInputDeviceList, PUINT puiNumDevices, UINT cbSize);
		UINT	(WINAPI * GetRegisteredRawInputDevices)	(PRAWINPUTDEVICE pRawInputDevices, PUINT puiNumDevices, UINT cbSize);
		LRESULT (WINAPI * DefRawInputProc)				(HRAWINPUT *paRawInput, INT nInput, UINT cbSizeHeader);
		BOOL	(WINAPI * RegisterRawInputDevices)		(PRAWINPUTDEVICE pRawInputDevices, UINT uiNumDevices, UINT cbSize);
	};
	struct {
		FARPROC	user32_proc[1];
	};
};
struct {
	PTCHAR	name;
	union {
		PVOID	proc;
		FARPROC *	user32_proc;
	};
} User32_Proc[] = {
	"SetLayeredWindowAttributes"	,&SetLayeredWindowAttributes,
	"GetRawInputDeviceInfoA"		,&GetRawInputDeviceInfo,
	"GetRawInputBuffer"				,&GetRawInputBuffer,
	"GetRawInputData"				,&GetRawInputData,
	"GetRawInputDeviceList"			,&GetRawInputDeviceList,
	"GetRegisteredRawInputDevices"	,&GetRegisteredRawInputDevices,
	"DefRawInputProc"				,&DefRawInputProc,
	"RegisterRawInputDevices"		,&RegisterRawInputDevices,
	NULL
};

DWORD	User32_DLL(void) {
	HINSTANCE static	hUser32	= NULL;
	DWORD				errorFlags = 0;
	int					i = 0;
	if(hUser32)
		FreeLibrary(hUser32); // release the tune module handle
	else
	if(hUser32 = GetModuleHandle(_T("USER32.DLL")))
		while(User32_Proc[i].name)	// Get the module function
			errorFlags |= !(User32_Proc[i].user32_proc[0] = GetProcAddress(hUser32, User32_Proc[i].name))		? 1 << i : NULL, ++ i;
	else
		return GetLastError();
	return errorFlags;
}

#define	WS_EX_LAYERED		0x00080000
#define	LWA_ALPHA			0x00000002
#define	LWA_COLORKEY		0x00000001