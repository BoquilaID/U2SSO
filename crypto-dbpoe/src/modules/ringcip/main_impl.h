//
// Created by jayamine on 10/24/23.
//

#ifndef SECP256K_RINGCIP_MAIN_IMPL_H
#define SECP256K_RINGCIP_MAIN_IMPL_H

#include "include/secp256k1_ringcip.h"
#include "ring_impl.h"

boquila_ctx boquila_get_context(int L, int n, int m) {
    boquila_ctx ctx;
    uint8_t gen_seed[32];
    memset(gen_seed, 0xff, 32);
    ctx.ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    ctx.rctx = secp256k1_ringcip_context_create(ctx.ctx, L, n, m, gen_seed, NULL);
    return ctx;
}

boquila_DBPoE_ctx boquila_get_DBPoE_context(int L, int n, int m, uint8_t *topics, int topic_size) {
    boquila_DBPoE_ctx ctx;
    uint8_t gen_seed[32];
    memset(gen_seed, 0xff, 32);
    ctx.ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    ctx.rctx = secp256k1_ringcip_DBPoE_context_create(ctx.ctx, L, n, m, gen_seed, topics, topic_size);
    return ctx;
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
    secp256k1_gej tmpj;
    secp256k1_scalar scalara;

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);

    secp256k1_scalar_set_b32(&scalara, msk, &overflow);
    if (overflow) {
        return 0;
    }

    secp256k1_ecmult_const(&tmpj, &tmpG, &scalara, 256);
    secp256k1_ge_set_gej(&tmp, &tmpj);

    // serialize
    secp256k1_ge_save_boquila(mpk->buf, &tmp);

    return 1;
}

int secp256k1_boquila_gen_DBPoE_mpk(
        const secp256k1_context* ctx,
        const ringcip_DBPoE_context* rctx,
        pk_t *mpk,
        const uint8_t *msk) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpk != NULL);
    ARG_CHECK(msk != NULL);
    int j;

    int overflow;
    secp256k1_ge tmp;
    secp256k1_gej tmpj;
    uint8_t buf[32];

    secp256k1_ge* geng = (secp256k1_ge*) malloc((rctx->topic_size + 1) *sizeof(secp256k1_ge));
    for (int j = 0; j < rctx->topic_size + 1; j++) {
        secp256k1_generator_load(&geng[j], &rctx->geng[j]);
    }

    secp256k1_scalar vals[rctx->topic_size + 1];
    secp256k1_scalar_set_b32(&vals[0], msk, &overflow);
    if (overflow) {
        return 0;
    }
    secp256k1_sha256 sha;

    // val[j] = \mathsf{Hash}(msk, name)}
    uint8_t count[4];
    memset(count, 0xa, 4); // prefix for nullifiers
    for (int j = 1; j < rctx->topic_size + 1; j++) {
        secp256k1_sha256_initialize(&sha);
        secp256k1_sha256_write(&sha, msk, 32);
        secp256k1_sha256_write(&sha, rctx->topics, j*32);
        secp256k1_sha256_write(&sha, count, 4);
        secp256k1_sha256_finalize(&sha, buf);
        secp256k1_scalar_set_b32(&vals[j], buf, &overflow);
    }

    /* g, mu */
    secp256k1_ecmult_const(&tmpj, &geng[0], &vals[0], 256);
    secp256k1_ge_set_gej(&tmp, &tmpj);
    for (j = 1; j < rctx->topic_size + 1; j++) {
        secp256k1_ecmult_const(&tmpj, &geng[j], &vals[j], 256); // todo
        secp256k1_gej_add_ge(&tmpj, &tmpj, &tmp);
        secp256k1_ge_set_gej(&tmp, &tmpj);
    }

    free(geng);

    // serialize
    secp256k1_ge_save_boquila(mpk->buf, &tmp);

    for (int j = 1; j < rctx->topic_size + 1; j++) {
        secp256k1_scalar_clear(&vals[j]);
    }

    return 1;
}


int secp256k1_boquila_derive_ssk(
        const secp256k1_context* ctx,
        uint8_t* csk,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(csk != NULL);
    ARG_CHECK(msk != NULL);
    ARG_CHECK(name != NULL);
    ARG_CHECK(name_len != 0);

    uint8_t count = 0; // zero for service sks

    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, msk, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, csk);

    return 1;
}

