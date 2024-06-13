pragma circom 2.0.0;

include "../node_modules/circomlib/circuits/poseidon.circom";
include "../node_modules/circomlib/circuits/eddsa.circom";
include "../node_modules/circomlib/circuits/babyjub.circom";

template linkChildKeys() {
    // master_secret_key
    signal input master_secret_key;
    signal input web_name;
    signal input i;
    signal input i_plus_one;
    // TODO: remove the intermidiate secret_i and secret_i_plus_one
    signal input secret_i;
    signal input secret_i_plus_one;

    signal child_secret_key[2];

    signal output Ax_i, Ay_i;
    signal output Ax_i_plus_one, Ay_i_plus_one;

    component poseidon[2];

    poseidon[0] = Poseidon(3);
    poseidon[0].inputs[0] <== master_secret_key;
    poseidon[0].inputs[1] <== web_name;
    poseidon[0].inputs[2] <== i;
    child_secret_key[0] <== poseidon[0].out;

    poseidon[1] = Poseidon(3);
    poseidon[1].inputs[0] <== master_secret_key;
    poseidon[1].inputs[1] <== web_name;
    poseidon[1].inputs[2] <== i_plus_one;
    child_secret_key[1] <== poseidon[1].out;
    
    // TODO: connect poseidon[0] and poseidon[1] to the BabyPbk() component
    // Get the two Baby Jubjub points using the private key.
    (Ax_i, Ay_i) <== BabyPbk()(secret_i);
    (Ax_i_plus_one, Ay_i_plus_one) <== BabyPbk()(secret_i_plus_one);
}

component main = linkChildKeys();
