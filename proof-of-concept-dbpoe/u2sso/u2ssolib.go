package u2sso

import (
	"context"
	"crypto/ecdsa"
	"encoding/hex"
	"fmt"
	"io"
	"log"
	"math/big"
	"os"
	"unsafe"

	"github.com/ethereum/go-ethereum/accounts/abi/bind"
	"github.com/ethereum/go-ethereum/crypto"
	"github.com/ethereum/go-ethereum/ethclient"
)

// #cgo CFLAGS: -g -Wall
// #cgo LDFLAGS: -lcrypto -lsecp256k1
// #include <stdlib.h>
// #include <stdint.h>
// #include <string.h>
// #include <openssl/rand.h>
// #include <secp256k1.h>
// #include <secp256k1_ringcip.h>
import "C"

const gen_seed_fix = 11
const N = 2
const M = 10

func CreateChallenge() []byte {
	chal := make([]C.uint8_t, 32)
	chalPtr := (*C.uint8_t)(unsafe.Pointer(&chal[0]))
	C.RAND_bytes(chalPtr, 32)
	chalBytes := C.GoBytes(unsafe.Pointer(chalPtr), 32)
	return chalBytes
}

func CreatePasskey(filename string) {
	msk := make([]C.uint8_t, 32)
	mskPtr := (*C.uint8_t)(unsafe.Pointer(&msk[0]))
	C.RAND_bytes(mskPtr, 32)
	mskBytes := C.GoBytes(unsafe.Pointer(mskPtr), 32)

	file, err := os.OpenFile(filename, os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		log.Fatal(err)
	}
	// close fi on exit and check for its returned error
	defer func() {
		if err := file.Close(); err != nil {
			log.Fatal(err)
		}
	}()
	// write a chunk
	if _, err := file.Write(mskBytes); err != nil {
		panic(err)
	}
}

func LoadPasskey(filename string) ([]byte, bool) {
	mskBytes := make([]byte, 32)

	file, err := os.Open(filename)
	if err != nil {
		log.Fatal(err)
	}
	// close fi on exit and check for its returned error
	defer func() {
		if err := file.Close(); err != nil {
			log.Fatal(err)
		}
	}()
	n, err := file.Read(mskBytes)
	if err != nil && err != io.EOF {
		log.Fatal(err)
	}
	if n == 0 {
		return nil, false
	}
	return mskBytes, true
}

func CreateID(mskBytes []byte, topicList [][]byte, topicSize int) []byte {
	msk := make([]C.uint8_t, 32)
	mskPtr := (*C.uint8_t)(unsafe.Pointer(&msk[0]))
	for i := 0; i < 32; i++ {
		msk[i] = C.uint8_t(mskBytes[i])
	}
	//L := C.int(10)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(gen_seed_fix), 32) // 11
	totalLength := 0
	for _, topic := range topicList {
		totalLength += len(topic)
	}
	topicListC := C.malloc(C.size_t(totalLength))
	topicListPtr := (*C.uint8_t)(topicListC)
	offset := 0
	for _, topic := range topicList {
		topicBytes := []byte(topic)
		for j := 0; j < len(topicBytes); j++ {
			*(*C.uint8_t)(unsafe.Pointer(uintptr(unsafe.Pointer(topicListPtr)) + uintptr(offset+j))) = C.uint8_t(topicBytes[j])
		}
		offset += len(topicBytes)
	}
	rctx := C.secp256k1_ringcip_DBPoE_context_create(ctx, C.int(10), C.int(N), C.int(M), gen_seed, topicListPtr, C.int(topicSize))

	mpk := make([]C.pk_t, 1)
	if C.secp256k1_boquila_gen_DBPoE_mpk(ctx, &rctx, &mpk[0], mskPtr) == 0 {
		fmt.Println("mpk creation failed")
	}
	mpkBytes := C.GoBytes(unsafe.Pointer(&mpk[0]), C.int(unsafe.Sizeof(mpk[0])))
	return mpkBytes
}

