/**********************************************************************
 * Copyright (c) 2013-2015 Pieter Wuille, Gregory Maxwell             *
 * Distributed under the MIT software license, see the accompanying   *
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.*
 **********************************************************************/

#if defined HAVE_CONFIG_H
#include "libsecp256k1-config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "secp256k1.c"
#include "include/secp256k1.h"
#include "testrand_impl.h"

#ifdef ENABLE_OPENSSL_TESTS
#include "openssl/bn.h"
#include "openssl/ec.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"
# if OPENSSL_VERSION_NUMBER < 0x10100000L
void ECDSA_SIG_get0(const ECDSA_SIG *sig, const BIGNUM **pr, const BIGNUM **ps) {*pr = sig->r; *ps = sig->s;}
# endif
#endif

#include "contrib/lax_der_parsing.c"
#include "contrib/lax_der_privatekey_parsing.c"

#if !defined(VG_CHECK)
# if defined(VALGRIND)
#  include <valgrind/memcheck.h>
#  define VG_UNDEF(x,y) VALGRIND_MAKE_MEM_UNDEFINED((x),(y))
#  define VG_CHECK(x,y) VALGRIND_CHECK_MEM_IS_DEFINED((x),(y))
# else
#  define VG_UNDEF(x,y)
#  define VG_CHECK(x,y)
# endif
#endif

static int count = 64;
static secp256k1_context *ctx = NULL;


# include "modules/ringcip/tests_impl.h"
#include "modules/ringcip/tests_impl.h"

static void counting_illegal_callback_fn(const char* str, void* data) {
    /* Dummy callback function that just counts. */
    int32_t *p;
    (void)str;
    p = data;
    (*p)++;
}

static void uncounting_illegal_callback_fn(const char* str, void* data) {
    /* Dummy callback function that just counts (backwards). */
    int32_t *p;
    (void)str;
    p = data;
    (*p)--;
}

void random_scalar_order_test(secp256k1_scalar *num) {
    do {
        unsigned char b32[32];
        int overflow = 0;
        secp256k1_rand256_test(b32);
        secp256k1_scalar_set_b32(num, b32, &overflow);
        if (overflow || secp256k1_scalar_is_zero(num)) {
            continue;
        }
        break;
    } while(1);
}


