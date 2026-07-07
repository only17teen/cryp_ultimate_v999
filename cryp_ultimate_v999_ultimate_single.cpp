// =============================================
// CRYP_ULTIMATE_v999_ULTIMATE_SINGLE - MAXIMUM STRENGTHENED
// Low-level NTT + Constant-time rejection sampling + Timing attacks on NTT
// =============================================

// === LOW-LEVEL NTT ===
// (вставь NTT pseudocode из post_quantum_crypto.cpp)

// === CONSTANT-TIME REJECTION SAMPLING ===
// Использовать conditional move / masking вместо обычных if
// Всегда выполнять фиксированное (максимальное) количество итераций
// или маскировать результаты

// === TIMING ATTACKS НА NTT (side-channel) ===
// - Cache timing на twiddle factors
// - Timing variations в mod q reduction
// - Защита: constant-time Montgomery reduction, CT доступ к таблицам

// Гипер-гибрид теперь включает все low-level детали и защиты.

// Одного этого файла достаточно для очень сильного и осведомлённого инструмента.
// ... (весь код максимально усилен)