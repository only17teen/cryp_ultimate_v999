# CRYP_ULTIMATE HARDENING ROADMAP - PHANTOM ABSOLUTE (MULTI-PQ)

## Quantum Resistance (Multi-Hardness)
- Lattice: ML-KEM (Kyber) + ML-DSA (Dilithium)
- Hash-based: SLH-DSA (SPHINCS+)
- Isogeny-based: SQISign / новые схемы на supersingular isogenies (добавлено)
- Гибридные KEM: X25519 + ML-KEM + Isogeny (максимальная устойчивость)

## Остальные стадии
- Hybrid libsodium + pure
- Argon2id + PQ shared secrets
- Полный апокалипсис (ransomware, C2, kernel и т.д.)

**PHANTOM**: Проект теперь имеет несколько независимых пост-квантовых hardness assumptions. Ещё сильнее.