/*
M should be log(N)
*/
func RegistrationProof(index int, currentm int, currentN int, serviceName []byte, challenge []byte, mskBytes []byte, idList [][]byte, topicList [][]byte, topicSize int, topicIndex int) (string, []byte, []byte, bool) {
	msk := make([]C.uint8_t, 32)
	ssk := make([]C.uint8_t, 32)
	mskPtr := (*C.uint8_t)(unsafe.Pointer(&msk[0]))
	sskPtr := (*C.uint8_t)(unsafe.Pointer(&ssk[0]))
	for i := 0; i < 32; i++ {
		msk[i] = C.uint8_t(mskBytes[i])
	}
	chal := make([]C.uint8_t, 32)
	chalPtr := (*C.uint8_t)(unsafe.Pointer(&chal[0]))
	for i := 0; i < 32; i++ {
		chal[i] = C.uint8_t(challenge[i])
	}
	//L := C.int(10)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(gen_seed_fix), 32) // 11
	totalLength := 0
	for _, topic := range topicList {
		totalLength += len(topic)
	}
	topicListC := C.malloc(C.size_t(totalLength))
	topicListPtr := (*C.uint8_t)(topicListC)
	offset := 0
	for _, topic := range topicList {
		topicBytes := []byte(topic)
		for j := 0; j < len(topicBytes); j++ {
			*(*C.uint8_t)(unsafe.Pointer(uintptr(unsafe.Pointer(topicListPtr)) + uintptr(offset+j))) = C.uint8_t(topicBytes[j])
		}
		offset += len(topicBytes)
	}
	rctx := C.secp256k1_ringcip_DBPoE_context_create(ctx, C.int(10), C.int(N), C.int(M), gen_seed, topicListPtr, C.int(topicSize))

	// create proof
	// create a webkey for mpk j
	snameSize := len(serviceName)
	name := make([]C.uint8_t, snameSize)
	namePtr := (*C.uint8_t)(unsafe.Pointer(&name[0]))
	for i := 0; i < snameSize; i++ {
		name[i] = C.uint8_t(serviceName[i])
	}
	name_len := C.int(32)

	var spk C.pk_t

	if int(C.secp256k1_boquila_derive_ssk(ctx, sskPtr, mskPtr, namePtr, name_len)) == 0 {
		fmt.Println("ssk creation failed")
		return "", nil, nil, false
	}
	if int(C.secp256k1_boquila_derive_DBPoE_spk(ctx, &rctx, &spk, sskPtr)) == 0 {
		fmt.Println("spk creation failed")
		return "", nil, nil, false
	}
	proofnew := make([]C.uint8_t, 65)
	proofnewPtr := (*C.uint8_t)(unsafe.Pointer(&proofnew[0]))
	r := make([]C.uint8_t, 32)
	rPtr := (*C.uint8_t)(unsafe.Pointer(&r[0]))
	C.RAND_bytes(rPtr, 32)
	if int(C.secp256k1_boquila_DBPoE_auth_prove(ctx, &rctx, proofnewPtr, mskPtr, rPtr, namePtr, name_len, &spk, chalPtr)) == 0 {
		fmt.Println("auth prove failed")
		return "", nil, nil, false
	}
	if int(C.secp256k1_boquila_DBPoE_auth_verify(ctx, &rctx, proofnewPtr, &spk, chalPtr)) == 0 {
		fmt.Println("auth verify failed")
		return "", nil, nil, false
	}

	// arrange ID list
	mpks := make([]C.pk_t, C.int(currentN))
	mpksPtr := (*C.pk_t)(unsafe.Pointer(&mpks[0]))
	for i := 0; i < currentN; i++ {
		for l := 0; l < 33; l++ {
			mpks[i].buf[l] = C.uint8_t(idList[i][l])
		}
	}

	// create a membership proof
	proofLen := C.secp256k1_zero_mcom_DBPoE_get_size(&rctx, C.int(currentm))
	proof := make([]C.uint8_t, proofLen)
	proofPtr := (*C.uint8_t)(unsafe.Pointer(&proof[0]))

	nullifierC := make([]C.uint8_t, 32)
	nullifierPtr := (*C.uint8_t)(unsafe.Pointer(&nullifierC[0]))
	if int(C.secp256k1_boquila_prove_DBPoE_memmpk(ctx, &rctx, proofPtr, nullifierPtr, mpksPtr, mskPtr, chalPtr,
		namePtr, name_len, &spk, C.int(index), C.int(topicIndex), C.int(currentN), C.int(currentm))) == 0 {
		fmt.Println("proving membership failed")
		return "", nil, nil, false
	}

	// Convert proof to hex string
	proofHex := hex.EncodeToString(C.GoBytes(unsafe.Pointer(proofPtr), proofLen))
	proofHexConverted, err := hex.DecodeString(proofHex)
	if err != nil {
		log.Fatal(err)
	}
	proof = make([]C.uint8_t, proofLen)
	proofPtr = (*C.uint8_t)(unsafe.Pointer(&proof[0]))
	for i := 0; i < int(proofLen); i++ {
		proof[i] = C.uint8_t(proofHexConverted[i])
	}

	if int(C.secp256k1_boquila_verify_DBPoE_memmpk(ctx, &rctx, proofPtr, nullifierPtr, mpksPtr, chalPtr,
		namePtr, name_len, &spk, C.int(topicIndex), C.int(currentN), C.int(currentm))) == 0 {
		fmt.Println("verifying membership failed")
		return "", nil, nil, false
	}
	spkBytes := C.GoBytes(unsafe.Pointer(&spk.buf[0]), C.int(33))
	nullifier := C.GoBytes(unsafe.Pointer(nullifierPtr), 32)
	return proofHex, spkBytes, nullifier, true
}