int secp256k1_boquila_derive_spk(
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


int secp256k1_boquila_derive_DBPoE_spk(
        const secp256k1_context* ctx,
        const ringcip_DBPoE_context* rctx,
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
    secp256k1_generator_load(&tmpG, &rctx->geng[0]);

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

int secp256k1_boquila_prove_memmpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t *mpks,
        const uint8_t* msk,
        const uint8_t* W,
        const uint8_t* name,
        int name_len,
        pk_t * spk,
        int32_t j,
        int32_t N, int m) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpks != NULL);
    ARG_CHECK(spk != NULL);

    cint_pt cints[N];

    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_ge tmpg;
    // secp256k1_ge tmpwpk;
    // secp256k1_gej tmpwpkj;
    secp256k1_gej tmpj;
    secp256k1_scalar scalara;
    secp256k1_sha256 sha;
    secp256k1_scalar tmpsc;
    uint8_t chalBuf[32];
    int overflow;

    secp256k1_scalar_set_b32(&scalara, msk, &overflow);
    if (overflow) {
        return 0;
    }

    // challenge
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    secp256k1_sha256_write(&sha, W, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, chalBuf);


    for (int i = 0; i < N; i++) {
        memcpy(cints[i].buf, mpks[i].buf, 33);
    }

    int res = secp256k1_create_zero_mcom_proof(ctx, rctx, proof, cints, j, &scalara, N, m, chalBuf);
    secp256k1_scalar_clear(&scalara);
    return res;
}

int secp256k1_boquila_verify_memmpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t *mpks,
        const uint8_t* W,
        const uint8_t* name,
        int name_len,
        pk_t * spk,
        int32_t N, int m) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpks != NULL);
    ARG_CHECK(spk != NULL);

    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_ge tmpg;
    // secp256k1_ge tmpwpk;
    // secp256k1_gej tmpwpkj;
    secp256k1_gej tmpj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;
    secp256k1_scalar tmpsc;
    uint8_t chalBuf[32];
    int overflow;

    cint_pt cints[N];

    // challenge
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    secp256k1_sha256_write(&sha, W, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, chalBuf);

    for (int i = 0; i < N; i++) {
        memcpy(cints[i].buf, mpks[i].buf, 33);
    }

    return secp256k1_verify_zero_mcom_proof(ctx, rctx, proof, cints, N, m, chalBuf);
}


int secp256k1_boquila_prove_DBPoE_memmpk(
        const secp256k1_context* ctx,
        const ringcip_DBPoE_context* rctx,
        uint8_t *proof,
        uint8_t *nullifier,
        pk_t *mpks,
        const uint8_t* msk,
        //const uint8_t* W,
        const uint8_t* name,
        int name_len,
        pk_t * spk,
        int32_t j,
        int topic_index,
        int32_t N, int m) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpks != NULL);
    ARG_CHECK(spk != NULL);
    ARG_CHECK(topic_index > 0);
    ARG_CHECK(topic_index < rctx->topic_size);

    cint_pt cints[N];

    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_ge tmpg;
    // secp256k1_gej tmpwpkj;
    secp256k1_gej tmpj;
    secp256k1_sha256 sha;
    secp256k1_scalar tmpsc;
    uint8_t chalBuf[32];
    uint8_t buf[32];
    int overflow;

    secp256k1_ge* geng = (secp256k1_ge*) malloc((rctx->topic_size + 1) *sizeof(secp256k1_ge));
    for (int j = 0; j < rctx->topic_size + 1; j++) {
        secp256k1_generator_load(&geng[j], &rctx->geng[j]);
    }

    // challenge
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    //secp256k1_sha256_write(&sha, W, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, chalBuf);

    // values
    secp256k1_scalar* vals = (secp256k1_scalar*) malloc((rctx->topic_size + 1)*sizeof(secp256k1_scalar));
    secp256k1_scalar_set_b32(&vals[0], msk, &overflow);
    if (overflow) {
        return 0;
    }

    // val[j] = \mathsf{Hash}(msk, name)}
    uint8_t count[4];
    memset(count, 0xa, 4); // prefix for nullifiers
    for (int j = 1; j < rctx->topic_size + 1; j++) {
        secp256k1_sha256_initialize(&sha);
        secp256k1_sha256_write(&sha, msk, 32);
        secp256k1_sha256_write(&sha, rctx->topics, j*32);
        secp256k1_sha256_write(&sha, count, 4);
        secp256k1_sha256_finalize(&sha, buf);
        secp256k1_scalar_set_b32(&vals[j], buf, &overflow);
    }

    secp256k1_scalar_get_b32(nullifier, &vals[topic_index]);
    secp256k1_scalar_negate(&tmpsc, &vals[topic_index]);
    // secp256k1_ecmult_const(&tmpwpkj, &geng[topic_index], &tmpsc, 256);
    for (int i = 0; i < N; i++) {
        secp256k1_ge_load_boquila(mpks[i].buf, &tmpg);
        // secp256k1_gej_add_ge(&tmpj, &tmpwpkj, &tmpg);
        secp256k1_ge_set_gej(&tmp, &tmpj);
        secp256k1_ge_save_boquila(cints[i].buf, &tmp);
    }

    int res = secp256k1_create_zero_mcom_DBPoE_proof(ctx, rctx, proof, cints, j, vals, N, topic_index, m, chalBuf);
    for (int j = 1; j < rctx->topic_size + 1; j++) {
        secp256k1_scalar_clear(&vals[j]);
    }
    free(vals);
    free(geng);
    return res;
}


