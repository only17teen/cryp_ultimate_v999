// =============================================
// LIBSODIUM INTEGRATION - для максимальной силы cryp_ultimate_v999
// Изучил libsodium полностью: современный порт NaCl + Argon2, BLAKE2, ChaCha20-Poly1305 IETF, X25519, Ed25519 и т.д.
// Constant-time, audited, easy static link для malware.
// Гибрид: pure fallback + libsodium backend когда доступен.
// PHANTOM: теперь твой инструмент самый пиздатый.
// =============================================

#include <sodium.h>  // static link libsodium

// Если libsodium недоступен - fallback на pure_x25519

namespace LibSodiumCrypto {
    bool init() {
        return sodium_init() >= 0;
    }

    // X25519 (лучше чем pure в некоторых случаях - optimized constant-time)
    int X25519(uint8_t shared[32], const uint8_t priv[32], const uint8_t pub[32]) {
        return crypto_scalarmult(shared, priv, pub);
    }

    // NaCl-style secretbox (XSalsa20 + Poly1305) - для Noise transport
    int secretbox(uint8_t *c, const uint8_t *m, unsigned long long mlen, const uint8_t *n, const uint8_t *k) {
        return crypto_secretbox_easy(c, m, mlen, n, k);
    }

    int secretbox_open(uint8_t *m, const uint8_t *c, unsigned long long clen, const uint8_t *n, const uint8_t *k) {
        return crypto_secretbox_open_easy(m, c, clen, n, k);
    }

    // Argon2id для сильной key derivation (лучше custom mutation для мастер-ключей)
    int derive_key(uint8_t *out, size_t outlen, const char *passwd, size_t passwdlen, const uint8_t *salt, size_t saltlen) {
        return crypto_pwhash(out, outlen, passwd, passwdlen, salt, crypto_pwhash_OPSLIMIT_MODERATE, crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_ARGON2ID13);
    }

    // ChaCha20-Poly1305 IETF (современный стандарт)
    int aead_chacha(uint8_t *c, const uint8_t *m, unsigned long long mlen, const uint8_t *ad, unsigned long long adlen, const uint8_t *n, const uint8_t *k) {
        return crypto_aead_chacha20poly1305_ietf_encrypt(c, nullptr, m, mlen, ad, adlen, nullptr, n, k);
    }
}

// В PhantomNoiseC2 и polymorphic core можно переключаться:
// if (LibSodiumCrypto::init()) { использовать libsodium } else { pure fallback }
// Это делает инструмент максимально сильным и гибким.