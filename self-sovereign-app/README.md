# Boquila self-sovereign app 

This document explains how to run the proof of concept website with u2sso.

### deploy contract

Deploy the contract in `u2ssoContract` folder in Truffle:
``truffle compile``
``truffle test``
``truffle deploy``

Get your contract address from Ganache, e.g., `0xFf9e0936C2d65D66E9e66d3b1467982C9bfA0e45`
and an Ethereum account private key (without `0x`), e.g., `F30290f26CE49C145cC64f2f2c722DB3298e8056`.

### Running the server

``go build server.go``
``./server -contract 0xFf9e0936C2d65D66E9e66d3b1467982C9bfA0e45``

Open the webpage in:

``http://localhost:8080/``

To log in or sign up, you need to run the client app with the given service name and the random challenge

### Running the client

``go build clientapp.go``

Create and store SSO-ids in the contract

``./clientapp -command create -contract 0xFf9e0936C2d65D66E9e66d3b1467982C9bfA0e45 -keypath key1.txt -ethkey F30290f26CE49C145cC64f2f2c722DB3298e8056``

Load passkey

``./clientapp -command load -contract 0xFf9e0936C2d65D66E9e66d3b1467982C9bfA0e45 -keypath key1.txt``

Register for the service (Note: make sure that you do not copy whitespaces)

When the service name is `6d7e78af064c86eb9b9cb1c3611c9ab60a2f9317e3891891ef31770939f78ef8` and challenge is `b3d2482b609777d36d6faa48b3f0921f64d1d314665ab96b8d667838d2f51207`:
``./clientapp -command auth -contract 0xFf9e0936C2d65D66E9e66d3b1467982C9bfA0e45 -keypath key1.txt -sname 6d7e78af064c86eb9b9cb1c3611c9ab60a2f9317e3891891ef31770939f78ef8 -challenge b3d2482b609777d36d6faa48b3f0921f64d1d314665ab96b8d667838d2f51207``

Authentication for a service (Note: make sure that you do not copy whitespaces)

When the service name is `6d7e78af064c86eb9b9cb1c3611c9ab60a2f9317e3891891ef31770939f78ef8` and challenge is `b3d2482b609777d36d6faa48b3f0921f64d1d314665ab96b8d667838d2f51207`:
``./clientapp -command auth -contract 0xFf9e0936C2d65D66E9e66d3b1467982C9bfA0e45 -keypath key1.txt -sname 6d7e78af064c86eb9b9cb1c3611c9ab60a2f9317e3891891ef31770939f78ef8 -challenge b3d2482b609777d36d6faa48b3f0921f64d1d314665ab96b8d667838d2f51207``
