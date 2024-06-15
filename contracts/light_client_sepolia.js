const {Web3} = require('web3');
const ABI = require('./truffle/build/contracts/Boquila.json'); // SET HERE ABI of the contract
     
// SET UP THE WEB3 PROVIDER
let web3 = new Web3('https://sepolia.infura.io/v3/90f323dad12a4bc5bacb7249ee2931c3');

let privateKey = '36e8cffd19966fac96e718f030ce8bb49dd0105516fc2177844913de6814470b'; // PASTE THE PRIVATE KEY HERE
const myaccount = web3.eth.accounts.privateKeyToAccount('0x' + privateKey);
web3.eth.accounts.wallet.add(myaccount);
web3.eth.defaultAccount = myaccount.address;
console.log(web3.eth.defaultAccount);

let contract_address = '0xFb5d7945C242c4f99d642F521A0034F53D6B652D'; // PASTE THE CONTRACT ADDRESS HERE
let contract = new web3.eth.Contract(ABI.abi, contract_address);

// console.log(contract.methods);

async function getDefaultAccaunt() {    
        // Get the accounts to use one as default to sign transactions 
        const accounts = await web3.eth.getAccounts();
        return accounts[0];
}

async function getAccount() {
    const account = web3.eth.accounts.privateKeyToAccount(privateKey);
    return account;
}

async function addElement(element, account) {
    const result = await contract.methods.addElement(element).send({from: account, gas: 3000000});
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
    console.log(defaultAccount);
    // const account = await getAccount();
    let account = web3.eth.defaultAccount;
    console.log(account);

    // await addElement("Element1", account);
    await addElement("Element2", account);
    await addElement("Element3", account);
    await addElement("Element4", account);
    await addElement("Element5", account);
    const list = await getList();
    console.log("List: " + list);
    const index = await getIndexOfElement("Element3");
    console.log("Index of Element3: " + index);
}

interact(); 