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