void run_context_tests(int use_prealloc) {
    secp256k1_pubkey pubkey;
    secp256k1_pubkey zero_pubkey;
    secp256k1_ecdsa_signature sig;
    unsigned char ctmp[32];
    int32_t ecount;
    int32_t ecount2;
    secp256k1_context *none;
    secp256k1_context *sign;
    secp256k1_context *vrfy;
    secp256k1_context *both;
    void *none_prealloc = NULL;
    void *sign_prealloc = NULL;
    void *vrfy_prealloc = NULL;
    void *both_prealloc = NULL;

    secp256k1_gej pubj;
    secp256k1_ge pub;
    secp256k1_scalar msg, key, nonce;
    secp256k1_scalar sigr, sigs;

    if (use_prealloc) {
        none_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE));
        sign_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN));
        vrfy_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY));
        both_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY));
        CHECK(none_prealloc != NULL);
        CHECK(sign_prealloc != NULL);
        CHECK(vrfy_prealloc != NULL);
        CHECK(both_prealloc != NULL);
        none = secp256k1_context_preallocated_create(none_prealloc, SECP256K1_CONTEXT_NONE);
        sign = secp256k1_context_preallocated_create(sign_prealloc, SECP256K1_CONTEXT_SIGN);
        vrfy = secp256k1_context_preallocated_create(vrfy_prealloc, SECP256K1_CONTEXT_VERIFY);
        both = secp256k1_context_preallocated_create(both_prealloc, SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    } else {
        none = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
        sign = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        vrfy = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
        both = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    }

    memset(&zero_pubkey, 0, sizeof(zero_pubkey));

    ecount = 0;
    ecount2 = 10;
    secp256k1_context_set_illegal_callback(vrfy, counting_illegal_callback_fn, &ecount);
    secp256k1_context_set_illegal_callback(sign, counting_illegal_callback_fn, &ecount2);
    secp256k1_context_set_error_callback(sign, counting_illegal_callback_fn, NULL);
    CHECK(vrfy->error_callback.fn != sign->error_callback.fn);

    /* check if sizes for cloning are consistent */
    CHECK(secp256k1_context_preallocated_clone_size(none) == secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE));
    CHECK(secp256k1_context_preallocated_clone_size(sign) == secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN));
    CHECK(secp256k1_context_preallocated_clone_size(vrfy) == secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY));
    CHECK(secp256k1_context_preallocated_clone_size(both) == secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY));

    /*** clone and destroy all of them to make sure cloning was complete ***/
    {
        secp256k1_context *ctx_tmp;

        if (use_prealloc) {
            /* clone into a non-preallocated context and then again into a new preallocated one. */
            ctx_tmp = none; none = secp256k1_context_clone(none); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(none_prealloc); none_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)); CHECK(none_prealloc != NULL);
            ctx_tmp = none; none = secp256k1_context_preallocated_clone(none, none_prealloc); secp256k1_context_destroy(ctx_tmp);

            ctx_tmp = sign; sign = secp256k1_context_clone(sign); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(sign_prealloc); sign_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN)); CHECK(sign_prealloc != NULL);
            ctx_tmp = sign; sign = secp256k1_context_preallocated_clone(sign, sign_prealloc); secp256k1_context_destroy(ctx_tmp);

            ctx_tmp = vrfy; vrfy = secp256k1_context_clone(vrfy); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(vrfy_prealloc); vrfy_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY)); CHECK(vrfy_prealloc != NULL);
            ctx_tmp = vrfy; vrfy = secp256k1_context_preallocated_clone(vrfy, vrfy_prealloc); secp256k1_context_destroy(ctx_tmp);

            ctx_tmp = both; both = secp256k1_context_clone(both); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(both_prealloc); both_prealloc = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY)); CHECK(both_prealloc != NULL);
            ctx_tmp = both; both = secp256k1_context_preallocated_clone(both, both_prealloc); secp256k1_context_destroy(ctx_tmp);
        } else {
            /* clone into a preallocated context and then again into a new non-preallocated one. */
            void *prealloc_tmp;

            prealloc_tmp = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_NONE)); CHECK(prealloc_tmp != NULL);
            ctx_tmp = none; none = secp256k1_context_preallocated_clone(none, prealloc_tmp); secp256k1_context_destroy(ctx_tmp);
            ctx_tmp = none; none = secp256k1_context_clone(none); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(prealloc_tmp);

            prealloc_tmp = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN)); CHECK(prealloc_tmp != NULL);
            ctx_tmp = sign; sign = secp256k1_context_preallocated_clone(sign, prealloc_tmp); secp256k1_context_destroy(ctx_tmp);
            ctx_tmp = sign; sign = secp256k1_context_clone(sign); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(prealloc_tmp);

            prealloc_tmp = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_VERIFY)); CHECK(prealloc_tmp != NULL);
            ctx_tmp = vrfy; vrfy = secp256k1_context_preallocated_clone(vrfy, prealloc_tmp); secp256k1_context_destroy(ctx_tmp);
            ctx_tmp = vrfy; vrfy = secp256k1_context_clone(vrfy); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(prealloc_tmp);

            prealloc_tmp = malloc(secp256k1_context_preallocated_size(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY)); CHECK(prealloc_tmp != NULL);
            ctx_tmp = both; both = secp256k1_context_preallocated_clone(both, prealloc_tmp); secp256k1_context_destroy(ctx_tmp);
            ctx_tmp = both; both = secp256k1_context_clone(both); secp256k1_context_preallocated_destroy(ctx_tmp);
            free(prealloc_tmp);
        }
    }

    /* Verify that the error callback makes it across the clone. */
    CHECK(vrfy->error_callback.fn != sign->error_callback.fn);
    /* And that it resets back to default. */
    secp256k1_context_set_error_callback(sign, NULL, NULL);
    CHECK(vrfy->error_callback.fn == sign->error_callback.fn);

    /*** attempt to use them ***/
    random_scalar_order_test(&msg);
    random_scalar_order_test(&key);
    secp256k1_ecmult_gen(&both->ecmult_gen_ctx, &pubj, &key);
    secp256k1_ge_set_gej(&pub, &pubj);

    /* Verify context-type checking illegal-argument errors. */
    memset(ctmp, 1, 32);
    CHECK(secp256k1_ec_pubkey_create(vrfy, &pubkey, ctmp) == 0);
    CHECK(ecount == 1);
    VG_UNDEF(&pubkey, sizeof(pubkey));
    CHECK(secp256k1_ec_pubkey_create(sign, &pubkey, ctmp) == 1);
    VG_CHECK(&pubkey, sizeof(pubkey));
    CHECK(secp256k1_ecdsa_sign(vrfy, &sig, ctmp, ctmp, NULL, NULL) == 0);
    CHECK(ecount == 2);
    VG_UNDEF(&sig, sizeof(sig));
    CHECK(secp256k1_ecdsa_sign(sign, &sig, ctmp, ctmp, NULL, NULL) == 1);
    VG_CHECK(&sig, sizeof(sig));
    CHECK(ecount2 == 10);
    CHECK(secp256k1_ecdsa_verify(sign, &sig, ctmp, &pubkey) == 0);
    CHECK(ecount2 == 11);
    CHECK(secp256k1_ecdsa_verify(vrfy, &sig, ctmp, &pubkey) == 1);
    CHECK(ecount == 2);
    CHECK(secp256k1_ec_pubkey_tweak_add(sign, &pubkey, ctmp) == 0);
    CHECK(ecount2 == 12);
    CHECK(secp256k1_ec_pubkey_tweak_add(vrfy, &pubkey, ctmp) == 1);
    CHECK(ecount == 2);
    CHECK(secp256k1_ec_pubkey_tweak_mul(sign, &pubkey, ctmp) == 0);
    CHECK(ecount2 == 13);
    CHECK(secp256k1_ec_pubkey_negate(vrfy, &pubkey) == 1);
    CHECK(ecount == 2);
    CHECK(secp256k1_ec_pubkey_negate(sign, &pubkey) == 1);
    CHECK(ecount == 2);
    CHECK(secp256k1_ec_pubkey_negate(sign, NULL) == 0);
    CHECK(ecount2 == 14);
    CHECK(secp256k1_ec_pubkey_negate(vrfy, &zero_pubkey) == 0);
    CHECK(ecount == 3);
    CHECK(secp256k1_ec_pubkey_tweak_mul(vrfy, &pubkey, ctmp) == 1);
    CHECK(ecount == 3);
    CHECK(secp256k1_context_randomize(vrfy, ctmp) == 0);
    CHECK(ecount == 4);
    CHECK(secp256k1_context_randomize(sign, NULL) == 1);
    CHECK(ecount2 == 14);
    secp256k1_context_set_illegal_callback(vrfy, NULL, NULL);
    secp256k1_context_set_illegal_callback(sign, NULL, NULL);

    /* obtain a working nonce */
    do {
        random_scalar_order_test(&nonce);
    } while(!secp256k1_ecdsa_sig_sign(&both->ecmult_gen_ctx, &sigr, &sigs, &key, &msg, &nonce, NULL));

    /* try signing */
    CHECK(secp256k1_ecdsa_sig_sign(&sign->ecmult_gen_ctx, &sigr, &sigs, &key, &msg, &nonce, NULL));
    CHECK(secp256k1_ecdsa_sig_sign(&both->ecmult_gen_ctx, &sigr, &sigs, &key, &msg, &nonce, NULL));

    /* try verifying */
    CHECK(secp256k1_ecdsa_sig_verify(&vrfy->ecmult_ctx, &sigr, &sigs, &pub, &msg));
    CHECK(secp256k1_ecdsa_sig_verify(&both->ecmult_ctx, &sigr, &sigs, &pub, &msg));

    /* cleanup */
    if (use_prealloc) {
        secp256k1_context_preallocated_destroy(none);
        secp256k1_context_preallocated_destroy(sign);
        secp256k1_context_preallocated_destroy(vrfy);
        secp256k1_context_preallocated_destroy(both);
        free(none_prealloc);
        free(sign_prealloc);
        free(vrfy_prealloc);
        free(both_prealloc);
    } else {
        secp256k1_context_destroy(none);
        secp256k1_context_destroy(sign);
        secp256k1_context_destroy(vrfy);
        secp256k1_context_destroy(both);
    }
    /* Defined as no-op. */
    secp256k1_context_destroy(NULL);
    secp256k1_context_preallocated_destroy(NULL);

}



