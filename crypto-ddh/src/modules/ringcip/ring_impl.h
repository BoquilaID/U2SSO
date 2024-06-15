//
// Created by jayamine on 6/13/24.
//

#ifndef BOQUILA_RING_IMPL_H
#define BOQUILA_RING_IMPL_H

#include "include/secp256k1_ringcip.h"



static void secp256k1_ge_save1(unsigned char *buf, secp256k1_ge* ge) {
    secp256k1_fe_normalize(&ge->x);
    secp256k1_fe_get_b32(&buf[1], &ge->x);
    buf[0] = 9 ^ secp256k1_fe_is_quad_var(&ge->y);
}

static void secp256k1_ge_load1(unsigned char *buf, secp256k1_ge* ge) {
    secp256k1_fe fe;
    secp256k1_fe_set_b32(&fe, &buf[1]);
    secp256k1_ge_set_xquad(ge, &fe);
    if (buf[0] & 1) {
        secp256k1_ge_neg(ge, ge);
    }
}



#endif //BOQUILA_RING_IMPL_H
