// Test program to interact with the Boquila contract
const Boquila = artifacts.require("Boquila");

contract("Boquila", async () => {
    it("should deploy the Boquila contract", async () => {
        const boquila = await Boquila.deployed();
        console.log(boquila.address);
    });

    // check the initial state of the contract
    it("should return an empty list", async () => { 
        const boquila = await Boquila.deployed();
        const list = await boquila.getList.call();
        assert.equal(list.length, 0, "List not empty");
    });

    //test adding an element to the list
    it("should add a new element", async () => {
        const boquila = await Boquila.deployed();
        const element = "element"
        await boquila.addElement(element);
        const list = await boquila.getList.call();
        assert.equal(list[0], element, "Element not added");
    });

    it("should return the index of the element", async () => {
        const boquila = await Boquila.deployed();
        const element = "element"
        const index = await boquila.getIndexOfElement.call(element);
        assert.equal(index, 0, "Index not returned");
    });

    
    // it("should return the current size of the list", async () => {
    //     const boquila = await Boquila.deployed();
    //     const element = "element"
    //     const size = await boquila.addElement(element); //check the return value, should it be unwrapped?
    //     assert.equal(size, 1, "Size not updated");
    // });

    it("should return the current size of the list", async () => {
        const boquila = await Boquila.deployed();
        const size = await boquila.getList.call();
        assert.equal(size.length, 1, "Size not updated");
    });


});