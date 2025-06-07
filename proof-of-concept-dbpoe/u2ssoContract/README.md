# U2SSO contract

This contract stores a SSO-Id of 33 bytes.

``truffle compile``
``truffle test``

`` solc --abi contracts/U2SSO.sol -o build``

``export GOPATH=/<HOME_PATH>/go/``
``export PATH=$PATH:$GOROOT/bin:$GOPATH/bin``
`` go install github.com/ethereum/go-ethereum/cmd/abigen@latest``
``abigen --abi=./build/U2SSO.abi --pkg=u2sso --out=U2SSO.go``