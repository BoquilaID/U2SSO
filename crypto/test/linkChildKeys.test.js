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
} = require("../src");

describe("Boquila", function () {
    this.timeout(200000);

    describe("#genMasterPk", function () {
        it("should generate master public key", async function () {
            const msk = "123";
            const mpk = await genMasterPk(msk);
            assert.equal(mpk.length, 2);
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

    describe("#Pedersen main", function () {
        it("should prove pederson output", async function () {
            const babyJub = await buildBabyjub();
            const Fr = babyJub.F;
            const pk = await genMasterPk([2, 1]);
            console.log(Fr.toString(pk[0]));
            const inputs = {
                in: [1, 2],
            };
            const proof = await snarkjs.groth16.fullProve(
                inputs,
                "semaphore_js/semaphore.wasm",
                "semaphore_js.zkey"
            );
            console.log(proof);
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
            const message = "message";

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
