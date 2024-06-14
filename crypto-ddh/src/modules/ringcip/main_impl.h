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
        pk_t *mpk,
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
    secp256k1_ge_save_boquila(mpk->buf, &tmp);

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
        pk_t* cpk,
        const uint8_t* csk) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(cpk != NULL);
    ARG_CHECK(csk != NULL);

    int overflow;
    secp256k1_ge tmpG;
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
    secp256k1_ge_save_boquila(cpk->buf, &tmp);

    return 1;
}

int secp256k1_boquila_derive_webpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        pk_t* wpk,
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
    secp256k1_scalar scalartmp;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;
    uint8_t buf[32];

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);
    secp256k1_generator_load(&tmpH, &rctx->genh);

    secp256k1_scalar_set_b32(&scalartmp, msk, &overflow);
    if (overflow) {
        return 0;
    }
    secp256k1_scalar_negate(&scalara, &scalartmp);

    // wpk := h^{msk_a}g^{\mathsf{Hash}(msk_b, name)}
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, msk + 32, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&scalarb, buf, NULL);

    secp256k1_pedersen_blind_ecmult(&tmpGj, &scalara, &scalarb, &tmpG, &tmpH);
    secp256k1_ge_set_gej(&tmp, &tmpGj);

    // serialize
    secp256k1_ge_save_boquila(wpk->buf, &tmp);

    secp256k1_scalar_clear(&scalara);
    secp256k1_scalar_clear(&scalarb);
    secp256k1_scalar_clear(&scalartmp);

    return 1;
}

int secp256k1_boquila_prove_memmpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t *mpks,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len,
        pk_t * wpk,
        int32_t j,
        int32_t N, int m) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpks != NULL);
    ARG_CHECK(wpk != NULL);

    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_ge tmpg;
    secp256k1_ge tmpwpk;
    secp256k1_gej tmpwpkj;
    secp256k1_gej tmpj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;
    secp256k1_scalar tmpsc;
    uint8_t buf[32];
    int overflow;

    cint_pt cints[N];

    /* g, h */
    secp256k1_generator_load(&tmpG, &rctx->geng);

    secp256k1_scalar_set_b32(&scalarb, msk + 32, &overflow);
    if (overflow) {
        return 0;
    }

    secp256k1_ge_load_boquila(wpk->buf, &tmpwpk);
    secp256k1_gej_set_ge(&tmpwpkj, &tmpwpk);
    for (int i = 0; i < N; i++) {
        secp256k1_ge_load_boquila(mpks[i].buf, &tmpg);
        secp256k1_gej_add_ge(&tmpj, &tmpwpkj, &tmpg);
        secp256k1_ge_set_gej(&cints[i].c, &tmpj);
        secp256k1_ge_save_boquila(cints[i].buf, &cints[i].c);
    }

    // sk creation
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, msk + 32, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&tmpsc, buf, NULL);
    secp256k1_scalar_add(&tmpsc, &tmpsc, &scalarb);

    secp256k1_scalar_clear(&scalara);
    secp256k1_scalar_clear(&scalarb);

//    // create m
//    int tmpN = N;
//    int m = 1;
//    for (int t = 0; t < rctx->m; t++) {
//        tmpN = tmpN/rctx->n;
//        m++;
//        if (tmpN == 0) {
//            break;
//        }
//    }

    return secp256k1_create_zero_mcom_proof(ctx, rctx, proof, cints, j, &tmpsc, N, m);
}

int secp256k1_boquila_verify_memmpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t *mpks,
        pk_t * wpk,
        int32_t N, int m) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpks != NULL);
    ARG_CHECK(wpk != NULL);

    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_ge tmpg;
    secp256k1_ge tmpwpk;
    secp256k1_gej tmpwpkj;
    secp256k1_gej tmpj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;
    secp256k1_scalar tmpsc;
    uint8_t buf[32];
    int overflow;

    cint_pt cints[N];

    /* g, h */
    secp256k1_generator_load(&tmpG, &rctx->geng);

    secp256k1_ge_load_boquila(wpk->buf, &tmpwpk);
    secp256k1_gej_set_ge(&tmpwpkj, &tmpwpk);
    for (int i = 0; i < N; i++) {
        secp256k1_ge_load_boquila(mpks[i].buf, &tmpg);
        secp256k1_gej_add_ge(&tmpj, &tmpwpkj, &tmpg);
        secp256k1_ge_set_gej(&cints[i].c, &tmpj);
        secp256k1_ge_save_boquila(cints[i].buf, &cints[i].c);
    }

    // create m
//    int tmpN = N;
//    int m = 0;
//    for (int t = 0; t < rctx->m; t++) {
//        tmpN = tmpN/rctx->n;
//        m++;
//        if (tmpN == 0) {
//            break;
//        }
//    }

    return secp256k1_verify_zero_mcom_proof(ctx, rctx, proof, cints, N, m);
}


