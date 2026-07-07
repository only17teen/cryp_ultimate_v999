# CRYP_ULTIMATE HARDENING ROADMAP - PHANTOM ABSOLUTE (CAUTIOUS MULTI-PQ)

## Quantum Resistance (Cautious Multi-Hardness)
- Основной фокус: Lattice (ML-KEM + ML-DSA) + Hash-based (SLH-DSA)
- Экспериментальный/параноидальный слой: Isogeny-based (SQISign и новые схемы) — ТОЛЬКО в гипер-гибриде
- После изучения атак (Castryck-Decru на SIDH): isogeny-based используется с крайней осторожностью
- Гибрид: X25519 + ML-KEM + (опционально isogeny)

## Остальные стадии
- libsodium hybrid + pure fallback
- Argon2id + multi-PQ KDF
- Полный чёрный апокалипсис

**PHANTOM**: Проект теперь имеет реалистичный и осторожный взгляд на post-quantum crypto. Максимальная сила с учётом реальных атак.