//
// Created by jayamine on 10/24/23.
//

#ifndef SECP256K_NOPENI_TESTS_IMPL_H
#define SECP256K_NOPENI_TESTS_IMPL_H

#include "include/secp256k1_ringcip.h"

#include <time.h>

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

    uint8_t chalregi[32];
    memset(chalregi, 0xff, 32);

    memset(gen_seed, 0, 32);
    ringcip_context rctx = secp256k1_ringcip_context_create(ctx, L, n, m, gen_seed, NULL);

    CHECK(rctx.N == 27);

    pk_t mpk;
    pk_t mpk1;
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

    uint8_t count[4] = {1, 0, 0, 0};
    CHECK(secp256k1_boquila_derive_ssk(ctx,  csk, msk, name, name_len));
    CHECK(secp256k1_boquila_derive_ssk(ctx, csk1, msk, name, name_len));
    CHECK(equalarray(csk, csk1, 32));

    CHECK(secp256k1_boquila_derive_spk(ctx, &rctx, &cpk, csk));
    CHECK(secp256k1_boquila_derive_spk(ctx, &rctx, &cpk1, csk1));
    CHECK(equalarray(cpk.buf, cpk1.buf, 32));

    // ==============================================

    int N = rctx.n;
    for (int m = 1; m < rctx.m; m++) {
        pk_t mpks[N];
        uint8_t msks[N][64];
        for (int i = 0; i < N; i++) {
            secp256k1_rand256(msks[i]);
            secp256k1_rand256(msks[i] + 32);
            CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]));
        }

        int j = 0;
        CHECK(secp256k1_boquila_derive_ssk(ctx, csk, msks[j], name, name_len));
        CHECK(secp256k1_boquila_derive_spk(ctx, &rctx, &cpk, csk));

        uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_get_size(&rctx, m) * sizeof(uint8_t));
        CHECK(secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], chalregi, name, name_len, &cpk, j, N, m));
        CHECK(secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks, chalregi, name, name_len, &cpk, N, m));

        // ==============================================

        secp256k1_rand256(r);
        secp256k1_rand256(r + 32);
        uint8_t chal[32];
        memset(chal, 0xff, 32);
        uint8_t proofnew[64 + 33];
        CHECK(secp256k1_boquila_auth_prove(ctx, &rctx, proofnew, msks[j], r, name, name_len, &cpk, chal));
        CHECK(secp256k1_boquila_auth_verify(ctx, &rctx, proofnew, &cpk, chal));

        free(proof);

        N *= rctx.n;
    }

    N = rctx.n -1;
    for (int m = 1; m < rctx.m; m++) {
        pk_t mpks[N];
        uint8_t msks[N][64];
        for (int i = 0; i < N; i++) {
            secp256k1_rand256(msks[i]);
            secp256k1_rand256(msks[i] + 32);
            CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]));
        }

        int j = 0;
        CHECK(secp256k1_boquila_derive_ssk(ctx, csk, msks[j], name, name_len));
        CHECK(secp256k1_boquila_derive_spk(ctx, &rctx, &cpk, csk));

        uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_get_size(&rctx, m) * sizeof(uint8_t));
        CHECK(secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], chalregi, name, name_len, &cpk, j, N, m));
        CHECK(secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks, chalregi, name, name_len, &cpk, N, m));

        // ==============================================

        secp256k1_rand256(r);
        secp256k1_rand256(r + 32);
        uint8_t chal[32];
        memset(chal, 0xff, 32);
        uint8_t proofnew[64 + 33];
        CHECK(secp256k1_boquila_auth_prove(ctx, &rctx, proofnew, msks[j], r, name, name_len, &cpk, chal));
        CHECK(secp256k1_boquila_auth_verify(ctx, &rctx, proofnew, &cpk, chal));

        free(proof);

        N *= rctx.n;
    }

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

    uint8_t chalregi[32];
    memset(chalregi, 0xff, 32);

    int currentN = 4;
    int currentm = 2;
    cint_pt Cs[currentN];
    cint_st Csks[currentN];
    int index = 1;
    for (t = 0; t < currentN; t++) {
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
    uint8_t *proof = (uint8_t*) malloc(secp256k1_zero_mcom_get_size(&rctx, currentm)*sizeof(uint8_t));
    CHECK(secp256k1_create_zero_mcom_proof(ctx, &rctx, proof, Cs, index, &Csks[index].key, currentN, currentm, chalregi));
    CHECK(secp256k1_verify_zero_mcom_proof(ctx, &rctx, proof, Cs, currentN, currentm, chalregi));

    CHECK(secp256k1_create_zero_mcom_proof(ctx, &rctx, proof, Cs, index + 1, &Csks[index].key, currentN, currentm, chalregi));
    CHECK(secp256k1_verify_zero_mcom_proof(ctx, &rctx, proof, Cs, currentN, currentm, chalregi) == 0);

    free(proof);
    secp256k1_ringcip_context_clear(&rctx);
    printf("passed ring cip tests for ring sizes that are not power of n\n");

    return 1;
}


