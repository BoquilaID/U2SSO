#!/bin/bash

for ((i=1; i<=10; i++))
do
    ./semaphore_cpp/semaphore input.json witness.wtns
    ../rapidsnark/package/bin/prover ./semaphore_js.zkey ./witness.wtns ./proof.json ./public.json
done