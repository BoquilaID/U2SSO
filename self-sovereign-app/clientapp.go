package main

import (
	"context"
	"encoding/hex"
	"flag"
	"fmt"
	"github.com/ethereum/go-ethereum/common"
	"github.com/ethereum/go-ethereum/ethclient"
	"log"
	u2sso "main/u2sso"
	"math/big"
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

var contractFlag string
var clientFlag string
var skFlag string
var commandFlag string
var passkeypathFlag string
var snameFlag string
var challengeFlag string

func main() {
	commands := make([]string, 5)
	commands[0] = "create"
	commands[1] = "load"
	commands[2] = "register"
	commands[3] = "auth"
	commands[4] = "help"

	flag.StringVar(&contractFlag, "contract", "", "The hexAddress of your contract, e.g., 0xf...")
	flag.StringVar(&clientFlag, "client", "", "The ethereum client IP of your contract")
	flag.StringVar(&skFlag, "ethkey", "", "The private key of your account (without 0x), e.g., f...")
	flag.StringVar(&commandFlag, "command", "", "Choose commands create, check, load, register, auth, help")
	flag.StringVar(&passkeypathFlag, "keypath", "", "If you want to create or load an SSO-Id")
	flag.StringVar(&snameFlag, "sname", "", "Service name if you want to register or authenticate")
	flag.StringVar(&challengeFlag, "challenge", "", "Challenge given by the service if you want to register or authenticate")
	flag.Parse()

	log.SetFlags(log.LstdFlags | log.Lshortfile)

	var client *ethclient.Client
	var err error
	var instance *u2sso.U2sso
	var idsize *big.Int

	if commandFlag == "" {
		fmt.Println("Please provide a name using the -command flag from", commands)
		return
	}

	if clientFlag == "" {
		fmt.Println("No client address was given. Taking default: http://127.0.0.1:7545")
		clientFlag = "http://127.0.0.1:7545"
		client, err = ethclient.Dial("http://127.0.0.1:7545")
		if err != nil {
			fmt.Println("Error connecting to eth client", err)
		}
	}

	if contractFlag == "" {
		fmt.Println("Please provide a contract address using the -contract flag")
		return
	} else {
		contractAddress := common.HexToAddress(contractFlag)
		bytecode, err := client.CodeAt(context.Background(), contractAddress, nil) // nil is latest block
		if err != nil {
			log.Fatal(err)
		}
		isContract := len(bytecode) > 0
		instance, err = u2sso.NewU2sso(contractAddress, client)
		if err != nil || !isContract {
			fmt.Println("No U2SSO contract at ", contractFlag)
			return
		}
		fmt.Println("Found the contract at ", contractFlag) // is contract: true

		idsize, err = instance.GetIDSize(nil)
		if err != nil {
			fmt.Println("Could not get id size from ", contractFlag)
		}
		fmt.Println("Current id size:", idsize)
	}

	if commandFlag == "create" {
		if passkeypathFlag == "" {
			fmt.Println("Please provide a passkeypath using the -keypath flag to store your passkey")
			return
		}
		if skFlag == "" {
			fmt.Println("Please provide your ethereum account private key using the -ethkey flag (without 0x)")
			return
		}

		u2sso.CreatePasskey(passkeypathFlag)
		mskBytes, val := u2sso.LoadPasskey(passkeypathFlag)
		if !val {
			fmt.Println("could not create and load passkey")
			return
		}
		fmt.Println("passkey:", mskBytes)

		idBytes := u2sso.CreateID(mskBytes)
		fmt.Println("added ID to index:", u2sso.AddIDstoIdR(client, skFlag, instance, idBytes), ", ", idBytes)
		return
	}

	if commandFlag == "load" {
		if passkeypathFlag == "" {
			fmt.Println("Please provide a passkeypath using the -keypath flag to store your passkey")
			return
		}
		mskBytes, val := u2sso.LoadPasskey(passkeypathFlag)
		if !val {
			fmt.Println("could not create and load passkey")
			return
		}
		fmt.Println("passkey:", mskBytes)
	}

	if commandFlag == "register" {
		if passkeypathFlag == "" {
			fmt.Println("Please provide a passkeypath using the -keypath flag to store your passkey")
			return
		}
		// service name
		if snameFlag == "" {
			fmt.Println("Please provide a service name using the -sname flag")
			return
		}
		fmt.Println("service name size:", len(snameFlag))
		serviceName, err := hex.DecodeString(snameFlag)
		if err != nil {
			fmt.Println("Please provide a valid service name of hex characters")
		}

		// challenge
		if challengeFlag == "" {
			fmt.Println("Please provide a challenge using the -challenge flag")
			return
		}
		fmt.Println("challenge size:", len(challengeFlag))
		challenge, err := hex.DecodeString(challengeFlag)
		if err != nil {
			fmt.Println("Please provide a valid challenge of hex characters")
		}

		// ring size
		idsize, err = instance.GetIDSize(nil)
		if err != nil {
			fmt.Println("Could not get id size from ", passkeypathFlag)
			return
		}
		fmt.Println("total Id size:", idsize.Int64())
		if idsize.Int64() < 2 {
			fmt.Println("At least two SSO-Ids are required.. You can wait or create new SSO-Ids..")
			return
		}
		currentm := 1
		ringSize := 1
		for i := 1; i < u2sso.M; i++ {
			ringSize = u2sso.N * ringSize
			if ringSize >= int(idsize.Int64()) {
				currentm = i
				break
			}
		}
		fmt.Println("chosen ring size:", idsize.Int64(), " and m:", currentm)

		// passkey
		mskBytes, val := u2sso.LoadPasskey(passkeypathFlag)
		if !val {
			fmt.Println("could not create and load passkey")
			return
		}

		// get ID index
		idBytes := u2sso.CreateID(mskBytes)
		index := u2sso.GetIDIndexfromContract(instance, idBytes)
		if index == -1 {
			fmt.Println("The SSO-id for this passkey is not registered in the contract")
			return
		}

		IdList := u2sso.GetallActiveIDfromContract(instance)
		proofHex, spkBytes, val := u2sso.RegistrationProof(int(index), currentm, int(idsize.Int64()), serviceName, challenge, mskBytes, IdList)
		fmt.Println("proof hex format: ", proofHex)
		fmt.Println("spkBytes hex format: ", hex.EncodeToString(spkBytes))
		fmt.Println("N: ", idsize.Int64())
	}

	if commandFlag == "auth" {
		if passkeypathFlag == "" {
			fmt.Println("Please provide a passkeypath using the -keypath flag to store your passkey")
			return
		}
		// service name
		if snameFlag == "" {
			fmt.Println("Please provide a service name using the -sname flag")
			return
		}
		fmt.Println("service name size:", len(snameFlag))
		serviceName, err := hex.DecodeString(snameFlag)
		if err != nil {
			fmt.Println("Please provide a valid service name of hex characters")
		}

		// challenge
		if challengeFlag == "" {
			fmt.Println("Please provide a challenge using the -challenge flag")
			return
		}
		fmt.Println("challenge size:", len(challengeFlag))
		challenge, err := hex.DecodeString(challengeFlag)
		if err != nil {
			fmt.Println("Please provide a valid challenge of hex characters")
		}

		// passkey
		mskBytes, val := u2sso.LoadPasskey(passkeypathFlag)
		if !val {
			fmt.Println("could not create and load passkey")
			return
		}

		proofAuthHex, val := u2sso.AuthProof(serviceName, challenge, mskBytes)
		fmt.Println("proof auth hex format: ", proofAuthHex)
	}
}
