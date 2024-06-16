pragma solidity >=0.8.13;

import "../contracts/Boquila.sol";

contract TestBoquila {
    Boquila boquila = new Boquila();
    
    function testAddElementBoquila() public {
        string memory elem = "element";
        boquila.addElement(elem);
        string[] memory list = boquila.getList();
        string memory expected = "element";
        assert(keccak256(abi.encodePacked(list[0])) == keccak256(abi.encodePacked(expected)));
    }
    
    function testGetListBoquila() public {
        Boquila boquila = new Boquila(); //this resets the contract state
        string memory elem = "element";
        boquila.addElement(elem);
        string memory elem2 = "element2";
        boquila.addElement(elem2);
        string[] memory list = boquila.getList();
        string[] memory expected = new string[](2);
        expected[0] = "element";
        expected[1] = "element2";
        assert(keccak256(abi.encodePacked(list[0])) == keccak256(abi.encodePacked(expected[0])));
        assert(keccak256(abi.encodePacked(list[1])) == keccak256(abi.encodePacked(expected[1])));
    }   
}