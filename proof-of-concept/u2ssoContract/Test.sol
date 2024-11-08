// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.0;

contract Test {

    function ecrecover2(bytes32 msgHash, uint8 v, bytes32 r, bytes32 s) public pure returns (address) {
        return ecrecover(msgHash, v, r, s);
    }

    function ecrecover3(bytes32 msgHash, uint8 v, bytes32 r, bytes32 s) public returns (address) {

    bool ret;
    address addr;

    assembly {
        let size := mload(0x40)
        mstore(size, msgHash)
        mstore(add(size, 32), v)
        mstore(add(size, 64), r)
        mstore(add(size, 96), s)
        ret := call(3000, 1, 0, size, 128, size, 32)
        addr := mload(size)
    }
    return addr;
    }
}