// SPDX-License-Identifier: MIT
// Tells the Solidity compiler to compile only from v0.8.13 to v0.9.0
pragma solidity ^0.8.13;

// This is just a simple example of a coin-like contract.
// It is not ERC20 compatible and cannot be expected to talk to other
// coin/token contracts.

contract U2SSO {
	struct ID {
        uint256 id;
        uint id33;
        bool active;
        address owner;
    }

    address private _owner;
    constructor() {
        _owner = msg.sender;
    }

    ID[] public idList;

    uint nextIndex;

    function addID (uint256 _id, uint _id33) public returns (uint) {
        idList.push(ID(_id, _id33, true, _owner));
        nextIndex = nextIndex + 1;
        return nextIndex - 1;
    }

    function revokeID (uint _index) public {
        ID storage id = idList[_index];
        if (_owner == id.owner) {
            id.active = false;
        }
    }

	function getIDs (uint _index) public view returns (uint256, uint) {
		ID storage id = idList[_index];
		return (id.id, id.id33);
	}

	function getState (uint _index) public view returns (bool) {
		ID storage id = idList[_index];
		return id.active;
	}

	function getIDSize () public view returns (uint) {
		return nextIndex;
	}

	function getIDIndex (uint256 _id, uint _id33) public view returns (int) {
		for (uint i = 0; i < nextIndex; i++) {
			bool exists = (idList[i].id == _id) && (idList[i].id33 == _id33);
			if(exists == true) {
				return int(i);
			}
		}
		return -1;
	}
}
