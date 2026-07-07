// =============================================
// POST-QUANTUM CRYPTO v9 - LOW-LEVEL PSEUDOCODE (NTT + Constant-time Rejection) + TIMING ATTACKS ON NTT
// Максимально низкоуровнево. Усилено.
// =============================================

// === LOW-LEVEL NTT PSEUDOCODE (Dilithium / Kyber style) ===

// NTT (Number Theoretic Transform) - для быстрого умножения полиномов
// Используется Cooley-Tukey подход с butterfly операциями.

NTT(a):                          // a - массив из 256 коэффициентов
    for layer = 0 to 7:          // 256 = 2^8
        len = 2^layer
        for start = 0 to 255 step 2*len:
            zeta = twiddle_factors[...]   // предвычисленные корни
            for j = start to start + len - 1:
                t = zeta * a[j + len] mod q
                a[j + len] = a[j] - t mod q
                a[j]       = a[j] + t mod q
    return a

// Inverse NTT (INTT) аналогично, с другим zeta и scaling.

// В Dilithium/Kyber NTT используется для:
// - Умножения A * y
// - Умножения c * s1 / s2
// - И т.д.

// === CONSTANT-TIME REJECTION SAMPLING (усиленная версия) ===

// Проблема: обычный rejection имеет data-dependent количество итераций.
// Решение (constant-time подход):

ConstantTimeRejection(z, r0, h):
    // Всегда выполняем фиксированное количество итераций (максимально возможное)
    // или используем conditional move / masking вместо веток

    reject_z   = (norm_inf(z) >= gamma1 - beta)
    reject_r0  = (norm_inf(r0) >= gamma2 - beta)
    reject_h   = (weight(h) > omega)

    // Используем bitwise операции вместо if
    final_reject = reject_z | reject_r0 | reject_h

    // Если final_reject == true, отбрасываем, но делаем это "постоянно по времени"
    // В реальности часто используют технику "always perform max iterations"
    // или masking на выходные значения.

// В хороших реализациях стараются минимизировать утечки через timing rejection sampling.

// === TIMING ATTACKS НА NTT ===

// 1. Cache timing attacks:
//    - Twiddle factors (zeta) хранятся в памяти.
//    - При доступе к разным zeta возможен cache hit/miss, leak'ящий информацию.
//    - Flush+Reload или Prime+Probe на таблицы twiddle factors.

// 2. Timing variations в modular reduction:
//    - Операции mod q (особенно Barrett или Montgomery reduction)
//      могут иметь разное время в зависимости от входных данных.

// 3. Branch prediction / speculative execution:
//    - Если есть data-dependent ветки внутри NTT butterfly.

// 4. Power/EM analysis на NTT butterfly операциях.

// Защита от timing attacks на NTT:
// - Constant-time modular arithmetic (Montgomery reduction в constant-time)
// - Precompute и хранить twiddle factors в constant-time accessible way
// - Избегать data-dependent веток внутри NTT
// - Использовать защищённые реализации (например, из pqclean, liboqs с CT флагами)

// PHANTOM: Добавил низкоуровневый NTT псевдокод, constant-time rejection sampling
// и детальный разбор timing атак на NTT. Код усилен максимально.