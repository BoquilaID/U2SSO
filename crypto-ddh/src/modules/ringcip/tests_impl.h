//
// Created by jayamine on 10/24/23.
//

#ifndef SECP256K_NOPENI_TESTS_IMPL_H
#define SECP256K_NOPENI_TESTS_IMPL_H

#include "include/secp256k1_ringcip.h"

void printarray(uint8_t *buf, int len) {
    for (int i = 0; i < len; i++) {
        printf("%d ", buf[i]);
    }
    printf("\n");
}

int equalarray(uint8_t *buf, uint8_t *buf1, int len) {
    for (int i = 0; i < len; i++) {
        if (buf[i] != buf1[i]){
            return 0;
        }
    }
    return 1;
}

int test_boquila(void) {
    int L = 10;
    int n = 3;
    int m = 3;
    uint8_t gen_seed[32];
    uint8_t msk[64];
    uint8_t r[64];
    uint8_t name[4] = {'a', 'b', 'c', 'd'};
    int32_t name_len = 4;
    int t;

    memset(gen_seed, 0, 32);
    ringcip_context rctx = secp256k1_ringcip_context_create(ctx, L, n, m, gen_seed, NULL);

    CHECK(rctx.N == 27);

    pk_t mpk;
    pk_t mpk1;
    pk_t wpk;
    pk_t wpk1;
    pk_t cpk;
    pk_t cpk1;
    uint8_t csk[32];
    uint8_t csk1[32];
    cint_st cintsk;
    secp256k1_rand256(msk);
    secp256k1_rand256(msk + 32);

    CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpk, msk));
    CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpk1, msk));
    CHECK(equalarray(mpk.buf, mpk1.buf, 33));

    CHECK(secp256k1_boquila_derive_webpk(ctx, &rctx, &wpk, msk, name, name_len));
    CHECK(secp256k1_boquila_derive_webpk(ctx, &rctx, &wpk1, msk, name, name_len));
    CHECK(equalarray(wpk.buf, wpk1.buf, 33));

    uint8_t count[4] = {1, 0, 0, 0};
    CHECK(secp256k1_boquila_derive_csk(ctx, &rctx, csk, msk, name, name_len, count));
    CHECK(secp256k1_boquila_derive_csk(ctx, &rctx, csk1, msk, name, name_len, count));
    CHECK(equalarray(csk, csk1, 32));

    CHECK(secp256k1_boquila_derive_cpk(ctx, &rctx, &cpk, csk));
    CHECK(secp256k1_boquila_derive_cpk(ctx, &rctx, &cpk1, csk1));
    CHECK(equalarray(cpk.buf, cpk1.buf, 32));

    // ==============================================

    int N = rctx.N;
    pk_t mpks[N];
    uint8_t msks[N][64];
    for (int i = 0; i < N; i++) {
        secp256k1_rand256(msks[i]);
        secp256k1_rand256(msks[i] + 32);
        CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]));
    }

    int j = 0;
    CHECK(secp256k1_boquila_derive_webpk(ctx, &rctx, &wpk, msks[j], name, name_len));

    uint8_t *proof = (uint8_t*) malloc(secp256k1_zero_mcom_get_size(&rctx)*sizeof(uint8_t));
    CHECK(secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], name, name_len, &wpk, j, N));
    CHECK(secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks,&wpk, N));

    // ==============================================

    secp256k1_rand256(r);
    secp256k1_rand256(r + 32);
    uint8_t chal[32];
    memset(chal, 0xff, 32);
    uint8_t proofnew[64+33];
    CHECK(secp256k1_boquila_prove_newcpk(ctx, &rctx, proofnew, msks[j], r, name, name_len, &wpk, &cpk, chal));
    CHECK(secp256k1_boquila_verify_newcpk(ctx, &rctx, proofnew, &wpk, &cpk, chal));

    free(proof);

    secp256k1_ringcip_context_clear(&rctx);
}




int test_ringcip(void) {
    int L = 10;
    int n = 3;
    int m = 3;
    uint8_t gen_seed[32];
    int t;

    memset(gen_seed, 0, 32);
    ringcip_context rctx = secp256k1_ringcip_context_create(ctx, L, n, m, gen_seed, NULL);

    CHECK(rctx.N == 27);

    cint_pt cint;
    cint_st cintsk;
    uint8_t keybuf[32];
    secp256k1_rand256(keybuf);

    CHECK(secp256k1_create_secret_cint(ctx, &rctx, &cintsk, 10, keybuf));
    CHECK(secp256k1_create_cint(ctx, &rctx, &cint, &cintsk));

    cint_pt Cs[rctx.N];
    cint_st Csks[rctx.N];
    int index = 3;
    for (t = 0; t < rctx.N; t++) {
        if (t != index) {
            secp256k1_rand256(keybuf);
            CHECK(secp256k1_create_secret_cint(ctx, &rctx, &Csks[t], 10, keybuf));
            CHECK(secp256k1_create_cint(ctx, &rctx, &Cs[t], &Csks[t]));
        } else {
            secp256k1_rand256(keybuf);
            CHECK(secp256k1_create_secret_cint(ctx, &rctx, &Csks[t], 0, keybuf));
            CHECK(secp256k1_create_cint(ctx, &rctx, &Cs[t], &Csks[t]));
        }
    }
    uint8_t *proof = (uint8_t*) malloc(secp256k1_zero_mcom_get_size(&rctx)*sizeof(uint8_t));
    CHECK(secp256k1_create_zero_mcom_proof(ctx, &rctx, proof, Cs, index, &Csks[index].key));
    CHECK(secp256k1_verify_zero_mcom_proof(ctx, &rctx, proof, Cs));

    CHECK(secp256k1_create_zero_mcom_proof(ctx, &rctx, proof, Cs, index + 1, &Csks[index].key));
    CHECK(secp256k1_verify_zero_mcom_proof(ctx, &rctx, proof, Cs) == 0);

    free(proof);
    secp256k1_ringcip_context_clear(&rctx);

    return 1;
}


#endif //SECP256K_NOPENI_TESTS_IMPL_H
