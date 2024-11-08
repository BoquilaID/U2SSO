const snarkjs = require("snarkjs");
const { poseidon2, poseidon3 } = require("poseidon-lite");
const { deriveSecretScalar } = require("@zk-kit/eddsa-poseidon");
const { encodeBytes32String } = require("ethers/abi");
const { toBigInt } = require("ethers/utils");

const { Identity } = require("@semaphore-protocol/identity");
const buildBabyjub = require("circomlibjs").buildBabyjub;

const { generateProof, verifyProof } = require("@semaphore-protocol/proof");

function convertMessage(message) {
    try {
        return toBigInt(message);
    } catch (e) {
        return toBigInt(encodeBytes32String(message));
    }
}

async function genMasterPk(msk) {
    const babyJub = await buildBabyjub();
    const Fr = babyJub.F;
    const PBASE_G = [
        Fr.e(
            "5299619240641551281634865583518297030282874472190772894086521144482721001553"
        ),
        Fr.e(
            "16950150798460657717958625567821834550301663161624707787222815936182638968203"
        ),
    ];
    const PBASE_H = [
        Fr.e(
            "10457101036533406547632367118273992217979173478358440826365724437999023779287"
        ),
        Fr.e(
            "19824078218392094440610104313265183977899662750282163392862422243483260492317"
        ),
    ];

    const mpk = babyJub.addPoint(
        babyJub.mulPointEscalar(PBASE_G, msk[1]),
        babyJub.mulPointEscalar(PBASE_H, msk[0])
    );
    return mpk;
}

async function deriveWebKey(msk, name) {
    const babyJub = await buildBabyjub();
    const Fr = babyJub.F;
    const PBASE_G = [
        Fr.e(
            "5299619240641551281634865583518297030282874472190772894086521144482721001553"
        ),
        Fr.e(
            "16950150798460657717958625567821834550301663161624707787222815936182638968203"
        ),
    ];
    const PBASE_H = [
        Fr.e(
            "10457101036533406547632367118273992217979173478358440826365724437999023779287"
        ),
        Fr.e(
            "19824078218392094440610104313265183977899662750282163392862422243483260492317"
        ),
    ];

    const convertedName = convertMessage(name);
    const hash = poseidon2([msk[1], convertedName]);
    const negMsk0 = Fr.neg(Fr.e(msk[0]));
    const negMskString = Fr.toString(negMsk0);
    const wpk = babyJub.addPoint(
        babyJub.mulPointEscalar(PBASE_G, hash),
        babyJub.mulPointEscalar(PBASE_H, negMskString) // -msk[0]
    );
    return wpk;
}


function deriveChildSecretKey(msk, name, count) {
    const convertedName = convertMessage(name);
    const csk = poseidon3([msk[1], convertedName, count]);
    return csk;
}

async function deriveChildPublicKey(csk) {
    const babyJub = await buildBabyjub();
    const Fr = babyJub.F;
    const PBASE_G = [
        Fr.e(
            "5299619240641551281634865583518297030282874472190772894086521144482721001553"
        ),
        Fr.e(
            "16950150798460657717958625567821834550301663161624707787222815936182638968203"
        ),
    ];

    const cpk = babyJub.addPoint(
        babyJub.Base8,
        babyJub.mulPointEscalar(PBASE_G, csk)
    );
    return cpk;
}

async function ProveMPKMemb(sk, group, message) {
    const identity = new Identity(sk);
    const scope = "1234";
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


async function createID(msk) {
    const identity = new Identity(msk);
    return identity;
}


async function createSPK(msk, serviceName) {
    const convertedName = convertMessage(serviceName);
    const csk = poseidon3([msk[1], convertedName, 100]);
    const spk = new Identity(csk.toString());
    return spk;
}

async function authProof(msk, challenge, serviceName) {
    const convertedName = convertMessage(serviceName);
    const csk = poseidon3([msk[1], convertedName, 100]);
    const spk = new Identity(csk.toString());
    const signature = spk.signMessage(challenge);
    return signature;
}

async function authVerify(spk, signature, challenge) {
    const val = Identity.verifySignature(challenge, signature, spk.publicKey);
    return val;
}

async function proveMem(msk, group, serviceName, challenge) {
    const identity = new Identity(msk);
    const proof = await generateProof(identity, group, challenge, serviceName);
    return proof;
}

async function getNullifier(proof) {
    return proof.nullifier;
}

async function verifyMem(proof, group, serviceName, challenge) {
    const isValid = await verifyProof(proof);
    const checkRoot = proof.merkleTreeRoot === group.root.toString();
    const checkMessage = proof.message === convertMessage(challenge).toString();
    const checkScope = proof.scope === convertMessage(serviceName).toString();
    return isValid && checkRoot && checkMessage && checkScope;
}

async function verifyMemWithNullifier(proof, group, serviceName, challenge) {
    const isValid = await verifyProof(proof);
    const checkRoot = proof.merkleTreeRoot === group.root.toString();
    const checkMessage = proof.message === convertMessage(challenge).toString();
    const checkScope = proof.scope === convertMessage(serviceName).toString();
    return isValid && checkRoot && checkMessage && checkScope;
}

exports.genMasterPk = genMasterPk;
exports.deriveWebKey = deriveWebKey;
exports.deriveChildSecretKey = deriveChildSecretKey;
exports.deriveChildPublicKey = deriveChildPublicKey;
exports.ProveMPKMemb = ProveMPKMemb;
exports.VerifyMPKMemb = VerifyMPKMemb;
exports.ReplaceCPK = ReplaceCPK;
exports.VerifyReplacedCPK = VerifyReplacedCPK;
exports.createID = createID
exports.createSPK = createSPK
exports.authProof = authProof
exports.authVerify = authVerify
exports.proveMem = proveMem
exports.verifyMem = verifyMem
exports.getNullifier = getNullifier