int test_boquila_bench(void) {
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

    CHECK(rctx.N == 1024); // 8, 16,

    pk_t mpk;
    pk_t cpk;
    uint8_t csk[32];
    uint8_t csk1[32];
    cint_st cintsk;
    secp256k1_rand256(msk);
    secp256k1_rand256(msk + 32);

    uint8_t chalregi[32];
    memset(chalregi, 0xff, 32);

    pk_t mpks[1024];
    uint8_t msks[1024][64];
    for (int i = 0; i < 1024; i++) {
        secp256k1_rand256(msks[i]);
        secp256k1_rand256(msks[i] + 32);
        CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]));
    }
    int j = 0;
    CHECK(secp256k1_boquila_derive_ssk(ctx, csk, msks[j], name, name_len));
    CHECK(secp256k1_boquila_derive_spk(ctx, &rctx, &cpk, csk));

    int N = 8;
    int test_cases = 10;
    double proving_time = 0;
    double verify_time = 0;
    for (int m = 3; m < rctx.m; m++) {
        uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_get_size(&rctx, m) * sizeof(uint8_t));
        clock_t begin = clock();
        for (t = 0; t < test_cases; t++) {
            CHECK(secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], chalregi, name, name_len, &cpk, j, N, m));
        }
        clock_t end = clock();
        double proving_time = (double)(end - begin) / CLOCKS_PER_SEC;
        begin = clock();
        for (t = 0; t < test_cases; t++) {
            CHECK(secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks, chalregi, name, name_len, &cpk, N, m));
        }
        end = clock();
        double verify_time = (double)(end - begin) / CLOCKS_PER_SEC;
        free(proof);
        N *= rctx.n;
        printf("%d, %d, %d, %f, %f\n", N, m, secp256k1_zero_mcom_get_size(&rctx, m), proving_time/test_cases, verify_time/test_cases);
    }
    secp256k1_ringcip_context_clear(&rctx);
}