int secp256k1_boquila_verify_DBPoE_memmpk(
        const secp256k1_context* ctx,
        const ringcip_DBPoE_context* rctx,
        uint8_t *proof,
        uint8_t *nullifier,
        pk_t *mpks,
        //const uint8_t* W,
        const uint8_t* name,
        int name_len,
        pk_t * spk,
        int topic_index,
        int32_t N, int m) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(mpks != NULL);
    ARG_CHECK(spk != NULL);
    ARG_CHECK(topic_index > 0);
    ARG_CHECK(topic_index < rctx->topic_size);

    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_ge tmpg;
    // secp256k1_ge tmpwpk;
    // secp256k1_gej tmpwpkj;
    secp256k1_gej tmpj;
    secp256k1_scalar scalara;
    secp256k1_scalar scalarb;
    secp256k1_sha256 sha;
    secp256k1_scalar tmpsc;
    uint8_t chalBuf[32];
    int overflow;

    cint_pt cints[N];

    secp256k1_ge* geng = (secp256k1_ge*) malloc((rctx->topic_size + 1) *sizeof(secp256k1_ge));
    for (int j = 0; j < rctx->topic_size + 1; j++) {
        secp256k1_generator_load(&geng[j], &rctx->geng[j]);
    }

    // challenge
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    //secp256k1_sha256_write(&sha, W, 32);
    secp256k1_sha256_write(&sha, name, name_len);
    secp256k1_sha256_finalize(&sha, chalBuf);

    secp256k1_scalar_set_b32(&tmpsc, nullifier, &overflow);
    secp256k1_scalar_negate(&tmpsc, &tmpsc);
    // secp256k1_ecmult_const(&tmpwpkj, &geng[topic_index], &tmpsc, 256);
    for (int i = 0; i < N; i++) {
        secp256k1_ge_load_boquila(mpks[i].buf, &tmpg);
        // secp256k1_gej_add_ge(&tmpj, &tmpwpkj, &tmpg);
        secp256k1_ge_set_gej(&tmp, &tmpj);
        secp256k1_ge_save_boquila(cints[i].buf, &tmp);
    }

    return secp256k1_verify_zero_mcom_DBPoE_proof(ctx, rctx, proof, cints, N, topic_index, m, chalBuf);
}


int secp256k1_boquila_auth_prove(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        const uint8_t* msk,
        const uint8_t* r,
        const uint8_t* name,
        int name_len,
        pk_t * spk,
        uint8_t *chal) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(spk != NULL);
    ARG_CHECK(r != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar ra;
    secp256k1_scalar sa;
    secp256k1_scalar x;
    secp256k1_sha256 sha;
    uint8_t buf[32];
    uint8_t Rbuf[33];

    uint8_t csk[32];
    secp256k1_boquila_derive_ssk(ctx, csk, msk, name, name_len);

    secp256k1_scalar_set_b32(&scalara, csk, &overflow);
    if (overflow) {
        return 0;
    }

    secp256k1_scalar_set_b32(&ra, r, NULL);
    if (secp256k1_scalar_is_zero(&ra)) {
        return 0;
    }

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);
    secp256k1_ecmult_const(&tmpGj, &tmpG, &ra, 256);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(Rbuf, &tmp);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    secp256k1_sha256_write(&sha, chal, 32);
    secp256k1_sha256_write(&sha, Rbuf, 33);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&x, buf, NULL);

    /* s1*/
    secp256k1_scalar_mul(&scalara, &x, &scalara);
    secp256k1_scalar_add(&sa, &scalara, &ra);

    memcpy(proof, Rbuf, 33);
    secp256k1_scalar_get_b32(proof + 33, &sa);

    return 1;
}


