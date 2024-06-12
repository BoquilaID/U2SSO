const snarkjs = require("snarkjs");
const { poseidon3 } = require("poseidon-lite");
const {
    deriveSecretScalar,
} = require("@zk-kit/eddsa-poseidon");


async function ProveCPK(masterSecretKey, webName, i, iPlusOne) {
    // child keys
    const childSecretKeyi = poseidon3([masterSecretKey, webName, i]);
    const childSecretKeyiPlusOne = poseidon3([
        masterSecretKey,
        webName,
        iPlusOne,
    ]);
    const privKeyi = Buffer.from(childSecretKeyi.toString());
    const privKeyiPlusOne = Buffer.from(childSecretKeyiPlusOne.toString());

    const inputs = {
        master_secret_key: masterSecretKey,
        web_name: webName,
        i: i,
        i_plus_one: iPlusOne,
        secret_i: deriveSecretScalar(privKeyi),
        secret_i_plus_one: deriveSecretScalar(privKeyiPlusOne),
    };
    const proof = await snarkjs.groth16.fullProve(
        inputs,
        "link_child_keys_js/link_child_keys.wasm",
        "link_child_keys_js.zkey"
    );
    return proof;
}

async function VerifyCPK(proof) {
    const vkey = require("../link_child_keys_js.json");
    const isValid = await snarkjs.groth16.verify(
        vkey,
        proof.publicSignals,
        proof.proof
    );
    return isValid;
}

exports.ProveCPK = ProveCPK;
exports.VerifyCPK = VerifyCPK;
