const {Web3} = require('web3');
const ABI = require('./truffle/build/contracts/Boquila.json'); // SET HERE ABI of the contract
     
// SET UP THE WEB3 PROVIDER
// let web3 = new Web3('https://ethereum-sepolia-rpc.publicnode.com');




function setProvider(providerUrl){
    return  new Web3(new Web3.providers.HttpProvider(providerUrl));
}

function getContract(web3){
    let contract_address = '0xEc42bF8f83815A773cf70bB17e6EFF71a99438bd'; // PASTE THE CONTRACT ADDRESS HERE
    let contract = new web3.eth.Contract(ABI.abi, contract_address);
    return contract;
}

async function getAccountAddress(privateKey, web3) {
    const account = web3.eth.accounts.privateKeyToAccount(privateKey);
    web3.eth.accounts.wallet.add(account);
    web3.eth.defaultAccount = account.address;
    return web3.eth.defaultAccount;
}

async function addElement(element, account, contract) {
    const result = await contract.methods.addElement(element).send({from: account, gas: 3000000});
    console.log("Transaction Hash: " + result.transactionHash)
}

async function getList(contract) {
    const list = await contract.methods.getList().call();
    return list;
}

async function getIndexOfElement(element, contract) {
    const index = await contract.methods.getIndexOfElement(element).call();
    return index;
}

async function interact() {
    let web3 = setProvider('https://ethereum-sepolia-rpc.publicnode.com')

    let contract = getContract(web3);
    let privateKey = '<YOUR_PRIVATE_KEY>'; // PASTE THE PRIVATE KEY HERE
    let account = await getAccountAddress(privateKey, web3);
    // let account = web3.eth.defaultAccount;
    console.log(account);

    // await addElement("Element1", account);
    await addElement("Element2", account, contract);
    await addElement("Element3", account, contract);
    await addElement("Element4", account, contract);
    await addElement("Element5", account, contract);
    const list = await getList(contract);
    console.log("List: " + list);
    const index = await getIndexOfElement("Element3", contract);
    console.log("Index of Element3: " + index);
}

interact(); 