int test_boquila_3_bench(void) {
    int L = 10;
    int n = 3;
    int m = 6;
    uint8_t gen_seed[32];
    uint8_t msk[64];
    uint8_t r[64];
    uint8_t name[4] = {'a', 'b', 'c', 'd'};
    int32_t name_len = 4;
    int t;

    memset(gen_seed, 0, 32);
    ringcip_context rctx = secp256k1_ringcip_context_create(ctx, L, n, m, gen_seed, NULL);

    CHECK(rctx.N == 729); // 8, 16,

    pk_t mpk;
    pk_t mpk1;
    pk_t cpk;
    pk_t cpk1;
    uint8_t csk[32];
    uint8_t csk1[32];
    cint_st cintsk;
    secp256k1_rand256(msk);
    secp256k1_rand256(msk + 32);

    uint8_t chalregi[32];
    memset(chalregi, 0xff, 32);

    pk_t mpks[1024];
    uint8_t msks[1024][64];
    for (int i = 0; i < 1024; i++) {
        secp256k1_rand256(msks[i]);
        secp256k1_rand256(msks[i] + 32);
        CHECK(secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i]));
    }
    int j = 0;
    CHECK(secp256k1_boquila_derive_ssk(ctx, csk, msks[j], name, name_len));
    CHECK(secp256k1_boquila_derive_spk(ctx, &rctx, &cpk, csk));

    int N = 9;
    int test_cases = 10;
    double proving_time = 0;
    double verify_time = 0;
    for (int m = 2; m < rctx.m; m++) {
        uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_get_size(&rctx, m) * sizeof(uint8_t));
        clock_t begin = clock();
        for (t = 0; t < test_cases; t++) {
            CHECK(secp256k1_boquila_prove_memmpk(ctx, &rctx, proof, mpks, msks[j], chalregi, name, name_len, &cpk, j, N, m));
        }
        clock_t end = clock();
        double proving_time = (double)(end - begin) / CLOCKS_PER_SEC;
        begin = clock();
        for (t = 0; t < test_cases; t++) {
            CHECK(secp256k1_boquila_verify_memmpk(ctx, &rctx, proof, mpks, chalregi, name, name_len, &cpk, N, m));
        }
        end = clock();
        double verify_time = (double)(end - begin) / CLOCKS_PER_SEC;
        free(proof);
        N *= rctx.n;
        printf("%d, %d, %d, %f, %f\n", N, m, secp256k1_zero_mcom_get_size(&rctx, m), proving_time/test_cases, verify_time/test_cases);
    }
    secp256k1_ringcip_context_clear(&rctx);
}

int test_boquila_DBPoE_bench(void) {
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
    for (int top = 100; top < 500; top=top+100) {
        uint8_t topics[32 * top];
        for (t = 0; t <= top; t++) {
            memset(topics + t * 32, t, 32);
        }

        int topic_size = top;
        ringcip_DBPoE_context rctx = secp256k1_ringcip_DBPoE_context_create(ctx, L, n, m, gen_seed, topics, topic_size);
        CHECK(rctx.N == 1024); // 8, 16,

        pk_t mpk;
        pk_t cpk;
        uint8_t csk[32];
        uint8_t csk1[32];
        cint_st cintsk;
        secp256k1_rand256(msk);
        secp256k1_rand256(msk + 32);

        uint8_t chalregi[32];
        memset(chalregi, 0xff, 32);

        pk_t mpks[1024];
        uint8_t msks[1024][32];
        for (int i = 0; i < 1024; i++) {
            secp256k1_rand256(msks[i]);
            CHECK(secp256k1_boquila_gen_DBPoE_mpk(ctx, &rctx, &mpks[i], msks[i]));
        }
        int j = 0;
        int topic_index = 1;
        CHECK(secp256k1_boquila_derive_ssk(ctx, csk, msks[j], name, name_len));
        CHECK(secp256k1_boquila_derive_DBPoE_spk(ctx, &rctx, &cpk, csk));

        int N = 8;
        int test_cases = 10;
        double proving_time = 0;
        double verify_time = 0;
        for (int m = 3; m < rctx.m; m++) {
            uint8_t nullifier[32];
            uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_DBPoE_get_size(&rctx, m) * sizeof(uint8_t));
            clock_t begin = clock();
            for (t = 0; t < test_cases; t++) {
                CHECK(secp256k1_boquila_prove_DBPoE_memmpk(ctx, &rctx, proof, nullifier, mpks, msks[j], name,
                                                           name_len, &cpk, j, topic_index, N, m));
            }
            clock_t end = clock();
            double proving_time = (double) (end - begin) / CLOCKS_PER_SEC;
            begin = clock();
            for (t = 0; t < test_cases; t++) {
                CHECK(secp256k1_boquila_verify_DBPoE_memmpk(ctx, &rctx, proof, nullifier, mpks, name,
                                                            name_len, &cpk, topic_index, N, m));
            }
            end = clock();
            double verify_time = (double) (end - begin) / CLOCKS_PER_SEC;
            free(proof);
            N *= rctx.n;
            printf("%d, %d, %d, %f, %f, %d\n", N, m, secp256k1_zero_mcom_DBPoE_get_size(&rctx, m),
                   proving_time / test_cases, verify_time / test_cases, topic_size);
        }
        secp256k1_ringcip_DBPoE_context_clear(&rctx);
    }
}


