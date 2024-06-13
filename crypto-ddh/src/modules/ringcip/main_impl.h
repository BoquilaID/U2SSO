//
// Created by jayamine on 10/24/23.
//

#ifndef SECP256K_RINGCIP_MAIN_IMPL_H
#define SECP256K_RINGCIP_MAIN_IMPL_H

#include "include/secp256k1_ringcip.h"

static void secp256k1_ge_save_boquila(unsigned char *buf, secp256k1_ge* ge) {
    secp256k1_fe_normalize(&ge->x);
    secp256k1_fe_get_b32(&buf[1], &ge->x);
    buf[0] = 9 ^ secp256k1_fe_is_quad_var(&ge->y);
}

static void secp256k1_ge_load_boquila(unsigned char *buf, secp256k1_ge* ge) {
    secp256k1_fe fe;
    secp256k1_fe_set_b32(&fe, &buf[1]);
    secp256k1_ge_set_xquad(ge, &fe);
    if (buf[0] & 1) {
        secp256k1_ge_neg(ge, ge);
    }
}

int secp256k1_boquila_gen_mpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *mpk,
        const uint8_t *msk) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpk != NULL);
    ARG_CHECK(msk != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;

    /* g, mu */
    secp256k1_generator_load(&tmpH, &rctx->genh);
    secp256k1_generator_load(&tmpG, &rctx->geng);

    secp256k1_scalar_set_b32(&scalara, msk, &overflow);
    if (overflow) {
        return 0;
    }
    secp256k1_scalar_set_b32(&scalarb, msk + 32, &overflow);
    if (overflow) {
        return 0;
    }

    secp256k1_pedersen_blind_ecmult(&tmpGj, &scalara, &scalarb, &tmpG, &tmpH);
    secp256k1_ge_set_gej(&tmp, &tmpGj);

    // serialize
    secp256k1_ge_save_boquila(mpk, &tmp);

    return 1;
}


int secp256k1_boquila_derive_webpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t* wpk,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(wpk != NULL);
    ARG_CHECK(msk != NULL);
    ARG_CHECK(name != NULL);
    ARG_CHECK(name_len != 0);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;
    uint8_t buf[32];

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);
    secp256k1_generator_load(&tmpH, &rctx->genh);

    secp256k1_scalar_set_b32(&scalara, msk, &overflow);
    if (overflow) {
        return 0;
    }

    // wpk := h^{msk_a}g^{\mathsf{Hash}(msk_b, name)}
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, msk + 32, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&scalarb, buf, NULL);
    if (overflow) {
        return 0;
    }

    secp256k1_pedersen_blind_ecmult(&tmpGj, &scalara, &scalarb, &tmpG, &tmpH);
    secp256k1_ge_set_gej(&tmp, &tmpGj);

    // serialize
    secp256k1_ge_save_boquila(wpk, &tmp);

    return 1;
}

int secp256k1_boquila_derive_csk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t* csk,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len,
        uint8_t* count) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(csk != NULL);
    ARG_CHECK(msk != NULL);
    ARG_CHECK(name != NULL);
    ARG_CHECK(name_len != 0);

    int overflow;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;

    // wpk := h^{msk_a}g^{\mathsf{Hash}(msk_b, name)}
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, msk + 32, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_write(&sha, count, 4);
    secp256k1_sha256_finalize(&sha, csk);

    return 1;
}

int secp256k1_boquila_derive_cpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t* cpk,
        const uint8_t* csk) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(cpk != NULL);
    ARG_CHECK(csk != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpMu;
    secp256k1_ge tmp;
    secp256k1_gej tmpj;
    secp256k1_scalar scalarb;

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);

    secp256k1_scalar_set_b32(&scalarb, csk, &overflow);
    if (overflow) {
        return 0;
    }
    secp256k1_ecmult_const(&tmpj, &tmpG, &scalarb, 256);
    secp256k1_ge_set_gej(&tmp, &tmpj);

    // serialize
    secp256k1_ge_save_boquila(cpk, &tmp);

    return 1;
}

#endif //SECP256K_RINGCIP_MAIN_IMPL_H
