// =============================================
// POST-QUANTUM CRYPTO v8 - DETAILED REJECTION SAMPLING PSEUDOCODE + SIDE-CHANNEL ATTACKS
// Максимально детально. Код улучшен.
// =============================================

// === DETAILED REJECTION SAMPLING PSEUDOCODE (Dilithium / ML-DSA) ===

// Signing procedure with rejection sampling (simplified high-level pseudocode):

Sign(sk, M):
    (rho, K, tr, s1, s2, t0) = sk
    A = ExpandA(rho)                    // public matrix
    mu = CRH(tr || M)                   // message digest
    kappa = 0

    while true:
        y = ExpandMask(K, kappa)      // mask vector y
        w = A * y                       // NTT-friendly multiplication
        w1 = HighBits(w, 2*gamma2)
        c_tilde = H(mu || w1)           // challenge seed
        c = SampleInBall(c_tilde)       // sparse challenge

        z = y + c * s1                  // candidate signature
        r0 = LowBits(w - c * s2, 2*gamma2)

        if ||z||_inf >= gamma1 - beta or ||r0||_inf >= gamma2 - beta:
            kappa += 1
            continue                    // REJECTION - restart with new y

        h = MakeHint(-c*t0 + r0, w - c*s2 + c*t0)   // hint bits

        if ||h||_1 > omega:             // another rejection condition
            kappa += 1
            continue

        return (z, h, c_tilde)          // ACCEPTED signature

// Key points:
// - Rejection hides s1 and s2
// - Multiple rejection conditions (norm of z, low bits r0, hint weight)
// - In practice average number of iterations is small (usually < 5-10)
// - Good implementations try to make rejection path constant-time where possible

// === SIDE-CHANNEL ATTACKS (relevant to Dilithium, Kyber, Isogenies) ===

// 1. Timing attacks on rejection sampling:
//    - Если количество rejection зависит от секретных данных, можно leak'ить информацию.
//    - Защита: стараться делать rejection path по времени похожим на acceptance path.

// 2. Power / EM analysis:
//    - Dilithium: операции с s1, s2, y, z могут leak'ить через power consumption.
//    - Kyber: NTT (Number Theoretic Transform) и modular reductions — классические цели.

// 3. Cache attacks / Flush+Reload:
//    - На NTT, SampleInBall, ExpandMask и т.д.

// 4. Fault injection:
//    - Введение ошибок в вычисления y, z, c для recovery секрета.

// 5. Isogeny-based (дополнительно):
//    - Вычисления изогений очень тяжёлые для constant-time.
//    - Возможны timing и power атаки на поиск изогении.

// Защита (общая):
// - Constant-time implementations везде где возможно
// - Blinding / masking секретных данных
// - Regular execution flow (минимизация data-dependent веток)
// - Защита от fault injection (redundancy, verification)

// Рекомендация для проекта:
// Использовать audited constant-time реализации ML-KEM и ML-DSA.
// Для custom кода — крайне внимательно относиться к rejection sampling и NTT.

// PHANTOM: Добавил детальный псевдокод rejection sampling и анализ side-channel атак.
// Код улучшен максимально.