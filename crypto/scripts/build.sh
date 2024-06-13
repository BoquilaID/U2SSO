#!/bin/bash

PTAU="powersOfTau28_hez_final_14.ptau"
circom --r1cs --wasm ./circuits/link_child_keys.circom
if [ ! -f "$PTAU" ]; then
    wget https://hermez.s3-eu-west-1.amazonaws.com/"$PTAU"
fi
snarkjs groth16 setup link_child_keys.r1cs "$PTAU" link_child_keys_js.zkey
snarkjs zkey export verificationkey link_child_keys_js.zkey link_child_keys_js.json