// SPDX-License-Identifier: MIT
pragma solidity ^0.8.13;

contract Boquila {

    string[] public list;
    mapping (string => uint256) public pkToIndex;

    function addElement(string memory elem) public { //returns (uint256){
        // Add element to the blockchain
        // uint256 size = list.length;
        // TODO not add the element if it is already in the list
        list.push(elem);
        pkToIndex[elem] = list.length - 1;
        // return size; //cannot return something here because it is not allowed by the interface
    }

    function getIndexOfElement(string memory elem) public view returns (uint256) {
        // Get the index of an element from the blockchain
        // handle teh case where the element is not in the list
        return pkToIndex[elem];
    }

    function getList() public view returns (string[] memory) {
        // Get the list of elements from the blockchain
        return list;
    }   
}