package main

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -lcrypto -lsecp256k1
// #include <stdlib.h>
// #include <openssl/rand.h>
// #include <stdlib.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <string.h>
// #include <secp256k1.h>
// #include <secp256k1_ringcip.h>
import "C"
import (
	"fmt"
	"unsafe"
)

func simulator1() (mskBytes []byte, mpkBytes []byte) {
	// initiation
	n := C.int(2)
	m := C.int(3)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(0), 32)
	rctx := C.secp256k1_ringcip_context_create(ctx, C.int(10), n, m, gen_seed, nil)

	// to create a single msk and mpk
	msk := make([]C.uint8_t, 64)
	mskPtr := (*C.uint8_t)(unsafe.Pointer(&msk[0]))
	C.RAND_bytes(mskPtr, 64)

	mpk := make([]C.pk_t, 1)
	if C.secp256k1_boquila_gen_mpk(ctx, &rctx, &mpk[0], mskPtr) == 0 {
		fmt.Println("mpk creation failed")
	}

	// Convert the results to Go byte arrays
	mskBytes = C.GoBytes(unsafe.Pointer(mskPtr), 64)
	mpkBytes = C.GoBytes(unsafe.Pointer(&mpk[0]), C.int(unsafe.Sizeof(mpk[0])))

	return mskBytes, mpkBytes
}

func main() {
	mskBytes, mpkBytes := simulator1()
	fmt.Printf("Returned MSK: %x\n", mskBytes)
	fmt.Printf("Returned MPK: %x\n", mpkBytes)
}
