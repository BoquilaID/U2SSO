//
// Created by jayamine on 6/14/24.
//
#include <openssl/rand.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <secp256k1.h>
#include <secp256k1_ringcip.h>


boquila_ctx boquila_get_context(int L, int n, int m) {
    boquila_ctx ctx;
    uint8_t gen_seed[32];
    memset(gen_seed, 0xff, 32);
    ctx.ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    ctx.rctx = secp256k1_ringcip_context_create(ctx.ctx, L, n, m, gen_seed, NULL);
}

pk_t boquila_gen_mpk(boquila_ctx ctx, uint8_t *msk) {
    pk_t mpk;
    secp256k1_boquila_gen_mpk(ctx.ctx, &ctx.rctx, &mpk, msk);
    return mpk;
}

int main() {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

    int L = 10;
    int n = 2;
    int m = 10;
    uint8_t gen_seed[32];
    uint8_t msk[64];
    uint8_t ssk[32];
    pk_t spk;
    uint8_t r[64];
    uint8_t name[4] = {'a', 'b', 'c', 'd'};
    int name_len = 4;
    int t;

    memset(gen_seed, 0, 32);
    ringcip_context rctx = secp256k1_ringcip_context_create(ctx, L, n, m, gen_seed, NULL);

    printf("Maximum N: %d\n", rctx.N);

    uint8_t chalregi[32];
    memset(chalregi, 0xff, 32);


    pk_t mpks[1024];
    uint8_t msks[1024][64];
    for (int i = 0; i < 1024; i++) {
        RAND_bytes(msks[i], 64);
        if (secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]) == 0) {
            printf("mpk creation failed");
        }
    }
    int j = 0;
    if (secp256k1_boquila_derive_ssk(ctx, ssk, msks[j], name, name_len) == 0) {
        printf("csk creation failed");
    }
    if (secp256k1_boquila_derive_spk(ctx, &rctx, &spk, ssk) == 0) {
        printf("wpk creation failed");
    }

    int N = 8;
    for (int m = 3; m < rctx.m; m++) {
        uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_get_size(&rctx, m) * sizeof(uint8_t));
        clock_t begin = clock();
        if (secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], chalregi, name, name_len, &spk, j, N, m) == 0) {
            printf("proving membership failed\n");
        }
        clock_t end = clock();
        double proving_time = (double)(end - begin) / CLOCKS_PER_SEC;
        begin = clock();
        if (secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks, chalregi, name, name_len, &spk, N, m) == 0)  {
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

