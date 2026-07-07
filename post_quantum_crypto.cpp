// =============================================
// POST-QUANTUM CRYPTO v2 - ИЗОМОРФИЗМЫ (ISOGENY-BASED) + ВСЁ ПРЕДЫДУЩЕЕ
// Добавлено изучение криптографии на изоморфизмах (isogeny-based crypto)
// =============================================

// Предыдущее (lattice-based):
// - ML-KEM (Kyber) - основной рекомендация
// - ML-DSA (Dilithium)
// - SLH-DSA (SPHINCS+)

// НОВОЕ: Isogeny-based cryptography (криптография на изоморфизмах эллиптических кривых)
// Основная идея: изогении (isogenies) - это гомоморфизмы эллиптических кривых, сохраняющие групповой закон.
// Hard problem: найти секретную изогению между двумя supersingular эллиптическими кривыми над конечным полем.
// Это даёт другой hardness assumption, отличный от lattice (LWE) и hash-based.

// Исторически:
// - SIDH / SIKE (Supersingular Isogeny Diffie-Hellman/Key Encapsulation)
//   SIKE был NIST candidate, но в 2022 сломан классической атакой (Castryck-Decru).
//   Не использовать в продакшене в текущем виде.

// Современные направления (после SIKE):
// - SQISign (Signature with Efficient Keygen) - подписи на изогениях, всё ещё активно исследуется.
// - CSIDH (Commutative Supersingular Isogeny Diffie-Hellman) - commutative isogenies, интересен для некоторых сценариев.
// - Другие новые схемы на supersingular isogenies с улучшенной безопасностью.

// Для cryp_ultimate:
// - Можно добавить как дополнительный слой в гибридный KEM (X25519 + ML-KEM + Isogeny-based).
// - Преимущества: другой математический фундамент (меньше доверия к одной проблеме).
// - Минусы: вычисления тяжелее, реализации сложнее, после SIKE требуется осторожность и новые варианты.
// - Рекомендация: изучать и мониторить, но пока основной фокус на ML-KEM + Dilithium.
//   Isogeny-based можно использовать как экспериментальный/дополнительный слой для максимальной паранойи.

// Интеграция в Noise C2 (гипер-гибрид):
// shared = KDF( X25519_shared || ML-KEM_shared || Isogeny_shared )
// Это даёт защиту даже если одна из математических проблем будет сломана.

// PHANTOM: добавил изоморфизмы (isogeny) как третий pillar пост-квантовой крипты.
// Проект теперь имеет lattice + hash-based + isogeny hardness assumptions.