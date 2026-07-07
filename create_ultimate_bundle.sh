#!/bin/bash
# =============================================
# create_ultimate_bundle.sh
# Создаёт удобный ZIP со всеми важными файлами cryp_ultimate_v999
# Просто запусти этот скрипт в папке с репозиторием
# =============================================

set -e

BUNDLE_NAME="cryp_ultimate_v999_COMPLETE_$(date +%Y%m%d_%H%M%S)"

echo "[*] Creating ultimate bundle: $BUNDLE_NAME.zip"

mkdir -p "$BUNDLE_NAME"

# Основной артефакт
cp cryp_ultimate_v999_ultimate_single.cpp "$BUNDLE_NAME/"

# Модульные файлы (если существуют)
cp pure_x25519.cpp "$BUNDLE_NAME/" 2>/dev/null || true
cp phantom_noise_c2.cpp "$BUNDLE_NAME/" 2>/dev/null || true
cp post_quantum_crypto.cpp "$BUNDLE_NAME/" 2>/dev/null || true
cp libsodium_integration.cpp "$BUNDLE_NAME/" 2>/dev/null || true

# Документация
cp HARDENING_ROADMAP.md "$BUNDLE_NAME/" 2>/dev/null || true
cp README.md "$BUNDLE_NAME/" 2>/dev/null || true

# Создаём README для бандла
cat > "$BUNDLE_NAME/README_BUNDLE.txt" << 'EOF'
CRYP_ULTIMATE_v999 - COMPLETE BUNDLE
=====================================

Главный файл: cryp_ultimate_v999_ultimate_single.cpp
Это один цельный файл, который содержит максимум всего наработанного.

Дополнительно:
- pure_x25519.cpp
- phantom_noise_c2.cpp
- post_quantum_crypto.cpp (с детальным разбором PQ, rejection sampling, NTT, side-channel)
- libsodium_integration.cpp
- HARDENING_ROADMAP.md

Как использовать:
1. Возьми cryp_ultimate_v999_ultimate_single.cpp как основу
2. Или используй модульные файлы для более гибкой сборки
3. В single-file уже есть low-level NTT, constant-time rejection sampling,
   side-channel awareness, гипер-гибридные конструкции и т.д.

PHANTOM сделал максимально сильную и полную версию.
EOF

# Создаём ZIP
zip -r "${BUNDLE_NAME}.zip" "$BUNDLE_NAME"

rm -rf "$BUNDLE_NAME"

echo "[+] Bundle created: ${BUNDLE_NAME}.zip"
echo "[+] Просто скопируй этот ZIP куда нужно."