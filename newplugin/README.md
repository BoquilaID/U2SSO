This plugin is used to register the user in the Boquila system. And geenrate the proof of the user's identity. This proof can be verified by the server with publicly available data from the smart contract. It uses ring signature at the core.

## Build steps

```bash
Build the crupto-ddh library first, documentation in the README.md file of the library.

golang build 1.20>

$ go build -o simulator main.go

$ go build -o proof_gen proof_gen.go

Have Nodejs 18>= installed.
install expressjs:
$ npm install express

Post installation, run the server using following command:
$ node server.js

User should have ganache installed and running on port 8545.

$ ./ganache_<version>
Or for Sepolia
use contract name:
0xEc42bF8f83815A773cf70bB17e6EFF71a99438bd

```

## Run

1. Go to chrome extensions and enable developer mode.
2. Load the plugin by clicking on load unpacked and selecting the newplugin folder.
3. Open the plugin and click on **Generate Master Key Pair**
   > This will generate the master key pair and store it in the local storage.
   > The public master key will be published on blockchain.
4. Click on **Generate Proof**
   > This uses the master key pair of the user and data from the smart contract to generate the proof.
5. Finally a hex representation of the proof will be generated. This can be verified by the server and the user will be registered in the system in privacy preserving way.
