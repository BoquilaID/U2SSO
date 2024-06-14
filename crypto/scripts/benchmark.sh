#!/bin/bash

for ((i=1; i<=10; i++))
do
    ./semaphore_cpp/semaphore input.json witness.wtns
    ./rapidsnark/package/bin/prover ./crypto/semaphore_js.zkey ./crypto/witness.wtns ./crypto/proof.json ./crypto/public.json
done