/*
M should be log(N)
*/
func RegistrationVerify(proofHex string, currentm int, currentN int, serviceName []byte, challenge []byte, idList [][]byte, spkBytes []byte, topicList [][]byte, topicSize int, topicIndex int, nullifier []byte) bool {
	chal := make([]C.uint8_t, 32)
	chalPtr := (*C.uint8_t)(unsafe.Pointer(&chal[0]))
	for i := 0; i < 32; i++ {
		chal[i] = C.uint8_t(challenge[i])
	}
	//L := C.int(10)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(gen_seed_fix), 32) // 11
	totalLength := 0
	for _, topic := range topicList {
		totalLength += len(topic)
	}
	topicListC := C.malloc(C.size_t(totalLength))
	topicListPtr := (*C.uint8_t)(topicListC)
	offset := 0
	for _, topic := range topicList {
		topicBytes := []byte(topic)
		for j := 0; j < len(topicBytes); j++ {
			*(*C.uint8_t)(unsafe.Pointer(uintptr(unsafe.Pointer(topicListPtr)) + uintptr(offset+j))) = C.uint8_t(topicBytes[j])
		}
		offset += len(topicBytes)
	}
	rctx := C.secp256k1_ringcip_DBPoE_context_create(ctx, C.int(10), C.int(N), C.int(M), gen_seed, topicListPtr, C.int(topicSize))

	// create proof
	// create a webkey for mpk j
	name := make([]C.uint8_t, 32)
	namePtr := (*C.uint8_t)(unsafe.Pointer(&name[0]))
	for i := 0; i < 32; i++ {
		name[i] = C.uint8_t(serviceName[i])
	}
	name_len := C.int(32)

	var spk C.pk_t
	for i := 0; i < 33; i++ {
		spk.buf[i] = C.uint8_t(spkBytes[i])
	}

	// arrange ID list
	mpks := make([]C.pk_t, C.int(currentN))
	mpksPtr := (*C.pk_t)(unsafe.Pointer(&mpks[0]))
	for i := 0; i < currentN; i++ {
		for l := 0; l < 33; l++ {
			mpks[i].buf[l] = C.uint8_t(idList[i][l])
		}
	}

	proofLen := C.secp256k1_zero_mcom_DBPoE_get_size(&rctx, C.int(currentm))
	proofHexConverted, err := hex.DecodeString(proofHex)
	if err != nil {
		log.Fatal(err)
	}
	proof := make([]C.uint8_t, proofLen)
	proofPtr := (*C.uint8_t)(unsafe.Pointer(&proof[0]))
	for i := 0; i < int(proofLen); i++ {
		proof[i] = C.uint8_t(proofHexConverted[i])
	}

	nul := make([]C.uint8_t, 32)
	nullifierPtr := (*C.uint8_t)(unsafe.Pointer(&nul[0]))
	for i := 0; i < 32; i++ {
		nul[i] = C.uint8_t(nullifier[i])
	}
	if int(C.secp256k1_boquila_verify_DBPoE_memmpk(ctx, &rctx, proofPtr, nullifierPtr, mpksPtr, chalPtr,
		namePtr, name_len, &spk, C.int(topicIndex), C.int(currentN), C.int(currentm))) == 0 {
		fmt.Println("verifying membership failed")
		return false
	}
	return true
}

