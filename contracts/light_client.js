const {Web3} = require('web3');

const ABI = require('./truffle/build/contracts/Boquila.json'); // SET HERE ABI of the contract
     
// SET UP THE WEB3 PROVIDER
let web3 = new Web3('http://localhost:7545');

let contract_address = '0x961B50CF253B974BEA3110BbeE4A81414D8c3ae5'; // PASTE THE CONTRACT ADDRESS HERE

let contract = new web3.eth.Contract(ABI.abi, contract_address);

console.log(contract.methods);

async function getDefaultAccaunt() {    
        // Get the accounts to use one as default to sign transactions 
        const accounts = await web3.eth.getAccounts();
        return accounts[1];
}

async function addElement(element, defaultAccount) {
    const result = await contract.methods.addElement(element).send({from: defaultAccount, gas: 3000000});
    console.log("Transaction Hash: " + result.transactionHash)
}

async function getList() {
    const list = await contract.methods.getList().call();
    return list;
}

async function getIndexOfElement(element) {
    const index = await contract.methods.getIndexOfElement(element).call();
    return index;
}

async function interact() {
    const defaultAccount = await getDefaultAccaunt();
    await addElement("Element1", defaultAccount);
    await addElement("Element2", defaultAccount);
    await addElement("Element3", defaultAccount);
    await addElement("Element4", defaultAccount);
    await addElement("Element5", defaultAccount);
    const list = await getList();
    console.log("List: " + list);
    const index = await getIndexOfElement("Element3");
    console.log("Index of Element3: " + index);
}

interact(); 