int secp256k1_boquila_auth_verify(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t * spk,
        uint8_t *chal) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(spk != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar sa;
    secp256k1_scalar x;
    secp256k1_sha256 sha;
    uint8_t buf[32];
    uint8_t Rbuf[33];

    memcpy(Rbuf, proof, 33);
    secp256k1_scalar_set_b32(&sa, proof + 33, NULL);

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    secp256k1_sha256_write(&sha, chal, 32);
    secp256k1_sha256_write(&sha, Rbuf, 33);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&x, buf, NULL);

    // verification
    uint8_t LHS[33];
    uint8_t RHS[33];
    secp256k1_ge_load_boquila(spk->buf, &tmp);
    secp256k1_ecmult_const(&tmpGj, &tmp, &x, 256);
    secp256k1_ge_load_boquila(Rbuf, &tmp);
    secp256k1_gej_add_ge(&tmpGj, &tmpGj, &tmp);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(LHS, &tmp);

    secp256k1_ecmult_const(&tmpGj, &tmpG, &sa, 256);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(RHS, &tmp);

    return memcmp(LHS, RHS, 33) == 0;
}


int secp256k1_boquila_DBPoE_auth_prove(
        const secp256k1_context* ctx,
        const ringcip_DBPoE_context* rctx,
        uint8_t *proof,
        const uint8_t* msk,
        const uint8_t* r,
        const uint8_t* name,
        int name_len,
        pk_t * spk,
        uint8_t *chal) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(spk != NULL);
    ARG_CHECK(r != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar ra;
    secp256k1_scalar sa;
    secp256k1_scalar x;
    secp256k1_sha256 sha;
    uint8_t buf[32];
    uint8_t Rbuf[33];

    uint8_t csk[32];
    secp256k1_boquila_derive_ssk(ctx, csk, msk, name, name_len);

    secp256k1_scalar_set_b32(&scalara, csk, &overflow);
    if (overflow) {
        return 0;
    }

    secp256k1_scalar_set_b32(&ra, r, NULL);
    if (secp256k1_scalar_is_zero(&ra)) {
        return 0;
    }

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng[0]);
    secp256k1_ecmult_const(&tmpGj, &tmpG, &ra, 256);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(Rbuf, &tmp);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    secp256k1_sha256_write(&sha, chal, 32);
    secp256k1_sha256_write(&sha, Rbuf, 33);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&x, buf, NULL);

    /* s1*/
    secp256k1_scalar_mul(&scalara, &x, &scalara);
    secp256k1_scalar_add(&sa, &scalara, &ra);

    memcpy(proof, Rbuf, 33);
    secp256k1_scalar_get_b32(proof + 33, &sa);

    return 1;
}


int secp256k1_boquila_DBPoE_auth_verify(
        const secp256k1_context* ctx,
        const ringcip_DBPoE_context* rctx,
        uint8_t *proof,
        pk_t * spk,
        uint8_t *chal) {
    ARG_CHECK(ctx != NULL);
    ARG_CHECK(rctx != NULL);
    ARG_CHECK(spk != NULL);

    int overflow;
    secp256k1_ge tmpG;
    secp256k1_ge tmpH;
    secp256k1_ge tmp;
    secp256k1_gej tmpGj;
    secp256k1_scalar scalara;
    secp256k1_scalar sa;
    secp256k1_scalar x;
    secp256k1_sha256 sha;
    uint8_t buf[32];
    uint8_t Rbuf[33];

    memcpy(Rbuf, proof, 33);
    secp256k1_scalar_set_b32(&sa, proof + 33, NULL);

    /* g, mu */
    secp256k1_generator_load(&tmpG, &rctx->geng[0]);

    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, spk->buf, 33);
    secp256k1_sha256_write(&sha, chal, 32);
    secp256k1_sha256_write(&sha, Rbuf, 33);
    secp256k1_sha256_finalize(&sha, buf);
    secp256k1_scalar_set_b32(&x, buf, NULL);

    // verification
    uint8_t LHS[33];
    uint8_t RHS[33];
    secp256k1_ge_load_boquila(spk->buf, &tmp);
    secp256k1_ecmult_const(&tmpGj, &tmp, &x, 256);
    secp256k1_ge_load_boquila(Rbuf, &tmp);
    secp256k1_gej_add_ge(&tmpGj, &tmpGj, &tmp);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(LHS, &tmp);

    secp256k1_ecmult_const(&tmpGj, &tmpG, &sa, 256);
    secp256k1_ge_set_gej(&tmp, &tmpGj);
    secp256k1_ge_save_boquila(RHS, &tmp);

    return memcmp(LHS, RHS, 33) == 0;
}


#endif //SECP256K_RINGCIP_MAIN_IMPL_H