int secp256k1_boquila_prove_newcpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        const uint8_t* msk,
        const uint8_t* r,
        const uint8_t* name,
        int name_len,
        pk_t * wpk,
        pk_t * cpk,
        uint8_t *chal) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(wpk != NULL);
    ARG_CHECK(r != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalartmp;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_scalar ra;
    secp256k1_scalar rb;
    secp256k1_scalar sa;
    secp256k1_scalar sb;
    secp256k1_scalar x;
    secp256k1_sha256 sha;
    uint8_t buf[32];
    uint8_t Rbuf[33];

    secp256k1_scalar_set_b32(&scalartmp, msk, &overflow);
    if (overflow) {
        return 0;
    }
    secp256k1_scalar_negate(&scalara, &scalartmp);

    // wpk := h^{msk_a}g^{\mathsf{Hash}(msk_b, name)}
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, msk + 32, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&scalarb, buf, NULL);

    secp256k1_scalar_set_b32(&ra, r, NULL);
    if (secp256k1_scalar_is_zero(&ra)) {
        return 0;
    }

    secp256k1_scalar_set_b32(&rb, r + 32, NULL);
    if (secp256k1_scalar_is_zero(&rb)) {
        return 0;
    }

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);
    secp256k1_generator_load(&tmpH, &rctx->genh);

    secp256k1_pedersen_blind_ecmult(&tmpGj, &ra, &rb, &tmpG, &tmpH);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(Rbuf, &tmp);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, wpk->buf, 33);
    secp256k1_sha256_write(&sha, cpk->buf, 33);
    secp256k1_sha256_write(&sha, chal, 32);
    secp256k1_sha256_write(&sha, Rbuf, 33);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&x, buf, NULL);

    /* s1, s2 */
    secp256k1_scalar_mul(&scalartmp, &x, &scalara);
    secp256k1_scalar_add(&sa, &scalartmp, &ra);

    secp256k1_scalar_mul(&scalartmp, &x, &scalarb);
    secp256k1_scalar_add(&sb, &scalartmp, &rb);

    memcpy(proof, Rbuf, 33);
    secp256k1_scalar_get_b32(proof + 33, &sa);
    secp256k1_scalar_get_b32(proof + 33 + 32, &sb);

    // verification
    uint8_t LHS[33];
    uint8_t RHS[33];
    secp256k1_ge_load_boquila(wpk->buf, &tmp);
    secp256k1_ecmult_const(&tmpGj, &tmp, &x, 256);
    secp256k1_ge_load_boquila(Rbuf, &tmp);
    secp256k1_gej_add_ge(&tmpGj, &tmpGj, &tmp);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(LHS, &tmp);

    secp256k1_pedersen_blind_ecmult(&tmpGj, &sa, &sb, &tmpG, &tmpH);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(RHS, &tmp);

    secp256k1_scalar_clear(&scalara);
    secp256k1_scalar_clear(&scalarb);
    secp256k1_scalar_clear(&scalartmp);
    secp256k1_scalar_clear(&ra);
    secp256k1_scalar_clear(&rb);

    return memcmp(LHS, RHS, 33) == 0;
}


int secp256k1_boquila_verify_newcpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t * wpk,
        pk_t * cpk,
        uint8_t *chal) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(wpk != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_scalar sa;
    secp256k1_scalar sb;
    secp256k1_scalar x;
    secp256k1_sha256 sha;
    uint8_t buf[32];
    uint8_t Rbuf[33];

    memcpy(Rbuf, proof, 33);
    secp256k1_scalar_set_b32(&sa, proof + 33, NULL);
    secp256k1_scalar_set_b32(&sb, proof + 33 + 32, NULL);

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);
    secp256k1_generator_load(&tmpH, &rctx->genh);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, wpk->buf, 33);
    secp256k1_sha256_write(&sha, cpk->buf, 33);
    secp256k1_sha256_write(&sha, chal, 32);
    secp256k1_sha256_write(&sha, Rbuf, 33);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&x, buf, NULL);

    // verification
    uint8_t LHS[33];
    uint8_t RHS[33];
    secp256k1_ge_load_boquila(wpk->buf, &tmp);
    secp256k1_ecmult_const(&tmpGj, &tmp, &x, 256);
    secp256k1_ge_load_boquila(Rbuf, &tmp);
    secp256k1_gej_add_ge(&tmpGj, &tmpGj, &tmp);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(LHS, &tmp);

    secp256k1_pedersen_blind_ecmult(&tmpGj, &sa, &sb, &tmpG, &tmpH);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(RHS, &tmp);

    return memcmp(LHS, RHS, 33) == 0;
}

#endif //SECP256K_RINGCIP_MAIN_IMPL_H