func AuthProof(serviceName []byte, challenge []byte, mskBytes []byte, topicList [][]byte, topicSize int) (string, bool) {
	msk := make([]C.uint8_t, 32)
	ssk := make([]C.uint8_t, 32)
	mskPtr := (*C.uint8_t)(unsafe.Pointer(&msk[0]))
	sskPtr := (*C.uint8_t)(unsafe.Pointer(&ssk[0]))
	for i := 0; i < 32; i++ {
		msk[i] = C.uint8_t(mskBytes[i])
	}
	chal := make([]C.uint8_t, 32)
	chalPtr := (*C.uint8_t)(unsafe.Pointer(&chal[0]))
	for i := 0; i < 32; i++ {
		chal[i] = C.uint8_t(challenge[i])
	}
	//L := C.int(10)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(gen_seed_fix), 32) // 11
	totalLength := 0
	for _, topic := range topicList {
		totalLength += len(topic)
	}
	topicListC := C.malloc(C.size_t(totalLength))
	topicListPtr := (*C.uint8_t)(topicListC)
	offset := 0
	for _, topic := range topicList {
		topicBytes := []byte(topic)
		for j := 0; j < len(topicBytes); j++ {
			*(*C.uint8_t)(unsafe.Pointer(uintptr(unsafe.Pointer(topicListPtr)) + uintptr(offset+j))) = C.uint8_t(topicBytes[j])
		}
		offset += len(topicBytes)
	}
	rctx := C.secp256k1_ringcip_DBPoE_context_create(ctx, C.int(10), C.int(N), C.int(M), gen_seed, topicListPtr, C.int(topicSize))

	// create proof
	// create a webkey for mpk j
	name := make([]C.uint8_t, 32)
	namePtr := (*C.uint8_t)(unsafe.Pointer(&name[0]))
	for i := 0; i < 32; i++ {
		name[i] = C.uint8_t(serviceName[i])
	}
	name_len := C.int(32)

	var spk C.pk_t

	if int(C.secp256k1_boquila_derive_ssk(ctx, sskPtr, mskPtr, namePtr, name_len)) == 0 {
		fmt.Println("ssk creation failed")
		return "", false
	}
	if int(C.secp256k1_boquila_derive_DBPoE_spk(ctx, &rctx, &spk, sskPtr)) == 0 {
		fmt.Println("spk creation failed")
		return "", false
	}
	proofnew := make([]C.uint8_t, 65)
	proofnewPtr := (*C.uint8_t)(unsafe.Pointer(&proofnew[0]))
	r := make([]C.uint8_t, 32)
	rPtr := (*C.uint8_t)(unsafe.Pointer(&r[0]))
	C.RAND_bytes(rPtr, 32)
	if int(C.secp256k1_boquila_DBPoE_auth_prove(ctx, &rctx, proofnewPtr, mskPtr, rPtr, namePtr, name_len, &spk, chalPtr)) == 0 {
		fmt.Println("auth prove failed")
		return "", false
	}
	if int(C.secp256k1_boquila_DBPoE_auth_verify(ctx, &rctx, proofnewPtr, &spk, chalPtr)) == 0 {
		fmt.Println("auth verify failed")
		return "", false
	}

	// Convert proof to hex string
	proofHex := hex.EncodeToString(C.GoBytes(unsafe.Pointer(proofnewPtr), 65))
	return proofHex, true
}

func AuthVerify(proofHex string, serviceName []byte, challenge []byte, spkBytes []byte, topicList [][]byte, topicSize int) bool {
	chal := make([]C.uint8_t, 32)
	chalPtr := (*C.uint8_t)(unsafe.Pointer(&chal[0]))
	for i := 0; i < 32; i++ {
		chal[i] = C.uint8_t(challenge[i])
	}
	//L := C.int(10)
	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(gen_seed_fix), 32) // 11

	totalLength := 0
	for _, topic := range topicList {
		totalLength += len(topic)
	}
	topicListC := C.malloc(C.size_t(totalLength))
	topicListPtr := (*C.uint8_t)(topicListC)
	offset := 0
	for _, topic := range topicList {
		topicBytes := []byte(topic)
		for j := 0; j < len(topicBytes); j++ {
			*(*C.uint8_t)(unsafe.Pointer(uintptr(unsafe.Pointer(topicListPtr)) + uintptr(offset+j))) = C.uint8_t(topicBytes[j])
		}
		offset += len(topicBytes)
	}
	rctx := C.secp256k1_ringcip_DBPoE_context_create(ctx, C.int(10), C.int(N), C.int(M), gen_seed, topicListPtr, C.int(topicSize))

	var spk C.pk_t
	for i := 0; i < 33; i++ {
		spk.buf[i] = C.uint8_t(spkBytes[i])
	}

	proofHexConverted, err := hex.DecodeString(proofHex)
	if err != nil {
		log.Fatal(err)
	}
	proof := make([]C.uint8_t, 65)
	proofPtr := (*C.uint8_t)(unsafe.Pointer(&proof[0]))
	for i := 0; i < int(65); i++ {
		proof[i] = C.uint8_t(proofHexConverted[i])
	}

	if int(C.secp256k1_boquila_DBPoE_auth_verify(ctx, &rctx, proofPtr, &spk, chalPtr)) == 0 {
		fmt.Println("auth verify failed")
		return false
	}
	return true
}

