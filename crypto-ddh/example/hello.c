//
// Created by jayamine on 6/14/24.
//
#include <openssl/rand.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <secp256k1_ringcip.h>

int main() {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

    int L = 10;
    int n = 2;
    int m = 10;
    uint8_t gen_seed[32];
    uint8_t msk[64];
    uint8_t r[64];
    uint8_t name[4] = {'a', 'b', 'c', 'd'};
    int32_t name_len = 4;
    int t;

    memset(gen_seed, 0, 32);
    ringcip_context rctx = secp256k1_ringcip_context_create(ctx, L, n, m, gen_seed, NULL);

    printf("Maximum N: ", rctx.N);

    pk_t mpk;
    pk_t wpk;
    pk_t cpk;
    uint8_t csk[32];
    uint8_t csk1[32];
    cint_st cintsk;
    secp256k1_rand256(msk);
    secp256k1_rand256(msk + 32);

    pk_t mpks[1024];
    uint8_t msks[1024][64];
    for (int i = 0; i < 1024; i++) {
        secp256k1_rand256(msks[i]);
        secp256k1_rand256(msks[i] + 32);
        if (secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]) == 0) {
            printf("mpk creation failed");
        }
    }
    int j = 0;
    CHECK(secp256k1_boquila_derive_webpk(ctx, &rctx, &wpk, msks[j], name, name_len));

    int N = 8;
    for (int m = 3; m < rctx.m; m++) {
        uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_get_size(&rctx, m) * sizeof(uint8_t));
        clock_t begin = clock();
        if (secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], name, name_len, &wpk, j, N, m) == 0) {
            printf("proving membership failed\n");
        }
        clock_t end = clock();
        double proving_time = (double)(end - begin) / CLOCKS_PER_SEC;
        begin = clock();
        if (secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks, &wpk, N, m) == 0)  {
            printf("proving membership failed\n");
        }
        end = clock();
        double verify_time = (double)(end - begin) / CLOCKS_PER_SEC;
        free(proof);
        N *= rctx.n;
        printf("%d, %d %f %f\n", N, secp256k1_zero_mcom_get_size(&rctx, m), proving_time, verify_time);
    }
    secp256k1_ringcip_context_clear(&rctx);
}