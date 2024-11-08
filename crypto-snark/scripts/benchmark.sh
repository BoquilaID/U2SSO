#!/bin/bash

for ((i=4; i<=10; i++))
do
    cd ./build/semaphore_$((2**i))_cpp
    make
    cd ../../
done

for ((i=4; i<=10; i++))
do
    for ((j=1; j<=10; j++))
    do
        ./build/semaphore_$((2**i))_cpp/semaphore_$((2**i)) ./build/input_$((2**i)).json ./build/witness_$((2**i)).wtns
        ./rapidsnark/package/bin/prover ./build/semaphore_$((2**i))_js.zkey ./build/witness_$((2**i)).wtns ./proof_$((2**i)).json ./public_$((2**i)).json
        time ./rapidsnark/package/bin/verifier ./build/semaphore_$((2**i))_js.json ./public_$((2**i)).json ./proof_$((2**i)).json
        ls -lh proof_$((2**i)).json
        ls -lh public_$((2**i)).json
    done
done