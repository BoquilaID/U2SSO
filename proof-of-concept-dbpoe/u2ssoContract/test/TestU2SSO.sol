// SPDX-License-Identifier: MIT
pragma solidity ^0.8.13;

// These files are dynamically created at test time
import "truffle/Assert.sol";
import "../contracts/U2SSO.sol";

contract TestU2SSO {

  function testAgeSetting() public {
    U2SSO meta = new U2SSO();
    uint256  expected = 1000;
    uint expected32 = 1;
    uint256 actual = 10000;
    uint actual32 = 1;

    uint index = meta.addID(expected, expected32);
    (actual, actual32) = meta.getIDs(index);
    Assert.equal(expected, actual, "value should be expected");
    Assert.equal(expected32, actual32, "value should be expected");

    int receivedIndex = meta.getIDIndex(expected, expected32);
    Assert.equal(int(index), receivedIndex, "value should be expected");

    expected = 10001;

    index = meta.addID(expected, expected32);
    (actual, actual32)  = meta.getIDs(index);
    Assert.equal(expected, actual, "value should be expected");

    meta.revokeID(index);
    bool  state = meta.getState(index);
    Assert.equal(false, state, "value should be false");
  }
}
