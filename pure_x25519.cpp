// =============================================
// PURE X25519 - Продолжение улучшений
// =============================================

#include <cstdint>
#include <cstring>

namespace PureX25519 {

using i64 = int64_t;
using u8 = uint8_t;

struct fe { i64 v[10]; };

// ... предыдущие функции fe_add, fe_sub, fe_mul и т.д. ...

// Улучшенный ladder step
static void x25519_ladder_step(fe& x2, fe& z2, fe& x3, fe& z3, const fe& x1) {
    fe t0, t1, t2, t3, t4;

    fe_add(t0, x2, z2);
    fe_sub(t1, x2, z2);
    fe_add(t2, x3, z3);
    fe_sub(t3, x3, z3);

    fe_mul(t4, t0, t3);      // DA
    fe_mul(t3, t1, t2);      // CB

    fe_add(t0, t4, t3);
    fe_square(x3, t0);       // X5

    fe_sub(t0, t4, t3);
    fe_square(t1, t0);
    fe_mul(z3, x1, t1);      // Z5

    fe_square(t0, t0);       // переиспользуем для AA
    fe_square(t1, t1);       // BB
    fe_sub(t2, t0, t1);      // E

    fe_mul(x2, t0, t1);      // X4

    // Z4 = E * (BB + a24 * E)
    fe t5;
    fe_mul(t5, t2, t1);
    fe_copy(z2, t5);
}

static void montgomery_ladder(fe& x, fe& z, const u8* scalar, const fe& x1) {
    fe x2, z2, x3, z3;
    fe_1(x2); fe_0(z2);
    fe_copy(x3, x1); fe_1(z3);

    for (int i = 254; i >= 0; --i) {
        int bit = (scalar[i >> 3] >> (i & 7)) & 1;

        if (bit) {
            x25519_ladder_step(x3, z3, x2, z2, x1);
        } else {
            x25519_ladder_step(x2, z2, x3, z3, x1);
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
    e[0] &= 248; e[31] &= 127; e[31] |= 64;

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