package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -lcrypto -lsecp256k1
// #include <stdlib.h>
// #include "stdlib.h"
// #include <openssl/rand.h>
// #include <stdlib.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>
// #include <secp256k1.h>
// #include <secp256k1_ringcip.h>
import "C"
import (
	"encoding/hex"
	"fmt"
	"unsafe"
)

type byteStruct struct {
	Buf *C.uint8_t
}

func main() {
	proof := gen_proof()
	fmt.Println(proof)
}

func gen_proof() string {
	// initiation
	n := C.int(2)
	m := C.int(3)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(0), 32)
	rctx := C.secp256k1_ringcip_context_create(ctx, C.int(10), n, m, gen_seed, nil)

	// master public key
	mpks := make([]C.pk_t, 8)
	mpksPtr := (*C.pk_t)(unsafe.Pointer(&mpks[0]))
	//mpks := make([]C.pk_t, 1024)
	msks := make([]byteStruct, 8)
	for i := 0; i < 8; i++ {
		msks[i].Buf = (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 64))
		defer C.free(unsafe.Pointer(msks[i].Buf))

		C.RAND_bytes(msks[i].Buf, 64) // getting the msk
		if C.secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i].Buf) == 0 {
			fmt.Println("mpk creation failed")
		}
	}

	// create a webkey for mpk j
	name := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 4))
	defer C.free(unsafe.Pointer(name))
	C.memset(unsafe.Pointer(name), C.int(1), 4)
	name_len := C.int(4)
	j := C.int(1) // index of your mpk
	var wpk C.pk_t
	if C.secp256k1_boquila_derive_webpk(ctx, &rctx, &wpk, msks[0].Buf, name, name_len) == C.int(0) {
		fmt.Println("wpk creation failed")
	}
	fmt.Println("passed")

	// create a membership proof
	N := C.int(8)
	proofLen := C.secp256k1_zero_mcom_get_size(&rctx, m)
	fmt.Println("prooflen:", proofLen)
	proof := make([]C.uint8_t, proofLen)
	proofPtr := (*C.uint8_t)(unsafe.Pointer(&proof[0]))

	if C.secp256k1_boquila_prove_memmpk(ctx, &rctx, proofPtr, mpksPtr, msks[0].Buf, name, name_len, &wpk, j, N, m) == 0 {
		fmt.Println("proving membership failed")
	}

	// Convert proof to hex string
	proofHex := hex.EncodeToString(C.GoBytes(unsafe.Pointer(proofPtr), proofLen))
	return proofHex
}