/*
*
Adds an service to the contract
*/
func AddServicetoContract(client *ethclient.Client, sk string, inst *U2sso, service []byte) {
	privateKey, err := crypto.HexToECDSA(sk)
	if err != nil {
		log.Fatal(err)
	}
	publicKey := privateKey.Public()
	publicKeyECDSA, ok := publicKey.(*ecdsa.PublicKey)
	if !ok {
		log.Fatal("cannot assert type: publicKey is not of type *ecdsa.PublicKey")
	}
	fromAddress := crypto.PubkeyToAddress(*publicKeyECDSA)
	nonce, err := client.PendingNonceAt(context.Background(), fromAddress)
	if err != nil {
		log.Fatal(err)
	}
	gasPrice, err := client.SuggestGasPrice(context.Background())
	if err != nil {
		log.Fatal(err)
	}
	auth := bind.NewKeyedTransactor(privateKey)
	auth.Nonce = big.NewInt(int64(nonce))
	auth.Value = big.NewInt(0)     // in wei
	auth.GasLimit = uint64(300000) // in units
	auth.GasPrice = gasPrice
	_, err = inst.RegisterService(auth, service)
	if err != nil {
		log.Fatal(err)
	}
}

/*
*
Adds an id to the contract
*/
func AddIDstoIdR(client *ethclient.Client, sk string, inst *U2sso, id []byte) int64 {
	privateKey, err := crypto.HexToECDSA(sk)
	if err != nil {
		log.Fatal(err)
	}

	publicKey := privateKey.Public()
	publicKeyECDSA, ok := publicKey.(*ecdsa.PublicKey)
	if !ok {
		log.Fatal("cannot assert type: publicKey is not of type *ecdsa.PublicKey")
	}

	fromAddress := crypto.PubkeyToAddress(*publicKeyECDSA)
	nonce, err := client.PendingNonceAt(context.Background(), fromAddress)
	if err != nil {
		log.Fatal(err)
	}

	gasPrice, err := client.SuggestGasPrice(context.Background())
	if err != nil {
		log.Fatal(err)
	}

	auth := bind.NewKeyedTransactor(privateKey)
	auth.Nonce = big.NewInt(int64(nonce))
	auth.Value = big.NewInt(0)     // in wei
	auth.GasLimit = uint64(300000) // in units
	auth.GasPrice = gasPrice

	//key := [32]byte{}
	//copy(key[:], []byte("foo"))
	byte32 := new(big.Int).SetBytes(id[:32])
	byte33 := new(big.Int).SetBytes(id[32:])

	_, err = inst.AddID(auth, byte32, byte33)
	if err != nil {
		log.Fatal(err)
	}

	index, err := inst.GetIDIndex(nil, byte32, byte33)
	if err != nil {
		log.Fatal(err)
	}

	return index.Int64()
}

/*
Gives the total ID size in the contract
*/
func GetIDfromContract(inst *U2sso) int64 {
	result, err := inst.GetIDSize(nil)
	if err != nil {
		log.Fatal(err)
	}

	return result.Int64()
}

/*
Gives the total ID index in the contract
*/
func GetIDIndexfromContract(inst *U2sso, id []byte) int64 {
	//key := [32]byte{}
	//copy(key[:], []byte("foo"))
	byte32 := new(big.Int).SetBytes(id[:32])
	byte33 := new(big.Int).SetBytes(id[32:])

	index, err := inst.GetIDIndex(nil, byte32, byte33)
	if err != nil {
		log.Fatal(err)
	}

	return index.Int64()
}

func GetServiceIndexfromContract(inst *U2sso, service []byte) int64 {
	index, err := inst.GetServiceIndex(nil, service)
	if err != nil {
		log.Fatal(err)
	}

	return index.Int64()
}

/*
Get all active IDs from the list
*/
func GetallActiveIDfromContract(inst *U2sso) [][]byte {

	idlist := make([][]byte, 0)

	idSize, err := inst.GetIDSize(nil)
	if err != nil {
		log.Fatal(err)
	}
	i := int64(0)
	for i = 0; i < idSize.Int64(); i++ {
		index := big.NewInt(i)
		state, err := inst.GetState(nil, index)
		if err != nil {
			log.Fatal(err)
		}
		if state == true {
			idbytes32, idbyte33, err := inst.GetIDs(nil, index)
			if err != nil {
				log.Fatal(err)
			}
			id := idbytes32.Bytes()
			if idbyte33.Int64() == 0 {
				id = append(id, byte(0))
			} else {
				id = append(id, idbyte33.Bytes()[0])
			}

			idlist = append(idlist, id)
		}
	}

	return idlist
}
