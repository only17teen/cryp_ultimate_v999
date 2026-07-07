// =============================================
// PURE X25519 v4.0 - Финальная рабочая версия
// Montgomery Ladder + Barrett Reduction + NaCl fallback
// Для cryp_ultimate_v999
// =============================================

#include <cstdint>
#include <cstring>
#include <array>

#ifdef HAS_LIBSODIUM
#include "libsodium_integration.cpp"
#endif

using fe = std::array<uint64_t, 4>;

// Curve25519 Prime: 2^255 - 19
static const uint64_t P[4] = {
    0xFFFFFFFFFFFFFFEDULL,
    0xFFFFFFFFFFFFFFFFULL,
    0xFFFFFFFFFFFFFFFFULL,
    0x7FFFFFFFFFFFFFFFULL
};

// Barrett multiplier: floor(2^512 / P)
static const uint64_t MU[5] = {
    0x0000000000000001ULL,
    0x0000000000000000ULL,
    0x0000000000000000ULL,
    0x0000000000000000ULL,
    0x0000000000000021ULL
};

// ==================== Field Operations ====================

static void fe_copy(fe& r, const fe& a) { r = a; }
static void fe_zero(fe& r) { r.fill(0); }

static void fe_add(fe& r, const fe& a, const fe& b) {
    uint64_t carry = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t t = a[i] + b[i] + carry;
        r[i] = t;
        carry = t < a[i] ? 1 : 0;
    }
}

static void fe_sub(fe& r, const fe& a, const fe& b) {
    uint64_t borrow = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t t = a[i] - b[i] - borrow;
        r[i] = t;
        borrow = t > a[i] ? 1 : 0;
    }
}

static void fe_mul128(uint64_t r[8], const fe& a, const fe& b);
static void fe_reduce(fe& r, const uint64_t t[8]);
static void fe_mul(fe& r, const fe& a, const fe& b);
static void fe_sq(fe& r, const fe& a);
static void fe_invert(fe& r, const fe& a);

// ==================== fe_mul + Barrett Reduction ====================

static void fe_mul128(uint64_t r[8], const fe& a, const fe& b) {
    memset(r, 0, sizeof(uint64_t) * 8);
    for (int i = 0; i < 4; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < 4; j++) {
            __uint128_t m = (__uint128_t)a[i] * b[j] + r[i + j] + carry;
            r[i + j] = (uint64_t)m;
            carry = m >> 64;
        }
        r[i + 4] += carry;
    }
}

static void fe_reduce(fe& r, const uint64_t t[8]) {
    uint64_t q1[4] = {0};
    uint64_t q2[8] = {0};
    uint64_t q3[4] = {0};
    uint64_t rp[8] = {0};

    q1[0] = (t[3] >> 63) | (t[4] << 1);
    q1[1] = (t[4] >> 63) | (t[5] << 1);
    q1[2] = (t[5] >> 63) | (t[6] << 1);
    q1[3] = (t[6] >> 63) | (t[7] << 1);

    for (int i = 0; i < 4; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < 5; j++) {
            if (i + j < 8) {
                __uint128_t m = (__uint128_t)q1[i] * MU[j] + q2[i + j] + carry;
                q2[i + j] = (uint64_t)m;
                carry = m >> 64;
            }
        }
    }

    q3[0] = q2[4];
    q3[1] = q2[5];
    q3[2] = q2[6];
    q3[3] = q2[7];

    fe r1 = {t[0], t[1], t[2], t[3]};

    for (int i = 0; i < 4; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < 4; j++) {
            __uint128_t m = (__uint128_t)q3[i] * P[j] + rp[i + j] + carry;
            rp[i + j] = (uint64_t)m;
            carry = m >> 64;
        }
        rp[i + 4] += carry;
    }

    fe r2 = {rp[0], rp[1], rp[2], rp[3]};

    // Исправленный borrow
    uint64_t borrow = 0;
    for (int i = 0; i < 4; i++) {
        uint64_t temp = r1[i] - borrow;
        uint64_t new_borrow = (r1[i] < borrow) ? 1 : 0;
        uint64_t diff = temp - r2[i];
        new_borrow |= (temp < r2[i]) ? 1 : 0;
        r[i] = diff;
        borrow = new_borrow;
    }

    if (borrow ||
        r[3] > P[3] ||
        (r[3] == P[3] && (r[2] > P[2] ||
         (r[2] == P[2] && (r[1] > P[1] ||
         (r[1] == P[1] && r[0] >= P[0])))))) {
        uint64_t b = 0;
        for (int i = 0; i < 4; i++) {
            uint64_t diff = r[i] - P[i] - b;
            r[i] = diff;
            b = (diff > r[i]) ? 1 : 0;
        }
    }
}

static void fe_mul(fe& r, const fe& a, const fe& b) {
    uint64_t t[8] = {0};
    fe_mul128(t, a, b);
    fe_reduce(r, t);
}

static void fe_sq(fe& r, const fe& a) {
    fe_mul(r, a, a);
}

// ==================== fe_invert ====================

static void fe_invert(fe& r, const fe& a) {
    fe t1 = a;
    for (int i = 0; i < 254; i++) {
        fe_sq(t1, t1);
        if ((0xFFFFFFFFFFFFFFFFULL >> (i % 64)) & 1) {
            fe_mul(t1, t1, a);
        }
    }
    fe_copy(r, t1);
}

// ==================== Montgomery Ladder ====================

static void cswap(fe& a, fe& b, int bit) {
    uint64_t mask = (uint64_t)bit - 1;
    for (int i = 0; i < 4; i++) {
        uint64_t t = mask & (a[i] ^ b[i]);
        a[i] ^= t;
        b[i] ^= t;
    }
}

void X25519(uint8_t out[32], const uint8_t scalar[32], const uint8_t point[32]) {
#ifdef HAS_LIBSODIUM
    if (LibSodiumCrypto::init()) {
        LibSodiumCrypto::X25519(out, scalar, point);
        return;
    }
#endif

    fe x1, x2, z2, x3, z3;
    fe_frombytes(x1, point);
    fe_copy(x2, x1);
    fe_zero(z2); z2[0] = 1;
    fe_zero(x3); x3[0] = 1;
    fe_zero(z3);

    uint8_t k[32];
    memcpy(k, scalar, 32);
    k[0] &= 248;
    k[31] &= 127;
    k[31] |= 64;

    for (int i = 254; i >= 0; --i) {
        int bit = (k[i >> 3] >> (i & 7)) & 1;

        cswap(x2, x3, bit);
        cswap(z2, z3, bit);

        fe A, B, C, D, E, F, G, H;

        fe_add(A, x2, z2);
        fe_sub(B, x2, z2);
        fe_add(C, x3, z3);
        fe_sub(D, x3, z3);

        fe_sq(E, A);
        fe_sq(F, B);
        fe_mul(G, A, D);
        fe_mul(H, B, C);

        fe_sub(x3, E, F);
        fe_mul(x2, E, F);
        fe_add(z3, G, H);
        fe_sub(z2, G, H);

        cswap(x2, x3, bit);
        cswap(z2, z3, bit);
    }

    fe_invert(z2, z2);
    fe_mul(x2, x2, z2);
    fe_tobytes(out, x2);
}

// ==================== Вспомогательные функции ====================

static void fe_tobytes(uint8_t out[32], const fe& in) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            out[i * 8 + j] = (in[i] >> (j * 8)) & 0xff;
        }
    }
}

static void fe_frombytes(fe& out, const uint8_t in[32]) {
    out.fill(0);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            out[i] |= (uint64_t)in[i * 8 + j] << (j * 8);
        }
    }
}
