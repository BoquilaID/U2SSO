#!/bin/bash

CIRCUIT="semaphore"
PTAU="powersOfTau28_hez_final_12.ptau"
circom --sym --wasm --r1cs --c -p bn128 -l ./node_modules/circomlib/circuits -l ./node_modules/@zk-kit/binary-merkle-root.circom/src --verbose --inspect --O2 ./circuits/semaphore.circom
if [ ! -f "$PTAU" ]; then
    wget https://hermez.s3-eu-west-1.amazonaws.com/"$PTAU"
fi
snarkjs groth16 setup "$CIRCUIT".r1cs "$PTAU" "$CIRCUIT"_js.zkey
snarkjs zkey export verificationkey "$CIRCUIT"_js.zkey "$CIRCUIT"_js.json