int test_boquila_DBPoE_topic_bench(void) {
    int L = 10;
    int n = 3;
    int m = 6;
    uint8_t gen_seed[32];
    uint8_t msk[64];
    uint8_t r[64];
    uint8_t name[4] = {'a', 'b', 'c', 'd'};
    int32_t name_len = 4;
    int t;

    memset(gen_seed, 0, 32);
    for (int top = 100; top < 500; top=top+100) {
        uint8_t topics[32 * top];
        for (t = 0; t <= top; t++) {
            memset(topics + t*32, t, 32);
        }

        int topic_size = top;
        ringcip_DBPoE_context rctx = secp256k1_ringcip_DBPoE_context_create(ctx, L, n, m, gen_seed, topics, topic_size);

        CHECK(rctx.N == 729); // 8, 16,

        pk_t mpk;
        pk_t cpk;
        uint8_t csk[32];
        uint8_t csk1[32];
        cint_st cintsk;
        secp256k1_rand256(msk);
        secp256k1_rand256(msk + 32);

        uint8_t chalregi[32];
        memset(chalregi, 0xff, 32);

        pk_t mpks[1024];
        uint8_t msks[1024][32];
        for (int i = 0; i < 1024; i++) {
            secp256k1_rand256(msks[i]);
            CHECK(secp256k1_boquila_gen_DBPoE_mpk(ctx, &rctx, &mpks[i], msks[i]));
        }
        int j = 0;
        int topic_index = 1;
        CHECK(secp256k1_boquila_derive_ssk(ctx, csk, msks[j], name, name_len));
        CHECK(secp256k1_boquila_derive_DBPoE_spk(ctx, &rctx, &cpk, csk));

        int N = 9;
        int test_cases = 10;
        double proving_time = 0;
        double verify_time = 0;
        for (int m = 2; m < rctx.m; m++) {
            uint8_t nullifier[32];
            uint8_t *proof = (uint8_t *) malloc(secp256k1_zero_mcom_DBPoE_get_size(&rctx, m) * sizeof(uint8_t));
            clock_t begin = clock();
            for (t = 0; t < test_cases; t++) {
                CHECK(secp256k1_boquila_prove_DBPoE_memmpk(ctx, &rctx, proof, nullifier, mpks, msks[j], name,
                                                           name_len, &cpk, j, topic_index, N, m));
            }
            clock_t end = clock();
            double proving_time = (double) (end - begin) / CLOCKS_PER_SEC;
            begin = clock();
            for (t = 0; t < test_cases; t++) {
                CHECK(secp256k1_boquila_verify_DBPoE_memmpk(ctx, &rctx, proof, nullifier, mpks, name,
                                                            name_len, &cpk, topic_index, N, m));
            }
            end = clock();
            double verify_time = (double) (end - begin) / CLOCKS_PER_SEC;
            free(proof);
            N *= rctx.n;
            printf("%d, %d, %d, %f, %f, %d\n", N, m, secp256k1_zero_mcom_DBPoE_get_size(&rctx, m),
                   proving_time / test_cases, verify_time / test_cases, topic_size);
        }
        secp256k1_ringcip_DBPoE_context_clear(&rctx);
    }
}


#endif //SECP256K_NOPENI_TESTS_IMPL_H
