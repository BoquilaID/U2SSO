const snarkjs = require("snarkjs");
const { poseidon3 } = require("poseidon-lite");
const { deriveSecretScalar } = require("@zk-kit/eddsa-poseidon");
const { encodeBytes32String } = require("ethers/abi");
const { toBigInt } = require("ethers/utils");

const { Identity } = require("@semaphore-protocol/identity");

const { generateProof, verifyProof } = require("@semaphore-protocol/proof");

function convertMessage(message) {
    try {
        return toBigInt(message);
    } catch (e) {
        return toBigInt(encodeBytes32String(message));
    }
}

async function ProveMPKMemb(sk, group, message) {
    const identity = new Identity(sk);
    const scope = "Boquila";
    const proof = await generateProof(identity, group, message, scope);
    return proof;
}

async function VerifyMPKMemb(proof, group, message) {
    const isValid = await verifyProof(proof);
    const checkRoot = proof.merkleTreeRoot === group.root.toString();
    const checkMessage = proof.message === convertMessage(message).toString();
    return isValid && checkRoot && checkMessage;
}

async function ReplaceCPK(masterSecretKey, webName, i, iPlusOne) {
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

async function VerifyReplacedCPK(proof) {
    const vkey = require("../link_child_keys_js.json");
    const isValid = await snarkjs.groth16.verify(
        vkey,
        proof.publicSignals,
        proof.proof
    );
    return isValid;
}

exports.ProveMPKMemb = ProveMPKMemb;
exports.VerifyMPKMemb = VerifyMPKMemb;
exports.ReplaceCPK = ReplaceCPK;
exports.VerifyReplacedCPK = VerifyReplacedCPK;
