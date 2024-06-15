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
	"fmt"
	"html/template"
	"log"
	"net/http"
	"path/filepath"
	"unsafe"

	"github.com/gorilla/mux"
	"gorm.io/driver/sqlite"
	"gorm.io/gorm"

	"boquila/app/config"
	"boquila/app/models"
	"boquila/app/routes"
)

type byteStruct struct {
	Buf *C.uint8_t
}

func main() {
	config.InitConfig()

	db, err := gorm.Open(sqlite.Open(config.DatabaseURL), &gorm.Config{})
	if err != nil {
		log.Fatalf("failed to connect to the database: %v", err)
	}

	ctx := C.secp256k1_context_create(C.SECP256K1_CONTEXT_SIGN | C.SECP256K1_CONTEXT_VERIFY)
	gen_seed := (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 32))
	defer C.free(unsafe.Pointer(gen_seed))
	C.memset(unsafe.Pointer(gen_seed), C.int(0), 32)
	rctx := C.secp256k1_ringcip_context_create(ctx, C.int(10), C.int(3), C.int(3), gen_seed, nil)

	// _ = C.secp256k1_ringcip_context_create(ctx, C.int(10), C.int(3), C.int(3), gen_seed, nil)

	mpks := make([]C.pk_t, 1024)
	msks := make([]byteStruct, 1024)
	for i := 0; i < 1024; i++ {
		msks[i].Buf = (*C.uint8_t)(C.malloc(C.sizeof_uint8_t * 64))
		defer C.free(unsafe.Pointer(msks[i].Buf))

		C.RAND_bytes(msks[i].Buf, 64)
		if C.secp256k1_boquila_gen_mpk(ctx, &rctx, &mpks[i], msks[i].Buf) == 0 {
			fmt.Println("mpk creation failed")
		}
	}

	db.AutoMigrate(&models.User{}, &models.Challenge{})

	r := mux.NewRouter()
	routes.SetupRoutes(r, db)

	r.PathPrefix("/static/").Handler(http.StripPrefix("/static/", http.FileServer(http.Dir("./static/"))))

	r.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		tmpl, _ := template.ParseFiles(filepath.Join("templates", "index.html"))
		tmpl.Execute(w, nil)
	})

	log.Println("Server running on port 8080")
	log.Fatal(http.ListenAndServe(":8080", r))
}
