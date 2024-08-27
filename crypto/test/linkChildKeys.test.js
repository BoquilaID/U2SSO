const assert = require("assert");
const snarkjs = require("snarkjs");
const { poseidon3 } = require("poseidon-lite");
const { derivePublicKey } = require("@zk-kit/eddsa-poseidon");
const { Identity } = require("@semaphore-protocol/identity");
const { Group } = require("@semaphore-protocol/group");
const buildBabyjub = require("circomlibjs").buildBabyjub;

const {
    genMasterPk,
    deriveWebKey,
    deriveChildSecretKey,
    deriveChildPublicKey,
    ReplaceCPK,
    VerifyReplacedCPK,
    ProveMPKMemb,
    VerifyMPKMemb,
    createID,
    createSPK,
    authProof,
    authVerify,
    proveMem,
    verifyMem,
} = require("../src");
const { channel } = require("diagnostics_channel");

describe("Boquila", function () {
    this.timeout(200000);

    describe("Verifying auth proofs", function () {
        it("should generate valid signatures", async function () {
            const msk = "123";
            const id = await createID(msk);
            const message = "Hello World"
            const signature = id.signMessage(message)
            assert.equal(true, Identity.verifySignature(message, signature, id.publicKey));
        });
    });

    describe("Verifying auth proofs for service keys", function () {
        it("should generate valid signatures", async function () {
            const msk = "123";
            const id = await createID(msk);
            const challenge = "Hello World";
            const serviceName = "abc_service";
            const spk = await createSPK(msk, serviceName)
            const signature = spk.signMessage(challenge)
            assert.equal(true, Identity.verifySignature(challenge, signature, spk.publicKey));
        });
    });

    describe("Verifying auth proofs for service keys", function () {
        it("should generate valid signatures", async function () {
            const msk = "123";
            const id = await createID(msk);
            const challenge = "Hello World";
            const serviceName = "abc_service";
            const spk = await createSPK(msk, serviceName)
            const signature = await authProof(msk, challenge, serviceName)
            const val = await authVerify(spk, signature, challenge)
            assert.equal(true, val);
        });
    });

    describe("Verifying membership proofs", function () {
        it("should generate valid proofs", async function () {
            const msk1 = "1";
            const id1 = await createID(msk1);
            const msk2 = "2";
            const id2 = await createID(msk2);
            const msk3 = "3";
            const id3 = await createID(msk3);
            const msk4 = "4";
            const id4 = await createID(msk4);
            
            const challenge = "1234";
            const serviceName = "1234";
            
            const group = new Group([
                id1.commitment, 
                id2.commitment,
                id3.commitment,
                id4.commitment,
            ]);

            const proof = await proveMem(msk1, group, serviceName, challenge);
            const val = await verifyMem(proof, group, serviceName, challenge);
            assert.equal(true, val);
        });
    });

    describe("#deriveWebKeys", function () {
        it("should derive web key", async function () {
            const msk = "123";
            const name = "web1";
            const wpk = await deriveWebKey(msk, name);
            assert.equal(wpk.length, 2);
        });
    });

    describe("#deriveChildSecretKey", function () {
        it("should derive child secret key", async function () {
            const msk = "123";
            const name = "web1";
            const count = 0;
            const csk = deriveChildSecretKey(msk, name, count);
            assert.notEqual(csk, null);
        });
    });

    describe("#deriveChildPublicKey", function () {
        it("should derive child public key", async function () {
            const msk = "123";
            const name = "web1";
            const count = 0;
            const csk = deriveChildSecretKey(msk, name, count);
            const cpk = await deriveChildPublicKey(csk);
            assert.equal(cpk.length, 2);
        });
    });

    describe("#ProveMPKMemb/VerifyMPKMemb", function () {
        it("should prove and verify MPK Membership", async function () {
            const sk = "user1";
            const sk2 = "user2";
            const identity = new Identity(sk); // user
            const identity1 = new Identity(sk2);
            const identity2 = new Identity();
            const identity3 = new Identity();
            const message = "1234";

            const group = new Group([
                identity.commitment, // user
                identity1.commitment,
                identity2.commitment,
                identity3.commitment,
            ]);
            const proof = await ProveMPKMemb(sk, group, message);
            const proof2 = await ProveMPKMemb(sk2, group, message);

            const isValid = await VerifyMPKMemb(proof, group, message);
            const isValid2 = await VerifyMPKMemb(proof2, group, message);
            assert.equal(isValid, true);
            assert.equal(isValid2, true);
        });
    });

    // describe("#ReplaceCPK/VerifyReplacedCPK", function () {
    //     it("should prove and verify CPK", async function () {
    //         const masterSecretKey =
    //             "0001020304050607080900010203040506070809000102030405060708090001";

    //         const webName = "111";
    //         const i = 0;
    //         const iPlusOne = 1;

    //         // child keys
    //         const childSecretKeyi = poseidon3([masterSecretKey, webName, i]);
    //         const childSecretKeyiPlusOne = poseidon3([
    //             masterSecretKey,
    //             webName,
    //             iPlusOne,
    //         ]);
    //         const privKeyi = Buffer.from(childSecretKeyi.toString());
    //         const privKeyiPlusOne = Buffer.from(
    //             childSecretKeyiPlusOne.toString()
    //         );

    //         // public keys
    //         const pubKeyi = derivePublicKey(privKeyi);
    //         const pubKeyiPlusOne = derivePublicKey(privKeyiPlusOne);
    //         const proof = await ReplaceCPK(
    //             masterSecretKey,
    //             webName,
    //             i,
    //             iPlusOne
    //         );

    //         // check public keys
    //         assert.equal(
    //             pubKeyi[0].toString(),
    //             proof.publicSignals[0].toString()
    //         );
    //         assert.equal(
    //             pubKeyi[1].toString(),
    //             proof.publicSignals[1].toString()
    //         );
    //         assert.equal(
    //             pubKeyiPlusOne[0].toString(),
    //             proof.publicSignals[2].toString()
    //         );
    //         assert.equal(
    //             pubKeyiPlusOne[1].toString(),
    //             proof.publicSignals[3].toString()
    //         );

    //         // check proof
    //         const isValid = await VerifyReplacedCPK(proof);
    //         assert.equal(isValid, true);
    //     });
    // });
});
