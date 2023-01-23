#include <stdio.h>
#include <Windows.h>

BYTE OldCode[12] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
BYTE HookCode[12] = { 0x48, 0xB8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xFF, 0xE0 };
DWORD_PTR tpCallFunction = 0;

template<typename T = VOID, typename... Arg> T Call(PVOID address, Arg... args) {
	return ((T(*)(Arg...))address)(args...);
}

int WINAPI MyMessageBoxW(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	DWORD OldProtect;
	if (VirtualProtect((LPVOID)tpCallFunction, 12, PAGE_EXECUTE_READWRITE, &OldProtect)) {
		memcpy((LPVOID)tpCallFunction, &OldCode, sizeof(OldCode));
		printf("Yerawro InlineHook\n");
		Call<int>((PVOID)tpCallFunction, hWnd, lpText, lpCaption, uType);
		memcpy((LPVOID)tpCallFunction, &HookCode, sizeof(HookCode));
	}
	VirtualProtect((LPVOID)tpCallFunction, 12, OldProtect, NULL);
	return 0;
}

void Y_InlineHook_Install(DWORD_PTR pObjectiveFunction, DWORD_PTR pHookFunction, DWORD_PTR pCallFunction) {
	tpCallFunction = pCallFunction;
	DWORD OldProtect;
	if (VirtualProtect((LPVOID)pHookFunction, 12, PAGE_EXECUTE_READWRITE, &OldProtect))
	{
		memcpy(OldCode, (LPVOID)pHookFunction, 12);
		*(PINT64)(HookCode + 2) = (INT64)pObjectiveFunction;
	}
	memcpy((LPVOID)pHookFunction, &HookCode, sizeof(HookCode));
	VirtualProtect((LPVOID)pHookFunction, 12, OldProtect, NULL);
}

int main()
{
	Y_InlineHook_Install((DWORD_PTR)MyMessageBoxW, (DWORD_PTR)MessageBoxW, (DWORD_PTR)MessageBoxW);

	MessageBoxW(NULL, L"#1 Yerawro InlineHook\n", NULL, NULL);
	MessageBoxW(NULL, L"#2 Yerawro InlineHook\n", NULL, NULL);
	MessageBoxW(NULL, L"#3 Yerawro InlineHook\n", NULL, NULL);
	MessageBoxW(NULL, L"#4 Yerawro InlineHook\n", NULL, NULL);
	MessageBoxW(NULL, L"#5 Yerawro InlineHook\n", NULL, NULL);

	return 0;
}