int main(int argc, char **argv) {
    unsigned char seed16[16] = {0};
    unsigned char run32[32] = {0};
    /* find iteration count */
    if (argc > 1) {
        count = strtol(argv[1], NULL, 0);
    }

    /* find random seed */
    if (argc > 2) {
        int pos = 0;
        const char* ch = argv[2];
        while (pos < 16 && ch[0] != 0 && ch[1] != 0) {
            unsigned short sh;
            if (sscanf(ch, "%2hx", &sh)) {
                seed16[pos] = sh;
            } else {
                break;
            }
            ch += 2;
            pos++;
        }
    } else {
        FILE *frand = fopen("/dev/urandom", "r");
        if ((frand == NULL) || fread(&seed16, sizeof(seed16), 1, frand) != sizeof(seed16)) {
            uint64_t t = time(NULL) * (uint64_t)1337;
            seed16[0] ^= t;
            seed16[1] ^= t >> 8;
            seed16[2] ^= t >> 16;
            seed16[3] ^= t >> 24;
            seed16[4] ^= t >> 32;
            seed16[5] ^= t >> 40;
            seed16[6] ^= t >> 48;
            seed16[7] ^= t >> 56;
        }
        if (frand) {
            fclose(frand);
        }
    }
    secp256k1_rand_seed(seed16);

    printf("test count = %i\n", count);
    printf("random seed = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", seed16[0], seed16[1], seed16[2], seed16[3], seed16[4], seed16[5], seed16[6], seed16[7], seed16[8], seed16[9], seed16[10], seed16[11], seed16[12], seed16[13], seed16[14], seed16[15]);

    /* initialize */
    run_context_tests(0);



    run_context_tests(1);

    ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
    if (secp256k1_rand_bits(1)) {
        secp256k1_rand256(run32);
        CHECK(secp256k1_context_randomize(ctx, secp256k1_rand_bits(1) ? run32 : NULL));
    }

    test_ringcip();
    test_boquila();
    test_boquila_bench();
    test_boquila_3_bench();
    test_boquila_DBPoE_bench();
    test_boquila_DBPoE_topic_bench();

    printf("passed ringcip tests!\n");


#ifdef  DO_OTHER_TESTS

    run_rand_bits();
    run_rand_int();
    run_util_tests();

    run_sha256_tests();
    run_hmac_sha256_tests();
    run_rfc6979_hmac_sha256_tests();

#ifndef USE_NUM_NONE
    /* num tests */
    run_num_smalltests();
#endif

    /* scalar tests */
    run_scalar_tests();

    /* field tests */
    run_field_inv();
    run_field_inv_var();
    run_field_inv_all_var();
    run_field_misc();
    run_field_convert();
    run_sqr();
    run_sqrt();

    /* group tests */
    run_ge();
    run_group_decompress();

    /* ecmult tests */
    run_wnaf();
    run_point_times_order();
    run_ecmult_chain();
    run_ecmult_constants();
    run_ecmult_gen_blind();
    run_ecmult_const_tests();
    run_ecmult_multi_tests();
    run_ec_combine();

    /* endomorphism tests */
#ifdef USE_ENDOMORPHISM
    run_endomorphism_tests();
#endif

    /* EC point parser test */
    run_ec_pubkey_parse_test();

    /* EC key edge cases */
    run_eckey_edge_case_test();

#ifdef ENABLE_MODULE_ECDH
    /* ecdh tests */
    run_ecdh_tests();
#endif

#ifdef ENABLE_MODULE_SCHNORRSIG
    /* Schnorrsig tests */
    run_schnorrsig_tests();
#endif

    /* ecdsa tests */
    run_random_pubkeys();
    run_ecdsa_der_parse();
    run_ecdsa_sign_verify();
    run_ecdsa_end_to_end();
    run_ecdsa_edge_cases();
#ifdef ENABLE_OPENSSL_TESTS
    run_ecdsa_openssl();
#endif

#ifdef ENABLE_MODULE_RECOVERY
    /* ECDSA pubkey recovery tests */
    run_recovery_tests();
#endif

#ifdef ENABLE_MODULE_GENERATOR
    run_generator_tests();
#endif

#ifdef ENABLE_MODULE_RANGEPROOF
    run_rangeproof_tests();
#endif

#ifdef ENABLE_MODULE_BULLETPROOF
    run_bulletproofs_tests();
#endif

#ifdef ENABLE_MODULE_COMMITMENT
    run_commitment_tests();
#endif

#ifdef ENABLE_MODULE_AGGSIG
    run_aggsig_tests();
#endif

#ifdef ENABLE_MODULE_WHITELIST
    /* Key whitelisting tests */
    run_whitelist_tests();
#endif

#ifdef ENABLE_MODULE_SURJECTIONPROOF
    run_surjection_tests();
#endif

    secp256k1_rand256(run32);
    printf("random run = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", run32[0], run32[1], run32[2], run32[3], run32[4], run32[5], run32[6], run32[7], run32[8], run32[9], run32[10], run32[11], run32[12], run32[13], run32[14], run32[15]);

#endif

    /* shutdown */
    secp256k1_context_destroy(ctx);

    printf("no problems found\n");
    return 0;
}
