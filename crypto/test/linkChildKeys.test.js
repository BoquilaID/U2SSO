const assert = require("assert");
const { poseidon3 } = require("poseidon-lite");
const { derivePublicKey } = require("@zk-kit/eddsa-poseidon");

const { ReplaceCPK, VerifyReplacedCPK } = require("../src");

describe("Boquila", function () {
    this.timeout(100000);
    describe("#ReplaceCPK/VerifyReplacedCPK", function () {
        it("should prove and verify CPK", async function () {
            const masterSecretKey =
                "0001020304050607080900010203040506070809000102030405060708090001";

            const webName = "111";
            const i = 0;
            const iPlusOne = 1;

            // child keys
            const childSecretKeyi = poseidon3([masterSecretKey, webName, i]);
            const childSecretKeyiPlusOne = poseidon3([
                masterSecretKey,
                webName,
                iPlusOne,
            ]);
            const privKeyi = Buffer.from(childSecretKeyi.toString());
            const privKeyiPlusOne = Buffer.from(
                childSecretKeyiPlusOne.toString()
            );

            // public keys
            const pubKeyi = derivePublicKey(privKeyi);
            const pubKeyiPlusOne = derivePublicKey(privKeyiPlusOne);
            const proof = await ReplaceCPK(
                masterSecretKey,
                webName,
                i,
                iPlusOne
            );

            // check public keys
            assert.equal(
                pubKeyi[0].toString(),
                proof.publicSignals[0].toString()
            );
            assert.equal(
                pubKeyi[1].toString(),
                proof.publicSignals[1].toString()
            );
            assert.equal(
                pubKeyiPlusOne[0].toString(),
                proof.publicSignals[2].toString()
            );
            assert.equal(
                pubKeyiPlusOne[1].toString(),
                proof.publicSignals[3].toString()
            );

            // check proof
            const isValid = await VerifyReplacedCPK(proof);
            assert.equal(isValid, true);
        });
    });
});
