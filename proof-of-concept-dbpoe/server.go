package main

import (
	"bytes"
	"context"
	"crypto/sha256"
	"encoding/hex"
	"flag"
	"fmt"
	"log"
	u2sso "main/u2sso"
	"math/big"
	"net/http"
	"os"
	"strconv"
	"strings"

	"github.com/ethereum/go-ethereum/common"
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

type account struct {
	name       string
	spk        []byte
	nullifier  []byte
	currentn   int
	proof      string
	userip     string
	challenge  []byte
	registered bool
}

type authchallenges struct {
	authChallenge []byte
	userip        string
}

var registeredSPK = make([]account, 0)
var regAuthChallenges = make([]authchallenges, 0)

func newAccount(challenge []byte, userip string) account {
	acc := account{challenge: challenge, userip: userip}
	return acc
}

func newAuthChallenges(chal []byte, userip string) authchallenges {
	au := authchallenges{authChallenge: []byte(chal), userip: userip}
	return au
}

const sname = "dbpoe_service"
const SignupForm = "   <label>User Name </label><input name=\"name\" type=\"text\" value=\"\" /><br>\n    <label>Ring size (N) </label><input name=\"n\" type=\"number\" value=\"\" min=\"2\" max=\"1024\"/><br>\n   <label>Public Key (spk) </label><input name=\"spk\" type=\"text\" value=\"\" /><br>\n   <label>Nullifier </label><input name=\"nullifier\" type=\"text\" value=\"\" /><br>\n    <label>Membership Proof </label><input name=\"proof\" type=\"text\" value=\"\" /><br>\n    <input type=\"submit\" value=\"submit\" />\n  </form>"
const LoginForm = "<label>User Name </label><input name=\"name\" type=\"text\" value=\"\" /><br>\n        <label>Public Key (spk) </label><input name=\"spk\" type=\"text\" value=\"\" /><br>\n        <label>Digital Signature </label><input name=\"signature\" type=\"text\" value=\"\" /><br>\n        <input type=\"submit\" value=\"submit\" />\n    </form>"
const Header = "<head>\n<style> input[type=button], input[type=submit], input[type=reset] {\n  background-color: #04AA6D;\n  border: none;\n  color: white;\n  padding: 16px 32px;\n  text-decoration: none;\n  margin: 4px 2px;\n  cursor: pointer;\n}" +
	"textarea {\n  width: 100%;\n  height: 150px;\n  padding: 12px 20px;\n  box-sizing: border-box;\n  border: 2px solid #ccc;\n  border-radius: 4px;\n  background-color: #f8f8f8;\n  font-size: 16px;\n  resize: none;\n}\n" +
	"input[type=text] {\n  width: 100%;\n  padding: 12px 20px;\n  margin: 8px 0;\n  box-sizing: border-box;\n}\n</style>\n</head>"

var contractSFlag string
var clientSFlag string
var skFlag string
var services [][]byte

var instanceS *u2sso.U2sso

func main() {
	flag.StringVar(&contractSFlag, "contract", "", "The hexAddress of your contract, e.g., 0xf...")
	flag.StringVar(&clientSFlag, "client", "", "The ethereum client IP of your contract")
	flag.StringVar(&skFlag, "ethkey", "", "The private key of your account (without 0x), e.g., f...")

	flag.Parse()
	log.SetFlags(log.LstdFlags | log.Lshortfile)

	var client *ethclient.Client
	var err error
	var idsize *big.Int

	if clientSFlag == "" {
		fmt.Println("No client address was given. Taking default: http://127.0.0.1:7545")
		clientSFlag = "http://127.0.0.1:7545"
		client, err = ethclient.Dial("http://127.0.0.1:7545")
		if err != nil {
			fmt.Println("Error connecting to eth client", err)
		}
	}

	if contractSFlag == "" {
		fmt.Println("Please provide a contract address using the -contract flag")
		return
	} else {
		contractAddress := common.HexToAddress(contractSFlag)
		bytecode, err := client.CodeAt(context.Background(), contractAddress, nil) // nil is latest block
		if err != nil {
			log.Fatal(err)
		}
		isContract := len(bytecode) > 0
		instanceS, err = u2sso.NewU2sso(contractAddress, client)
		if err != nil || !isContract {
			fmt.Println("No U2SSO contract at ", contractSFlag)
			return
		}
		fmt.Println("Found the contract at ", contractSFlag) // is contract: true

		idsize, err = instanceS.GetIDSize(nil)
		if err != nil {
			fmt.Println("Could not get id size from ", contractSFlag)
		}
		fmt.Println("Current id size:", idsize)

		if skFlag == "" {
			fmt.Println("Please provide your ethereum account private key using the -ethkey flag (without 0x)")
			return
		}

		sha := sha256.New()
		sha.Write([]byte(sname))
		services, err = instanceS.GetAllServices(nil)

		//check if sha.Sum(nil) is in services
		found := false
		for i := 0; i < len(services); i++ {
			if bytes.Compare(services[i], sha.Sum(nil)) == 0 {
				found = true
			}
		}

		if !found {
			u2sso.AddServicetoContract(client, skFlag, instanceS, sha.Sum(nil))
			fmt.Println("Service registered to contract")
		} else {
			fmt.Println("Service already registered to contract")
		}

	}
	fileServer := http.FileServer(http.Dir("./static"))
	http.Handle("/", fileServer)
	http.HandleFunc("/directLogin", loginFormHandler)
	http.HandleFunc("/directSignup", signupFormHandler)
	http.HandleFunc("/signup", signupHandler)
	http.HandleFunc("/login", loginHandler)

	fmt.Printf("Server started at port 8080\n")
	if err := http.ListenAndServe(":8080", nil); err != nil {
		log.Fatal(err)
	}
}

func signupFormHandler(w http.ResponseWriter, r *http.Request) {
	// Generate dynamic content
	challenge := u2sso.CreateChallenge()
	acc := newAccount(challenge, userIP(r))
	registeredSPK = append(registeredSPK, acc)

	sha := sha256.New()
	sha.Write([]byte(sname))
	serviceName := hex.EncodeToString(sha.Sum(nil))

	// Read signup.html file
	htmlFilePath := "./static/signup.html"
	htmlContent, err := os.ReadFile(htmlFilePath)
	if err != nil {
		http.Error(w, "Could not load signup page", http.StatusInternalServerError)
		return
	}

	// Replace placeholders with dynamic content
	htmlContentStr := string(htmlContent)
	htmlContentStr = strings.Replace(htmlContentStr, "{{CHALLENGE}}", hex.EncodeToString(challenge), -1)
	htmlContentStr = strings.Replace(htmlContentStr, "{{SERVICE_NAME}}", serviceName, -1)

	// Serve the updated HTML
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	fmt.Fprint(w, htmlContentStr)
}

func loginFormHandler(w http.ResponseWriter, r *http.Request) {
	// Generate dynamic content
	challenge := u2sso.CreateChallenge()
	au := newAuthChallenges(challenge, userIP(r))
	regAuthChallenges = append(regAuthChallenges, au)

	sha := sha256.New()
	sha.Write([]byte(sname))
	serviceName := hex.EncodeToString(sha.Sum(nil))

	// Read login.html file
	htmlFilePath := "./static/login.html"
	htmlContent, err := os.ReadFile(htmlFilePath)
	if err != nil {
		http.Error(w, "Could not load login page", http.StatusInternalServerError)
		return
	}

	// Replace placeholders with dynamic content
	htmlContentStr := string(htmlContent)
	htmlContentStr = strings.Replace(htmlContentStr, "{{CHALLENGE}}", hex.EncodeToString(challenge), -1)
	htmlContentStr = strings.Replace(htmlContentStr, "{{SERVICE_NAME}}", serviceName, -1)

	// Serve the updated HTML
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	fmt.Fprint(w, htmlContentStr)
}

func signupHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	if err := r.ParseForm(); err != nil {
		fmt.Fprintf(w, "ParseForm() err: %v", err)
		return
	}

	name := r.FormValue("name")
	challenge, err := hex.DecodeString(r.FormValue("challenge"))
	if err != nil {
		fmt.Fprintf(w, "challenge err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return

	}
	spkBytes, err := hex.DecodeString(r.FormValue("spk"))
	if err != nil {
		fmt.Fprintf(w, "spkBytes err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return
	}
	serviceName, err := hex.DecodeString(r.FormValue("sname"))
	if err != nil {
		fmt.Fprintf(w, "serviceName err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return
	}
	nullifier, err := hex.DecodeString(r.FormValue("nullifier"))
	if err != nil {
		fmt.Fprintf(w, "nullifier err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return
	}

	proofHex := r.FormValue("proof")
	ringSize, err := strconv.Atoi(r.FormValue("n"))
	if err != nil {
		fmt.Fprintf(w, "ringSize err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return
	}
	currentm := 1
	tmp := 1
	for currentm = 1; currentm < u2sso.M; currentm++ {
		tmp *= u2sso.N
		if tmp >= ringSize {
			break
		}
	}
	IdList := u2sso.GetallActiveIDfromContract(instanceS)

	topicList, err := instanceS.GetAllServices(nil)
	if err != nil {
		fmt.Println("Could not get topic list")
		return
	}
	topicListSize := len(topicList)
	topicIndex := u2sso.GetServiceIndexfromContract(instanceS, serviceName)
	res := u2sso.RegistrationVerify(proofHex, currentm, ringSize, serviceName, challenge, IdList, spkBytes, topicList, topicListSize, int(topicIndex), nullifier)

	if res {
		for i := 0; i < len(registeredSPK); i++ {
			if bytes.Compare(registeredSPK[i].challenge, challenge) == 0 {
				registeredSPK[i].name = name
				registeredSPK[i].spk = spkBytes
				registeredSPK[i].nullifier = nullifier
				registeredSPK[i].proof = proofHex
				registeredSPK[i].registered = true
			}
		}
		// Load the registration_success.html file
		htmlFilePath := "./static/registration_success.html"
		htmlContent, err := os.ReadFile(htmlFilePath)
		if err != nil {
			http.Error(w, "Could not load registration success page", http.StatusInternalServerError)
			return
		}

		// Replace {{NAME}} with the actual user's name
		htmlContentStr := strings.Replace(string(htmlContent), "{{NAME}}", name, -1)

		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprint(w, htmlContentStr)
	} else {
		// Load the registration_fail.html file
		htmlFilePath := "./static/registration_fail.html"
		htmlContent, err := os.ReadFile(htmlFilePath)
		if err != nil {
			http.Error(w, "Could not load registration fail page", http.StatusInternalServerError)
			return
		}

		// Serve the registration fail page
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprint(w, string(htmlContent))
	}
}

func loginHandler(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", "text/html; charset=utf-8")
	if err := r.ParseForm(); err != nil {
		fmt.Fprintf(w, "ParseForm() err: %v", err)
		return
	}

	name := r.FormValue("name")
	challenge, err := hex.DecodeString(r.FormValue("challenge"))
	if err != nil {
		fmt.Fprintf(w, "challenge err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return

	}
	spkBytes, err := hex.DecodeString(r.FormValue("spk"))
	if err != nil {
		fmt.Fprintf(w, "spkBytes err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return
	}
	serviceName, err := hex.DecodeString(r.FormValue("sname"))
	if err != nil {
		fmt.Fprintf(w, "serviceName err: %v\n", err)
		fmt.Fprintf(w, "Sign up request failed\n")
		return
	}
	signature := r.FormValue("signature")
	topicList, err := instanceS.GetAllServices(nil)
	if err != nil {
		fmt.Println("Could not get topic list")
		return
	}
	topicListSize := len(topicList)
	res := u2sso.AuthVerify(signature, serviceName, challenge, spkBytes, topicList, topicListSize)
	res2 := false

	for i := 0; i < len(registeredSPK); i++ {
		if bytes.Compare(registeredSPK[i].spk, spkBytes) == 0 {
			res2 = true
		}
	}

	if res && res2 {
		// fmt.Fprintf(w, "<b>Welcome %s! <b><br>\n", name)
		// fmt.Fprintf(w, "log in request successful\n")

		// Load the registration_success.html file
		htmlFilePath := "./static/login_success.html"
		htmlContent, err := os.ReadFile(htmlFilePath)
		if err != nil {
			http.Error(w, "Could not load login success page", http.StatusInternalServerError)
			return
		}

		// Replace {{NAME}} with the actual user's name
		htmlContentStr := strings.Replace(string(htmlContent), "{{NAME}}", name, -1)

		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprint(w, htmlContentStr)
	} else {
		// Load the registration_fail.html file
		htmlFilePath := "./static/login_fail.html"
		htmlContent, err := os.ReadFile(htmlFilePath)
		if err != nil {
			http.Error(w, "Could not load login fail page", http.StatusInternalServerError)
			return
		}

		// Serve the registration fail page
		w.Header().Set("Content-Type", "text/html; charset=utf-8")
		fmt.Fprint(w, string(htmlContent))
	}
}

func userIP(r *http.Request) string {
	IPAddress := r.Header.Get("X-Real-Ip")
	if IPAddress == "" {
		IPAddress = r.Header.Get("X-Forwarded-For")
	}
	if IPAddress == "" {
		IPAddress = r.RemoteAddr
	}
	return IPAddress
}
