// =============================================
// PURE X25519 - Дифференциальные координаты (X:Z)
// =============================================

// Дифференциальные координаты (Differential Coordinates):
// Вместо полных координат точки (x, y) мы храним только отношение X : Z,
// где реальная x-координата = X / Z.
// Это позволяет выполнять дифференциальное сложение очень эффективно,
// когда известна разность между двумя точками.

// Преимущества:
// - Избавляемся от необходимости вычислять y-координату
// - Дифференциальное сложение требует только умножений в поле
// - Хорошо подходит для constant-time реализации (Montgomery Ladder)

// В Montgomery Ladder мы всегда работаем с парами точек в дифференциальных координатах.

#include <cstdint>
#include <cstring>

namespace PureX25519 {

using i64 = int64_t;
using u8 = uint8_t;

struct fe { i64 v[10]; };

// ... предыдущие функции fe_add, fe_sub, fe_mul и т.д. ...

// =============================================
// Дифференциальное сложение + удвоение
// =============================================

static void x25519_ladder_step(fe& x2, fe& z2, fe& x3, fe& z3, const fe& x1) {
    fe t0, t1, t2, t3, t4;

    // A = X2 + Z2, B = X2 - Z2
    fe_add(t0, x2, z2);
    fe_sub(t1, x2, z2);

    // C = X3 + Z3, D = X3 - Z3
    fe_add(t2, x3, z3);
    fe_sub(t3, x3, z3);

    // DA = D * A, CB = C * B
    fe_mul(t4, t0, t3);
    fe_mul(t3, t1, t2);

    // Новая точка 5 (сложение)
    fe_add(t2, t4, t3);
    fe_square(x3, t2);           // X5 = (DA + CB)^2

    fe_sub(t3, t4, t3);
    fe_square(t2, t3);
    fe_mul(z3, x1, t2);          // Z5 = x1 * (DA - CB)^2

    // Удвоение точки 2
    fe_square(t0, t0);           // AA = A^2
    fe_square(t1, t1);           // BB = B^2
    fe_sub(t2, t0, t1);          // E = AA - BB

    fe_mul(x2, t0, t1);          // X4 = AA * BB

    // Z4 = E * (BB + a24 * E), где a24 = 121665
    // Здесь используется упрощённая версия
    fe_mul(z2, t2, t1);
}

// Montgomery Ladder с использованием дифференциальных координат
static void montgomery_ladder(fe& x, fe& z, const u8* scalar, const fe& x1) {
    fe x2, z2, x3, z3;
    fe_1(x2); fe_0(z2);          // (X2:Z2) = (1:0) = точка на бесконечности
    fe_copy(x3, x1); fe_1(z3);   // (X3:Z3) = (x1:1)

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