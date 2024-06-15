//
// Created by jayamine on 10/24/23.
//

#ifndef SECP256K_NOPENI_SECP256K1_RINGCIP_H
#define SECP256K_NOPENI_SECP256K1_RINGCIP_H

# include "secp256k1.h"
# include "secp256k1_generator.h"

# ifdef __cplusplus
extern "C" {
# endif

typedef struct ringcip_context_struct {
    int L; // range bit
    int n; // N = n^m when N is the ring size and n is the base
    int m;
    int N;
    secp256k1_generator *multigen;
    secp256k1_generator geng;
    secp256k1_generator genh;
    secp256k1_generator genmu;
} ringcip_context;

typedef struct cint_public_struct {
    uint8_t buf[33]; // could be uninitialized
} cint_pt;


typedef struct zero_com_proof_struct {
    //secp256k1_ge A; // could be uninitialized
    uint8_t bufA[32]; // could be uninitialized
    //secp256k1_ge B; // could be uninitialized
    uint8_t bufB[32]; // could be uninitialized
    //secp256k1_ge C; // could be uninitialized
    uint8_t bufC[32]; // could be uninitialized
    //secp256k1_ge D; // could be uninitialized
    uint8_t bufD[32]; // could be uninitialized
} gb_zero_proof_t;

typedef struct pk_struct {
    uint8_t buf[33];
} pk_t;


/**
 * Create the master secret public keys from the given master secret key
 * @param ctx - main context
 * @param rctx -  ring context
 * @param mpk -  mpk (33 bytes)
 * @param msk -  master secret key of 64 bytes
 * @return 1 (success) or 0 (failure)
 */
SECP256K1_API int secp256k1_boquila_gen_mpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        pk_t *mpk,
        const uint8_t *msk)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4);


/**
 * Create the web public keys from the master secret key
 * @param ctx - main context
 * @param rctx -  ring context
 * @param wpk -  web public key for a webservice of ``name'' (33 bytes)
 * @param msk -  master secret key (64 bytes)
 * @param name -  name or identity of the web service
 * @param name_len -  name byte size
 * @return web public key
 */
SECP256K1_API int secp256k1_boquila_derive_webpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        pk_t* wpk,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4) SECP256K1_ARG_NONNULL(5);

/**
 * Create the child secret keys from the master secret key
 * @param ctx - main context
 * @param rctx -  ring context
 * @param csk -  child secret key of 32 bytes
 * @param msk -  master secret key (64 bytes)
 * @param name -  name or identity of the web service
 * @param name_len -  name byte size
 * @param count -  child key count (4 bytes)
 * @return child secret key
 */
SECP256K1_API int secp256k1_boquila_derive_csk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t* csk,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len,
        uint8_t* count)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4) SECP256K1_ARG_NONNULL(5);


/**
 * Create the child public keys from the master secret key
 * @param ctx - main context
 * @param rctx -  ring context
 * @param cpk -  child public key for a webservice of ``name, count'' (33 bytes)
 * @param msk -  master secret key
 * @param name -  name or identity of the web service
 * @param name_len -  name byte size
 * @param count -  child key count
 * @return child public key
 */
SECP256K1_API int secp256k1_boquila_derive_cpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        pk_t* cpk,
        const uint8_t* csk)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4);


SECP256K1_API int secp256k1_boquila_prove_memmpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t *mpks,
        const uint8_t* msk,
        const uint8_t* name,
        int name_len,
        pk_t * wpk,
        int32_t j,
        int32_t N, int m)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4);


SECP256K1_API int secp256k1_boquila_verify_memmpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t *mpks,
        pk_t * wpk,
        int32_t N, int m)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3);


SECP256K1_API int secp256k1_boquila_prove_newcpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        const uint8_t* msk,
        const uint8_t* r,
        const uint8_t* name,
        int name_len,
        pk_t * wpk,
        pk_t * cpk,
        uint8_t *chal)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4) SECP256K1_ARG_NONNULL(5);


SECP256K1_API int secp256k1_boquila_verify_newcpk(
        const secp256k1_context* ctx,
        const ringcip_context* rctx,
        uint8_t *proof,
        pk_t * wpk,
        pk_t * cpk,
        uint8_t *chal)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2) SECP256K1_ARG_NONNULL(3) SECP256K1_ARG_NONNULL(4) SECP256K1_ARG_NONNULL(5);




/**
 * Create the context object
 * @param ctx - main context
 * @param L -  range bit
 * @param n -
 * @param m -
 * @param gen_seed - generate seed
 * @return ring cip context
 */
SECP256K1_API  ringcip_context secp256k1_ringcip_context_create(
        const secp256k1_context* ctx,
        int L, int n, int m, uint8_t *gen_seed,
        secp256k1_generator *blind_gen)
SECP256K1_ARG_NONNULL(1);

/**
 * Clear the ring cip context
 * @param ctx - main context
 * @param rctx - ring cip context
 */
SECP256K1_API  void secp256k1_ringcip_context_clear(ringcip_context *rctx)
SECP256K1_ARG_NONNULL(1) SECP256K1_ARG_NONNULL(2);

/**
 * return the byte size of the proof
 * @param rctx
 * @return
 */
SECP256K1_API  int secp256k1_zero_mcom_get_size(const ringcip_context *rctx, int m);



# ifdef __cplusplus
}
# endif

#endif //SECP256K_NOPENI_SECP256K1_RINGCIP_H


