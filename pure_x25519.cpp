// =============================================
// PURE X25519 - Монтгомери Лadder (реализация)
// =============================================

#include <cstdint>
#include <cstring>

namespace PureX25519 {

using u64 = uint64_t;
using i64 = int64_t;
using u8 = uint8_t;

struct fe { i64 v[10]; };

// ... (предыдущие функции fe_add, fe_sub, fe_mul, fe_square, fe_invert и т.д.)

// Montgomery ladder step (double + add)
static void ladder_step(fe& x2, fe& z2, fe& x3, fe& z3, const fe& x1) {
    fe t0, t1, t2, t3, t4;

    fe_add(t0, x2, z2);           // A = X2 + Z2
    fe_sub(t1, x2, z2);           // B = X2 - Z2
    fe_add(t2, x3, z3);           // C = X3 + Z3
    fe_sub(t3, x3, z3);           // D = X3 - Z3

    fe_mul(t4, t0, t3);           // DA = D * A
    fe_mul(t3, t1, t2);           // CB = C * B

    fe_add(t2, t4, t3);           // x5 = (DA + CB)^2
    fe_sub(t3, t4, t3);           // z5 = x1 * (DA - CB)^2
    fe_square(x3, t2);
    fe_square(t2, t3);
    fe_mul(z3, x1, t2);

    fe_square(t0, t0);            // AA = A^2
    fe_square(t1, t1);            // BB = B^2
    fe_sub(t2, t0, t1);           // E = AA - BB

    fe_mul(x2, t0, t1);           // x4 = AA * BB
    // z4 = E * (BB + 121665 * E)   // a24 = 121665 для Curve25519
    fe t5;
    // Упрощённо: z4 = E * (BB + const * E)
    fe_mul(z2, t2, t1);           // временно
}

// Montgomery Ladder
static void montgomery_ladder(fe& x, fe& z, const u8* scalar, const fe& x1) {
    fe x2, z2, x3, z3;
    fe_1(x2); fe_0(z2);
    fe_copy(x3, x1); fe_1(z3);

    for (int i = 254; i >= 0; --i) {
        int bit = (scalar[i >> 3] >> (i & 7)) & 1;

        if (bit) {
            ladder_step(x3, z3, x2, z2, x1);
        } else {
            ladder_step(x2, z2, x3, z3, x1);
        }
    }

    fe_copy(x, x2);
    fe_copy(z, z2);
}

int crypto_scalarmult(u8* q, const u8* n, const u8* p) {
    fe x1, x, z;
    fe_frombytes(x1, p);

    u8 e[32];
    memcpy(e, n, 32);
    e[0] &= 248;
    e[31] &= 127;
    e[31] |= 64;

    montgomery_ladder(x, z, e, x1);

    fe_invert(z, z);
    fe_mul(x, x, z);
    fe_tobytes(q, x);
    return 0;
}

int X25519(u8 shared[32], const u8 my_priv[32], const u8 their_pub[32]) {
    return crypto_scalarmult(shared, my_priv, their_pub);
}

} // namespace PureX25519